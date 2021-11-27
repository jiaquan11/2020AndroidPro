package com.example.xjq_fragmentmanagers;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

public class BlankFragment1 extends Fragment {
    private static String TAG = "BlankFragment1";

    View rootview;

    public BlankFragment1() {
        // Required empty public constructor
    }

    private IFragmentCallback fragmentCallback;
    public void setFragmentCallback(IFragmentCallback callback){
        fragmentCallback = callback;
    }

    @Override
    public void onAttach(@NonNull Context context) {
        super.onAttach(context);
        Log.i(TAG, "onAttach: ");
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Bundle bundle = this.getArguments();
        String string = bundle.getString("message");
        Log.i(TAG, "onCreate: " + string);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        if (rootview == null){
           rootview = inflater.inflate(R.layout.fragment_blank1, container, false);
        }

        Button btn = rootview.findViewById(R.id.btn_3);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                fragmentCallback.sendMsgToActivity("hello, I'm from fragment");

                String msg = fragmentCallback.getMsgFromActivity("null");
                Toast.makeText(BlankFragment1.this.getContext(), msg, Toast.LENGTH_SHORT).show();
            }
        });

        Log.i(TAG, "onCreateView: ");
        return rootview;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        Log.i(TAG, "onActivityCreated: ");
    }

    @Override
    public void onStart() {
        super.onStart();

        Log.i(TAG, "onStart: ");
    }

    @Override
    public void onResume() {
        super.onResume();

        Log.i(TAG, "onResume: ");
    }

    @Override
    public void onPause() {
        super.onPause();

        Log.i(TAG, "onPause: ");
    }

    @Override
    public void onStop() {
        super.onStop();

        Log.i(TAG, "onStop: ");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();

        Log.i(TAG, "onDestroyView: ");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        Log.i(TAG, "onDestroy: ");
    }

    @Override
    public void onDetach() {
        super.onDetach();

        Log.i(TAG, "onDetach: ");
    }
}