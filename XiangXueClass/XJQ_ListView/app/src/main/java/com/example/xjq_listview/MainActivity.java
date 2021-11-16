package com.example.xjq_listview;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private List<Bean> data = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        for (int i = 0; i < 100; i++){
            Bean bean = new Bean();
            bean.setName("享学" + i);
            data.add(bean);
        }

        ListView listView = findViewById(R.id.lv);
        //将链表数据放入BaseAdapter中，用于显示。BaseAdapter是作为一个数据存储辅助类
        listView.setAdapter(new MyAdapter(data, this));

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                Log.e("xjq", "onItemClick: " + i);
            }
        });
    }
}