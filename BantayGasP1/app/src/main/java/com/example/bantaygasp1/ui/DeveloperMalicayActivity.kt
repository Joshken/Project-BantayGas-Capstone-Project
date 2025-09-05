package com.example.bantaygasp1.ui

import android.os.Bundle
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class DeveloperMalicayActivity : AppCompatActivity() {

    private var backButton: ImageView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_developer_malicay)

        try {
            initializeViews()
            setupClickListeners()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    private fun initializeViews() {
        backButton = findViewById(R.id.backButton)
    }

    private fun setupClickListeners() {
        backButton?.setOnClickListener {
            onBackPressed()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        backButton = null
    }
}
