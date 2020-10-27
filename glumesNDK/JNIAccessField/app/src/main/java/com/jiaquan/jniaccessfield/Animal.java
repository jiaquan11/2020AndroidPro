package com.jiaquan.jniaccessfield;

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
        Log.i("Animal", "call instance method and num is " + num);
    }

    //C++调用Java的类方法
    public static String callStaticMethod(String str){
        if (str != null){
            Log.i("Animal", "call static method with " + str);
        }else{
            Log.i("Animal", "call static method str is null");
        }
        return "";
    }

    public static String callStaticMethod(String[] strs, int num){
        if (strs != null){
            for (String str : strs){
                Log.i("Animal", "str in array is" + str);
            }
        }
        return "";
    }
}
