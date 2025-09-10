package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.ImageView
import android.widget.Spinner
import android.os.Handler
import android.os.Looper
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R
import com.google.android.material.bottomnavigation.BottomNavigationView

class SensorsActivity : AppCompatActivity() {

    private var backButton: ImageView? = null
    private var addSensorButton: Button? = null
    private var addFloorPlanButton: Button? = null
    private var scanSensorsButton: Button? = null
    private var threshold001EditText: EditText? = null
    private var threshold002EditText: EditText? = null
    private var editSensor001Button: Button? = null
    private var editSensor002Button: Button? = null
    private var removeSensor001Button: Button? = null
    private var removeSensor002Button: Button? = null
    private var bottomNavigationView: BottomNavigationView? = null
    private var scanProgress: View? = null

    // Track edit mode for each sensor
    private var isSensor001InEditMode = false
    private var isSensor002InEditMode = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_sensors)
        supportActionBar?.hide()

        try {
            initializeViews()
            setupClickListeners()
            setupNavigation()
            setViewMode() // Start in view mode
        } catch (e: Exception) {
            Toast.makeText(this, "Error initializing sensors screen", Toast.LENGTH_SHORT).show()
            finish()
        }
    }

    private fun initializeViews() {
        backButton = findViewById(R.id.backButton)
        addSensorButton = findViewById(R.id.addSensorButton)
        addFloorPlanButton = findViewById(R.id.addFloorPlanButton)
        scanSensorsButton = findViewById(R.id.scanSensorsButton)
        threshold001EditText = findViewById(R.id.threshold001EditText)
        threshold002EditText = findViewById(R.id.threshold002EditText)
        editSensor001Button = findViewById(R.id.editSensor001Button)
        editSensor002Button = findViewById(R.id.editSensor002Button)
        removeSensor001Button = findViewById(R.id.removeSensor001Button)
        removeSensor002Button = findViewById(R.id.removeSensor002Button)
        bottomNavigationView = findViewById(R.id.bottomNavigationView)
        scanProgress = findViewById(R.id.scanProgress)
    }

    private fun setupClickListeners() {
        backButton?.setOnClickListener {
            finish()
        }

        addSensorButton?.setOnClickListener {
            addNewSensor()
        }

        addFloorPlanButton?.setOnClickListener {
            addFloorPlan()
        }

        scanSensorsButton?.setOnClickListener {
            scanForSensors()
        }

        editSensor001Button?.setOnClickListener {
            toggleEditMode("Sensor_001", true)
        }

        editSensor002Button?.setOnClickListener {
            toggleEditMode("Sensor_002", false)
        }

        removeSensor001Button?.setOnClickListener {
            removeSensor("Sensor_001")
        }

        removeSensor002Button?.setOnClickListener {
            removeSensor("Sensor_002")
        }
    }

    private fun setupNavigation() {
        bottomNavigationView?.setOnItemSelectedListener { menuItem ->
            when (menuItem.itemId) {
                R.id.navigation_monitoring -> {
                    try {
                        if (bottomNavigationView?.selectedItemId == R.id.navigation_monitoring) return@setOnItemSelectedListener true
                        val intent = Intent(this, MonitoringActivity::class.java)
                        intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                        startActivity(intent)
                        // Don't call finish() - preserve activity stack
                    } catch (e: Exception) {
                        Toast.makeText(this, "Error navigating to monitoring", Toast.LENGTH_SHORT).show()
                    }
                    true
                }
                R.id.navigation_sensors -> true
                R.id.navigation_alerts -> {
                    try {
                        if (bottomNavigationView?.selectedItemId == R.id.navigation_alerts) return@setOnItemSelectedListener true
                        val intent = Intent(this, AlertHistoryActivity::class.java)
                        intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                        startActivity(intent)
                        // Don't call finish() - preserve activity stack
                    } catch (e: Exception) {
                        Toast.makeText(this, "Error navigating to alerts", Toast.LENGTH_SHORT).show()
                    }
                    true
                }
                R.id.navigation_users -> {
                    try {
                        if (bottomNavigationView?.selectedItemId == R.id.navigation_users) return@setOnItemSelectedListener true
                        val intent = Intent(this, UserManageActivity::class.java)
                        intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                        startActivity(intent)
                        // Don't call finish() - preserve activity stack
                    } catch (e: Exception) {
                        Toast.makeText(this, "Error navigating to user management", Toast.LENGTH_SHORT).show()
                    }
                    true
                }
                R.id.navigation_settings -> {
                    try {
                        if (bottomNavigationView?.selectedItemId == R.id.navigation_settings) return@setOnItemSelectedListener true
                        val intent = Intent(this, SettingsActivity::class.java)
                        intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                        startActivity(intent)
                        // Don't call finish() - preserve activity stack
                    } catch (e: Exception) {
                        Toast.makeText(this, "Error navigating to settings", Toast.LENGTH_SHORT).show()
                    }
                    true
                }
                else -> false
            }
        }
    }

    override fun onResume() {
        super.onResume()
        // Ensure correct highlight when activity is brought to front
        bottomNavigationView?.selectedItemId = R.id.navigation_sensors
    }

    private fun setViewMode() {
        try {
            // Make threshold fields non-editable
            threshold001EditText?.apply {
                isEnabled = false
                isFocusable = false
                isFocusableInTouchMode = false
            }
            
            threshold002EditText?.apply {
                isEnabled = false
                isFocusable = false
                isFocusableInTouchMode = false
            }

            // Reset edit mode flags
            isSensor001InEditMode = false
            isSensor002InEditMode = false

            // Update button texts
            editSensor001Button?.text = "EDIT"
            editSensor002Button?.text = "EDIT"
        } catch (e: Exception) {
            Toast.makeText(this, "Error setting view mode", Toast.LENGTH_SHORT).show()
        }
    }

    private fun toggleEditMode(sensorName: String, isSensor001: Boolean) {
        try {
            if (isSensor001) {
                if (isSensor001InEditMode) {
                    // Save changes and exit edit mode
                    saveThresholdChanges(sensorName, threshold001EditText?.text?.toString() ?: "")
                    setViewMode()
                    Toast.makeText(this, "$sensorName threshold updated!", Toast.LENGTH_SHORT).show()
                } else {
                    // Enter edit mode
                    isSensor001InEditMode = true
                    threshold001EditText?.apply {
                        isEnabled = true
                        isFocusable = true
                        isFocusableInTouchMode = true
                        requestFocus()
                    }
                    editSensor001Button?.text = "SAVE"
                    Toast.makeText(this, "Editing $sensorName threshold", Toast.LENGTH_SHORT).show()
                }
            } else {
                if (isSensor002InEditMode) {
                    // Save changes and exit edit mode
                    saveThresholdChanges(sensorName, threshold002EditText?.text?.toString() ?: "")
                    setViewMode()
                    Toast.makeText(this, "$sensorName threshold updated!", Toast.LENGTH_SHORT).show()
                } else {
                    // Enter edit mode
                    isSensor002InEditMode = true
                    threshold002EditText?.apply {
                        isEnabled = true
                        isFocusable = true
                        isFocusableInTouchMode = true
                        requestFocus()
                    }
                    editSensor002Button?.text = "SAVE"
                    Toast.makeText(this, "Editing $sensorName threshold", Toast.LENGTH_SHORT).show()
                }
            }
        } catch (e: Exception) {
            Toast.makeText(this, "Error toggling edit mode", Toast.LENGTH_SHORT).show()
        }
    }

    private fun saveThresholdChanges(sensorName: String, newThreshold: String) {
        try {
            // Here you would typically save the threshold to your data source
            // For now, we'll just update the UI
            when (sensorName) {
                "Sensor_001" -> threshold001EditText?.setText(newThreshold)
                "Sensor_002" -> threshold002EditText?.setText(newThreshold)
            }
        } catch (e: Exception) {
            Toast.makeText(this, "Error saving threshold changes", Toast.LENGTH_SHORT).show()
        }
    }

    private fun addNewSensor() {
        showAddSensorDialog()
    }
    
    private fun showAddSensorDialog() {
        try {
            val dialog = android.app.AlertDialog.Builder(this)
            val dialogView = layoutInflater.inflate(R.layout.dialog_add_sensor, null)
            
            // Initialize dialog views
            val sensorNameEditText = dialogView.findViewById<EditText>(R.id.sensorNameEditText)
            val sensorTypeSpinner = dialogView.findViewById<Spinner>(R.id.sensorTypeSpinner)
            val locationSpinner = dialogView.findViewById<Spinner>(R.id.locationSpinner)
            val thresholdEditText = dialogView.findViewById<EditText>(R.id.thresholdEditText)
            val cancelButton = dialogView.findViewById<TextView>(R.id.cancelButton)
            val addButton = dialogView.findViewById<TextView>(R.id.addButton)
            
            // Setup sensor type spinner
            val sensorTypes = arrayOf("Gas Detector", "Smoke Detector", "Temperature Sensor", "Humidity Sensor", "Motion Sensor")
            val adapter = android.widget.ArrayAdapter(this, android.R.layout.simple_spinner_item, sensorTypes)
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
            sensorTypeSpinner.adapter = adapter
            
            // Setup location spinner
            val locations = arrayOf("Room 101", "Room 102", "Kitchen", "Living Room", "Basement", "Office", "Hallway", "Storage")
            val locationAdapter = android.widget.ArrayAdapter(this, android.R.layout.simple_spinner_item, locations)
            locationAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
            locationSpinner.adapter = locationAdapter
            
            // Create and show dialog
            val alertDialog = dialog.create()
            alertDialog.setView(dialogView)
            
            // Setup click listeners
            cancelButton.setOnClickListener {
                alertDialog.dismiss()
            }
            
            addButton.setOnClickListener {
                val sensorName = sensorNameEditText.text.toString().trim()
                val sensorType = sensorTypeSpinner.selectedItem.toString()
                val location = locationSpinner.selectedItem.toString()
                val threshold = thresholdEditText.text.toString().trim()
                
                if (sensorName.isNotEmpty() && threshold.isNotEmpty()) {
                    // Here you would typically save the sensor to your data source
                    Toast.makeText(this, "Sensor '$sensorName' added successfully!", Toast.LENGTH_SHORT).show()
                    alertDialog.dismiss()
                } else {
                    Toast.makeText(this, "Please fill in all fields", Toast.LENGTH_SHORT).show()
                }
            }
            
            alertDialog.show()
            
        } catch (e: Exception) {
            Toast.makeText(this, "Error showing add sensor dialog: ${e.message}", Toast.LENGTH_SHORT).show()
        }
    }

    private fun addFloorPlan() {
        showAddFloorPlanDialog()
    }
    
    private fun showAddFloorPlanDialog() {
        try {
            val dialog = android.app.AlertDialog.Builder(this)
            val dialogView = layoutInflater.inflate(R.layout.dialog_add_floor_plan, null)
            
            // Initialize dialog views
            val floorPlanNameEditText = dialogView.findViewById<EditText>(R.id.floorPlanNameEditText)
            val buildingTypeSpinner = dialogView.findViewById<Spinner>(R.id.buildingTypeSpinner)
            val floorLevelSpinner = dialogView.findViewById<Spinner>(R.id.floorLevelSpinner)
            val descriptionEditText = dialogView.findViewById<EditText>(R.id.descriptionEditText)
            val cancelButton = dialogView.findViewById<TextView>(R.id.cancelButton)
            val addButton = dialogView.findViewById<TextView>(R.id.addButton)
            
            // Setup building type spinner
            val buildingTypes = arrayOf("Residential", "Commercial", "Industrial", "Educational", "Healthcare", "Office", "Mixed Use")
            val buildingAdapter = android.widget.ArrayAdapter(this, android.R.layout.simple_spinner_item, buildingTypes)
            buildingAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
            buildingTypeSpinner.adapter = buildingAdapter
            
            // Setup floor level spinner
            val floorLevels = arrayOf("Basement", "Ground Floor", "First Floor", "Second Floor", "Third Floor", "Fourth Floor", "Fifth Floor", "Roof")
            val floorAdapter = android.widget.ArrayAdapter(this, android.R.layout.simple_spinner_item, floorLevels)
            floorAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
            floorLevelSpinner.adapter = floorAdapter
            
            // Create and show dialog
            val alertDialog = dialog.create()
            alertDialog.setView(dialogView)
            
            // Setup click listeners
            cancelButton.setOnClickListener {
                alertDialog.dismiss()
            }
            
            addButton.setOnClickListener {
                val floorPlanName = floorPlanNameEditText.text.toString().trim()
                val buildingType = buildingTypeSpinner.selectedItem.toString()
                val floorLevel = floorLevelSpinner.selectedItem.toString()
                val description = descriptionEditText.text.toString().trim()
                
                if (floorPlanName.isNotEmpty()) {
                    // Here you would typically save the floor plan to your data source
                    Toast.makeText(this, "Floor Plan '$floorPlanName' added successfully!", Toast.LENGTH_SHORT).show()
                    alertDialog.dismiss()
                } else {
                    Toast.makeText(this, "Please enter a floor plan name", Toast.LENGTH_SHORT).show()
                }
            }
            
            alertDialog.show()
            
        } catch (e: Exception) {
            Toast.makeText(this, "Error showing add floor plan dialog: ${e.message}", Toast.LENGTH_SHORT).show()
        }
    }

    private fun scanForSensors() {
        try {
            scanSensorsButton?.isEnabled = false
            scanProgress?.visibility = View.VISIBLE
            Toast.makeText(this, "Scanning for sensors...", Toast.LENGTH_SHORT).show()

            Handler(Looper.getMainLooper()).postDelayed({
                // Simulate found sensors; in real app, trigger actual scan here
                scanProgress?.visibility = View.GONE
                scanSensorsButton?.isEnabled = true
                Toast.makeText(this, "Scan complete. 2 sensors found.", Toast.LENGTH_SHORT).show()
            }, 2000)
        } catch (e: Exception) {
            scanProgress?.visibility = View.GONE
            scanSensorsButton?.isEnabled = true
            Toast.makeText(this, "Scan failed: ${e.message}", Toast.LENGTH_SHORT).show()
        }
    }

    private fun removeSensor(sensorName: String) {
        Toast.makeText(this, "Remove $sensorName feature coming soon!", Toast.LENGTH_SHORT).show()
    }

    override fun onDestroy() {
        super.onDestroy()
        // Clean up references to prevent memory leaks
        backButton = null
        addSensorButton = null
        addFloorPlanButton = null
        scanSensorsButton = null
        threshold001EditText = null
        threshold002EditText = null
        editSensor001Button = null
        editSensor002Button = null
        removeSensor001Button = null
        removeSensor002Button = null
        bottomNavigationView = null
    }
}
