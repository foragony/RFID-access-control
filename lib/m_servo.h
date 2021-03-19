
/*******************************************************************************************
  @ 天津大然  Mr_Wang_Tju
舵机使用函数库说明
为方便用户在不研究本舵机通信协议情况下使用该舵机、特将舵机使用函数封装为库
*******************************************************************************************/


#ifndef M_SERVO_H
#define M_SERVO_H

//导入Arduino核心头文件
#include"Arduino.h"  

extern int servo_sdata[]; 
extern double servo_rpara[];
extern String data;
class m_servo
{
     private:
          byte choose;        //选择Arduino串口
     public:
          
          m_servo(byte choose);   //构造函数
          
          ~m_servo();          //析构函数
          
//          void on();      //打开LED

//          void off();     //关闭LED
//          
//          void disattach(); //释放引脚与m_servo的绑定，使得引脚可以被控制
  
/*Arduino串口初始化
*/
void senddata(char kk);	
	 
/*单个舵机控制函数 -- 控制指定舵机编号的舵机按照指定的速度转动到指定的角度。
   参数及解释：
    id_num: 舵机编号1~255（除去0x79、0x7E、0x7F）共253个ID,出厂默认为0; 121号为广播编号，即所有舵机都会响应到控制指令并执行
    angle: 舵机角度0~270度；         
    step:步数，舵机要达到指定角度分步的次数，以10ms为周期每周期最多运动3度，步数设置为“1”时则以最快速度转动。
    
注：设置完毕后，请留出足够运行的时间
*/
  void set_angle(int id, float  angle, int steps);
  
/*多个舵机控制函数
控制指定舵机编号的舵机按照指定的速度转动到指定的角度。
参数及解释:
    id_list: 舵机编号组成的列表
    angle_list: 舵机角度组成的列表（0~270°）
    step: 步数，这里多个舵机共用同一个步数，以保证在不“丢步”的情况下多个舵机同时开始同时结束
    n: 用来控制控制多个舵机的协议，目前有两个版本，分别对应于n=1,n=2,  默认n=1
        n=1,长协议控制：[0x7B, ID, angle_H, angle_L, step_H, step_L, Check, Cmd, CmdData, 0x7D]
        n=2 协议格式为：[126, ID1, angle0, angle1] [126, ID2, angle0, angle1], .....[126, step0, step1, 127]
*/
  void set_angles(int id_list[20], float angle_list[20], int step,int n);
  
/* 设置舵机模式函数
 舵机有四种模式:阻尼模式，锁死模式，掉电模式，轮子模式
    处于阻尼模式时，舵机能够被掰动，但是阻力很大，而且转动越快，阻力越大
    处于锁死模式时，舵机控制程序启动，将舵机角度固定在某个角度，不能被掰动
处于掉电模式时，舵机可以被随意掰动，阻力很小
处于轮子模式时，舵机变成减速电机，可以在指定速度下整周连续转动
参数及解释：
id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
 	mode_num: 用来选择不同的模式
    mode_num=1,阻尼模式
    mode_num=2,锁死模式
    mode_num=3,掉电模式
    mode_num=4,轮子模式
注：在设置为轮子模式后，再设置轮子模式下速度，则可使舵机连续输出一定转速
*/  
  void change_mode(int id_num, int mode_num);
  
  /* 设置舵机编号
参数及解释：
    id_num: 需要重新设置编号的舵机编号。如果不知道当前舵机编号，可以用121广播，但是这时总线上只能连一个舵机，否则多个舵机会被设置成相同编号
    id_new: 新舵机编号
*/
  void set_id(int id_num, int id_new);
  
  /* 设置pid控制的p参数  
调节pid参数，因为目前舵机控制只用了只用了比例环节，所以这里只要给p参数即可。
参数及解释：
    id_num: 需要重新设置pid的舵机编号,如果不知道当前舵机编号，可以用121广播
    pid: pid参数，调节pid参数可改变舵机响应速度，但过大则导致舵机超调。默认=20，调节时在这附近给值，一般情况下不需要改变。
*/
  void set_pid(int id_num,int pid);
  
  /*获取当前舵机状态信息，例如当前角度等
  参数及解释：
    id_num：需要返回参数的舵机编号  除总线上只有一个舵机外，勿使用广播模式（即id_num=121）
    para_num: 想要查询的参数编号，int型函数可直接返回一个指定参数
    para_num=0, 不利用函数返回指定参数，可根据全局变量servo_rpara[4]数组取用
    para_num=1, 返回当前舵机编号
    para_num=2, 返回当前角度
    para_num=3, 返回当前期望角度
    para_num=4, 返回运行时间
 Returns:
    返回值会根据para_num的值相应改变
      para_num=0, 返回所有信息组成的列表
      para_num=1, 返回当前舵机编号
      para_num=2, 返回当前角度
      para_num=3, 返回当前期望角度
      para_num=4, 返回运行时间
注：servo_rpara[]数组为返回参数存储数组，0~3位分别存储数据为舵机编号、当前舵机角度、当前期望角度、
到达期望角度所剩余步数。此数组已定义为全局变量，允许在main函数中直接调用。
返回的数据可作为是否继续下一步的标志。
*/
  float get_state(int id_num, int para_num,int o_m);
  
/*轮子模式下设置舵机的转动速度
参数及解释：
  id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
  speed: 机转动速度（-1000~1000），速度为正舵机正转，反之反转
*/
  void set_speed(int id_num,int speed);

  /* 修改E2PROM中的舵机参数值
参数及解释：
    id_num: 舵机编号，即要设置第几号舵机的模式，这里可以用广播模式。
    address: E2PROM中参数的相对地址
    value: E2PROM中对应地址的设置目标值
*/
void write_e2(int id_num, int address, int value);

/* 读取E2PROM中指定位置的舵机参数值
参数及解释：
    id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
    address: flash中的相对位置，0代表0x00, 以此类推，每一位的具体含义如E2PROM表
Returns:
    返回舵机发送过来的8数据。
*/
int read_e2(int id_num, int address);

/* 
读取E2PROM中全部的舵机参数值
参数及解释：
    id_num: 要读取的舵机编号,如果总线上多于一个舵机这里不可以用广播模式。
Returns:
     返回舵机发送过来的16数据格式：
     [0x7B, ID, data1~data11, 0x00, 0x00, 0x7D]
*/
void read_e2_all(int id_num);

/*将指定舵机的E2PROM中的参数初始化（除ID和波特率以外）
参数及解释：
    id_num: 舵机编号,即要初始化第几号舵机的E2PROM中除了ID号与波特率、舵机ID修改次数的所有值，不可以广播
*/
void e2_init(int id_num);

void preset_angle(int id_num, float  angle, int steps, int rn);

void unlock(int id_num);
 
};


#endif
