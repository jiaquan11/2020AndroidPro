package com.jiaquan.jnithreadsynchronize;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JNIThreadSYN jniThreadSYN = new JNIThreadSYN();

        findViewById(R.id.wait).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                jniThreadSYN.waitNativeThread();
            }
        });

        findViewById(R.id.notify).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                jniThreadSYN.notifyNativeThread();
            }
        });
    }
}