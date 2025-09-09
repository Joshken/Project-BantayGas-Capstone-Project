package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.animation.AnimatorSet
import android.animation.ObjectAnimator
import android.graphics.Color
import android.view.animation.AnimationUtils
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class SplashActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_splash)
        supportActionBar?.hide()

        // Force logout only on cold start (when app process was terminated)
        if (BantayGasApp.isColdStart) {
            kotlin.runCatching {
                SessionManager(this).setLoggedIn(false)
                val legacy = getSharedPreferences("BantayGasPrefs", MODE_PRIVATE)
                legacy.edit().putBoolean("isLoggedIn", false).remove("currentUser").apply()
            }
            BantayGasApp.isColdStart = false
        }

        val logoImageView = findViewById<ImageView>(R.id.logoImageView)
        val logoGlow = findViewById<ImageView>(R.id.logoGlow)
        val title = findViewById<android.widget.TextView>(R.id.appTitle)
        val subtitle = findViewById<android.widget.TextView>(R.id.appSubtitle)
        val loadingText = findViewById<android.widget.TextView>(R.id.loadingText)

        // Stage 1: fade+scale logo + glow pulse
        val fadeScale = AnimationUtils.loadAnimation(this, R.anim.fade_scale_in)
        logoImageView.startAnimation(fadeScale)
        val glowPulse = AnimatorSet().apply {
            val glowIn = ObjectAnimator.ofFloat(logoGlow, "alpha", 0f, 1f)
            glowIn.duration = 600
            val glowOut = ObjectAnimator.ofFloat(logoGlow, "alpha", 1f, 0.2f)
            glowOut.duration = 600
            playSequentially(glowIn, glowOut)
            startDelay = 200
        }
        glowPulse.start()

        // Stage 2: elevate logo slightly and fade in texts
        logoImageView.postDelayed({
            val lift = ObjectAnimator.ofFloat(logoImageView, "translationY", 0f, -40f)
            lift.duration = 400

            val titleAlpha = ObjectAnimator.ofFloat(title, "alpha", 0f, 1f)
            titleAlpha.duration = 350
            val titleTrans = ObjectAnimator.ofFloat(title, "translationY", 12f, 0f)
            titleTrans.duration = 350

            val subtitleAlpha = ObjectAnimator.ofFloat(subtitle, "alpha", 0f, 1f)
            subtitleAlpha.startDelay = 80
            subtitleAlpha.duration = 350
            val subtitleTrans = ObjectAnimator.ofFloat(subtitle, "translationY", 12f, 0f)
            subtitleTrans.startDelay = 80
            subtitleTrans.duration = 350

            AnimatorSet().apply {
                playTogether(lift, titleAlpha, titleTrans, subtitleAlpha, subtitleTrans)
                start()
            }
        }, 650)

        // No ring behind logo anymore

        logoImageView.postDelayed({
            // Crossfade splash out, then fade into next activity
            val root = findViewById<android.view.View>(android.R.id.content)

            // Animate loadingText dots: "Loading", "Loading.", "Loading..", "Loading..."
            val handler = android.os.Handler(mainLooper)
            val states = arrayOf("LOADING", "LOADING.", "LOADING..", "LOADING...")
            var idx = 0
            val runnable = object : Runnable {
                override fun run() {
                    loadingText.text = states[idx % states.size]
                    idx++
                    handler.postDelayed(this, 300)
                }
            }
            handler.post(runnable)

            root.animate()
                .alpha(0f)
                .setDuration(500)
                .withEndAction {
                    handler.removeCallbacksAndMessages(null)
                    startActivity(Intent(this, LoginActivity::class.java))
                    overridePendingTransition(android.R.anim.fade_in, android.R.anim.fade_out)
                    finish()
                }
                .start()
        }, 5000)
    }
}


