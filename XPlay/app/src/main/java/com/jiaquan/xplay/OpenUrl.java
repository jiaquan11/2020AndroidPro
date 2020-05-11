package com.jiaquan.xplay;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;

public class OpenUrl extends AppCompatActivity {
    private Button btfile = null;
    private Button btrtmp = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.openurl);
        btfile = findViewById(R.id.playvideo);
        btfile.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EditText t = findViewById(R.id.fileurl);
                //用户输入的url
                Open(t.getText().toString());
                //关闭当前窗口
                finish();
            }
        });

        btrtmp = findViewById(R.id.playrtmp);
        btrtmp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EditText t = findViewById(R.id.rtmpurl);
                //用户输入的url
                Open(t.getText().toString());
                //关闭当前窗口
                finish();
            }
        });
    }

    public native void Open(String url);
}
