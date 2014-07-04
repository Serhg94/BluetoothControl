package com.example.android.BluetoothControl;


import java.util.Timer;
import java.util.TimerTask;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import com.android.CarBluetoothControl.R;


@SuppressLint("HandlerLeak")
public class BluetoothControl extends Activity implements SensorEventListener 
{
    private static final String TAG = "BluetoothChat";
    private static final boolean D = true;

    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";

    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;

    private TextView mTitle;
    private EditText mOutEditText;
    private Button mSendButton;
    
    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private float mSensorX;
    private float mSensorY;
    private int mX;
    private int mY;
    private int mXold;
    private int mYold;
    private boolean active;
    private Joystick mJoy;
    private Timer timer;
    private String mConnectedDeviceName = null;
    private ArrayAdapter<String> mConversationArrayAdapter;
    private StringBuffer mOutStringBuffer;
    private BluetoothAdapter mBluetoothAdapter = null;
    public BluetoothService mChatService = null;
    
    
    


    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        if(D) Log.e(TAG, "+++ ON CREATE +++");
     
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mXold = 0;
        mYold = 0;
        requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
        setContentView(R.layout.main);
        getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.custom_title);
        mTitle = (TextView) findViewById(R.id.title_left_text);
        mTitle.setText(R.string.app_name);
        mTitle = (TextView) findViewById(R.id.title_right_text);
        mJoy = (Joystick) findViewById(R.id.joy);
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth не доступен", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        
    }

    @Override
    public void onStart() 
    {
        super.onStart();
        if(D) Log.e(TAG, "++ ON START ++");
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_UI);
        if (!mBluetoothAdapter.isEnabled()) 
        {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        } else 
        {
            if (mChatService == null) setupChat();
        }
        timer = new Timer();
        timer.schedule(new SendTask(), 100, 10);
    }

    @Override
    public synchronized void onResume() 
    {
        super.onResume();
        if(D) Log.e(TAG, "+ ON RESUME +");
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_UI);
        if (mChatService != null) 
        {
            if (mChatService.getState() == BluetoothService.STATE_NONE) 
            {
              mChatService.start();
            }
        }
    }

    private void setupChat() 
    {
        Log.d(TAG, "setupChat()");
        mConversationArrayAdapter = new ArrayAdapter<String>(this, R.layout.message);
        mOutEditText = (EditText) findViewById(R.id.edit_text_out);
        mOutEditText.setOnEditorActionListener(mWriteListener);
        mSendButton = (Button) findViewById(R.id.button_send);
        mSendButton.setOnClickListener(new OnClickListener() 
        {
            public void onClick(View v) 
            {
                // обработка кнопки отправить
                TextView view = (TextView) findViewById(R.id.edit_text_out);
                String message = view.getText().toString();
                sendMessage(message);
            }
        });
        mChatService = new BluetoothService(this, mHandler);
        mOutStringBuffer = new StringBuffer("");
    }

    @Override
    public synchronized void onPause() 
    {
        super.onPause();
        //mSensorManager.unregisterListener(this);
        if(D) Log.e(TAG, "- ON PAUSE -");
    }

    @Override
    public void onStop() 
    {
    	mSensorManager.unregisterListener(this);
        super.onStop();
        if(D) Log.e(TAG, "-- ON STOP --");
    }

    @Override
    public void onDestroy() 
    {
    	mSensorManager.unregisterListener(this);
        super.onDestroy();
        if (mChatService != null) mChatService.stop();
        if(D) Log.e(TAG, "--- ON DESTROY ---");
    }


    public int checkString(String string, int from) 
    {
        if (string == null || string.length() == 0) return -1;

        int i = from;

        char c;
        StringBuilder result=new StringBuilder(10);
        for (; i < string.length(); i++) 
        {
            c = string.charAt(i);
            if (!(c >= '0' && c <= '9')) 
            {
                return -1;
            }
            else
            	result.append(c);
        }
        return Integer.parseInt(result.toString());
    }
    

    public void sendMessage(String message) 
    {
    	if (message.length() > 0) 
    	{
        	if(message.startsWith(("set rate").toString()))
        	{
        		int val = checkString(message, 9);
        		
        		if((val > 0 )&&(val < 1001 )) 
        		{
        			timer.cancel();
        			timer = new Timer();
        			timer.schedule(new SendTask(), 10, val);
        			Toast.makeText(this, "Rate upgraded to "+Integer.toString(val), Toast.LENGTH_SHORT).show();
        		}
        		return;
        	}
    	}

        if (mChatService.getState() != BluetoothService.STATE_CONNECTED) {
            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }


        if (message.length() > 0) 
        {
        	String tx_msg = message + "\r\n";
            byte[] send = tx_msg.getBytes();
            mChatService.write(send);
            mOutStringBuffer.setLength(0);
            mOutEditText.setText(mOutStringBuffer);
        }
    }

    private TextView.OnEditorActionListener mWriteListener =
        new TextView.OnEditorActionListener() {
        public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
            if (actionId == EditorInfo.IME_NULL && event.getAction() == KeyEvent.ACTION_UP) {
                String message = view.getText().toString();
                sendMessage(message);
            }
            if(D) Log.i(TAG, "END onEditorAction");
            return true;
        }
    };

    
    // обрабатываем сообщения от BluetoothChatService
    private final Handler mHandler = new Handler() 
    {
		@Override
        public void handleMessage(Message msg) 
		{
            switch (msg.what) {
            case MESSAGE_STATE_CHANGE:
                if(D) Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                switch (msg.arg1) 
                {
                case BluetoothService.STATE_CONNECTED:
                    mTitle.setText(R.string.title_connected_to);
                    mTitle.append(mConnectedDeviceName);
                    mConversationArrayAdapter.clear();
                    break;
                case BluetoothService.STATE_CONNECTING:
                    mTitle.setText(R.string.title_connecting);
                    break;
                case BluetoothService.STATE_LISTEN:
                case BluetoothService.STATE_NONE:
                    mTitle.setText(R.string.title_not_connected);
                    break;
                }
                break;
            case MESSAGE_WRITE:
                byte[] writeBuf = (byte[]) msg.obj;
                String writeMessage = new String(writeBuf);
                mConversationArrayAdapter.add("Tx:  " + writeMessage);
                break;
            case MESSAGE_READ:
                byte[] readBuf = (byte[]) msg.obj;
                String readMessage = new String(readBuf, 0, msg.arg1);
                mConversationArrayAdapter.add("Rx:  " + readMessage);
                break;
            case MESSAGE_DEVICE_NAME:
                mConnectedDeviceName = msg.getData().getString(DEVICE_NAME);
                Toast.makeText(getApplicationContext(), "Подключен к "
                               + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                break;
            case MESSAGE_TOAST:
                Toast.makeText(getApplicationContext(), msg.getData().getString(TOAST),
                               Toast.LENGTH_SHORT).show();
                break;
            }
        }
    };

    
    public void onActivityResult(int requestCode, int resultCode, Intent data) 
    {
        if(D) Log.d(TAG, "onActivityResult " + resultCode);
        switch (requestCode) 
        {
        case REQUEST_CONNECT_DEVICE:
            // когда DeviceListActivity вернет адрес устройства для подключения
            if (resultCode == Activity.RESULT_OK) 
            {
                String address = data.getExtras()
                    .getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
                BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
            	mChatService.connect(device);
            }
            break;
 
        case REQUEST_ENABLE_BT:
            if (resultCode == Activity.RESULT_OK) 
            {
                setupChat();
                //timer.cancel();
                
            } else 
            {
                Log.d(TAG, "Bluetooth выключен");
                Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }



    @Override
    public boolean onCreateOptionsMenu(Menu menu) 
    {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.option_menu, menu);
        return true;
    }

   
    @Override
    public boolean onOptionsItemSelected(MenuItem item) 
    {

        switch (item.getItemId()) 
        {
        case R.id.connect_scan:
        //запуск DeviceListActivity для поиска и выбора устройства для подключения
            Intent serverIntent = new Intent(this, DeviceListActivity.class);
            startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
            return true;
        //сброс привязки джойстиков
        case R.id.joy_start:
            mJoy.Rf[0] = -100;
            mJoy.Lf[0] = -100;
            return true;
        }
        return false;
    }
    
    
    //обработчик кнопки переключения джойстик/акселерометр
    public void activeToggleClicked(View v) 
    {
    	if(D) Log.d(TAG, "Sensor enabled!!!");
        if (mChatService.getState() != BluetoothService.STATE_CONNECTED) 
        {
            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            ((ToggleButton) v).setChecked(false);
            return;
        }
        if (((ToggleButton) v).isChecked()) 
        {
        	active = true;
        	//mChatService.write(b_led_on);
        } else {
        	byte[] b_led_on = "90/".getBytes();
        	mChatService.write(b_led_on);
        	active = false;
        	b_led_on = "500/".getBytes();
        	mChatService.write(b_led_on);
        	//mChatService.write(b_led_on);
            
        }

    }
    

    
    @Override
    public void onSensorChanged(SensorEvent event) 
    {
        if (event.sensor.getType() != Sensor.TYPE_ACCELEROMETER)
            return;
        /*if  (!mBluetoothAdapter.isEnabled()) {
            return;
        }
        if  (mChatService == null) {
            return;
        }
    	if  (mChatService.getState() != BluetoothService.STATE_CONNECTED) {
            return;
        }*/
        //if(active)
        {
        	mSensorX = (-1)*event.values[0];
            mSensorY = event.values[1];
            mX = (int) mSensorX;
            //mY = (int) mSensorY;
        }
    }
    
    
    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) 
    {
    }
    
    
    
    
    
    class SendTask extends TimerTask 
    {
        public void run() {
        	if  (!mBluetoothAdapter.isEnabled()) 
        	{
                return;
            }
        	if  (mChatService == null) 
        	{
                return;
            }
        	if  (mChatService.getState() != BluetoothService.STATE_CONNECTED) 
        	{
                return;
            }
            if(active)
            {
            	byte[] b_led_on;
            	if(mX != mXold)
    	        {
    	        	mXold = mX;
    	        	switch (mX) 
    	        	{
    	        	case -9:
    	        		b_led_on = "351/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case -8:
                    	b_led_on = "400/".getBytes();
                    	mChatService.write(b_led_on);
                        break;
                    case -7:
                    	b_led_on = "450/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case -6:
                    	b_led_on = "470/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case -5:
                    	b_led_on = "500/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case -4:
                    	b_led_on = "500/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case -3:
                    	b_led_on = "525/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case -2:
                    	b_led_on = "550/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case -1:
                    	b_led_on = "575/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case 0:
                    	b_led_on = "600/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case 1:
                    	b_led_on = "650/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case 2:
                    	b_led_on = "700/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case 3:
                    	b_led_on = "750/".getBytes();
    	            	mChatService.write(b_led_on);
                        break;
                    case 4:

                        break;
                    case 5:

                        break;
                    case 6:

                        break;
                    case 7:

                        break;
                    case 8:

                        break;
                    case 9:

                        break;
    	        	}
    	        	//Log.v("Mytag", "Y:"+mY);
    	        }
            	mY = (int) (3.000*mSensorY+90.00);
            	if (mY > 111) mY = 111;
            	if (mY < 69) mY = 69;
    	        if(mY != mYold)
    	        {
    	        	mYold = mY;
    	        	StringBuilder result=new StringBuilder(10);
            		result.append(mY).append('/');
            		b_led_on = result.toString().getBytes();
                	mChatService.write(b_led_on);
    	        }
            }
            else
            {
            	int servo;
            	if (mJoy.Rout > 150) servo = 150;
            		else if (mJoy.Rout <-150) servo = -150;
            			else servo = mJoy.Rout;
            	servo = servo+150;
            	//servo = (int) (0.126666667*servo +71);
            	servo = (int) (0.14*servo +69);
            	if (servo != mXold){
            		StringBuilder result=new StringBuilder(10);
            		result.append(servo).append('/');
            		mXold = servo;
            		byte[] b_led_on = result.toString().getBytes();
                	mChatService.write(b_led_on);
            		//sendMessage(result.toString());
            		//Log.v("Mytag", "serv: "+servo);
            	}
            	
            	
            	int dc;
            	if (mJoy.Lout > 150) dc = -150;
        		else if (mJoy.Lout <-150) dc = 150;
        			else dc = (-1)*mJoy.Lout;
            	dc = dc+150;
            	dc = (int) (1.68333333*dc -252.5);
            	dc = dc+500;
            	
            	if (dc != mYold){
            		StringBuilder result=new StringBuilder(10);
            		result.append(dc).append('/');
            		mYold = dc;
            		byte[] b_led_on = result.toString().getBytes();
                	mChatService.write(b_led_on);
                	//Log.v("Mytag", "dc: "+dc);
            	}
            	//Log.v("Mytag", "X:"+mJoy.Rout);
            	//Log.v("Mytag", "Y:"+mJoy.Lout);
            }
        } 
    }
    

}




