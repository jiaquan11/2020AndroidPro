package com.jiaquan.jnidynamicload;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final JNIDynamicLoad jniDynamicLoad = new JNIDynamicLoad();
        final TextView textView = findViewById(R.id.sample_text);
        textView.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                textView.setText(jniDynamicLoad.getNativeString());
            }
        });
    }
}