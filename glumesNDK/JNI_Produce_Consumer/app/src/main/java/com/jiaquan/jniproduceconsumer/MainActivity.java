package com.jiaquan.jniproduceconsumer;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

import com.jiaquan.jni_produce_consumer.R;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JNIProduceConsumer jni_produce_consumer = new JNIProduceConsumer();

        findViewById(R.id.produceAndConsumer).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                jni_produce_consumer.startProductAndConsumer();
            }
        });
    }
}