package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.widget.FrameLayout
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R
import com.google.android.material.bottomnavigation.BottomNavigationView

class MonitoringActivity : AppCompatActivity() {
    
    private lateinit var contentContainer: FrameLayout
    private lateinit var bottomNavigationView: BottomNavigationView
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_monitoring)
        supportActionBar?.hide()
        
        initializeViews()
        loadContentBasedOnFloorPlanStatus()
        setupNavigation()
    }
    
    private fun initializeViews() {
        contentContainer = findViewById(R.id.contentContainer)
        bottomNavigationView = findViewById(R.id.bottomNavigationView)
    }
    
    private fun loadContentBasedOnFloorPlanStatus() {
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
        val hasFloorPlan = sharedPrefs.getBoolean("hasFloorPlan", false)
        
        if (hasFloorPlan) {
            // Load floor plan content
            val floorPlanView = LayoutInflater.from(this).inflate(R.layout.activity_monitoring_floorplan, contentContainer, false)
            contentContainer.addView(floorPlanView)
            
            // Initialize interactive floor plan
            val interactiveFloorPlan = floorPlanView.findViewById<InteractiveFloorPlanView>(R.id.interactiveFloorPlan)
            interactiveFloorPlan.setFloorPlanImage(R.drawable.sample_floorplan)
        } else {
            // Load no floor plan content (default for first-time users)
            val noFloorPlanView = LayoutInflater.from(this).inflate(R.layout.activity_monitoring_no_floor_plan, contentContainer, false)
            contentContainer.addView(noFloorPlanView)
            
            // Setup button click listeners for no floor plan content
            val uploadButton = noFloorPlanView.findViewById<android.widget.Button>(R.id.uploadFloorPlanButton)
            uploadButton.setOnClickListener {
                uploadFloorPlan()
            }
            uploadButton.setOnLongClickListener {
                resetFloorPlanStatus()
                true
            }
        }
    }
    
    private fun uploadFloorPlan() {
        android.widget.Toast.makeText(this, "Floor plan uploaded successfully!", android.widget.Toast.LENGTH_SHORT).show()
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
        sharedPrefs.edit().putBoolean("hasFloorPlan", true).apply()
        
        // Reload content to show floor plan
        contentContainer.removeAllViews()
        loadContentBasedOnFloorPlanStatus()
    }
    
    private fun resetFloorPlanStatus() {
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
        sharedPrefs.edit().putBoolean("hasFloorPlan", false).apply()
        android.widget.Toast.makeText(this, "Floor plan status reset! Refresh to see no floor plan screen.", android.widget.Toast.LENGTH_LONG).show()
        
        // Reload content to show no floor plan
        contentContainer.removeAllViews()
        loadContentBasedOnFloorPlanStatus()
    }
    
    private fun setupNavigation() {
        bottomNavigationView.selectedItemId = R.id.navigation_monitoring
        
        bottomNavigationView.setOnItemSelectedListener { menuItem ->
            when (menuItem.itemId) {
                R.id.navigation_monitoring -> {
                    true
                }
                R.id.navigation_sensors -> {
                    val intent = Intent(this, SensorsActivity::class.java)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_alerts -> {
                    val intent = Intent(this, AlertHistoryActivity::class.java)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_users -> {
                    val intent = Intent(this, UserManageActivity::class.java)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_settings -> {
                    val intent = Intent(this, SettingsActivity::class.java)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                else -> false
            }
        }
    }
}
