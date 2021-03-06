#include <Arduino.h>
#include <Wire.h>

#define MPU6050_ADDR 0x68       // MPU-6050 device address
#define MPU6050_SMPLRT_DIV 0x19 // MPU-6050 register address
#define MPU6050_CONFIG 0x1a
#define MPU6050_GYRO_CONFIG 0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1 0x6b

#define PIN_SDA 33 // SDA do MPU-6050
#define PIN_SCL 32 // SCL do MPU-6050

double offsetX = 0, offsetY = 0, offsetZ = 0;
double gyro_angle_x = 0, gyro_angle_y = 0, gyro_angle_z = 0;
float angleX, angleY, angleZ;
float interval, preInterval;
float acc_x, acc_y, acc_z, acc_angle_x, acc_angle_y;
float gx, gy, gz, dpsX, dpsY, dpsZ;

void calcRotation();

//Write I2C
void writeMPU6050(byte reg, byte data)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

//Read I2C
byte readMPU6050(byte reg)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.endTransmission(true);
    Wire.requestFrom(MPU6050_ADDR, 1 /*length*/);
    byte data = Wire.read();
    return data;
}

void setup()
{

    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0);    // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);

    Serial.begin(115200);
    delay(100);

    // Check whether it is connected normally
    if (readMPU6050(MPU6050_WHO_AM_I) != 0x68)
    {
        Serial.println("\nWHO_AM_I error.");
        while (true)
            ;
    }

    //Write setings
    writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);   // sample rate: 8kHz/(7+1) = 1kHz
    writeMPU6050(MPU6050_CONFIG, 0x00);       // disable DLPF, gyro output rate = 8kHz
    writeMPU6050(MPU6050_GYRO_CONFIG, 0x08);  // gyro range: ±500dps
    writeMPU6050(MPU6050_ACCEL_CONFIG, 0x00); // accel range: ±2g
    writeMPU6050(MPU6050_PWR_MGMT_1, 0x01);   // disable sleep mode, PLL with X gyro

    //Calibration
    Serial.print("Calculate Calibration");
    for (int i = 0; i < 3000; i++)
    {

        int16_t raw_acc_x, raw_acc_y, raw_acc_z, raw_t, raw_gyro_x, raw_gyro_y, raw_gyro_z;

        Wire.beginTransmission(MPU6050_ADDR);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU6050_ADDR, 14, true);

        raw_acc_x = Wire.read() << 8 | Wire.read();
        raw_acc_y = Wire.read() << 8 | Wire.read();
        raw_acc_z = Wire.read() << 8 | Wire.read();
        raw_t = Wire.read() << 8 | Wire.read();
        raw_gyro_x = Wire.read() << 8 | Wire.read();
        raw_gyro_y = Wire.read() << 8 | Wire.read();
        raw_gyro_z = Wire.read() << 8 | Wire.read();
        dpsX = ((float)raw_gyro_x) / 65.5;
        dpsY = ((float)raw_gyro_y) / 65.5;
        dpsZ = ((float)raw_gyro_z) / 65.5;
        offsetX += dpsX;
        offsetY += dpsY;
        offsetZ += dpsZ;
        if (i % 1000 == 0)
        {
            Serial.print(".");
        }
    }
    Serial.println();

    offsetX /= 3000;
    offsetY /= 3000;
    offsetZ /= 3000;

    Serial.print("offsetX : ");
    Serial.println(offsetX);
    Serial.print("offsetY : ");
    Serial.println(offsetY);
    Serial.print("offsetZ : ");
    Serial.println(offsetZ);
}

void loop()
{

    calcRotation();
    delay(500);

    //  Serial.print("angleX : ");
    //  Serial.print(angleX);
    //  Serial.print("    angleY : ");
    //  Serial.print(angleY);
    //  Serial.print("    angleZ : ");
    //  Serial.println(angleZ);
}

//Calculate angle from acceleration, gyro
void calcRotation()
{

    int16_t raw_acc_x, raw_acc_y, raw_acc_z, raw_t, raw_gyro_x, raw_gyro_y, raw_gyro_z;

    //Instructs the MPU-6050 to output data of 14 bytes in total from the register address 0×3B
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 14, true);

    //Read the output data, bit shift operation
    raw_acc_x = Wire.read() << 8 | Wire.read();
    raw_acc_y = Wire.read() << 8 | Wire.read();
    raw_acc_z = Wire.read() << 8 | Wire.read();
    raw_t = Wire.read() << 8 | Wire.read();
    raw_gyro_x = Wire.read() << 8 | Wire.read();
    raw_gyro_y = Wire.read() << 8 | Wire.read();
    raw_gyro_z = Wire.read() << 8 | Wire.read();

    //Convert to Unit G
    acc_x = ((float)raw_acc_x) / 16384.0;
    acc_y = ((float)raw_acc_y) / 16384.0;
    acc_z = ((float)raw_acc_z) / 16384.0;

    //Calculate angle from acceleration sensor
    acc_angle_y = atan2(acc_x, acc_z + abs(acc_y)) * 360 / -2.0 / PI;
    acc_angle_x = atan2(acc_y, acc_z + abs(acc_x)) * 360 / 2.0 / PI;

    dpsX = ((float)raw_gyro_x) / 65.5; // LSB sensitivity: 65.5 LSB/dps @ ±500dps
    dpsY = ((float)raw_gyro_y) / 65.5;
    dpsZ = ((float)raw_gyro_z) / 65.5;

    //Calculate the elapsed time from the time of the last calculation
    interval = millis() - preInterval;
    preInterval = millis();

    //Numerical integral
    gyro_angle_x += (dpsX - offsetX) * (interval * 0.001);
    gyro_angle_y += (dpsY - offsetY) * (interval * 0.001);
    gyro_angle_z += (dpsZ - offsetZ) * (interval * 0.001);

    //Complementary filter
    angleX = (0.996 * gyro_angle_x) + (0.004 * acc_angle_x);
    angleY = (0.996 * gyro_angle_y) + (0.004 * acc_angle_y);
    angleZ = gyro_angle_z;
    gyro_angle_x = angleX;
    gyro_angle_y = angleY;
    gyro_angle_z = angleZ;

    Serial.printf("ACC(%.2fg, %.2fg, %.2fg) GYRO(%.2f DPS, %.2f DPS, %.2f DPS)\n", acc_x, acc_y, acc_z, dpsX, dpsY, dpsZ);
}