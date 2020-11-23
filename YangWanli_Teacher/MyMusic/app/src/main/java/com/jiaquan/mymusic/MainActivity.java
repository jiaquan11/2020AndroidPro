package com.jiaquan.mymusic;

import android.os.Bundle;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.myplayer.Demo;

public class MainActivity extends AppCompatActivity {
    private Demo demo = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        demo = new Demo();

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(demo.stringFromJNI());

//        demo.testFFmpeg();
    }
}