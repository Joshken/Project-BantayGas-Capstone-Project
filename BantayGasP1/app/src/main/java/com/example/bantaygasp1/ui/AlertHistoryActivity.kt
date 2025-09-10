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
        bottomNavigationView?.setOnItemSelectedListener { menuItem ->
            when (menuItem.itemId) {
                R.id.navigation_monitoring -> {
                    if (bottomNavigationView?.selectedItemId == R.id.navigation_monitoring) return@setOnItemSelectedListener true
                    val intent = Intent(this, MonitoringActivity::class.java)
                    intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_sensors -> {
                    if (bottomNavigationView?.selectedItemId == R.id.navigation_sensors) return@setOnItemSelectedListener true
                    val intent = Intent(this, SensorsActivity::class.java)
                    intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_alerts -> {
                    // Already on alerts page
                    true
                }
                R.id.navigation_users -> {
                    if (bottomNavigationView?.selectedItemId == R.id.navigation_users) return@setOnItemSelectedListener true
                    val intent = Intent(this, UserManageActivity::class.java)
                    intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_settings -> {
                    if (bottomNavigationView?.selectedItemId == R.id.navigation_settings) return@setOnItemSelectedListener true
                    val intent = Intent(this, SettingsActivity::class.java)
                    intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                else -> false
            }
        }
    }

    override fun onResume() {
        super.onResume()
        // Ensure correct highlight when activity is brought to front
        bottomNavigationView?.selectedItemId = R.id.navigation_alerts
    }

    override fun onDestroy() {
        super.onDestroy()
        backButton = null
        exportButton = null
        bottomNavigationView = null
    }
}
