package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.animation.AnimatorSet
import android.animation.ObjectAnimator
import android.view.View
import android.view.animation.AnimationUtils
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.splashscreen.SplashScreen.Companion.installSplashScreen
import com.example.bantaygasp1.R

class SplashActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // Android 12 SplashScreen API
        val splash = installSplashScreen()
        supportActionBar?.hide()
        setContentView(R.layout.activity_splash)

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
        val sp1 = findViewById<ImageView>(R.id.sparkle1)
        val sp2 = findViewById<ImageView>(R.id.sparkle2)
        val sp3 = findViewById<ImageView>(R.id.sparkle3)
        val blobBlue = findViewById<ImageView>(R.id.blobBlue)
        val blobCyan = findViewById<ImageView>(R.id.blobCyan)
        val shimmer = findViewById<View>(R.id.shimmerSweep)
        val ripple = findViewById<ImageView>(R.id.rippleRing)

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
        // Fade in blobs then drift slowly
        blobBlue.animate().alpha(1f).setDuration(600).setStartDelay(200).start()
        blobBlue.animate().translationXBy(24f).translationYBy(12f).setDuration(5000).start()
        blobCyan.animate().alpha(1f).setDuration(600).setStartDelay(400).start()
        blobCyan.animate().translationXBy(-18f).translationYBy(-16f).setDuration(5000).start()

        // Gentle parallax drift for sparkles
        fun drift(view: ImageView, dx: Float, dy: Float, startDelay: Long) {
            view.alpha = 0f
            view.translationX = 0f
            view.translationY = 0f
            view.animate().alpha(0.9f).setDuration(600).setStartDelay(startDelay).start()
            view.postDelayed({
                view.animate()
                    .translationXBy(dx)
                    .translationYBy(dy)
                    .setDuration(2600)
                    .withEndAction {
                        view.animate().alpha(0f).setDuration(400).start()
                    }
                    .start()
            }, startDelay + 200)
        }
        // Slightly faster on dark mode to keep motion subtle
        val isDark = (resources.configuration.uiMode and android.content.res.Configuration.UI_MODE_NIGHT_MASK) == android.content.res.Configuration.UI_MODE_NIGHT_YES
        if (isDark) {
            drift(sp1, 28f, 12f, 300)
            drift(sp2, -24f, 18f, 520)
            drift(sp3, 20f, -14f, 780)
        } else {
            drift(sp1, 40f, 18f, 400)
            drift(sp2, -36f, 26f, 700)
            drift(sp3, 28f, -22f, 1000)
        }

        // Slow logo rotation for depth
        ObjectAnimator.ofFloat(logoImageView, "rotation", 0f, 2.5f).apply {
            duration = 5000
            start()
        }

        // Shimmer sweep across logo title
        shimmer.postDelayed({
            shimmer.alpha = 1f
            shimmer.translationX = -shimmer.width.toFloat()
            shimmer.animate().translationX(logoImageView.width.toFloat())
                .setDuration(500).withEndAction { shimmer.alpha = 0f }.start()
        }, 900)

        // Background gradient crossfade midway
        val root = findViewById<View>(android.R.id.content)
        root.postDelayed({
            root.setBackgroundResource(R.drawable.bg_splash_grad_2)
            root.alpha = 0f
            root.animate().alpha(1f).setDuration(800).start()
        }, 2200)

        // Ripple at mid timeline
        ripple.postDelayed({
            ripple.alpha = 1f
            ripple.scaleX = 0.6f
            ripple.scaleY = 0.6f
            ripple.animate().scaleX(1.4f).scaleY(1.4f).alpha(0f).setDuration(700).start()
        }, 1600)

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

        // Respect reduce-motion setting: shorten to 800ms and skip heavy effects
        val animatorScale = android.provider.Settings.Global.getFloat(contentResolver,
            android.provider.Settings.Global.ANIMATOR_DURATION_SCALE, 1f)

        logoImageView.postDelayed({
            // Smooth exit: particle-like glow pulse + circular reveal from logo, then navigate
            val root = findViewById<View>(android.R.id.content)
            val overlay = findViewById<View>(R.id.revealOverlay)

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

            // 1) Subtle scale+glow pulse
            val scaleX = ObjectAnimator.ofFloat(logoImageView, "scaleX", 1f, 1.2f)
            val scaleY = ObjectAnimator.ofFloat(logoImageView, "scaleY", 1f, 1.2f)
            val glowPulse = ObjectAnimator.ofFloat(findViewById<ImageView>(R.id.logoGlow), "alpha", 0.2f, 1f)
            AnimatorSet().apply {
                duration = 280
                playTogether(scaleX, scaleY, glowPulse)
                start()
            }

            // 2) Circular reveal overlay expanding from logo center
            overlay.visibility = View.VISIBLE
            val location = IntArray(2)
            logoImageView.getLocationOnScreen(location)
            val cx = (location[0] + logoImageView.width / 2)
            val cy = (location[1] + logoImageView.height / 2) - statusBarHeight()
            val finalRadius = Math.hypot(root.width.toDouble(), root.height.toDouble()).toFloat()
            val reveal = android.view.ViewAnimationUtils.createCircularReveal(overlay, cx, cy, 0f, finalRadius)
            reveal.duration = 420
            reveal.start()

            overlay.postDelayed({
                handler.removeCallbacksAndMessages(null)
                startActivity(Intent(this, LoginActivity::class.java))
                overridePendingTransition(android.R.anim.fade_in, android.R.anim.fade_out)
                finish()
            }, if (animatorScale == 0f) 200 else 420)
        }, if (animatorScale == 0f) 800 else 5000)
    }

    private fun statusBarHeight(): Int {
        val resId = resources.getIdentifier("status_bar_height", "dimen", "android")
        return if (resId > 0) resources.getDimensionPixelSize(resId) else 0
    }
}


