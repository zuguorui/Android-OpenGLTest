package com.zu.opengltest

import android.content.res.AssetManager
import androidx.annotation.IntDef

object GLTest {

    // 和native-lib.cpp中的定义保持一致。
    const val TEST_INIT = 0
    const val TEST_COMPUTE_SHADER = 1

    @IntDef(
        TEST_INIT,
        TEST_COMPUTE_SHADER
    )
    @Retention(AnnotationRetention.SOURCE)
    annotation class TestCase

    external fun startTest(@TestCase testCase: Int, surface: Any, width: Int, height: Int, assetManager: AssetManager): Boolean
    external fun stopTest()

    init {
        System.loadLibrary("opengltest")
    }
}