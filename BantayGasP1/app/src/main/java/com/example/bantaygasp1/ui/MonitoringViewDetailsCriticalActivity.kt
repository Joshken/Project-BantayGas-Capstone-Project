package com.example.bantaygasp1.ui

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class MonitoringViewDetailsCriticalActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_monitoring_view_details_critical)
        supportActionBar?.hide()
    }
}
