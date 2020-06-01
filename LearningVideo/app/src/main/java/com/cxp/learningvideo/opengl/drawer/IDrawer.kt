package com.cxp.learningvideo.opengl.drawer

import android.graphics.SurfaceTexture

/**
 * 渲染器
 */
interface IDrawer {
    fun setVideoSize(videoW: Int, videoH: Int)
    fun setWorldSize(worldW: Int, worldH: Int)
    fun setAlpha(alpha: Float)
    fun draw()
    fun setTextureID(id: Int)
    fun getSurfaceTexture(cb: (st: SurfaceTexture) -> Unit) {}
    fun release()
}