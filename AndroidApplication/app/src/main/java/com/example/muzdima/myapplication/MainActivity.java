package com.example.muzdima.myapplication;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.provider.Settings;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.UUID;

public class MainActivity extends AppCompatActivity implements SensorEventListener {


    float[] mGravity;
    float[] mGeomagnetic;
    float[] orientationLast;

    private SensorManager mSensorManager;
    private boolean isStart = false;
    private String server = "";
    //private String androidId = Settings.Secure.getString(getContentResolver(),
    //        Settings.Secure.ANDROID_ID);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

       FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mSensorManager.registerListener(this,mSensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION),
                SensorManager.SENSOR_DELAY_GAME);
        //mSensorManager.registerListener(this,mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD),
        //        SensorManager.SENSOR_DELAY_GAME);
        //mSensorManager.registerListener(this,mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY),
        //        SensorManager.SENSOR_DELAY_GAME);

        mGravity = null;
        mGeomagnetic = null;
        orientationLast = null;
        Button button = (Button)findViewById(R.id.buttonOK);
        button.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v)
            {
                    server = ((EditText) findViewById(R.id.textView)).getText().toString();
                    isStart = true;
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    class Run implements  Runnable{
        public float x,y,z;

        public void run() {
            try{
                DatagramSocket client_socket = new DatagramSocket();
                InetAddress IPAddress =  InetAddress.getByName(server);
                String androidId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);;
                String str = androidId+" "+x+" "+y+" "+z;
                byte[] send_data = str.getBytes();
                DatagramPacket send_packet = new DatagramPacket(send_data,send_data.length, IPAddress, 43567);
                client_socket.send(send_packet);

                //String androidId = UUID.randomUUID().toString();
                //Socket skt = new Socket(server,43567);
                //DataOutputStream output = new DataOutputStream(skt.getOutputStream());
                //output.writeChars(androidId);
                //output.writeChars(" ");
                //output.writeFloat(x);
                //output.writeChars(" ");
                //output.writeFloat(y);
                //output.writeChars(" ");
                //output.writeFloat(z);
                //output.flush();
                //output.close();
                //skt.close();

            }
            catch (Exception e) {
                Log.e("Error",e.toString());
            }
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event){
        if (event.sensor.getType()!=Sensor.TYPE_ORIENTATION) return;
        if (!isStart) return;
        /*if (event.sensor.getType()==Sensor.TYPE_GRAVITY)
            mGravity = event.values;
        else
        if (event.sensor.getType()==Sensor.TYPE_MAGNETIC_FIELD)
            mGeomagnetic = event.values;
        else
            return;*/

        /*float orientation[] = new float[3];
        if (mGravity != null && mGeomagnetic != null) {
            float R[] = new float[9];
            float I[] = new float[9];
            boolean success = SensorManager.getRotationMatrix(R, I, mGravity, mGeomagnetic);
            if (success)
                SensorManager.getOrientation(R, orientation);
            else
                return;
        }
        else
            return;

        if (orientationLast==null)
            orientationLast = orientation;

        float epsilon = 0.8f;

        float values[] = new float[3];
        for(int i=0;i<3;i++)
            values[i] = orientationLast[i]*(1-epsilon) +orientation[i]*epsilon;
*/
        float x = event.values[0];
    float y = event.values[1];
    float z = event.values[2];



    EditText textView = (EditText)findViewById(R.id.textView);
    String format = "%.2f";
    String xx = String.format(format, x);
    String yy = String.format(format, y);
    String zz = String.format(format, z);
    textView.setText(xx+", "+yy+", "+zz);

       Run runnable =  new Run();
        runnable.x = x;
        runnable.y = y;
        runnable.z = z;

        try {
            new Thread(runnable).start();
        } catch (Exception e) {
            textView.setText(e.toString());
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // not in use
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
