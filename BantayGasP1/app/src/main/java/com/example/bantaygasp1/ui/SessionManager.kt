package com.example.bantaygasp1.ui

import android.content.Context

class SessionManager(context: Context) {

    private val prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)

    fun isLoggedIn(): Boolean {
        return prefs.getBoolean(KEY_IS_LOGGED_IN, false)
    }

    fun setLoggedIn(loggedIn: Boolean) {
        prefs.edit().putBoolean(KEY_IS_LOGGED_IN, loggedIn).apply()
    }

    companion object {
        private const val PREFS_NAME = "bantaygas_prefs"
        private const val KEY_IS_LOGGED_IN = "is_logged_in"
    }
}


