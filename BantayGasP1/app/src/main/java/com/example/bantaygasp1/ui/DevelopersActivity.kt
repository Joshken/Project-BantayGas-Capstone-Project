package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import com.example.bantaygasp1.R

class DevelopersActivity : AppCompatActivity() {

    private var backButton: ImageView? = null
    private var developer1Card: CardView? = null
    private var developer2Card: CardView? = null
    private var developer3Card: CardView? = null
    private var developer4Card: CardView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_developers)

        try {
            initializeViews()
            setupClickListeners()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    private fun initializeViews() {
        backButton = findViewById(R.id.backButton)
        developer1Card = findViewById(R.id.developer1Card)
        developer2Card = findViewById(R.id.developer2Card)
        developer3Card = findViewById(R.id.developer3Card)
        developer4Card = findViewById(R.id.developer4Card)
    }

    private fun setupClickListeners() {
        backButton?.setOnClickListener {
            onBackPressed()
        }
        
        developer1Card?.setOnClickListener {
            val intent = Intent(this, DeveloperBaguioActivity::class.java)
            startActivity(intent)
        }
        
        developer2Card?.setOnClickListener {
            val intent = Intent(this, DeveloperBarabatActivity::class.java)
            startActivity(intent)
        }
        
        developer3Card?.setOnClickListener {
            val intent = Intent(this, DeveloperMalicayActivity::class.java)
            startActivity(intent)
        }
        
        developer4Card?.setOnClickListener {
            val intent = Intent(this, DeveloperMamaliasActivity::class.java)
            startActivity(intent)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        backButton = null
        developer1Card = null
        developer2Card = null
        developer3Card = null
        developer4Card = null
    }
}
