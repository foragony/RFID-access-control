#include <EEPROM.h>


#include <SPI.h>
#include <MFRC522.h>
#include <m_servo.h>  //引入函数库
m_servo se(0);  //设立对象、设置串口
String inputString = "";  // 待输入字符
String reString = "";  // 待输入字符
String id="121"; //承接舵机编号
bool stringComplete = false; // 字符是否结束

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_1_PIN        10         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN        8          // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 1


byte ssPins = SS_1_PIN;
byte CardValue1[4];
byte CardValue2[4];
byte CardValue3[4];
byte CardValue4[4];


MFRC522 mfrc522;   // Create MFRC522 instance.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();        // Init SPI bus

 
  mfrc522.PCD_Init(ssPins, RST_PIN); // Init each MFRC522 card
  Serial.print(F("Reader "));
  //Serial.print();
  //Serial.print(F(": "));
  mfrc522.PCD_DumpVersionToSerial();
  CardLoad(100,CardValue1);
  CardLoad(104,CardValue2);
  CardLoad(108,CardValue3);
  CardLoad(112,CardValue4);

  delay(100);
  Serial.println(F("init succeed"));
  se.set_angle(121,35,100);
  se.set_id(121,1);
  Serial.println(" ");
  delay(100*10);
  se.set_angle(121,135,100);
  delay(100*10);
  Serial.println("");
  se.write_e2(121,11,0);
}

void loop() {
  // put your main code here, to run repeatedly:
// Look for new cards

    //Protect();
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print(F("Reader "));
    //Serial.print(reader);
    //Show some details of the PICC (that is: the tag/card)
    Serial.print(F(": Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
    GetIN();
    if(FindCard())
    {Serial.println(F("BOOM "));
    Unlock();
    Serial.println("");
    delay(1000);
    
      }

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
  } //if (mfrc522[reader].PICC_IsNewC
//for(uint8_t reader
}


void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    //Serial.println(buffer[i], HEX);
    Serial.println(buffer[i]);
  }
}

//存储卡，最多四张
void RefleshValue(byte *buffer,byte *value)
{
  for(uint8_t i=0;i<4;i++)
  value[i]=buffer[i];
  Serial.println(F("Card Success"));
}

//比较函数，相等返回1
int EqualValue(byte *a,byte *b)
{
  uint8_t i;
  for(i=0;i<4;i++)
  if(a[i]!=b[i]) break;
  if(i==4) return 1;
  else return 0;
  }

//获取输入
void GetIN()
{
  char s;
  s=Serial.read();
  if(s == '1')
  {RefleshValue(mfrc522.uid.uidByte,CardValue1);CardWrite(mfrc522.uid.uidByte,100);}
  if(s == '2')
  {RefleshValue(mfrc522.uid.uidByte,CardValue2);CardWrite(mfrc522.uid.uidByte,104);}
  if(s == '3')
  {RefleshValue(mfrc522.uid.uidByte,CardValue3);CardWrite(mfrc522.uid.uidByte,108);}
  if(s == '4')
  {RefleshValue(mfrc522.uid.uidByte,CardValue4);CardWrite(mfrc522.uid.uidByte,112);}
  }

//查找卡号，在里面返回1
int FindCard()
{
  if(EqualValue(mfrc522.uid.uidByte,CardValue1))
  return 1;
  if(EqualValue(mfrc522.uid.uidByte,CardValue2))
  return 1;
  if(EqualValue(mfrc522.uid.uidByte,CardValue3))
  return 1;
  if(EqualValue(mfrc522.uid.uidByte,CardValue4))
  return 1;
  return 0;
  }

//写入EEPROM
void CardWrite(byte *buffer,int adrr)
{
  int adr;
  int l;
  adr=adrr;
  for(l=0;l<4;l++)
  EEPROM.write(adr+l,buffer[l]);
  }

//加载EEPROM
void CardLoad(int adrr,byte *b)
{
  int adr,l;
  adr=adrr;
  for(l=0;l<4;l++)
  {
    b[l]=EEPROM.read(adr+l);
  }
}

//舵机运动
void Unlock()
{
  se.set_angle(121,20,50);   
  delay(5000);
  se.set_angle(121,280,50);   
  delay(1000);
}
