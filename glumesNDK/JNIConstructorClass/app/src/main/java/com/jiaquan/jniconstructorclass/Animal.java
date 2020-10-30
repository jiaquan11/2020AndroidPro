package com.jiaquan.jniconstructorclass;

import android.util.Log;

public class Animal {
    protected String name;

    public static int num = 0;

    public Animal(String name){
        this.name = name;
    }

    public String getName(){
        return this.name;
    }

    public static int getNum(){
        return num;
    }

    //C++调用Java的实例方法
    public void callInstanceMethod(int num){
        Log.i("Animal", "call instance method");
    }
}
