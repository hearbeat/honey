package com.example.control_air;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;


import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class tcpActivity extends Activity {
	Socket socket=null;//开辟一个socket控件
	Button enterbut=null;//
	EditText IP=null;
	EditText PORT=null;
	EditText edttemp=null;//温度
	EditText edthumi=null;//湿度
	TextView lightled1=null;//LED1灯状态显示
	TextView lightled2=null;//LED2灯状态显示
	Button butLED1=null;//定义LED1按键发送
	Button butLED2=null;//定义LED2按键发送
	OutputStream outputstream;
	InputStream in;
	boolean isConnected=false;
	boolean led1data=false,led2data=false;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.tcp);
		enterbut=(Button)findViewById(R.id.conn);//获取ID号
		IP=(EditText)findViewById(R.id.editIp);
		IP.setText("192.168.4.1");
		PORT=(EditText)findViewById(R.id.editport);
		PORT.setText("8888");
		edttemp=(EditText)findViewById(R.id.edittemp);
		edthumi=(EditText)findViewById(R.id.edithumi);
		enterbut.setOnClickListener(new enterclick());
		lightled1 =(TextView)findViewById(R.id.light1);
		lightled2 =(TextView)findViewById(R.id.light2);
		butLED1=(Button)findViewById(R.id.BUTLED1);//发送LED1信号控制按钮
		butLED2=(Button)findViewById(R.id.BUTLED2);//发送LED1信号控制按钮
		butLED1.setOnClickListener(new sendLED1click());
		butLED2.setOnClickListener(new sendLED2click());
	}
	/*
     * 发送数据按键
     */
	public class sendLED1click implements OnClickListener
	{


		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
			if(socket!=null)
			{
				try {
					OutputStream outputstream = socket.getOutputStream();
					byte buf[]=new byte[2];//开辟2个空间
					buf[0]='1';//LED1
					if(led1data)//true
					{
						led1data=false;
						buf[1]='0';//1开灯，0灭灯
						butLED1.setText("开灯");
					}
					else
					{
						led1data=true;
						buf[1]='1';//1开灯，0灭灯
						butLED1.setText("关灯");
					}
					outputstream.write(buf);
					outputstream.flush();//清空发送数据*/
					//Toast.makeText(tcpActivity.this,"数据发送成功！"	, 0).show();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}   //蓝牙连接输出流
			}
			//else
				//Toast.makeText(tcpActivity.this,"WIFI连接错误，请检查"	, 0).show();
		}
	}
	/*
     * 发送数据按键
     */
	public class sendLED2click implements OnClickListener
	{


		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
			if(socket!=null)
			{
				try {
					OutputStream outputstream = socket.getOutputStream();
					byte buf[]=new byte[2];//开辟2个空间
					buf[0]='2';//LED2
					if(led2data)//true
					{
						led2data=false;
						buf[1]='0';//1开灯，0灭灯
						butLED2.setText("开灯");
					}
					else
					{
						led2data=true;
						buf[1]='1';//1开灯，0灭灯
						butLED2.setText("关灯");
					}
					outputstream.write(buf);
					outputstream.flush();//清空发送数据*/
					//Toast.makeText(tcpActivity.this,"数据发送成功！"	, 0).show();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}   //蓝牙连接输出流
			}
			///else
				//Toast.makeText(tcpActivity.this,"WIFI连接错误，请检查!"	, 0).show();
		}
	}
	/****************定义按键触发事件函数*******************/
	public class enterclick implements OnClickListener
	{

		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
			if(!isConnected)//没有连接上
				new ClientThread().start();//打开连接
			else//连接上，断开连接
			{
				if (socket != null) {
					try {
						socket.close();
						socket = null;
						isConnected=false;
						//得到一个消息对象，Message类是有Android操作系统提供
						Message msg = mHandler.obtainMessage();
						msg.what=0;
						mHandler.sendMessage(msg);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}


				}
			}
		}

	}
	Handler mHandler = new Handler() {  //等待socket连接成功
		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			String a,LED1status,LED2status;;
			switch (msg.what) {
				case 0://TCP断开连接
					enterbut.setText("连接");
					//Toast.makeText(tcpActivity.this,"服务器连接断开！"	, 0).show();
					break;
				case 1://表明TCP连接成功，可以进行数据交互了
					enterbut.setText("断开");
					//Toast.makeText(tcpActivity.this,"服务器连接成功！"	, 0).show();
					new InputThread().start();//开启接收线程
					//	edttemp.setText("456");
					break;
				case 2://有数据进来
					String result = msg.getData().get("msg").toString();
					//	edttemp.setText("123");
					//	txtshow.setText(""+i)
					a=result.substring(0, 1);
					if(a.equals("T"))
					{
						edttemp.setText(result.substring(1, 3)+"C");//获取温度
						edthumi.setText(result.substring(3, 5)+"%");//获取湿度
						LED1status=result.substring(5, 6);//获取LED1状态
						LED2status=result.substring(6, 7);//获取LED2状态
						if(LED1status.equals("1"))//灯关着
						{
							lightled1.setBackgroundResource(R.drawable.light_off);//更改图片样式
						}
						else
						{
							lightled1.setBackgroundResource(R.drawable.light_on);//更改图片样式
						}
						if(LED2status.equals("1"))//灯关着
						{
							lightled2.setBackgroundResource(R.drawable.light_off);//更改图片样式
						}
						else
						{
							lightled2.setBackgroundResource(R.drawable.light_on);//更改图片样式
						}
					}
					break;
			}
		}
	};
	//开辟一个线程 ,线程不允许更新UI  socket连接使用
	public class ClientThread extends Thread
	{

		public void run()
		{
			//	 Looper.prepare();
			try {
				socket=new Socket(IP.getText().toString(),Integer.parseInt(PORT.getText().toString()));//建立好连接之后，就可以进行数据通讯了
				isConnected=true;
				in = socket.getInputStream();
				//得到一个消息对象，Message类是有Android操作系统提供
				Message msg = mHandler.obtainMessage();
				msg.what=1;
				mHandler.sendMessage(msg);
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}//连接服务器
		}

	}


	public class InputThread extends Thread
	{

		public void run()
		{
			while(true)
			{
				if(socket!=null)
				{

					String result = readFromInputStream(in);
					try {


						if (!result.equals("")) {

							Message msg = new Message();
							msg.what = 2;
							Bundle data = new Bundle();
							data.putString("msg", result);
							msg.setData(data);
							mHandler.sendMessage(msg);
						}

					} catch (Exception e) {
						//Log.e(tag, "--->>read failure!" + e.toString());
					}

					try {
						//设置当前显示睡眠1秒
						Thread.sleep(100);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
		}
	}
	public String readFromInputStream(InputStream in) {
		int count = 0;
		byte[] inDatas = null;
		try {
			while (count == 0) {
				count = in.available();
			}
			inDatas = new byte[count];
			in.read(inDatas);
			return new String(inDatas, "gb2312");
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}
	/******************************************
	 *
	 *
	 *
	 *
	 *
	 * @return
	 */

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
}