package com.cxp.learningvideo

import android.Manifest
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // 要申请的权限
        val permissions = arrayOf(
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
            ,
            Manifest.permission.ACCESS_NETWORK_STATE,
            Manifest.permission.CHANGE_NETWORK_STATE
        )

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, 321)
        }
    }

    //简单播放器
    fun clickSimplePlayer(view: View) {
        startActivity(Intent(this, SimplePlayerActivity::class.java))
    }

    //绘制简单三角形
    fun clickSimpleTriangle(view: View) {
        val intent = Intent(this, SimpleRenderActivity::class.java)
        intent.putExtra("type", 0)
        startActivity(intent)
    }

    //绘制简单纹理贴图
    fun clickSimpleTexture(view: View) {
        val intent = Intent(this, SimpleRenderActivity::class.java)
        intent.putExtra("type", 1)
        startActivity(intent)
    }

    fun clickOpenGLPlayer(view: View?) {
        startActivity(Intent(this, OpenGLPlayerActivity::class.java))
    }

    fun clickMultiOpenGLPlayer(view: View?) {
        startActivity(Intent(this, MultiOpenGLPlayerActivity::class.java))
    }

    fun clickEGLPlayer(view: View?) {
        startActivity(Intent(this, EGLPlayerActivity::class.java))
    }

    fun clickSoulPlayer(view: View?) {
        startActivity(Intent(this, SoulPlayerActivity::class.java))
    }

    fun clickEncoder(view: View?) {
        startActivity(Intent(this, SynthesizerActivity::class.java))
    }

    fun clickFFmpegInfo(view: View?) {
        startActivity(Intent(this, FFmpegActivity::class.java))
    }
}
