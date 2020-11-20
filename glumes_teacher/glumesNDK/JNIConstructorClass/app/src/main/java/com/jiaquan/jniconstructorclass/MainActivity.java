package com.jiaquan.jniconstructorclass;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JNIConstructorClass jniConstructorClass = new JNIConstructorClass();
        Log.i("MainActivity", "name is " + jniConstructorClass.allocObjectConstructor().getName());

        Log.i("MainActivity", "name is " + jniConstructorClass.invokeAnimalConstructors().getName());
    }
}