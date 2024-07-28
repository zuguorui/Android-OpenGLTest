package com.zu.opengltest

import android.content.res.AssetManager
import androidx.annotation.IntDef

object GLTest {

    const val TEST_INIT = 0
    const val TEST_COMPUTE_SHADER = 1

    @IntDef(
        TEST_INIT,
        TEST_COMPUTE_SHADER
    )
    @Retention(AnnotationRetention.SOURCE)
    annotation class TestCase

    external fun testInit(surface: Any, width: Int, height: Int, assetManager: AssetManager)
    external fun testComputeShader(surface: Any, width: Int, height: Int, assetManager: AssetManager)
    external fun stopTest()

    init {
        System.loadLibrary("opengltest")
    }
}