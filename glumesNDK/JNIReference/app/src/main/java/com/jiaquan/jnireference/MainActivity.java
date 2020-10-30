package com.jiaquan.jnireference;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final JNIReference jniReference = new JNIReference();

        findViewById(R.id.sample_text).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                jniReference.errorCacheLocalReference();
                jniReference.cacheWithGlobalReference();
                jniReference.useWeakGlobalReference();
            }
        });
    }
}