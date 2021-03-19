/****
m_servo.cpp
å¤©æ´¥å¸‚å¤§ç„¶ç§‘æŠ€æœ‰é™å…¬å¸-æ€»çº¿èˆµæœºåº?

èˆµæœºåž‹å·ï¼šå¤§ç„¶A15-STåž‹èˆµæœ?
é€‚ç”¨å¹³å°ï¼šmicropythonå¹³å°ï¼ˆpyboardç­‰ï¼‰
åº“ç‰ˆæœ¬å·ï¼šv1.0
æµ‹è¯•ä¸»æŽ§ç‰ˆæœ¬ï¼šæ ‡å‡†pyboardèˆµæœºä¸»æŽ§æ?v2.0
æµ‹è¯•äººå‘˜ï¼šåˆ˜ç¿ ç¿ 
æµ‹è¯•æ—¶é—´ï¼?018.10.15


ä»£ç ç‰ˆæœ¬ï¼šv1.0
created in 2018.10.12   by åˆ˜ç¿ ç¿?
******************/

#include"m_servo.h"
#include"Arduino.h"

int servo_sdata[10]={ 0x7b,0x79,0,0,0,0,0x10,0x10,0x10,0x7d };  //å‘é€æ•°ç»? 10bit
int servo_sdata1[4]={0x7E,0,0,0};
int i,id_num,steps,rn,n,rem_step,id_number;
float  angle;
static char flag_serial = 0;
double cur_angle=0; //è¿”å›ž èˆµæœºå®žé™…è§’åº¦
double exp_angle;   //è¿”å›ž æœŸæœ›è§’åº¦
int cur_mode;       //è¿”å›ž å½“å‰æ¨¡å¼
int run_time;       //è¿”å›ž è¿è¡Œæ—¶é•¿
String data;

double servo_rpara[10];  //è¿”å›žå‚æ•°æ•°ç»„
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
     //ç›®å‰ä¸æ”¯æŒè´Ÿè§’åº¦
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
		//ä¸²å£å‘é€?
		senddata(10);
   delay(2);
}

//å¤šä¸ªèˆµæœº
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
      else   //n==0 åˆ™ä»£è¡¨ä¸ºç¬¬ä¸€ç»„æ•°æ?idå·å¯ä»¥ä¸º0 ä¸éœ€ç»è¿‡åˆ¤æ–­
      {
		    preset_angle(id_list[i],angle_list[i],step,1);     //
      }
	    delay(2);
    }       
    //å¤šèˆµæœºåŒæ—¶å¼€å§‹è½¬åŠ?
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
    //è®¾ç½®æ­¥æ•°
    servo_sdata1[0] = 0x7E;
    servo_sdata1[1] = (int)(step/100);
    servo_sdata1[2] =  (int)((int)step%100);
    servo_sdata1[3] = 0x7F;
	  senddata(4);
    delay(2);	
  }
}


//è®¾ç½®èˆµæœºæ¨¡å¼
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

//è®¾ç½®èˆµæœºç¼–å·

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
    servo_sdata[8] = 0;  //IDå·åœ¨E2ä¸­çš„åœ°å€ä¸?
    servo_sdata[9] = 125;
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    senddata(10);
	  delay(2);
}   

//è®¾ç½®PID  På‚æ•°
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

//è®¾ç½®è½®å­æ¨¡å¼ä¸‹é€Ÿåº¦
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

//èˆµæœºè¿”å›žå€?

float m_servo::get_state(int id_num, int para_num,int o_m)   //ç‰¹åˆ«æ³¨æ„ ä¸èƒ½å¤šä¸ªèˆµæœºåŒæ—¶è¿”å›žæ•°æ® å³é™¤ä½¿ç”¨ä¸€ä¸ªèˆµæœºå¤–ã€ä¸å…è®¸ä½¿ç”¨å¹¿æ’­æ¨¡å¼ï¼?21ï¼?
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
   else return -1;  //è¿”å›žæ•°æ®å¤±è´¥  å¯åšè¯»å–å¤±è´¥æ ‡å¿—ä½?
  }
}
//å†™E2PROM
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
    servo_sdata[8] = address;  //E2ä¸­çš„åœ°å€
    servo_sdata[9] = 125;
    servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    senddata(10);
    delay(2);
}
//è¯»E2PROMæŒ‡å®šåœ°å€çš„å‚æ•°å€?
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
    else return -1;  //è¿”å›žæ•°æ®å¤±è´¥  å¯åšè¯»å–å¤±è´¥æ ‡å¿—ä½?
    
}
//è¿”å›žE2PROMçš„å…¨éƒ¨å‚æ•°å€?
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
     //æŽ¥æ”¶åˆ°çš„æ•°æ®ä¿å­˜åœ¨servo_recvdata[]ä¸?
   }
}
//E2PROMåˆå§‹åŒ?
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
     //ç›®å‰ä¸æ”¯æŒè´Ÿè§’åº¦
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
//½âËø¶Â×ª±£»¤ 
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
