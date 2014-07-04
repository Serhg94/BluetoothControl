package com.example.android.BluetoothControl;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;

class Joystick extends View  
{ 
    Paint paint;//нужен чтобы установить цвет. 
    int Lout;
    int Rout;
    int[] Lf;
    int[] Rf;
    int Lcur;
    int Rcur;
    int Lpoint;
    int Rpoint;
    int width;
    int dead_zone;
    final static int Radius=30; 
    int PointerCount;
    private Display mDisplay;
    private WindowManager mWindowManager;
    
    
    public Joystick(Context context, AttributeSet attrs) 
    { 
        super(context, attrs); 
        paint = new Paint(); 
        paint.setColor(Color.YELLOW); 
        paint.setStyle(Style.FILL); 
        PointerCount=0; 
        Lf=new int[2];
        Rf=new int[2];
        Lf[0] = -100;
        Rf[0] = -100;
        Lf[1] = -100;
        Rf[1] = -100;
        dead_zone = 45;
        mWindowManager = (WindowManager) (context.getSystemService("window"));
        mDisplay = mWindowManager.getDefaultDisplay();
        width = mDisplay.getWidth()/2;
        Log.v("Mytag", " "+width);
    } 
    
    
    @Override 
    public boolean onTouchEvent(MotionEvent event) 
    { 
        PointerCount=event.getPointerCount();
        for(int i=0;i<PointerCount;i++) 
        { 
        	if(event.getX(i)>width)
        	{
        		if((Rpoint == -1)&&(Lpoint != event.getPointerId(i)))
        			Rpoint = event.getPointerId(i);
        		if(Rf[0] < 0)
        		{	
        			Rf[0] = (int) event.getX(i);
        			Rf[1] = (int) event.getY(i);
        		}
        	}
        	else
        	{
        		if((Lpoint == -1)&&(Rpoint != event.getPointerId(i)))
        			Lpoint = event.getPointerId(i);
        		if(Lf[0] < 0)
        		{	
        			Lf[0] = (int) event.getX(i);
        			Lf[1] = (int) event.getY(i);
        		}
        	}
        	if(Lpoint == event.getPointerId(i))
        	{
        		Lcur = (int) event.getY(i);
        		Lout = Lcur-Lf[1];
        		if((Lout>-dead_zone/2)&&(Lout<dead_zone/2))
        			Lout = 0;
        		else
        			if(Lout>0)
        				Lout = Lout-dead_zone/2;
        			else
        				Lout = Lout+dead_zone/2;
        	}
        	if(Rpoint == event.getPointerId(i))
        	{
        		Rcur = (int) event.getX(i);
        		Rout = Rcur-Rf[0];
        		if((Rout>-dead_zone/2)&&(Rout<dead_zone/2))
        			Rout = 0;
        		else
        			if(Rout>0)
        				Rout = Rout-dead_zone/2;
        			else
        				Rout = Rout+dead_zone/2;
        	}
        }
        if((event.getActionMasked() == MotionEvent.ACTION_UP))
        {
        	Lpoint = -1;
        	Lout = 0;
        }
        if((event.getActionMasked() == MotionEvent.ACTION_UP))
        {
        	Rpoint = -1;
        	Rout = 0;
        }
        if(Lpoint == Rpoint){
        	Lpoint = -1;
        	Rpoint = -1;
        	Lout = 0;
        	Rout = 0;
        }
        return true; 
    } 
     
    
    @Override 
    protected void onDraw(Canvas canvas) 
    { 
    	paint.setColor(Color.RED);
    	canvas.drawCircle(Lf[0], Lf[1], dead_zone, paint); 
    	canvas.drawCircle(Rf[0], Rf[1], dead_zone, paint); 
    	paint.setColor(Color.YELLOW);
    	if(Lpoint != -1)
		{
    		canvas.drawCircle(Lf[0], Lcur, Radius, paint); 
		}
    	if(Rpoint != -1)
		{
    		canvas.drawCircle(Rcur, Rf[1], Radius, paint); 
		}
        invalidate();// invalidate() нужен для того, чтобы оповестить Android, что нужно выполнить метод OnDraw снова, без него View не будет перериовываться.
    } 
}