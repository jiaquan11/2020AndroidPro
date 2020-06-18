package com.cxp.learningvideo

import android.annotation.SuppressLint
import android.app.Application
import android.content.Context

@SuppressLint("StaticFieldLeak")
var CONTEXT: Context? = null

class MainApp: Application() {
    override fun onCreate() {
        super.onCreate()
        CONTEXT = this
    }
}
