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
//            toActivity(GLTest.TEST_INIT)
        }
        binding.btnComputeShader.setOnClickListener {
            toFragment(GLTest.TEST_COMPUTE_SHADER)
//            toActivity(GLTest.TEST_COMPUTE_SHADER)
        }

    }

    private fun toFragment(@GLTest.TestCase testCase: Int) {
        val fragment = when (testCase) {
            GLTest.TEST_INIT,
            GLTest.TEST_COMPUTE_SHADER-> {
                simpleTestFragment.testCase = testCase
                simpleTestFragment
            }
            else -> return
        }

        requireActivity().supportFragmentManager.beginTransaction()
            .hide(this)
            .add(R.id.fragment_container, fragment)
            .addToBackStack(null)
            .commitAllowingStateLoss()
    }

    private fun toActivity(@GLTest.TestCase testCase: Int) {
        val intent = Intent(requireActivity(), SimpleTestActivity::class.java).apply {
            putExtra("test_case", testCase)
        }
        requireActivity().startActivity(intent)
    }


}