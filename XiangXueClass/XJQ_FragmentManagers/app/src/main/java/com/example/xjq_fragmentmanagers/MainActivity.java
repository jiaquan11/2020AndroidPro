package com.example.xjq_fragmentmanagers;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button button = findViewById(R.id.btn);
        button.setOnClickListener(this);

        Button button2 = findViewById(R.id.btn_2);
        button2.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.btn:
                Bundle bundle = new Bundle();
                bundle.putString("message", "我喜欢享学课堂");
                BlankFragment1 bf = new BlankFragment1();
                bf.setArguments(bundle);

                bf.setFragmentCallback(new IFragmentCallback() {
                    @Override
                    public void sendMsgToActivity(String msg) {
                        Toast.makeText(MainActivity.this, msg, Toast.LENGTH_SHORT).show();;
                    }

                    @Override
                    public String getMsgFromActivity(String msg) {
                        return "hello, i am from activity";
                    }
                });

                replaceFragment(bf);
                break;
            case R.id.btn_2:
                replaceFragment(new ItemFragment());
                break;
        }
    }

    private void replaceFragment(Fragment fragment) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction transaction = fragmentManager.beginTransaction();
        transaction.replace(R.id.framelayout, fragment);
//        transaction.addToBackStack(null);
        transaction.commit();
    }
}