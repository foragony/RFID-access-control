/****
m_servo.cpp
天津市大然科技有限公司-总线舵机�?

舵机型号：大然A15-ST型舵�?
适用平台：micropython平台（pyboard等）
库版本号：v1.0
测试主控版本：标准pyboard舵机主控�?v2.0
测试人员：刘翠翠
测试时间�?018.10.15


代码版本：v1.0
created in 2018.10.12   by 刘翠�?
******************/

#include"m_servo.h"
#include"Arduino.h"

int servo_sdata[10]={ 0x7b,0x79,0,0,0,0,0x10,0x10,0x10,0x7d };  //发送数�? 10bit
int servo_sdata1[4]={0x7E,0,0,0};
int i,id_num,steps,rn,n,rem_step,id_number;
float  angle;
static char flag_serial = 0;
double cur_angle=0; //返回 舵机实际角度
double exp_angle;   //返回 期望角度
int cur_mode;       //返回 当前模式
int run_time;       //返回 运行时长
String data;

double servo_rpara[10];  //返回参数数组
int servo_recvdata[16];
int u_numm;


m_servo::m_servo(byte p = 0):choose(1)
{
	Serial.begin(19200); 
   flag_serial = p;
}

m_servo::~m_servo()
{
//    disattach();
	Serial.begin(19200); 
} 

void m_servo::senddata(char kk)	{
    char i;
	if(kk==10)
	{
	for(i=0;i<kk;i++)
	  {
		Serial.write(servo_sdata[i]);
	  } 
  }
	  else if(kk==4)	{
	  for(i=0;i<kk;i++)
	  {
		Serial.write(servo_sdata1[i]);
	  } 
	  }

}

void m_servo::set_angle(int id_num, float  angle, int steps)
 {
    if(steps<=0)    steps=1;
     //目前不支持负角度
    if(angle<0)    angle=0;
    if(angle>275)  angle=275;
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[2] = (int)((int)(angle/10))%100;
    servo_sdata[3] = ((int)(angle * 10)) % 100;
    servo_sdata[4] = (int)(steps/100);
    servo_sdata[5] = (int)((int)steps% 100);
    servo_sdata[7] = 16;
    servo_sdata[8] = 1;
    servo_sdata[9] = 125;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	  senddata(10);
    delay(2);
		servo_sdata[7]=17;
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
		//串口发�?
		senddata(10);
   delay(2);
}

//多个舵机
void m_servo::set_angles(int id_list[20], float angle_list[20], int step,int n) 
{
  if(n==1)
  {
    for(i=0;i<10;i++)
    {
      if(i!=0)
      {
		    if(id_list[i]!=0)
		    preset_angle(id_list[i],angle_list[i],step,1);     
      }
      else   //n==0 则代表为第一组数�?id号可以为0 不需经过判断
      {
		    preset_angle(id_list[i],angle_list[i],step,1);     //
      }
	    delay(2);
    }       
    //多舵机同时开始转�?
	  servo_sdata[1] = 121;
    servo_sdata[7]=17;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    senddata(10);
  }
  else if(n==2)
  {
    for(i=0;i<10;i++)
    {
      if(i!=0)
      {
        if(id_list[i]!=0)
        {
			    servo_sdata1[0] = 0x7E;
			    servo_sdata1[1] = id_list[i];
			    servo_sdata1[2] = (int)((int)angle_list[i]/10)%100;
			    servo_sdata1[3] = (int)(angle_list[i]*10)%100;
			    senddata(4);
			    delay(2);
        }  
      }
      else
      {
        servo_sdata1[0] = 0x7E;
        servo_sdata1[1] = id_list[i];
        servo_sdata1[2] = (int)((int)angle_list[i]/10)%100;
        servo_sdata1[3] = (int)(angle_list[i]*10)%100;
		    senddata(4);
		    delay(2);
      }
    }
    //设置步数
    servo_sdata1[0] = 0x7E;
    servo_sdata1[1] = (int)(step/100);
    servo_sdata1[2] =  (int)((int)step%100);
    servo_sdata1[3] = 0x7F;
	  senddata(4);
    delay(2);	
  }
}


//设置舵机模式
void m_servo::change_mode(int id_num, int mode_num)
{
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[7] = 22;
    servo_sdata[8] = 16 + mode_num;
    servo_sdata[9] = 125;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;

	  senddata(10);
    delay(2);
}

//设置舵机编号

void m_servo::set_id(int id_num, int id_new)
{
	  servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
	  servo_sdata[2] = 0;
    servo_sdata[7] = 0x42;
    servo_sdata[8] = 0; 
    servo_sdata[9] = 125;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	  senddata(10);
	  delay(2);
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
	  servo_sdata[2] = id_new;
    servo_sdata[7] = 0x44;
    servo_sdata[8] = 0;  //ID号在E2中的地址�?
    servo_sdata[9] = 125;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    senddata(10);
	  delay(2);
}   

//设置PID  P参数
void m_servo::set_pid(int id_num,int pid)
{
	  servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
	  servo_sdata[2] = 0;
    servo_sdata[7] = 0x42;
    servo_sdata[8] = 0; 
    servo_sdata[9] = 125;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	  senddata(10);
	  delay(2);
	
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
	  servo_sdata[2] = pid;
    servo_sdata[7] = 0x44;
    servo_sdata[8] = 3;
    servo_sdata[9] = 125;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;

	  senddata(10);
	  delay(2);
}

//设置轮子模式下速度
void m_servo::set_speed(int id_num,int speed)
{
   servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[7] = 22;
    servo_sdata[8] = 16 + 4;
    servo_sdata[9] = 125;
   servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;

    senddata(10);
    delay(2);
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    if(speed > 0)
        servo_sdata[2] = 33;
    else if(speed < 0)
        {servo_sdata[2] = 34;    speed = -speed;}
    else
        servo_sdata[2] = 32;
    speed=speed+150;   
    servo_sdata[4] = (int)(speed / 10);
    servo_sdata[5] = (int)((speed) % 10);
    servo_sdata[7] = 32;
    servo_sdata[8] = 1;
    servo_sdata[9] = 125;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    
    senddata(10);
	  delay(2);
}

//舵机返回�?

float m_servo::get_state(int id_num, int para_num,int o_m)   //特别注意 不能多个舵机同时返回数据 即除使用一个舵机外、不允许使用广播模式�?21�?
{
  if (id_num == 121 && o_m == 0)
  {
    return 0; 
  }
  else if(id_num == 121 && o_m > 1)
  {
    return 0; 
  }
  else
  {
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[7] = 0x13;
    servo_sdata[9] = 125;
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    
    u_numm=0;
    senddata(10);
    char i=0;
    int t=1000;
    bool n_s = 1;
    char num =16;
   while (Serial.available()<num&&t>0)
   {
     t-=1;
     if(Serial.available()>0&&n_s)
     {
       if(Serial.read()==123)
       {
         n_s=0;
         servo_recvdata[0] = 123;
         u_numm = 1;
       }
     }
   }
   while(Serial.available()>0)
   {
      servo_recvdata[u_numm] = (int)Serial.read();
      u_numm ++;
   }
   if(u_numm==num)
   {
      id_number = servo_recvdata[1];
      cur_angle = (double)(servo_recvdata[2] * 10 + servo_recvdata[3] * 0.1);
      exp_angle = servo_recvdata[4] * 10 + servo_recvdata[5] * 0.1;
      run_time = servo_recvdata[6] * 100 + servo_recvdata[7];
      servo_rpara[0]=id_number;
      servo_rpara[1]=cur_angle;
      servo_rpara[2]=exp_angle;
      servo_rpara[3]=run_time;      
      if(para_num==1)
        return id_number;
      else if(para_num == 2)
          return cur_angle;
      else if(para_num == 3)
          return exp_angle;
      else if(para_num == 4)
          return run_time;
      else if(para_num == 0)
      {
          cur_mode = servo_recvdata[8];
          servo_rpara[4]= cur_mode; 
      }   
   }
   else return -1;  //返回数据失败  可做读取失败标志�?
  }
}
//写E2PROM
void m_servo::write_e2(int id_num, int address, int value)
{
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[2] = 0;
    servo_sdata[7] = 0x42;
    servo_sdata[8] = 0; 
    servo_sdata[9] = 125;
    servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    /*for(i=0;i<10;i++) 
    {
      Serial.write(servo_sdata[i]);
    }*/
    senddata(10);
    delay(2);
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[2] = value;
    servo_sdata[7] = 0x44;
    servo_sdata[8] = address;  //E2中的地址
    servo_sdata[9] = 125;
    servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    senddata(10);
    delay(2);
}
//读E2PROM指定地址的参数�?
int m_servo::read_e2(int id_num, int address)
{
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[2] = 0;
    servo_sdata[3] = 0;
    servo_sdata[4] = 0;
    servo_sdata[5] = 0;
    servo_sdata[7] = 0x45;
    servo_sdata[8] = address; 
    servo_sdata[9] = 125;
    servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    
    u_numm=0;
    senddata(10);
    char i=0;
    int t=1000;
    bool n_s = 1;
    char num =16;
   while (Serial.available()<num&&t>0)
   {
     t-=1;
     if(Serial.available()>0&&n_s)
     {
       if(Serial.read()==123)
       {
         n_s=0;
         servo_recvdata[0] = 123;
         u_numm = 1;
       }
     }
   }
   while(Serial.available()>0)
   {
      servo_recvdata[u_numm] = (int)Serial.read();
      u_numm ++;
   }
   if(u_numm==num)
   {
     return servo_recvdata[3];
   }
    else return -1;  //返回数据失败  可做读取失败标志�?
    
}
//返回E2PROM的全部参数�?
void m_servo::read_e2_all(int id_num)
{
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[2] = 0;
    servo_sdata[3] = 0;
    servo_sdata[4] = 0;
    servo_sdata[5] = 0;
    servo_sdata[7] = 0x46;
    servo_sdata[8] = 0; 
    servo_sdata[9] = 125;
    servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    u_numm=0;
    senddata(10);
    char i=0;
    int t=1000;
    bool n_s = 1;
    char num =16;
   while (Serial.available()<num&&t>0)
   {
     t-=1;
     if(Serial.available()>0&&n_s)
     {
       if(Serial.read()==123)
       {
         n_s=0;
         servo_recvdata[0] = 123;
         u_numm = 1;
       }
     }
   }
   while(Serial.available()>0)
   {
      servo_recvdata[u_numm] = (int)Serial.read();
      u_numm ++;
   }
   if(u_numm==num)
   {
     //接收到的数据保存在servo_recvdata[]�?
   }
}
//E2PROM初始�?
void m_servo::e2_init(int id_num)
{
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[2] = 0;
    servo_sdata[3] = 0;
    servo_sdata[4] = 0;
    servo_sdata[5] = 0;
    servo_sdata[7] = 0x43;
    servo_sdata[8] = 0; 
    servo_sdata[9] = 125;
    servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    
    senddata(10);
    delay(2);
}
void m_servo::preset_angle(int id_num, float  angle, int steps, int rn)
 {
    if(steps<=0)    steps=1;
     //目前不支持负角度
    if(angle<0)    angle=0;
    if(angle>275)  angle=275;
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[2] = (int)((int)(angle/10))%100;
    servo_sdata[3] = ((int)(angle * 10)) % 100;
    servo_sdata[4] = (int)(steps/100);
    servo_sdata[5] = (int)((int)steps% 100);
    servo_sdata[7] = 16;
    servo_sdata[8] = 1;
    servo_sdata[9] = 125;
    servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    senddata(10);
    delay(2);
    if(rn == 0)
    {
      servo_sdata[7] = 17;
      servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
      senddata(10);
      delay(2);
      }
    
}
//������ת���� 
void m_servo::unlock(int id_num)
{
    servo_sdata[0] = 123;
    servo_sdata[1] = id_num;
    servo_sdata[2] = 0;
    servo_sdata[3] = 0;
    servo_sdata[4] = 0;
    servo_sdata[5] = 0;
    servo_sdata[7] = 0x30;
    servo_sdata[8] = 0; 
    servo_sdata[9] = 125;
    servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    
    senddata(10);
    delay(2);
}
