package com.example.bantaygasp1.ui

import android.app.Application

class BantayGasApp : Application() {
    override fun onCreate() {
        super.onCreate()
        isColdStart = true
    }

    companion object {
        @JvmStatic
        var isColdStart: Boolean = true
    }
}


