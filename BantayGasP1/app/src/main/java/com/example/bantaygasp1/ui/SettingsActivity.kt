package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.EditText
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import com.example.bantaygasp1.R
import com.google.android.material.bottomnavigation.BottomNavigationView

class SettingsActivity : AppCompatActivity() {

    private lateinit var searchEditText: EditText
    private lateinit var profileInfoLayout: LinearLayout
    private lateinit var languageLayout: LinearLayout
    private lateinit var helpSupportLayout: LinearLayout
    private lateinit var privacyPolicyLayout: LinearLayout
    private lateinit var termsServiceLayout: LinearLayout
    private lateinit var developersLayout: LinearLayout
    private lateinit var logoutButton: androidx.appcompat.widget.AppCompatButton
    private lateinit var bottomNavigationView: BottomNavigationView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_settings)
        supportActionBar?.hide()

        initializeViews()
        setupClickListeners()
        setupNavigation()
    }

    private fun initializeViews() {
        searchEditText = findViewById(R.id.searchEditText)
        profileInfoLayout = findViewById(R.id.profileInfoLayout)
        languageLayout = findViewById(R.id.languageLayout)
        helpSupportLayout = findViewById(R.id.helpSupportLayout)
        privacyPolicyLayout = findViewById(R.id.privacyPolicyLayout)
        termsServiceLayout = findViewById(R.id.termsServiceLayout)
        developersLayout = findViewById(R.id.developersLayout)
        logoutButton = findViewById(R.id.logoutButton)
        bottomNavigationView = findViewById(R.id.bottomNavigationView)
    }

    private fun setupClickListeners() {
        // backButton removed from layout; no-op
        
        profileInfoLayout.setOnClickListener {
            val intent = Intent(this, ProfileInformationActivity::class.java)
            startActivity(intent)
        }
        
        languageLayout.setOnClickListener {
            val intent = Intent(this, LanguageAppPreferencesActivity::class.java)
            startActivity(intent)
        }
        
        helpSupportLayout.setOnClickListener {
            val intent = Intent(this, HelpSupportActivity::class.java)
            startActivity(intent)
        }
        
        privacyPolicyLayout.setOnClickListener {
            Toast.makeText(this, "Privacy Policy coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        termsServiceLayout.setOnClickListener {
            Toast.makeText(this, "Terms of Service coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        developersLayout.setOnClickListener {
            val intent = Intent(this, DevelopersActivity::class.java)
            startActivity(intent)
        }
        
        logoutButton.setOnClickListener {
            // Clear user preferences
            val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
            sharedPrefs.edit().clear().apply()

            // Clear session flag
            SessionManager(this).setLoggedIn(false)

            // Show logout message
            Toast.makeText(this, "Logged out successfully!", Toast.LENGTH_SHORT).show()

            // Navigate back to login screen
            val intent = Intent(this, LoginActivity::class.java)
            intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
            startActivity(intent)
            finish()
        }
    }

    private fun setupNavigation() {
        bottomNavigationView.setOnItemSelectedListener { item ->
            when (item.itemId) {
                R.id.navigation_monitoring -> {
                    if (bottomNavigationView.selectedItemId == R.id.navigation_monitoring) return@setOnItemSelectedListener true
                    val intent = Intent(this, MonitoringActivity::class.java)
                    intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_sensors -> {
                    if (bottomNavigationView.selectedItemId == R.id.navigation_sensors) return@setOnItemSelectedListener true
                    val intent = Intent(this, SensorsActivity::class.java)
                    intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_alerts -> {
                    if (bottomNavigationView.selectedItemId == R.id.navigation_alerts) return@setOnItemSelectedListener true
                    val intent = Intent(this, AlertHistoryActivity::class.java)
                    intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_users -> {
                    if (bottomNavigationView.selectedItemId == R.id.navigation_users) return@setOnItemSelectedListener true
                    val intent = Intent(this, UserManageActivity::class.java)
                    intent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT)
                    startActivity(intent)
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_settings -> {
                    // Already on settings screen
                    true
                }
                else -> false
            }
        }
    }

    override fun onResume() {
        super.onResume()
        // Ensure correct highlight when activity is brought to front
        bottomNavigationView.selectedItemId = R.id.navigation_settings
    }
}
