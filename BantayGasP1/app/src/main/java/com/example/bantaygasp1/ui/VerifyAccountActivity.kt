package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class VerifyAccountActivity : AppCompatActivity() {
    
    private lateinit var otpEditText1: EditText
    private lateinit var otpEditText2: EditText
    private lateinit var otpEditText3: EditText
    private lateinit var otpEditText4: EditText
    private lateinit var verifyButton: Button
    private lateinit var backButton: Button
    private lateinit var emailText: TextView
    private lateinit var resendText: TextView
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_verify_account)
        supportActionBar?.hide()
        
        initializeViews()
        setupClickListeners()
        setupOtpInputs()
        
        // Get email from intent
        val email = intent.getStringExtra("email") ?: ""
        emailText.text = "Enter the 4-digit code sent to\n$email"
    }
    
    private fun initializeViews() {
        otpEditText1 = findViewById(R.id.otpEditText1)
        otpEditText2 = findViewById(R.id.otpEditText2)
        otpEditText3 = findViewById(R.id.otpEditText3)
        otpEditText4 = findViewById(R.id.otpEditText4)
        verifyButton = findViewById(R.id.verifyButton)
        backButton = findViewById(R.id.backButton)
        emailText = findViewById(R.id.emailText)
        resendText = findViewById(R.id.resendText)
    }
    
    private fun setupClickListeners() {
        verifyButton.setOnClickListener {
            performVerification()
        }
        
        backButton.setOnClickListener {
            onBackPressed()
        }
        
        resendText.setOnClickListener {
            Toast.makeText(this, "New code sent!", Toast.LENGTH_SHORT).show()
        }
    }
    
    private fun setupOtpInputs() {
        // Auto-focus to next OTP input
        otpEditText1.addTextChangedListener(object : android.text.TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: android.text.Editable?) {
                if (s?.length == 1) {
                    otpEditText2.requestFocus()
                }
            }
        })
        
        otpEditText2.addTextChangedListener(object : android.text.TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: android.text.Editable?) {
                if (s?.length == 1) {
                    otpEditText3.requestFocus()
                }
            }
        })
        
        otpEditText3.addTextChangedListener(object : android.text.TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: android.text.Editable?) {
                if (s?.length == 1) {
                    otpEditText4.requestFocus()
                }
            }
        })
    }
    
    private fun performVerification() {
        val otp1 = otpEditText1.text.toString()
        val otp2 = otpEditText2.text.toString()
        val otp3 = otpEditText3.text.toString()
        val otp4 = otpEditText4.text.toString()
        
        // Check if all OTP fields are filled
        if (otp1.isEmpty() || otp2.isEmpty() || otp3.isEmpty() || otp4.isEmpty()) {
            Toast.makeText(this, "Please enter complete OTP", Toast.LENGTH_SHORT).show()
            return
        }
        
        val otp = otp1 + otp2 + otp3 + otp4
        
        // Temporary OTP validation (accept any 4-digit code)
        if (otp.length == 4 && otp.all { it.isDigit() }) {
            Toast.makeText(this, "OTP verified successfully!", Toast.LENGTH_SHORT).show()
            
            // Navigate to new password screen
            val intent = Intent(this, NewPasswordActivity::class.java)
            startActivity(intent)
        } else {
            Toast.makeText(this, "Invalid OTP", Toast.LENGTH_SHORT).show()
        }
    }
}


