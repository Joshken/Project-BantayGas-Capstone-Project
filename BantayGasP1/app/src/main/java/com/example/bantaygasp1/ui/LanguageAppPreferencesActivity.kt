package com.example.bantaygasp1.ui

import android.os.Bundle
import android.util.Log
import android.widget.ArrayAdapter
import android.widget.CheckBox
import android.widget.ImageView
import android.widget.RadioButton
import android.widget.Spinner
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class LanguageAppPreferencesActivity : AppCompatActivity() {

    private lateinit var backButton: ImageView
    private lateinit var languageSpinner: Spinner
    private lateinit var themeLightRadioButton: RadioButton
    private lateinit var themeDarkRadioButton: RadioButton
    private lateinit var themeAutoRadioButton: RadioButton
    private lateinit var emailNotificationsCheckBox: CheckBox
    private lateinit var pushNotificationsCheckBox: CheckBox
    private lateinit var smsAlertsCheckBox: CheckBox

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        try {
            setContentView(R.layout.activity_language_app_pref)
            supportActionBar?.hide()

            initializeViews()
            setupLanguageSpinner()
            loadPreferences()
            setupClickListeners()
            
            Log.d("LanguageAppPreferences", "onCreate completed successfully")
        } catch (e: Exception) {
            Log.e("LanguageAppPreferences", "Error in onCreate: ${e.message}", e)
            Toast.makeText(this, "Error loading preferences", Toast.LENGTH_LONG).show()
            finish()
        }
    }

    private fun initializeViews() {
        try {
            backButton = findViewById(R.id.backButton)
            Log.d("LanguageAppPreferences", "Back button found: ${backButton != null}")
            
            languageSpinner = findViewById(R.id.languageSpinner)
            themeLightRadioButton = findViewById(R.id.themeLightRadioButton)
            themeDarkRadioButton = findViewById(R.id.themeDarkRadioButton)
            themeAutoRadioButton = findViewById(R.id.themeAutoRadioButton)
            emailNotificationsCheckBox = findViewById(R.id.emailNotificationsCheckBox)
            pushNotificationsCheckBox = findViewById(R.id.pushNotificationsCheckBox)
            smsAlertsCheckBox = findViewById(R.id.smsAlertsCheckBox)
            
            Log.d("LanguageAppPreferences", "All views initialized successfully")
        } catch (e: Exception) {
            Log.e("LanguageAppPreferences", "Error initializing views: ${e.message}", e)
            throw e
        }
    }

    private fun setupLanguageSpinner() {
        try {
            val languages = arrayOf("English", "Spanish", "French", "German", "Chinese", "Japanese", "Korean", "Filipino")
            val adapter = ArrayAdapter(this, android.R.layout.simple_spinner_item, languages)
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
            languageSpinner.adapter = adapter
            Log.d("LanguageAppPreferences", "Language spinner setup completed")
        } catch (e: Exception) {
            Log.e("LanguageAppPreferences", "Error setting up language spinner: ${e.message}", e)
        }
    }

    private fun loadPreferences() {
        try {
            val sharedPrefs = getSharedPreferences("BantayGasPrefs", MODE_PRIVATE)

            // Load Language
            val savedLanguage = sharedPrefs.getString("appLanguage", "English")
            val adapter = languageSpinner.adapter
            if (adapter is ArrayAdapter<*>) {
                val languages = (0 until adapter.count).map { adapter.getItem(it) as String }
                val languagePosition = languages.indexOf(savedLanguage)
                if (languagePosition != -1) {
                    languageSpinner.setSelection(languagePosition)
                }
            }

            // Load Theme
            when (sharedPrefs.getString("appTheme", "Light")) {
                "Light" -> themeLightRadioButton.isChecked = true
                "Dark" -> themeDarkRadioButton.isChecked = true
                "Auto" -> themeAutoRadioButton.isChecked = true
            }

            // Load Notifications
            emailNotificationsCheckBox.isChecked = sharedPrefs.getBoolean("emailNotifications", true)
            pushNotificationsCheckBox.isChecked = sharedPrefs.getBoolean("pushNotifications", true)
            smsAlertsCheckBox.isChecked = sharedPrefs.getBoolean("smsAlerts", false)

            Log.d("LanguageAppPreferences", "Preferences loaded successfully")
        } catch (e: Exception) {
            Log.e("LanguageAppPreferences", "Error loading preferences: ${e.message}", e)
        }
    }

    private fun setupClickListeners() {
        try {
            backButton.setOnClickListener {
                Log.d("LanguageAppPreferences", "Back button clicked - finishing activity")
                finish()
            }
            
            Log.d("LanguageAppPreferences", "Click listeners setup completed")
        } catch (e: Exception) {
            Log.e("LanguageAppPreferences", "Error setting up click listeners: ${e.message}", e)
        }
    }

    private fun savePreferences() {
        try {
            val sharedPrefs = getSharedPreferences("BantayGasPrefs", MODE_PRIVATE)
            val editor = sharedPrefs.edit()

            // Save Language
            editor.putString("appLanguage", languageSpinner.selectedItem.toString())

            // Save Theme
            when {
                themeLightRadioButton.isChecked -> editor.putString("appTheme", "Light")
                themeDarkRadioButton.isChecked -> editor.putString("appTheme", "Dark")
                themeAutoRadioButton.isChecked -> editor.putString("appTheme", "Auto")
            }

            // Save Notifications
            editor.putBoolean("emailNotifications", emailNotificationsCheckBox.isChecked)
            editor.putBoolean("pushNotifications", pushNotificationsCheckBox.isChecked)
            editor.putBoolean("smsAlerts", smsAlertsCheckBox.isChecked)

            editor.apply()
            Log.d("LanguageAppPreferences", "Preferences saved successfully")
        } catch (e: Exception) {
            Log.e("LanguageAppPreferences", "Error saving preferences: ${e.message}", e)
        }
    }
}
