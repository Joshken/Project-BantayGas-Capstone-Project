#!/usr/bin/env python3
"""
BantayGas IoT Calibration Tool

This tool provides a graphical interface for calibrating the BantayGas IoT
gas detection system. It can connect to the ESP32 via serial communication
and perform calibration procedures with real-time monitoring.

Features:
- Real-time sensor data visualization
- Automatic calibration procedure
- Data logging and analysis
- Calibration quality assessment
- Export calibration data

Author: BantayGas Development Team
Version: 1.0.0
"""

import serial
import time
import json
import csv
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import threading
import queue
import os

class BantayGasCalibrationTool:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("BantayGas IoT Calibration Tool")
        self.root.geometry("800x600")
        
        # Serial connection
        self.serial_connection = None
        self.is_connected = False
        self.is_calibrating = False
        
        # Data storage
        self.sensor_data = []
        self.calibration_data = []
        self.start_time = None
        
        # Threading
        self.data_queue = queue.Queue()
        self.serial_thread = None
        self.stop_thread = False
        
        # GUI variables
        self.port_var = tk.StringVar()
        self.baudrate_var = tk.StringVar(value="115200")
        self.gas_type_var = tk.StringVar(value="LPG")
        self.calibration_samples_var = tk.StringVar(value="100")
        
        # Create GUI
        self.create_gui()
        
        # Start data processing thread
        self.process_data_thread = threading.Thread(target=self.process_data, daemon=True)
        self.process_data_thread.start()
    
    def create_gui(self):
        """Create the graphical user interface"""
        # Main frame
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Configure grid weights
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(1, weight=1)
        
        # Connection frame
        connection_frame = ttk.LabelFrame(main_frame, text="Connection", padding="5")
        connection_frame.grid(row=0, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        ttk.Label(connection_frame, text="Port:").grid(row=0, column=0, sticky=tk.W, padx=(0, 5))
        port_combo = ttk.Combobox(connection_frame, textvariable=self.port_var, width=15)
        port_combo.grid(row=0, column=1, sticky=tk.W, padx=(0, 10))
        self.refresh_ports(port_combo)
        
        ttk.Label(connection_frame, text="Baudrate:").grid(row=0, column=2, sticky=tk.W, padx=(0, 5))
        ttk.Combobox(connection_frame, textvariable=self.baudrate_var, 
                    values=["9600", "19200", "38400", "57600", "115200"], width=10).grid(row=0, column=3, sticky=tk.W, padx=(0, 10))
        
        self.connect_button = ttk.Button(connection_frame, text="Connect", command=self.toggle_connection)
        self.connect_button.grid(row=0, column=4, padx=(0, 5))
        
        self.refresh_button = ttk.Button(connection_frame, text="Refresh", command=lambda: self.refresh_ports(port_combo))
        self.refresh_button.grid(row=0, column=5)
        
        # Status frame
        status_frame = ttk.LabelFrame(main_frame, text="Status", padding="5")
        status_frame.grid(row=1, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        self.status_label = ttk.Label(status_frame, text="Disconnected", foreground="red")
        self.status_label.grid(row=0, column=0, sticky=tk.W)
        
        self.sensor_value_label = ttk.Label(status_frame, text="Sensor Value: --")
        self.sensor_value_label.grid(row=0, column=1, sticky=tk.W, padx=(20, 0))
        
        self.gas_ppm_label = ttk.Label(status_frame, text="Gas PPM: --")
        self.gas_ppm_label.grid(row=0, column=2, sticky=tk.W, padx=(20, 0))
        
        # Calibration frame
        calibration_frame = ttk.LabelFrame(main_frame, text="Calibration", padding="5")
        calibration_frame.grid(row=2, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        ttk.Label(calibration_frame, text="Gas Type:").grid(row=0, column=0, sticky=tk.W, padx=(0, 5))
        ttk.Combobox(calibration_frame, textvariable=self.gas_type_var, 
                    values=["LPG", "Butane", "Propane", "Methane"], width=10).grid(row=0, column=1, sticky=tk.W, padx=(0, 10))
        
        ttk.Label(calibration_frame, text="Samples:").grid(row=0, column=2, sticky=tk.W, padx=(0, 5))
        ttk.Entry(calibration_frame, textvariable=self.calibration_samples_var, width=10).grid(row=0, column=3, sticky=tk.W, padx=(0, 10))
        
        self.calibrate_button = ttk.Button(calibration_frame, text="Start Calibration", 
                                         command=self.start_calibration, state=tk.DISABLED)
        self.calibrate_button.grid(row=0, column=4, padx=(0, 5))
        
        self.stop_calibration_button = ttk.Button(calibration_frame, text="Stop Calibration", 
                                                command=self.stop_calibration, state=tk.DISABLED)
        self.stop_calibration_button.grid(row=0, column=5)
        
        # Progress frame
        progress_frame = ttk.LabelFrame(main_frame, text="Progress", padding="5")
        progress_frame.grid(row=3, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        self.progress_var = tk.DoubleVar()
        self.progress_bar = ttk.Progressbar(progress_frame, variable=self.progress_var, maximum=100)
        self.progress_bar.grid(row=0, column=0, sticky=(tk.W, tk.E), padx=(0, 10))
        progress_frame.columnconfigure(0, weight=1)
        
        self.progress_label = ttk.Label(progress_frame, text="0%")
        self.progress_label.grid(row=0, column=1)
        
        # Data frame
        data_frame = ttk.LabelFrame(main_frame, text="Data", padding="5")
        data_frame.grid(row=4, column=0, columnspan=2, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(0, 10))
        main_frame.rowconfigure(4, weight=1)
        
        # Create treeview for data display
        columns = ("Time", "Raw Value", "Voltage", "Resistance", "Gas PPM", "Alert Level")
        self.data_tree = ttk.Treeview(data_frame, columns=columns, show="headings", height=10)
        
        for col in columns:
            self.data_tree.heading(col, text=col)
            self.data_tree.column(col, width=100)
        
        # Scrollbar for treeview
        scrollbar = ttk.Scrollbar(data_frame, orient=tk.VERTICAL, command=self.data_tree.yview)
        self.data_tree.configure(yscrollcommand=scrollbar.set)
        
        self.data_tree.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        scrollbar.grid(row=0, column=1, sticky=(tk.N, tk.S))
        data_frame.columnconfigure(0, weight=1)
        data_frame.rowconfigure(0, weight=1)
        
        # Control frame
        control_frame = ttk.Frame(main_frame)
        control_frame.grid(row=5, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        self.clear_data_button = ttk.Button(control_frame, text="Clear Data", command=self.clear_data)
        self.clear_data_button.grid(row=0, column=0, padx=(0, 5))
        
        self.export_data_button = ttk.Button(control_frame, text="Export Data", command=self.export_data)
        self.export_data_button.grid(row=0, column=1, padx=(0, 5))
        
        self.plot_data_button = ttk.Button(control_frame, text="Plot Data", command=self.plot_data)
        self.plot_data_button.grid(row=0, column=2, padx=(0, 5))
        
        self.save_calibration_button = ttk.Button(control_frame, text="Save Calibration", 
                                                command=self.save_calibration, state=tk.DISABLED)
        self.save_calibration_button.grid(row=0, column=3, padx=(0, 5))
        
        # Results frame
        results_frame = ttk.LabelFrame(main_frame, text="Calibration Results", padding="5")
        results_frame.grid(row=6, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        self.results_text = tk.Text(results_frame, height=4, width=80)
        self.results_text.grid(row=0, column=0, sticky=(tk.W, tk.E))
        results_frame.columnconfigure(0, weight=1)
    
    def refresh_ports(self, port_combo):
        """Refresh available serial ports"""
        import serial.tools.list_ports
        ports = [port.device for port in serial.tools.list_ports.comports()]
        port_combo['values'] = ports
        if ports and not self.port_var.get():
            port_combo.current(0)
    
    def toggle_connection(self):
        """Toggle serial connection"""
        if not self.is_connected:
            self.connect()
        else:
            self.disconnect()
    
    def connect(self):
        """Connect to serial port"""
        try:
            port = self.port_var.get()
            baudrate = int(self.baudrate_var.get())
            
            if not port:
                messagebox.showerror("Error", "Please select a port")
                return
            
            self.serial_connection = serial.Serial(port, baudrate, timeout=1)
            self.is_connected = True
            
            # Update GUI
            self.status_label.config(text="Connected", foreground="green")
            self.connect_button.config(text="Disconnect")
            self.calibrate_button.config(state=tk.NORMAL)
            
            # Start serial reading thread
            self.serial_thread = threading.Thread(target=self.read_serial_data, daemon=True)
            self.serial_thread.start()
            
            # Send initial commands
            self.send_command("status")
            
        except Exception as e:
            messagebox.showerror("Connection Error", f"Failed to connect: {str(e)}")
    
    def disconnect(self):
        """Disconnect from serial port"""
        self.stop_thread = True
        
        if self.serial_connection:
            self.serial_connection.close()
            self.serial_connection = None
        
        self.is_connected = False
        
        # Update GUI
        self.status_label.config(text="Disconnected", foreground="red")
        self.connect_button.config(text="Connect")
        self.calibrate_button.config(state=tk.DISABLED)
        self.stop_calibration_button.config(state=tk.DISABLED)
    
    def read_serial_data(self):
        """Read data from serial port in separate thread"""
        self.stop_thread = False
        
        while not self.stop_thread and self.is_connected:
            try:
                if self.serial_connection and self.serial_connection.in_waiting:
                    line = self.serial_connection.readline().decode('utf-8').strip()
                    if line:
                        self.data_queue.put(line)
                time.sleep(0.01)
            except Exception as e:
                print(f"Serial read error: {e}")
                break
    
    def process_data(self):
        """Process incoming data in separate thread"""
        while True:
            try:
                if not self.data_queue.empty():
                    line = self.data_queue.get_nowait()
                    self.parse_serial_data(line)
                time.sleep(0.01)
            except queue.Empty:
                time.sleep(0.01)
            except Exception as e:
                print(f"Data processing error: {e}")
    
    def parse_serial_data(self, line):
        """Parse incoming serial data"""
        try:
            # Parse sensor data line
            if "Gas Level:" in line:
                parts = line.split("|")
                if len(parts) >= 3:
                    # Extract gas PPM
                    gas_part = parts[0].strip()
                    gas_ppm = float(gas_part.split(":")[1].strip().split()[0])
                    
                    # Extract alert level
                    alert_part = parts[1].strip()
                    alert_level = int(alert_part.split(":")[1].strip())
                    
                    # Extract R0
                    r0_part = parts[2].strip()
                    r0_value = float(r0_part.split(":")[1].strip())
                    
                    # Update GUI
                    self.gas_ppm_label.config(text=f"Gas PPM: {gas_ppm:.2f}")
                    
                    # Store data
                    current_time = datetime.now()
                    data_point = {
                        'timestamp': current_time,
                        'raw_value': 0,  # Not available in this format
                        'voltage': 0,    # Not available in this format
                        'resistance': 0, # Not available in this format
                        'gas_ppm': gas_ppm,
                        'alert_level': alert_level,
                        'r0_value': r0_value
                    }
                    
                    self.sensor_data.append(data_point)
                    self.update_data_display()
            
            # Parse calibration progress
            elif "Calibration progress:" in line:
                progress_text = line.split(":")[1].strip().replace("%", "")
                progress = float(progress_text)
                self.progress_var.set(progress)
                self.progress_label.config(text=f"{progress:.0f}%")
            
            # Parse calibration results
            elif "Calibration completed successfully!" in line:
                self.is_calibrating = False
                self.calibrate_button.config(state=tk.NORMAL)
                self.stop_calibration_button.config(state=tk.DISABLED)
                self.save_calibration_button.config(state=tk.NORMAL)
                messagebox.showinfo("Calibration", "Calibration completed successfully!")
            
            elif "Calibration failed!" in line:
                self.is_calibrating = False
                self.calibrate_button.config(state=tk.NORMAL)
                self.stop_calibration_button.config(state=tk.DISABLED)
                messagebox.showerror("Calibration", "Calibration failed!")
            
            # Parse R0 value
            elif "R0 value:" in line:
                r0_text = line.split(":")[1].strip().split()[0]
                r0_value = float(r0_text)
                self.update_calibration_results(r0_value)
                
        except Exception as e:
            print(f"Data parsing error: {e}")
    
    def update_data_display(self):
        """Update the data display treeview"""
        if not self.sensor_data:
            return
        
        # Keep only last 100 data points
        if len(self.sensor_data) > 100:
            self.sensor_data = self.sensor_data[-100:]
        
        # Clear and repopulate treeview
        for item in self.data_tree.get_children():
            self.data_tree.delete(item)
        
        for data in self.sensor_data[-20:]:  # Show last 20 points
            time_str = data['timestamp'].strftime("%H:%M:%S")
            self.data_tree.insert("", "end", values=(
                time_str,
                data['raw_value'],
                f"{data['voltage']:.3f}",
                f"{data['resistance']:.2f}",
                f"{data['gas_ppm']:.2f}",
                data['alert_level']
            ))
    
    def update_calibration_results(self, r0_value):
        """Update calibration results display"""
        results = f"Calibration Results:\n"
        results += f"R0 Value: {r0_value:.2f} kΩ\n"
        results += f"Gas Type: {self.gas_type_var.get()}\n"
        results += f"Samples: {self.calibration_samples_var.get()}\n"
        results += f"Timestamp: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
        
        self.results_text.delete(1.0, tk.END)
        self.results_text.insert(1.0, results)
    
    def send_command(self, command):
        """Send command to ESP32"""
        if self.serial_connection and self.is_connected:
            try:
                self.serial_connection.write(f"{command}\n".encode())
            except Exception as e:
                print(f"Command send error: {e}")
    
    def start_calibration(self):
        """Start calibration procedure"""
        if not self.is_connected:
            messagebox.showerror("Error", "Not connected to device")
            return
        
        self.is_calibrating = True
        self.calibrate_button.config(state=tk.DISABLED)
        self.stop_calibration_button.config(state=tk.NORMAL)
        self.progress_var.set(0)
        self.progress_label.config(text="0%")
        
        # Clear previous calibration data
        self.calibration_data = []
        
        # Send calibration command
        self.send_command("calibrate")
    
    def stop_calibration(self):
        """Stop calibration procedure"""
        self.is_calibrating = False
        self.calibrate_button.config(state=tk.NORMAL)
        self.stop_calibration_button.config(state=tk.DISABLED)
        self.progress_var.set(0)
        self.progress_label.config(text="0%")
        
        # Send stop command (if supported)
        self.send_command("stop")
    
    def clear_data(self):
        """Clear all data"""
        self.sensor_data = []
        self.calibration_data = []
        self.update_data_display()
        self.results_text.delete(1.0, tk.END)
    
    def export_data(self):
        """Export data to CSV file"""
        if not self.sensor_data:
            messagebox.showwarning("Warning", "No data to export")
            return
        
        filename = filedialog.asksaveasfilename(
            defaultextension=".csv",
            filetypes=[("CSV files", "*.csv"), ("All files", "*.*")]
        )
        
        if filename:
            try:
                with open(filename, 'w', newline='') as csvfile:
                    fieldnames = ['timestamp', 'raw_value', 'voltage', 'resistance', 'gas_ppm', 'alert_level', 'r0_value']
                    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                    
                    writer.writeheader()
                    for data in self.sensor_data:
                        writer.writerow({
                            'timestamp': data['timestamp'].isoformat(),
                            'raw_value': data['raw_value'],
                            'voltage': data['voltage'],
                            'resistance': data['resistance'],
                            'gas_ppm': data['gas_ppm'],
                            'alert_level': data['alert_level'],
                            'r0_value': data['r0_value']
                        })
                
                messagebox.showinfo("Success", f"Data exported to {filename}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to export data: {str(e)}")
    
    def plot_data(self):
        """Plot sensor data"""
        if not self.sensor_data:
            messagebox.showwarning("Warning", "No data to plot")
            return
        
        try:
            # Extract data for plotting
            timestamps = [data['timestamp'] for data in self.sensor_data]
            gas_ppm = [data['gas_ppm'] for data in self.sensor_data]
            alert_levels = [data['alert_level'] for data in self.sensor_data]
            
            # Create plot
            fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
            
            # Plot gas PPM
            ax1.plot(timestamps, gas_ppm, 'b-', linewidth=2, label='Gas PPM')
            ax1.set_ylabel('Gas PPM')
            ax1.set_title('BantayGas Sensor Data')
            ax1.grid(True)
            ax1.legend()
            
            # Plot alert levels
            ax2.plot(timestamps, alert_levels, 'r-', linewidth=2, label='Alert Level')
            ax2.set_ylabel('Alert Level')
            ax2.set_xlabel('Time')
            ax2.grid(True)
            ax2.legend()
            
            # Format x-axis
            plt.xticks(rotation=45)
            plt.tight_layout()
            plt.show()
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to plot data: {str(e)}")
    
    def save_calibration(self):
        """Save calibration data to file"""
        if not self.sensor_data:
            messagebox.showwarning("Warning", "No calibration data to save")
            return
        
        filename = filedialog.asksaveasfilename(
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if filename:
            try:
                # Get latest R0 value
                latest_data = self.sensor_data[-1]
                r0_value = latest_data['r0_value']
                
                calibration_data = {
                    'r0_value': r0_value,
                    'gas_type': self.gas_type_var.get(),
                    'calibration_date': datetime.now().isoformat(),
                    'samples': len(self.sensor_data),
                    'confidence': 95.0,  # Placeholder
                    'device_info': {
                        'model': 'BantayGas IoT',
                        'sensor': 'MQ-6',
                        'firmware_version': '1.0.0'
                    }
                }
                
                with open(filename, 'w') as jsonfile:
                    json.dump(calibration_data, jsonfile, indent=2)
                
                messagebox.showinfo("Success", f"Calibration data saved to {filename}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to save calibration: {str(e)}")
    
    def run(self):
        """Run the application"""
        self.root.mainloop()

def main():
    """Main function"""
    print("BantayGas IoT Calibration Tool")
    print("Version 1.0.0")
    print("Starting application...")
    
    app = BantayGasCalibrationTool()
    app.run()

if __name__ == "__main__":
    main()


