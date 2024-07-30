package com.zu.opengltest

import android.Manifest
import android.graphics.Color
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import android.widget.TextView
import androidx.activity.SystemBarStyle
import androidx.activity.enableEdgeToEdge
import androidx.core.view.OnApplyWindowInsetsListener
import androidx.core.view.ViewCompat
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import androidx.core.view.updateLayoutParams
import com.google.android.material.internal.EdgeToEdgeUtils
import com.permissionx.guolindev.PermissionX
import com.zu.opengltest.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        // 可以让内容扩展到system bar区域，达到全屏显示的效果。
        enableEdgeToEdge()
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        window.navigationBarColor = 0x60000000.toInt()
        window.statusBarColor = 0x60000000.toInt()


        // 控制system bars隐藏及外观
        WindowCompat.getInsetsController(window, window.decorView).apply {
            hide(WindowInsetsCompat.Type.navigationBars())
            //hide(WindowInsetsCompat.Type.statusBars())
            // 控制状态栏色彩。light代表适配浅色界面，那么状态栏文本就是深色的
            // isAppearanceLightStatusBars = false
            // 控制system bars的行为。什么时候隐藏的bars会出现。这里是如果在bar区域滑动，则短暂出现，然后消失。
            systemBarsBehavior = WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        }

        // 使用衬区以防止内容被system bars遮挡
        ViewCompat.setOnApplyWindowInsetsListener(binding.fragmentContainer
        ) { v, windowInsets ->
            // 这里可以获取各种insets类型。这里获取的是system bars。
            // 也可以获取刘海屏displayCutout，或者系统手势衬区systemGestures。这样可以避免内容被系统某些区域遮挡
            // 或者手势与系统手势冲突。
            val insets = windowInsets.getInsets(WindowInsetsCompat.Type.systemBars())

            // Apply the insets as a margin to the view. This solution sets
            // only the bottom, left, and right dimensions, but you can apply whichever
            // insets are appropriate to your layout. You can also update the view padding
            // if that's more appropriate.

            v.updateLayoutParams<ViewGroup.MarginLayoutParams> {
                topMargin = insets.top
            }

            // Return CONSUMED if you don't want want the window insets to keep passing
            // down to descendant views.
            WindowInsetsCompat.CONSUMED
        }

        PermissionX.init(this).permissions(
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
        ).request(null)

        supportFragmentManager.beginTransaction()
            .add(R.id.fragment_container, TestCaseListFragment())
            .commitNowAllowingStateLoss()
    }

}