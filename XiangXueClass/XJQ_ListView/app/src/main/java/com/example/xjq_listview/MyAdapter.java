package com.example.xjq_listview;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import androidx.viewpager.widget.PagerTabStrip;

import java.util.List;

public class MyAdapter extends BaseAdapter {
    private List<Bean> data;
    private Context context;

    public MyAdapter(List<Bean> data, Context context) {
        this.data = data;
        this.context = context;
    }

    @Override
    public int getCount() {
        return data.size();
    }

    @Override
    public Object getItem(int i) {
        return null;
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @Override
    public View getView(int i, View view, ViewGroup parent) {
        ViewHolder viewHolder;
        if (view == null){//为了复用
            //获取自定义view
            viewHolder = new ViewHolder();
            view = LayoutInflater.from(context).inflate(R.layout.list_item, parent, false);
            viewHolder.textView = view.findViewById(R.id.tv);
            view.setTag(viewHolder);
        }else{
            viewHolder = (ViewHolder)view.getTag();
        }

        viewHolder.textView.setText(data.get(i).getName());

        Log.e("xjq", "getView: " + i);

        return view;
    }

    private final class ViewHolder{
        TextView textView;
    }
}
