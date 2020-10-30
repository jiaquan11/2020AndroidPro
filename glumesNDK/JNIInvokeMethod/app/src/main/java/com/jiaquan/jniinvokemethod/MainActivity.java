package com.jiaquan.jniinvokemethod;

import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JNIInvokeMethod jniInvokeMethod = new JNIInvokeMethod();
        jniInvokeMethod.nativeCallback(new ICallbackMethod() {
            @Override
            public void callback() {
                Log.i("MainActivity", "thread name is: " + Thread.currentThread().getName());
            }
        });

        jniInvokeMethod.nativeThreadCallback(new IThreadCallback() {
            @Override
            public void callback() {
                Log.i("MainActivity", "thread name is: " + Thread.currentThread().getName());
            }
        });


//        findViewById(R.id.sample_text).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                jniInvokeMethod.nativeCallback(new ICallbackMethod() {
//                    @Override
//                    public void callback() {
//                        Log.i("MainActivity", "thread name is: " + Thread.currentThread().getName());
//                    }
//                });
//
//                jniInvokeMethod.nativeThreadCallback(new IThreadCallback() {
//                    @Override
//                    public void callback() {
//                        Log.i("MainActivity", "thread name is: " + Thread.currentThread().getName());
//                    }
//                });
//            }
//        });
    }
}