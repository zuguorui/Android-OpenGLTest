package com.zu.opengltest

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.View
import android.view.ViewGroup
import com.zu.opengltest.databinding.FragmentSimpleTestBinding
import timber.log.Timber


class SimpleTestFragment : Fragment() {

    private lateinit var binding: FragmentSimpleTestBinding

    @GLTest.TestCase var testCase: Int = GLTest.TEST_COMPUTE_SHADER

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

    override fun onCreate(savedInstanceState: Bundle?) {
        Timber.d("onCreate")
        super.onCreate(savedInstanceState)
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        Timber.d("onCreateView")
        binding = FragmentSimpleTestBinding.inflate(layoutInflater, container, false)
        binding.surfaceView.holder.addCallback(surfaceCallback)
        return binding.root
    }

    private fun setupTest(width: Int, height: Int) {
        Timber.d("setupTest")
        val surface = binding.surfaceView.holder.surface
        when (testCase) {
            GLTest.TEST_INIT -> {
                GLTest.testInit(surface, width, height, requireContext().assets)
            }
            GLTest.TEST_COMPUTE_SHADER -> {
                GLTest.testComputeShader(surface, width, height, requireContext().assets)
            }
        }
    }

    override fun onDestroyView() {
        Timber.d("onDestroyView")
        GLTest.stopTest()
        super.onDestroyView()
    }

    override fun onDestroy() {
        Timber.d("onDestroy")
        super.onDestroy()
    }

    companion object {

    }
}