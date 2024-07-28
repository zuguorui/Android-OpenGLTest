package com.zu.opengltest

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
//            simpleTestFragment.testCase = GLTest.TEST_INIT
            val fragment = SimpleTestFragment()
            fragment.testCase = GLTest.TEST_INIT
            toFragment(fragment)
        }
        binding.btnComputeShader.setOnClickListener {
            //simpleTestFragment.testCase = GLTest.TEST_COMPUTE_SHADER
            val fragment = SimpleTestFragment()
            fragment.testCase = GLTest.TEST_COMPUTE_SHADER
            toFragment(fragment)
        }

    }

    private fun toFragment(fragment: Fragment) {
        requireActivity().supportFragmentManager.beginTransaction()
            .hide(this)
            .add(R.id.fragment_container, fragment)
            .addToBackStack(null)
            .commitAllowingStateLoss()
    }


}