package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class MonitoringNoFloorPlanActivity : AppCompatActivity() {
    
    private lateinit var uploadFloorPlanButton: Button
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_monitoring_no_floor_plan)
        supportActionBar?.hide()
        
        initializeViews()
    }
    
    private fun initializeViews() {
        uploadFloorPlanButton = findViewById(R.id.uploadFloorPlanButton)
        
        uploadFloorPlanButton.setOnClickListener {
            uploadFloorPlan()
        }
        
        uploadFloorPlanButton.setOnLongClickListener {
            resetFloorPlanStatus()
            true
        }
    }
    
    private fun uploadFloorPlan() {
        android.widget.Toast.makeText(this, "Floor plan uploaded successfully!", android.widget.Toast.LENGTH_SHORT).show()
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
        sharedPrefs.edit().putBoolean("hasFloorPlan", true).apply()
        val intent = Intent(this, MonitoringActivity::class.java)
        startActivity(intent)
        finish()
    }
    
    private fun resetFloorPlanStatus() {
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
        sharedPrefs.edit().putBoolean("hasFloorPlan", false).apply()
        android.widget.Toast.makeText(this, "Floor plan status reset! Login again to see no floor plan screen.", android.widget.Toast.LENGTH_LONG).show()
    }
}
