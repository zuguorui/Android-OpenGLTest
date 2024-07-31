package com.zu.opengltest

import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.zu.opengltest.databinding.FragmentTestCaseListBinding

class TestCaseListFragment: Fragment() {

    private lateinit var binding: FragmentTestCaseListBinding

    private val simpleTestFragment = SimpleTestFragment()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        binding = FragmentTestCaseListBinding.inflate(layoutInflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        binding.btnInitTest.setOnClickListener {
            toFragment(GLTest.TEST_INIT)
        }
        binding.btnComputeShader.setOnClickListener {
            toFragment(GLTest.TEST_COMPUTE_SHADER)
        }
        binding.btnRenderVideoFrame.setOnClickListener {
            toFragment(GLTest.TEST_RENDER_VIDEO_FRAME)
        }
    }

    private fun toFragment(@GLTest.TestCase testCase: Int) {
        val fragment = when (testCase) {
            GLTest.TEST_INIT,
            GLTest.TEST_COMPUTE_SHADER,
            GLTest.TEST_RENDER_VIDEO_FRAME -> {
                simpleTestFragment.testCase = testCase
                simpleTestFragment
            }
            else -> return
        }

        requireActivity().supportFragmentManager.beginTransaction()
            .setCustomAnimations(
                R.anim.slide_in,
                R.anim.fade_out,
                R.anim.fade_in,
                R.anim.slide_out
            )
            .hide(this)
            .add(R.id.fragment_container, fragment)
            .addToBackStack(null)
            .commitAllowingStateLoss()
    }



}