package com.example.opengltest

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.ViewGroup
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {


    private lateinit var previewSurface: SurfaceView
    private var previewCallback: SurfaceHolder.Callback = object : SurfaceHolder.Callback{
        override fun surfaceChanged(p0: SurfaceHolder?, p1: Int, p2: Int, p3: Int) {
            nResetSize(p2, p3)
        }

        override fun surfaceDestroyed(p0: SurfaceHolder?) {

        }

        override fun surfaceCreated(p0: SurfaceHolder?) {
            nInit()
            nSetSurface(p0!!.surface)
        }
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        initSurfaceView()

    }

    private fun initSurfaceView()
    {
        previewSurface = SurfaceView(this)
        var surfaceHolder: SurfaceHolder = previewSurface.holder
        surfaceHolder.addCallback(previewCallback)
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS)
        var layoutParams = ViewGroup.LayoutParams(400, 800)
        layoutParams.width = ViewGroup.LayoutParams.MATCH_PARENT
        layoutParams.height = ViewGroup.LayoutParams.MATCH_PARENT
        previewSurface.layoutParams = layoutParams
        surface_container.addView(previewSurface)
    }

    override fun onResume() {
        super.onResume()

    }

    override fun onStop() {
        super.onStop()
//        nStop()
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
    external fun nInit(): Boolean
    external fun nSetSurface(surfaceView: Any)
    external fun nResetSize(width: Int, height: Int)
    external fun nStop()

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
