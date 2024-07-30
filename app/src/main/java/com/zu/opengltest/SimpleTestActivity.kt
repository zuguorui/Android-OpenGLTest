package com.zu.opengltest

import android.os.Bundle
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.ViewGroup
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.zu.opengltest.databinding.ActivitySimpleTestBinding
import timber.log.Timber

class SimpleTestActivity : AppCompatActivity() {

    private val surfaceCallback = object : SurfaceHolder.Callback {
        override fun surfaceCreated(holder: SurfaceHolder) {
            Timber.d("surface created")
        }

        override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
            Timber.d("surface changed")
            setupTest(width, height)
        }

        override fun surfaceDestroyed(holder: SurfaceHolder) {
            Timber.d("surface destroyed")
        }
    }

    private lateinit var binding: ActivitySimpleTestBinding
    @GLTest.TestCase private var testCase: Int = GLTest.TEST_INIT
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivitySimpleTestBinding.inflate(layoutInflater)
        setContentView(binding.root)
        testCase = intent.getIntExtra("test_case", GLTest.TEST_INIT)
        binding.surfaceView.holder.addCallback(surfaceCallback)

    }

    override fun onStart() {
        super.onStart()
    }

    override fun onStop() {
        GLTest.stopTest()
        super.onStop()
    }
    override fun onDestroy() {
        super.onDestroy()
    }


    private fun setupTest(width: Int, height: Int) {
        Timber.d("setupTest")
        val surface = binding.surfaceView.holder.surface
        when (testCase) {
            GLTest.TEST_INIT -> {
                GLTest.testInit(surface, width, height, assets)
            }
            GLTest.TEST_COMPUTE_SHADER -> {
                GLTest.testComputeShader(surface, width, height, assets)
            }
        }
    }
}