package com.jiaquan.jnistring;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        final JNIString jniString = new JNIString();
        final TextView textView = findViewById(R.id.sample_text);
        textView.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                textView.setText(jniString.callNativeString("this is Java String"));
                jniString.stringMethod("this is Java String");
            }
        });
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
}