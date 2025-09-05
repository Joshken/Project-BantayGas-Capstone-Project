package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R
import com.google.android.material.bottomnavigation.BottomNavigationView

class AlertHistoryActivity : AppCompatActivity() {

    private var backButton: ImageView? = null
    private var exportButton: Button? = null
    private var bottomNavigationView: BottomNavigationView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_alert_history)
        supportActionBar?.hide()

        try {
            initializeViews()
            setupClickListeners()
            setupBottomNavigation()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    private fun initializeViews() {
        backButton = findViewById(R.id.backButton)
        exportButton = findViewById(R.id.exportButton)
        bottomNavigationView = findViewById(R.id.bottomNavigationView)
    }

    private fun setupClickListeners() {
        backButton?.setOnClickListener {
            onBackPressed()
        }

        exportButton?.setOnClickListener {
            // TODO: Implement export functionality
            // This could open a dialog to choose export format or directly export
        }
    }

    private fun setupBottomNavigation() {
        // Set the alerts item as selected
        bottomNavigationView?.selectedItemId = R.id.navigation_alerts
        
        bottomNavigationView?.setOnItemSelectedListener { menuItem ->
            when (menuItem.itemId) {
                R.id.navigation_monitoring -> {
                    val intent = Intent(this, MonitoringActivity::class.java)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_sensors -> {
                    val intent = Intent(this, SensorsActivity::class.java)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_alerts -> {
                    // Already on alerts page
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

    override fun onDestroy() {
        super.onDestroy()
        backButton = null
        exportButton = null
        bottomNavigationView = null
    }
}
