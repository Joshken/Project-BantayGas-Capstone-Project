package com.example.bantaygasp1.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.ImageButton
import android.widget.ImageView
import android.widget.Spinner
import android.widget.TextView
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R
import com.google.android.material.bottomnavigation.BottomNavigationView
import android.content.Intent
import android.widget.Toast

class UserManageActivity : AppCompatActivity() {

    private lateinit var backButton: ImageView
    private lateinit var searchEditText: EditText
    private lateinit var roleSpinner: Spinner
    private lateinit var bottomNavigationView: BottomNavigationView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_user_manage)
        supportActionBar?.hide()

        initializeViews()
        setupListeners()
        setupNavigation()
    }

    private fun initializeViews() {
        backButton = findViewById(R.id.backButton)
        searchEditText = findViewById(R.id.searchEditText)
        roleSpinner = findViewById(R.id.roleSpinner)
        bottomNavigationView = findViewById(R.id.bottomNavigationView)
    }

    private fun setupListeners() {
        backButton.setOnClickListener { finish() }
        
        // Setup dialog listeners for action buttons
        setupDialogListeners()
    }
    
    private fun setupNavigation() {
        // Set the selected item first
        bottomNavigationView.selectedItemId = R.id.navigation_users
        
        bottomNavigationView.setOnItemSelectedListener { item ->
            when (item.itemId) {
                R.id.navigation_monitoring -> {
                    startActivity(Intent(this, MonitoringActivity::class.java))
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_sensors -> {
                    startActivity(Intent(this, SensorsActivity::class.java))
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_alerts -> {
                    startActivity(Intent(this, AlertHistoryActivity::class.java))
                    // Don't call finish() - preserve activity stack
                    true
                }
                R.id.navigation_users -> {
                    // Already on users screen
                    true
                }
                R.id.navigation_settings -> {
                    startActivity(Intent(this, SettingsActivity::class.java))
                    // Don't call finish() - preserve activity stack
                    true
                }
                else -> false
            }
        }
        
        // Ensure the users item is checked
        bottomNavigationView.post {
            bottomNavigationView.selectedItemId = R.id.navigation_users
            bottomNavigationView.menu.findItem(R.id.navigation_users)?.isChecked = true
        }
    }
    
    private fun setupDialogListeners() {
        // Set up click listeners for all user rows
        setupUserRow1Listeners()
        setupUserRow2Listeners()
        setupUserRow3Listeners()
        setupUserRow4Listeners()
    }
    
    private fun setupUserRow1Listeners() {
        val deleteButton1 = findViewById<ImageButton>(R.id.deleteButton1)
        val roleButton1 = findViewById<ImageButton>(R.id.roleButton1)
        val resetButton1 = findViewById<ImageButton>(R.id.resetButton1)
        val userName1 = findViewById<TextView>(R.id.userName1)
        
        deleteButton1?.setOnClickListener {
            showDeactivateDialog("Christian Baguio")
        }
        
        roleButton1?.setOnClickListener {
            showChangeRoleDialog("Christian Baguio")
        }
        
        resetButton1?.setOnClickListener {
            showResetPasswordDialog("Christian Baguio")
        }
        
        userName1?.setOnClickListener {
            showUserOverview("Christian Baguio", "Staff", "6 hours ago")
        }
    }
    
    private fun setupUserRow2Listeners() {
        val deleteButton2 = findViewById<ImageButton>(R.id.deleteButton2)
        val roleButton2 = findViewById<ImageButton>(R.id.roleButton2)
        val resetButton2 = findViewById<ImageButton>(R.id.resetButton2)
        val userName2 = findViewById<TextView>(R.id.userName2)
        
        deleteButton2?.setOnClickListener {
            showDeactivateDialog("Maria Santos")
        }
        
        roleButton2?.setOnClickListener {
            showChangeRoleDialog("Maria Santos")
        }
        
        resetButton2?.setOnClickListener {
            showResetPasswordDialog("Maria Santos")
        }
        
        userName2?.setOnClickListener {
            showUserOverview("Maria Santos", "Admin", "2 hours ago")
        }
    }
    
    private fun setupUserRow3Listeners() {
        val deleteButton3 = findViewById<ImageButton>(R.id.deleteButton3)
        val roleButton3 = findViewById<ImageButton>(R.id.roleButton3)
        val resetButton3 = findViewById<ImageButton>(R.id.resetButton3)
        val userName3 = findViewById<TextView>(R.id.userName3)
        
        deleteButton3?.setOnClickListener {
            showDeactivateDialog("John Dela Cruz")
        }
        
        roleButton3?.setOnClickListener {
            showChangeRoleDialog("John Dela Cruz")
        }
        
        resetButton3?.setOnClickListener {
            showResetPasswordDialog("John Dela Cruz")
        }
        
        userName3?.setOnClickListener {
            showUserOverview("John Dela Cruz", "Manager", "1 day ago")
        }
    }
    
    private fun setupUserRow4Listeners() {
        val deleteButton4 = findViewById<ImageButton>(R.id.deleteButton4)
        val roleButton4 = findViewById<ImageButton>(R.id.roleButton4)
        val resetButton4 = findViewById<ImageButton>(R.id.resetButton4)
        val userName4 = findViewById<TextView>(R.id.userName4)
        
        deleteButton4?.setOnClickListener {
            showDeactivateDialog("Ana Rodriguez")
        }
        
        roleButton4?.setOnClickListener {
            showChangeRoleDialog("Ana Rodriguez")
        }
        
        resetButton4?.setOnClickListener {
            showResetPasswordDialog("Ana Rodriguez")
        }
        
        userName4?.setOnClickListener {
            showUserOverview("Ana Rodriguez", "Staff", "3 days ago")
        }
    }
    
    private fun showUserOverview(userName: String, userRole: String, lastActive: String) {
        val intent = Intent(this, UserOverviewActivity::class.java)
        intent.putExtra("user_name", userName)
        intent.putExtra("user_role", userRole)
        intent.putExtra("last_active", lastActive)
        startActivity(intent)
    }


    
    private fun showDeactivateDialog(userName: String) {
        val dialogView = LayoutInflater.from(this).inflate(R.layout.dialog_deactivate_account, null)
        val messageText = dialogView.findViewById<TextView>(R.id.messageText)
        val deactivateButton = dialogView.findViewById<Button>(R.id.deactivateButton)
        val cancelButton = dialogView.findViewById<Button>(R.id.cancelButton)
        
        messageText.text = "Do you want to Deactivate $userName's Account?"
        
        val dialog = AlertDialog.Builder(this)
            .setView(dialogView)
            .create()
        
        deactivateButton.setOnClickListener {
            Toast.makeText(this, "$userName's account has been deactivated", Toast.LENGTH_SHORT).show()
            dialog.dismiss()
        }
        
        cancelButton.setOnClickListener {
            dialog.dismiss()
        }
        
        dialog.show()
    }
    
    private fun showChangeRoleDialog(userName: String) {
        val dialogView = LayoutInflater.from(this).inflate(R.layout.dialog_change_role, null)
        val messageText = dialogView.findViewById<TextView>(R.id.messageText)
        val managerRoleButton = dialogView.findViewById<Button>(R.id.managerRoleButton)
        val staffRoleButton = dialogView.findViewById<Button>(R.id.staffRoleButton)
        val confirmButton = dialogView.findViewById<Button>(R.id.confirmButton)
        val cancelButton = dialogView.findViewById<Button>(R.id.cancelButton)
        
        messageText.text = "Do you want to change $userName's Role?"
        
        var selectedRole = "Manager"
        
        managerRoleButton.setOnClickListener {
            selectedRole = "Manager"
            managerRoleButton.setBackgroundResource(R.drawable.button_bg)
            managerRoleButton.setTextColor(resources.getColor(android.R.color.white, theme))
            staffRoleButton.setBackgroundResource(R.drawable.button_outline)
            staffRoleButton.setTextColor(resources.getColor(android.R.color.black, theme))
        }
        
        staffRoleButton.setOnClickListener {
            selectedRole = "Staff"
            staffRoleButton.setBackgroundResource(R.drawable.button_bg)
            staffRoleButton.setTextColor(resources.getColor(android.R.color.white, theme))
            managerRoleButton.setBackgroundResource(R.drawable.button_outline)
            managerRoleButton.setTextColor(resources.getColor(android.R.color.black, theme))
        }
        
        val dialog = AlertDialog.Builder(this)
            .setView(dialogView)
            .create()
        
        confirmButton.setOnClickListener {
            Toast.makeText(this, "$userName's role has been changed to $selectedRole", Toast.LENGTH_SHORT).show()
            dialog.dismiss()
        }
        
        cancelButton.setOnClickListener {
            dialog.dismiss()
        }
        
        dialog.show()
    }
    
    private fun showResetPasswordDialog(userName: String) {
        val dialogView = LayoutInflater.from(this).inflate(R.layout.dialog_reset_password, null)
        val messageText = dialogView.findViewById<TextView>(R.id.messageText)
        val sendCodeButton = dialogView.findViewById<Button>(R.id.sendCodeButton)
        val cancelButton = dialogView.findViewById<Button>(R.id.cancelButton)
        
        messageText.text = "$userName wants to Reset Password"
        
        val dialog = AlertDialog.Builder(this)
            .setView(dialogView)
            .create()
        
        sendCodeButton.setOnClickListener {
            Toast.makeText(this, "Reset code has been sent to $userName", Toast.LENGTH_SHORT).show()
            dialog.dismiss()
        }
        
        cancelButton.setOnClickListener {
            dialog.dismiss()
        }
        
        dialog.show()
    }
}
