package com.firewings.jnipractice;

import android.os.Bundle;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        findViewById(R.id.test).setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        test(true,
                                (byte) 1,
                                ',',
                                (short) 3,
                                4,
                                3.3f,
                                2.2d,
                                "DevYK",
                                28,
                                new int[]{1, 2, 3, 4, 5, 6, 7},
                                new String[]{"1", "2", "4"},
                                new Person(),
                                new boolean[]{false, true}
                        );
                    }
                }
        );

        findViewById(R.id.dynamic).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dynamicRegister("我是动态注册的");
            }
        });

        findViewById(R.id.exception).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dynamicRegister2("测试异常处理");
            }
        });

        findViewById(R.id.localRef).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                localRef();
            }
        });

        findViewById(R.id.count).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                thread(v);
            }
        });

        findViewById(R.id.thread).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testThread();
            }
        });
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    private native void test(boolean b, byte bt, char c, short s, long l, float f, double d,
                             String name, int age, int[] arr, String[] strArr, Person person, boolean[] bArr);

    private native void dynamicRegister(String s);

    private native void dynamicRegister2(String s);

    private void testException() throws NullPointerException {
        throw new NullPointerException("MainActivity testException NullPointerException");
    }

    private native void localRef();

    int count;

    public void thread(View view) {
        for (int i = 0; i < 10; i++) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    count();
                    nativeCount();
                }
            }).start();
        }
    }

    private void count() {
        synchronized (this) {
            count++;
            Log.d("Java", "count=" + count);
        }
    }

    private native void nativeCount();

    // AndroidUI操作，让C++线程里面来调用
    public void updateUI() {
        if (Looper.getMainLooper() == Looper.myLooper()) {
            new AlertDialog.Builder(MainActivity.this)
                    .setTitle("UI")
                    .setMessage("native 运行在主线程，直接更新 UI ...")
                    .setPositiveButton("确认", null)
                    .show();
        } else {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    new AlertDialog.Builder(MainActivity.this)
                            .setTitle("UI")
                            .setMessage("native运行在子线程切换为主线程更新 UI ...")
                            .setPositiveButton("确认", null)
                            .show();
                }
            });
        }
    }

    public native void testThread();
    public native void unThread();

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unThread();
    }

}
