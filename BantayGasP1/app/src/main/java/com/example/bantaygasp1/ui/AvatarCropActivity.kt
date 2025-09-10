package com.example.bantaygasp1.ui

import android.content.Intent
import android.graphics.Color
import android.net.Uri
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.FragmentTransaction
import com.yalantis.ucrop.UCrop
import com.yalantis.ucrop.UCropFragment
import com.yalantis.ucrop.UCropFragmentCallback
import com.example.bantaygasp1.R
import androidx.appcompat.widget.Toolbar
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat

class AvatarCropActivity : AppCompatActivity(), UCropFragmentCallback {

    private lateinit var source: Uri
    private var circleCrop: Boolean = true
    private lateinit var insetsController: WindowInsetsControllerCompat

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_avatar_crop)

        // Keep system bars visible (revert fullscreen change)
        WindowCompat.setDecorFitsSystemWindows(window, true)

        val btnClose: android.widget.ImageView = findViewById(R.id.btnClose)
        val btnDone: android.widget.TextView = findViewById(R.id.btnDone)
        btnClose.setOnClickListener { finish() }
        btnDone.setOnClickListener {
            val frag = supportFragmentManager.findFragmentById(R.id.cropContainer)
            if (frag is UCropFragment) {
                frag.cropAndSaveImage()
            }
        }

        val incoming = intent.getParcelableExtra<Uri>(EXTRA_SOURCE_URI)
        if (incoming == null) {
            Toast.makeText(this, "Missing image", Toast.LENGTH_SHORT).show()
            finish()
            return
        }
        source = incoming
        launchCropFragment(circle = true)

        // Default shape is circle; no toggle UI (simplified)
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    private fun launchCropFragment(circle: Boolean) {
        val destination = Uri.fromFile(java.io.File(cacheDir, "avatar_cropped_${System.currentTimeMillis()}.jpg"))

        val options = UCrop.Options().apply {
            setCircleDimmedLayer(circle)
            setShowCropGrid(false)
            setShowCropFrame(false)
            setCompressionQuality(90)
            setToolbarTitle("Edit Photo")
            setStatusBarColor(Color.parseColor("#1A237E"))
            setToolbarColor(Color.parseColor("#1A237E"))
            setToolbarWidgetColor(Color.WHITE)
            setRootViewBackgroundColor(Color.TRANSPARENT)
            setActiveControlsWidgetColor(Color.parseColor("#FF9800"))
            setDimmedLayerColor(Color.parseColor("#77000000"))
        }

        val uCrop = UCrop.of(source, destination)
            .withAspectRatio(1f, 1f)
            .withOptions(options)

        val fragment = UCropFragment.newInstance(uCrop.getIntent(this).extras!!)
        val tx: FragmentTransaction = supportFragmentManager.beginTransaction()
        tx.replace(R.id.cropContainer, fragment, "UCROP")
        tx.commit()
        fragment.setCallback(this)
    }

    override fun loadingProgress(showLoader: Boolean) {
        // no-op for now
    }

    override fun onCropFinish(result: UCropFragment.UCropResult) {
        if (result.mResultCode == RESULT_OK) {
            val resultUri = result.mResultData?.getParcelableExtra<Uri>(UCrop.EXTRA_OUTPUT_URI)
            val data = Intent().apply {
                putExtra(EXTRA_RESULT_URI, resultUri)
            }
            setResult(RESULT_OK, data)
        } else {
            setResult(UCrop.RESULT_ERROR, result.mResultData)
        }
        finish()
    }

    companion object {
        const val EXTRA_SOURCE_URI = "extra_source_uri"
        const val EXTRA_RESULT_URI = "extra_result_uri"
    }
}


