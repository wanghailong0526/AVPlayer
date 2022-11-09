package com.whl.avplayer

import android.graphics.Color
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.whl.avplayer.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private var mAVController: AVController? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        // TODO 记得添加 av 路径
        mAVController = AVController("")
        mAVController?.let { lifecycle.addObserver(it) }//mAvplayer 不为空 调用 let 里的方法
        mAVController?.setOnPrePareListener(object : AVController.OnPrepareListener {
            override fun onPrepare() {
                runOnUiThread {
                    // Toast.makeText(this@MainActivity, "准备成功，即将开始播放", Toast.LENGTH_SHORT).show();
                    binding.tvState.setTextColor(Color.GREEN) // 绿色
                    binding.tvState.text = "解封装成功"
                }
//                mAVController?.play()// 调用C++ 开始播放
            }
        })
        mAVController?.setOnErrorListener(object : AVController.OnErrorListener {
            override fun onError(errorMsg: String) {
                runOnUiThread {
                    binding.tvState.setTextColor(Color.RED)
                    binding.tvState.text = errorMsg
                }
            }
        })
    }
}