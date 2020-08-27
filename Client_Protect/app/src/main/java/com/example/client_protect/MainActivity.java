package com.example.client_protect;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.sql.Time;


public class MainActivity extends AppCompatActivity {

    public native String NativeString();
    public native void PsCommand();
    public native boolean TimeCheck();
    public native boolean TracerPidCheck();
    public native boolean DebuggingPortCheck();
    public native boolean PtraceCheck();
    public native boolean BreakPointCheck();
    public native boolean MemoryCheck();
    public native boolean CmdLineCheck();


    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final Context mContext = this;

        // Make RootChecker Object
        final Checker checker = new Checker(mContext);

        final TextView logview = findViewById(R.id.LogView);


        // Check_button1
        Button button_1 = findViewById(R.id.Check_Button1);
        button_1.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                boolean result = checker.Check_For_Hack(null);
                boolean a = MemoryCheck();
                if(a==false){
                    logview.setText("False");
                }
                else{
                    logview.setText("True");
                }

            }
        });

    }
}