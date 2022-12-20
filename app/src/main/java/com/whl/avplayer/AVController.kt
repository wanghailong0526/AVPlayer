package com.whl.avplayer

import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.lifecycle.*

/**
 *
 * @date
 * @author : wanghailong
 *
 * @description:
 *
 */
//constructor 可以省略
class AVController constructor(avPath: String) : LifecycleEventObserver, SurfaceHolder.Callback {
    //object 对象表达式或对象声明
    //companion object 表示静态方法
    companion object {
        init {
            System.loadLibrary("native-lib");
        }
    }

    interface OnPrepareListener {
        fun onPrepare()
    }

    interface OnErrorListener {
        fun onError(errorMsg: String)
    }

    //var_自动生成 get set 方法; val_自动生成 get 方法; private_修改的字段不会自动生成set get 方法
    private var mOnPrepareListener: OnPrepareListener? = null//c++层要通知上层时,调用此接口
    private var mOnErrorListener: OnErrorListener? = null //native 层 解封装出现错误的回调
    private var mNativeObj: Long? = null// native 层的 AVPlayer 地址
    private var mAVPath: String = avPath//视频路径或直播地址
    private var surfaceHolder: SurfaceHolder? = null;

    fun setAVPath(avPath: String) {
        mAVPath = avPath
    }

    fun getAVPath() {
        mAVPath
    }

    fun setOnPrePareListener(onPrepareListener: OnPrepareListener) {
        this.mOnPrepareListener = onPrepareListener
    }

    //native 准备好后调用此方法
    fun onPrepared() {
        mOnPrepareListener?.onPrepare();
    }

    fun setOnErrorListener(onErrorListener: OnErrorListener) {
        this.mOnErrorListener = onErrorListener;
    }

    fun onError(errorCode: Int, errorMsg: String) {
        mOnErrorListener?.let {
            var msg: String? = null
            when (errorCode) {
                FFMPGECODE.ERR_CAN_NOT_OPEN_MEDIA -> msg = "打开媒体文件失败:$errorMsg"
                FFMPGECODE.ERR_CAN_NOT_FIND_STREAM -> msg = "找不到媒体流:$errorMsg"
                FFMPGECODE.ERR_CAN_NOT_FIND_DECODER -> msg = "找不到解码器:$errorMsg"
                FFMPGECODE.ERR_AVCODEC_ALLOC_CONTEXT_FAIL -> msg = "创建解码器上下文失败:$errorMsg"
                FFMPGECODE.ERR_AVCODEC_PARAMETERS_TO_CONTEXT_FIAL -> msg =
                    "根据流信息配置解码器上下文失败:$errorMsg"
                FFMPGECODE.ERR_AVCODEC_OPEN_FAIL -> msg = "打开解码器失败:$errorMsg"
                FFMPGECODE.ERR_NO_AUDIO_VIDEO -> msg = "没有找到音频流和视频流:$errorMsg"
                else -> {
                    msg = "";
                }
            }
            it.onError(msg)
        }
    }

    //播放 av
    fun play() {
        mNativeObj?.let { play(it) }
    }

    //activity 生命周期回调
    override fun onStateChanged(source: LifecycleOwner, event: Lifecycle.Event) {
        when (event) {
            //activity 执行 onResume 方法时 执行 prepareNative 方法
            Lifecycle.Event.ON_RESUME -> {
//                mNativeObj = mAVPath?.let { prepareNative(it) }
            }
            Lifecycle.Event.ON_STOP -> {
                mNativeObj?.let { stop(it) }
            }
            Lifecycle.Event.ON_DESTROY -> {
                mNativeObj?.let { destory(it) }
            }
            else -> {

            }
        }
    }

    //点击播放
    fun clickPlay() {
        surfaceHolder?.let { setSurfaceNative(it.surface) }
        mNativeObj = mAVPath?.let { prepareNative(it) }
    }

    fun setSurfaceView(surfaceView: SurfaceView) {
        surfaceHolder?.removeCallback(this)
        surfaceHolder = surfaceView.holder
        surfaceHolder?.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        setSurfaceNative(holder.surface)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
    }


    private external fun prepareNative(avPath: String): Long
    private external fun play(nativeObj: Long)
    private external fun stop(nativeObj: Long)
    private external fun destory(nativeObj: Long)
    private external fun setSurfaceNative(surface: Surface)
}


