#include <Servo.h>    // 声明调用Servo.h库
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

Servo myservo;        // 创建一个舵机对象
int pos = 0;          
int lastPos = -1;

//0-静止，1-运动
int lastState = 0;
int lastCurrState = 0;
int currState = 0;
//计算两次测量的时间间隔dt，以秒为单位
unsigned long lastTime = 0;
const int16_t minSpeed = 3;

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

void initServo() {
    //舵机
     myservo.attach(9);  // 将引脚9上的舵机与声明的舵机对象连接起来
}

void initRoateButton() {
    //旋钮
    pinMode(A0,INPUT);
    pinMode(3,OUTPUT);
}

void initMPU() {
    //接入i2c总线
    Wire.begin();
    //初始化设备
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    //链接设备
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);//38400
  initServo();
  initMPU();
  initRoateButton();
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

int isManu(int lpos, int currpos) {
    //是否手动
    if (abs(lpos - currpos) >= 5)
    {
        return 1;
    }
    return 0;
}

void manuControl() {
    myservo.write(pos);
    delay(50);
}

int getCurrState() {
    if (abs(ax) < minSpeed && abs(ay) < minSpeed && abs(az) < minSpeed)
    {
        return 0;
    }
    return 1;
}

void loop() {
  // put your main code here, to run repeatedly:
//1.优先判断手动控制
//2.没有手动控制下，判断陀螺仪数据
//3.如果从运动中停止下来，进行对应操作。如果从停止变为运动，进行对应操作。
    Serial.println((map(analogRead(A0), 0, 1023, 0, 60)));
    pos = map(analogRead(A0), 0, 1023, 0, 60);
    if (lastPos == -1)
    {
        lastPos = pos;
    }

    if(isManu(lastPos, pos) == 1) {
        lastPos = pos;
        manuControl();
        lastTime = 0;
        return;
    }

    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    // accelgyro.getAcceleration(&ax, &ay, &az);
    // accelgyro.getRotation(&gx, &gy, &gz);
    fix();
    //计算两次测量的时间间隔dt，以毫秒为单位，50天后溢出---每次重启都会重置
    unsigned long nCurTime = millis();
    if (lastTime == 0)
    {
        lastTime = nCurTime;
    }
    
    currState = getCurrState();
    if (currState == lastCurrState)
    {
        if (nCurTime - lastTime >= 5000)
        {
            //更新时间
            lastTime = nCurTime;
            if (currState != lastState)
            {
                //更新状态
                lastState = currState;
                if (currState == 1)
                {
                    //由静止变为运动
                    myservo.write(60);
                    delay(50);
                }else {
                    //由运动变为静止
                    myservo.write(0);
                    delay(50);
                }
            }
        }
        
    }else {
        //状态变化，直接更新时间
        lastTime = nCurTime;
        lastCurrState = currState;
    }

    Serial.print("a/g:\t");
    //x,y,z轴加速度
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");

    //x,y,z轴角速度
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

    delay(100);
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

    //    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
//    accelgyro.getAcceleration(&ax, &ay, &az);
    // accelgyro.getRotation(&gx, &gy, &gz);
}
