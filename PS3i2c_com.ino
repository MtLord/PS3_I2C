#include <Wire.h>
#include <PS3BT.h>
#include <PS3USB.h>
#include <usbhub.h>

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
#define USBMODE
#ifdef USBMODE
PS3USB joystic(&Usb);
#else
BTD Btd(&Usb);
PS3BT joystic(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch
#endif

bool printTemperature, printAngle;
bool int_flag = false;

// Set CS pin

void setup()
{
  Serial.begin(115200);

  if (Usb.Init() == -1)
  {
    Serial.print(F("\r\nOSC did not start"));
    while (1)
      ; //halt
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));
  Wire.begin(0x74); // スレーブのアドレスを0x74に設定
  Wire.setClock(400000);
  Wire.onRequest(IntCallBack); // マスターに呼ばれたときに呼び出す関数
  Serial.print(F("\r\nI2C Set Up Finished"));
}

const int IN[8] = {0, 0, LeftHatX, LeftHatY, RightHatX, RightHatY, L2, R2};
const int BUTTON[2][8] = {{LEFT, DOWN, RIGHT, UP, START, R3, L3, SELECT}, {SQUARE, CROSS, CIRCLE, TRIANGLE, R1, L1, 0, 0}};
byte data[8];

int ledcount = 0;
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
  Serial.print("\r\nint!");
}
void ReadButton()
{
  if (joystic.PS3Connected)
  {
    data[0] = data[1] = 0;
    for (int j = 0; j < 8; j++)
      data[0] = (data[0] << 1) + joystic.getButtonPress(BUTTON[0][j]);
    for (int j = 0; j < 6; j++)
      data[1] = (data[1] << 1) + joystic.getButtonPress(BUTTON[1][j]);
    data[1] = data[1] << 2;

    data[2] = joystic.getAnalogHat(LeftHatX);
    data[3] = joystic.getAnalogHat(LeftHatY);
    data[4] = joystic.getAnalogHat(RightHatX);
    data[5] = joystic.getAnalogHat(RightHatY);
    data[6] = joystic.getAnalogButton(L2);
    data[7] = joystic.getAnalogButton(R2);
  }
  else
  {
    for (int i = 0; i < 8; i++)
      data[i] = 0;
    for (int i = 2; i < 6; i++)
      data[i] = 127;
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
  if (joystic.getButtonClick(PS) && joystic.PS3Connected)
  {
    joystic.disconnect();
  }
  ReadButton();
}
