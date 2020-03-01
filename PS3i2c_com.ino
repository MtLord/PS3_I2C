#include <Wire.h>

#include <PS3BT.h>
#include <PS3USB.h>
#include <usbhub.h>


USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
//#define USBMODE
#ifdef USBMODE
PS3USB PS3(&Usb);
#else
BTD Btd(&Usb);
//PS3BT PS3(&Btd);
PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch
#endif

bool printTemperature, printAngle;

bool int_flag=false;

                                  // Set CS pin

void setup() 
{
   Serial.begin(115200);
   
if (Usb.Init() == -1) 
  {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));
   Wire.begin(0x74); // スレーブのアドレスを0x74に設定
   Wire.setClock(400000);
   Wire.onRequest(IntCallBack); // マスターに呼ばれたときに呼び出す関数
   Serial.print(F("\r\nI2C Set Up Finished"));
//   pinMode(LED_BUILTIN, OUTPUT);
}

const int IN[8] = {0, 0, LeftHatX, LeftHatY, RightHatX, RightHatY, L2, R2};
const int BUTTON[2][8] = {{LEFT, DOWN, RIGHT, UP, START, R3, L3, SELECT},{SQUARE, CROSS, CIRCLE, TRIANGLE, R1, L1, 0, 0}};
byte data[8];

int ledcount=0;
int state = 0;
void IntCallBack()
{
  Wire.write(data[0]);
   Wire.write(data[1]);
   Wire.write(data[2]);
   Wire.write(data[3]);
   Wire.write(data[4]);
   Wire.write(data[5]);
   Wire.write(data[6]);
   Wire.write(data[7]);
  

}
void ReadButton() 
{
  if (PS3.PS3Connected) 
  {
    data[0] = data[1] = 0;
    for (int j = 0; j < 8; j++)
      data[0] = (data[0] << 1) + PS3.getButtonPress(BUTTON[0][j]);
    for (int j = 0; j < 6; j++)
      data[1] = (data[1] << 1) + PS3.getButtonPress(BUTTON[1][j]);
    data[1] = data[1] << 2;

    
    data[2] = PS3.getAnalogHat(LeftHatX);
    data[3] = PS3.getAnalogHat(LeftHatY);
    data[4] = PS3.getAnalogHat(RightHatX);
    data[5] = PS3.getAnalogHat(RightHatY);
    data[6] = PS3.getAnalogButton(L2);
    data[7] = PS3.getAnalogButton(R2);
  } 
  else 
  {
    for (int i = 0; i < 8; i++)  data[i] = 0;
    for (int i = 2; i < 6; i++)  data[i] = 127;
  }
}

void putchar_debug()
{
   Serial.print(PS3.getAnalogHat(RightHatY));
    Serial.print("\r\n");
}

void loop() 
{
  Usb.Task();
  if (PS3.getButtonClick(PS) && PS3.PS3Connected)
  {
     PS3.disconnect();
  }
     ReadButton() ;
 
}
