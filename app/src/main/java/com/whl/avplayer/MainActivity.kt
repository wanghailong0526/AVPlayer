package com.whl.avplayer

import android.graphics.Color
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.whl.avplayer.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private var mAVController: AVController? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val pathMp4: String = this.getExternalFilesDir("").toString().plus(File.separator).plus("test.mp4")
        Log.e("whl ***", pathMp4)

        
        mAVController = AVController(pathMp4)
        mAVController?.let { lifecycle.addObserver(it) }//mAvplayer 不为空 调用 let 里的方法
        mAVController?.setOnPrePareListener(object : AVController.OnPrepareListener {
            override fun onPrepare() {
                runOnUiThread {
                    binding.tvState.setTextColor(Color.GREEN) // 绿色
                    binding.tvState.text = "解封装成功"
                }
                mAVController?.play()// 调用 Native 开始播放
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