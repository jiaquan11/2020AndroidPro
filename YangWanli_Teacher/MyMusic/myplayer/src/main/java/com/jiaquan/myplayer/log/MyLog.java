package com.jiaquan.myplayer.log;

import android.util.Log;

public class MyLog {
    public static void i(String msg) {
        Log.i("MYPLAYER", msg);
    }

    public static void e(String msg) {
        Log.e("MYPLAYER", msg);
    }
}
