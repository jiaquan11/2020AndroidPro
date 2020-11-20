package com.jiaquan.jniexception;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final JNIException jniException = new JNIException();
        findViewById(R.id.sample_text).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try{
                    jniException.nativeThrowException();
                }catch (IllegalArgumentException e){
                    Log.i("MainActivity", e.getMessage());
                }

                jniException.nativeInvokeJavaException();
            }
        });
    }
}