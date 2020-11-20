package com.jiaquan.jniaccessfield;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    public String[] strings = {"apple", "pear", "banana"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final JNIAccessField jniAccessField = new JNIAccessField();
        final Animal animal = new Animal("animal");

        findViewById(R.id.sample_text).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                jniAccessField.accessStaticField(animal);
//                jniAccessField.accessInstanceField(animal);
//                JNIAccessField.staticAccessInstanceField();
//                Log.i("MainActivity", "name is " + animal.getName());
//                Log.i("MainActivity", "num is " + Animal.getNum());
//                Log.i("MainActivity", "num is " + JNIAccessField.num);

                jniAccessField.accessInstanceMethod(animal);
                jniAccessField.accessStaticMethod(animal);
            }
        });
    }
}