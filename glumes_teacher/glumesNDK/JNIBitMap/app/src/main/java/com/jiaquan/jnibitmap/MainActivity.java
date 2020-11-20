package com.jiaquan.jnibitmap;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final ImageView imageView = findViewById(R.id.bitmap);
        final Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.testimage);
        JNIBitmap jniBitmap = new JNIBitmap();
        imageView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Bitmap result = jniBitmap.callNativeMirrorBitmap(bitmap);
                imageView.setImageBitmap(result);
            }
        });

    }

}