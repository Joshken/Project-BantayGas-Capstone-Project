package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.EditText
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R
import com.google.android.material.floatingactionbutton.FloatingActionButton

class HelpSupportActivity : AppCompatActivity() {

    private lateinit var backButton: ImageView
    private lateinit var searchEditText: EditText
    private lateinit var setupLayout: LinearLayout
    private lateinit var alertsLayout: LinearLayout
    private lateinit var reportingLayout: LinearLayout
    private lateinit var maintenanceLayout: LinearLayout
    private lateinit var submitTicketButton: androidx.appcompat.widget.AppCompatButton
    private lateinit var contactSupportButton: androidx.appcompat.widget.AppCompatButton
    private lateinit var userManualLayout: LinearLayout
    private lateinit var troubleshootingLayout: LinearLayout
    private lateinit var safetyVideoLayout: LinearLayout
    private lateinit var chatFab: FloatingActionButton

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_help_support)
        supportActionBar?.hide()

        initializeViews()
        setupClickListeners()
    }

    private fun initializeViews() {
        backButton = findViewById(R.id.backButton)
        searchEditText = findViewById(R.id.searchEditText)
        setupLayout = findViewById(R.id.setupLayout)
        alertsLayout = findViewById(R.id.alertsLayout)
        reportingLayout = findViewById(R.id.reportingLayout)
        maintenanceLayout = findViewById(R.id.maintenanceLayout)
        submitTicketButton = findViewById(R.id.submitTicketButton)
        contactSupportButton = findViewById(R.id.contactSupportButton)
        userManualLayout = findViewById(R.id.userManualLayout)
        troubleshootingLayout = findViewById(R.id.troubleshootingLayout)
        safetyVideoLayout = findViewById(R.id.safetyVideoLayout)
        chatFab = findViewById(R.id.chatFab)
        

    }

    private fun setupClickListeners() {
        backButton.setOnClickListener { finish() }
        
        setupLayout.setOnClickListener {
            Toast.makeText(this, "Setup guide coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        alertsLayout.setOnClickListener {
            Toast.makeText(this, "Alerts help coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        reportingLayout.setOnClickListener {
            Toast.makeText(this, "Reporting help coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        maintenanceLayout.setOnClickListener {
            Toast.makeText(this, "Maintenance guide coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        submitTicketButton.setOnClickListener {
            Toast.makeText(this, "Submit ticket functionality coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        contactSupportButton.setOnClickListener {
            Toast.makeText(this, "Contact support functionality coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        userManualLayout.setOnClickListener {
            Toast.makeText(this, "User Manual PDF download coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        troubleshootingLayout.setOnClickListener {
            Toast.makeText(this, "Troubleshooting guide coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        safetyVideoLayout.setOnClickListener {
            Toast.makeText(this, "Safety Protocol Video coming soon!", Toast.LENGTH_SHORT).show()
        }
        
        chatFab.setOnClickListener {
            Toast.makeText(this, "Chat support coming soon!", Toast.LENGTH_SHORT).show()
        }
    }
}
