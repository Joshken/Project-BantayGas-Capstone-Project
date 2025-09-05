package com.example.bantaygasp1.ui

import android.os.Bundle
import android.widget.ImageView
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class UserOverviewActivity : AppCompatActivity() {

    private lateinit var backButton: ImageView
    private lateinit var userName: TextView
    private lateinit var userRole: TextView
    private lateinit var userStatus: TextView
    private lateinit var fullName: TextView
    private lateinit var userEmail: TextView
    private lateinit var userPhone: TextView
    private lateinit var userAddress: TextView
    private lateinit var userId: TextView
    private lateinit var accountRole: TextView
    private lateinit var accountStatus: TextView
    private lateinit var joinDate: TextView
    private lateinit var lastActive: TextView
    private lateinit var loginCount: TextView
    private lateinit var actionsTaken: TextView
    private lateinit var reportsGenerated: TextView


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.dialog_user_overview)
        supportActionBar?.hide()

        initializeViews()
        setupClickListeners()
        loadUserData()
    }

    private fun initializeViews() {
        backButton = findViewById(R.id.backButton)
        userName = findViewById(R.id.userName)
        userRole = findViewById(R.id.userRole)
        userStatus = findViewById(R.id.userStatus)
        fullName = findViewById(R.id.fullName)
        userEmail = findViewById(R.id.userEmail)
        userPhone = findViewById(R.id.userPhone)
        userAddress = findViewById(R.id.userAddress)
        userId = findViewById(R.id.userId)
        accountRole = findViewById(R.id.accountRole)
        accountStatus = findViewById(R.id.accountStatus)
        joinDate = findViewById(R.id.joinDate)
        lastActive = findViewById(R.id.lastActive)
        loginCount = findViewById(R.id.loginCount)
        actionsTaken = findViewById(R.id.actionsTaken)
        reportsGenerated = findViewById(R.id.reportsGenerated)
    }

    private fun setupClickListeners() {
        backButton.setOnClickListener {
            finish()
        }
    }

    private fun loadUserData() {
        // Get user data from intent extras
        val userNameText = intent.getStringExtra("user_name") ?: "Christian Baguio"
        val userRoleText = intent.getStringExtra("user_role") ?: "Staff"
        val lastActiveText = intent.getStringExtra("last_active") ?: "6 hours ago"

        // Update UI with user data
        userName.text = userNameText
        userRole.text = userRoleText
        accountRole.text = userRoleText
        lastActive.text = lastActiveText

        // Set different data based on user
        when (userNameText) {
            "Christian Baguio" -> {
                fullName.text = "Christian Baguio Santos"
                userEmail.text = "christian.baguio@bantaygas.com"
                userPhone.text = "+63 912 345 6789"
                userAddress.text = "Manila, Philippines"
                userId.text = "USR-001"
                joinDate.text = "January 15, 2024"
                loginCount.text = "156 times"
                actionsTaken.text = "89 actions"
                reportsGenerated.text = "23 reports"
            }
            "Maria Santos" -> {
                fullName.text = "Maria Santos Rodriguez"
                userEmail.text = "maria.santos@bantaygas.com"
                userPhone.text = "+63 923 456 7890"
                userAddress.text = "Quezon City, Philippines"
                userId.text = "USR-002"
                joinDate.text = "December 10, 2023"
                loginCount.text = "234 times"
                actionsTaken.text = "156 actions"
                reportsGenerated.text = "45 reports"
            }
            "John Dela Cruz" -> {
                fullName.text = "John Dela Cruz Martinez"
                userEmail.text = "john.delacruz@bantaygas.com"
                userPhone.text = "+63 934 567 8901"
                userAddress.text = "Makati, Philippines"
                userId.text = "USR-003"
                joinDate.text = "November 5, 2023"
                loginCount.text = "189 times"
                actionsTaken.text = "123 actions"
                reportsGenerated.text = "34 reports"
            }
            "Ana Rodriguez" -> {
                fullName.text = "Ana Rodriguez Garcia"
                userEmail.text = "ana.rodriguez@bantaygas.com"
                userPhone.text = "+63 945 678 9012"
                userAddress.text = "Cebu City, Philippines"
                userId.text = "USR-004"
                joinDate.text = "October 20, 2023"
                loginCount.text = "145 times"
                actionsTaken.text = "78 actions"
                reportsGenerated.text = "19 reports"
            }
        }
    }
}
