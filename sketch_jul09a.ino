#include <Servo.h>    // 声明调用Servo.h库
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

Servo myservo;        // 创建一个舵机对象
int pos = 0;          // 变量pos用来存储舵机位置

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  // put your setup code here, to run once:

  //旋钮
//  Serial.begin(9600);
//  pinMode(A0,INPUT);
//  pinMode(3,OUTPUT);

  //舵机
//  myservo.attach(9);  // 将引脚9上的舵机与声明的舵机对象连接起来

  //接入i2c总线
    Wire.begin();
    Serial.begin(38400);
    //初始化设备
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    //链接设备
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

//将原始值修正
void fix() {
    ax /= 16384;
    ay /= 16384;
    az /= 16384;

    gx /= 131;
    gy /= 131;
    gz /= 131;
}

//干干其他事
void process() {

}

void loop() {
  // put your main code here, to run repeatedly:
//1.优先判断手动控制
//2.没有手动控制下，判断陀螺仪数据
//3.如果从运动中停止下来，进行对应操作。如果从停止变为运动，进行对应操作。

//  analogWrite(3, (map(analogRead(A0), 0, 1023, 0, 235)));
//  Serial.println((map(analogRead(A0), 0, 1023, 0, 180)));
//  pos = map(analogRead(A0), 0, 1023, 0, 180);
//  myservo.write(pos);  
//  delay(15);

    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    //对原始价值做处理
    fix();

    //干干其他事
    process();

    Serial.print("a/g:\t");
    //x,y,z轴加速度
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");

    //x,y,z轴角速度
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

    delay(500);
}

void test() {
  for(pos = 0; pos < 180; pos += 10){    // 舵机从0°转到180°，每次增加1°          
      myservo.write(pos);           // 给舵机写入角度   
      delay(500);                    // 延时15ms让舵机转到指定位置
   }
    for(pos = 180; pos>=1; pos-=10) {    // 舵机从180°转回到0°，每次减小1°                               
       myservo.write(pos);        // 写角度到舵机     
       delay(500);                 // 延时15ms让舵机转到指定位置
    } 
}
