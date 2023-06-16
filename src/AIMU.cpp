/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗ 
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝
                                                                
 File: AIMU.cpp  
 Author: [name]
===============================================================
 [description here]
 
 Developer: [name], Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-05
 Version: 000
===============================================================
*/

/**
 * [x] application shall execute with minimum required arguments
 * [x] application shall able to load config file as mentioned from aguments
 * [x] the config file shall be shown registered TC and registered TM (which validated from FSW)
 * [x] the TC and TM in unordered_map (which will able to use find key)
 * [ ] every switch, boolean, shared data, shared value can be in form of atomic & shared pointers
 * - Aroonsak Bootchai
*/

/** At main thread,
 * 1 ) [X] read and check arguments is correct from defined
 * 2 ) [X] read file from argument, the config shall be parsed from the file then close file
 * 3 ) [X] read config, 
 * [ ] if key have __TC__, parse the line by TC::parse then push TC to unordered_map TCTable
 * [ ] if key have __TM__, parse the line by TM::parse then push TM to unordered_map TMTable
 * [ ] else push the config to unordered_map Config 
 * 4 ) [x] from config, parse the line by Config::parse then push Config to unordered_map ConfigTable
 * 5 ) [x] TMFunction (unordered_map)
 * 6 ) [x] TCFunction (unordered_map)
 * 7 ) [x] detach a thread, Command Thread with (reference)TMTable, (reference)TCTable, (reference)TMFUnction, (reference)TCFunction and Config as arguments to thread
 * 8 ) [x] while loop to prevent the main thread stopped
 * - Aroonsak Bootchai
*/

/** 1 [ ] detached thread (Pthread) named Command thread,
 *  reading message from message queue (using Config["mqueue"] as name ) and parse that to MsgPacket struct
 * (message packet is a struct that  ...), after parsed,
 * [ ] if MsgPacket.ctrlType == 0 then find MsgPacket.commandID in TMFunction, exec TCFunc(MsgPacket)
 * [ ] if MsgPacket.ctrlType == 2 then find MsgPacket.commandID in TCFunction, exec TMFunc(MsgPacket)
 * - Aroonsak Bootchai
**/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <fmt/format.h>
#include <atomic>
#include <mqueue.h>
#include <functional>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <bitset>
#include <iomanip>

#include "../include/CExecutor.hpp"
#include "../include/CConfig.hpp"
#include "../include/CLog.hpp"
#include "../include/CDataPool.hpp"
#include "../include/CMqueue.hpp"
#include "../include/ITTC.hpp"
#include "../include/CCommand.hpp"

using namespace std;

//! Write on paper all config

#define MPU9250_ADDRESS 0x68      // Gyroscope , Accelerometer , Temperature sensor
#define AK8963_SLAVE_ADDRESS 0x0C // Magetometer
#define BMP280_SLAVE_ADDRESS 0x76 // Preesure sensor

//! Register for config address in MPU9250

#define INT_PIN_CFG 0x37
#define CONFIG 0x1A
#define SMPLRT_DIV 0x19

//! Register for config address in AK8963

#define CNTL1 0x0A

//! Register for config address in BMP280
#define CTRL_MEAS 0xF4

//! Register for get data address

//? For MPU9250
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define MTM_XOUT_H 0x04
#define MTM_XOUT_L 0x03
#define MTM_YOUT_H 0x06
#define MTM_YOUT_L 0x05
#define MTM_ZOUT_H 0x08
#define MTM_ZOUT_L 0x07
#define MTM_Status 0x09

#define TEMP_OUT 0x41
//?

//? For BMP280
#define TEMP_XLSB 0xFC
#define TEMP_LSB 0xFB
#define TEMP_MSB 0xFA
#define PRESSURE_XLSB 0xF9
#define PRESSURE_LSB 0xF8
#define PRESSURE_MSB 0xF7
//?

//?Set baseline_time (S)
#define baseline_time 5

using namespace std;

//? Set  bus ID
const char *bus_ID = "/dev/i2c-4";

class accelerometer
{

private:
    // int MPU9250_Interface = wiringPiI2CSetup(MPU9250_ADDRESS); //! It initializes the I2C interface and returns a file descriptor that can be used for subsequent I2C operations. (for default  bus , default  bus is 1)
    int MPU9250_Interface = wiringPiI2CSetupInterface(bus_ID, MPU9250_ADDRESS); //! It initializes the I2C interface and returns a file descriptor that can be used for subsequent I2C operations.(for another  bus)

    uint8_t X_HIGH_Accelerometer, X_LOW_Accelerometer, Y_HIGH_Accelerometer, Y_LOW_Accelerometer, Z_HIGH_Accelerometer, Z_LOW_Accelerometer;
    int16_t X_RAW_Accelerometer, Y_RAW_Accelerometer, Z_RAW_Accelerometer;
    int32_t X_32bit_Accelerometer, Y_32bit_Accelerometer, Z_32bit_Accelerometer;
    float X_Accelerometer, Y_Accelerometer, Z_Accelerometer;
    pthread_t Accelerometer_thread;

    static void *helper(void *arg)
    {
        accelerometer *mt = reinterpret_cast<accelerometer *>(arg);
        mt->Accelerometer_Active();
        return 0;
    }

    void Accelerometer_Active()
    {
        Accelerometer_Readrawdata();  // Read raw data from sensor
        Accelerometer_Mergedata();    // Merge high bit and low bit from Accelerometer_Readrawdata()
        Accelerometer_Convertdata();  // Convert data from  Accelerometer_Mergedata() to physical value
        Accelerometer_Printdata();    // Print physical value
        Accelerometer_ConvertToSignint(); // Convert physical value to int 32 bit
        Accelerometer_Status(); // Check Status of Accelerometer

        // Accelerometer_Readrawdata_print(); // Print data from Accelerometer_Readrawdata()
        // Accelerometer_Mergedata_print();   // Print data from Accelerometer_Mergedata()
    }

public:
    void start_accelerometer()
    {
        pthread_create(&Accelerometer_thread, 0, helper, this);
    }

    void join_acc()
    {
        pthread_join(Accelerometer_thread, 0);
    }

    void Accelerometer_Readrawdata()
    {
        X_HIGH_Accelerometer = wiringPiI2CReadReg8(MPU9250_Interface, ACCEL_XOUT_H);
        X_LOW_Accelerometer = wiringPiI2CReadReg8(MPU9250_Interface, ACCEL_XOUT_L);
        Y_HIGH_Accelerometer = wiringPiI2CReadReg8(MPU9250_Interface, ACCEL_YOUT_H);
        Y_LOW_Accelerometer = wiringPiI2CReadReg8(MPU9250_Interface, ACCEL_YOUT_L);
        Z_HIGH_Accelerometer = wiringPiI2CReadReg8(MPU9250_Interface, ACCEL_ZOUT_H);
        Z_LOW_Accelerometer = wiringPiI2CReadReg8(MPU9250_Interface, ACCEL_ZOUT_L);
    }

    void Accelerometer_Mergedata()
    {
        X_RAW_Accelerometer = ((int16_t)(X_HIGH_Accelerometer << 8 | X_LOW_Accelerometer));
        Y_RAW_Accelerometer = ((int16_t)(Y_HIGH_Accelerometer << 8 | Y_LOW_Accelerometer));
        Z_RAW_Accelerometer = ((int16_t)(Z_HIGH_Accelerometer << 8 | Z_LOW_Accelerometer));
    }

    void Accelerometer_Convertdata()
    {
        //! Comment
        X_Accelerometer = (((float)X_RAW_Accelerometer) * 2) / 32768.0;
        Y_Accelerometer = (((float)Y_RAW_Accelerometer) * 2) / 32768.0;
        Z_Accelerometer = (((float)Z_RAW_Accelerometer) * 2) / 32768.0;
    }
    void Accelerometer_Printdata()
    {
        cout << "Accelerometer" << fixed << setprecision(4) << "\tX : " << X_Accelerometer << fixed << setprecision(4) << "\tY : " << Y_Accelerometer << fixed << setprecision(4) << "\tZ : " << Z_Accelerometer << endl;
    }

    void Accelerometer_ConvertToSignint()
    {
        Accelerometer_X_32bit = static_cast<int32_t>(X_Accelerometer * 100000);
        Accelerometer_Y_32bit = static_cast<int32_t>(Y_Accelerometer * 100000);
        Accelerometer_Z_32bit = static_cast<int32_t>(Z_Accelerometer * 100000);
    }

    void Accelerometer_Status()
    {
        if ((X_Accelerometer == Y_Accelerometer) && (Y_Accelerometer == Z_Accelerometer))
        {
            int Error_check = IMU_Error_Counter.fetch_add(1);
        }
    }

    void Accelerometer_Readrawdata_print() // print value from acc_sensor
    {
        cout << "READ X HIGH Accelerometer: " << bitset<8>(X_HIGH_Accelerometer) << endl;
        cout << "READ X LOW Accelerometer: " << bitset<8>(X_LOW_Accelerometer) << endl;
        cout << "READ Y HIGH Accelerometer: " << bitset<8>(Y_HIGH_Accelerometer) << endl;
        cout << "READ Y LOW Accelerometer: " << bitset<8>(Y_LOW_Accelerometer) << endl;
        cout << "READ Z HIHG Accelerometer: " << bitset<8>(Z_HIGH_Accelerometer) << endl;
        cout << "READ Z LOW Accelerometer: " << bitset<8>(Z_LOW_Accelerometer) << endl;
    }

    void Accelerometer_Mergedata_print() // print value from merge bit
    {
        cout << "MERGE X Accelerometer: " << (float)(X_RAW_Accelerometer) << endl;
        cout << "MERGE Y Accelerometer: " << (float)(Y_RAW_Accelerometer) << endl;
        cout << "MERGE Z Accelerometer: " << (float)(Z_RAW_Accelerometer) << endl;
    }
};

class gyroscope
{

private:
    // int MPU9250_Interface = wiringPiI2CSetup(MPU9250_ADDRESS); //! It initializes the I2C interface and returns a file descriptor that can be used for subsequent I2C operations. (for default  bus , default  bus is 1)
    int MPU9250_Interface = wiringPiI2CSetupInterface(bus_ID, MPU9250_ADDRESS); //! It initializes the I2C interface and returns a file descriptor that can be used for subsequent I2C operations. (for another  bus)

    uint8_t X_HIGH_Gyroscope, X_LOW_Gyroscope, Y_HIGH_Gyroscope, Y_LOW_Gyroscope, Z_HIGH_Gyroscope, Z_LOW_Gyroscope;
    int16_t X_RAW_Gyroscope, Y_RAW_Gyroscope, Z_RAW_Gyroscope;
    int32_t X_32bit_Gyroscope, Y_32bit_Gyroscope, Z_32bit_Gyroscope;
    float X_Gyroscope, Y_Gyroscope, Z_Gyroscope;
    pthread_t Gyroscope_thread;

    static void *helper(void *arg)
    {
        gyroscope *mt = reinterpret_cast<gyroscope *>(arg);
        mt->Gyroscope_Active();
        return 0;
    }

    void Gyroscope_Active()
    {

        Gyroscope_Readrawdata();  // Read raw data from sensor
        Gyroscope_Mergedata();    // Merge high bit and low bit from  Gyroscope_Readrawdata()
        Gyroscope_Convertdata();  // Convert data from   Gyroscope_Mergedata() to physical value
        Gyroscope_Printdata();    // Print physical value
        Gyroscope_ConvertToSignint(); // Convert physical value to int 32 bit
        Gyroscope_Status(); // Check Status of Gyroscope
      
        // Gyroscope_Readrawdata_print(); // Print data from  Gyroscope_Readrawdata()
        // Gyroscope_Mergedata_print(); // Print data from  Gyroscope_Mergedata()
    }

public:
    void start_gyroscope()
    {
        pthread_create(&Gyroscope_thread, 0, helper, this);
    }

    void join_gyroscope()
    {
        pthread_join(Gyroscope_thread, 0);
    }

    void Gyroscope_Readrawdata()
    {
        X_HIGH_Gyroscope = wiringPiI2CReadReg8(MPU9250_Interface, GYRO_XOUT_H);
        X_LOW_Gyroscope = wiringPiI2CReadReg8(MPU9250_Interface, GYRO_XOUT_L);
        Y_HIGH_Gyroscope = wiringPiI2CReadReg8(MPU9250_Interface, GYRO_YOUT_H);
        Y_LOW_Gyroscope = wiringPiI2CReadReg8(MPU9250_Interface, GYRO_YOUT_L);
        Z_HIGH_Gyroscope = wiringPiI2CReadReg8(MPU9250_Interface, GYRO_ZOUT_H);
        Z_LOW_Gyroscope = wiringPiI2CReadReg8(MPU9250_Interface, GYRO_ZOUT_L);
    }

    void Gyroscope_Mergedata()
    {
        X_RAW_Gyroscope = ((int16_t)(X_HIGH_Gyroscope << 8 | X_LOW_Gyroscope));
        Y_RAW_Gyroscope = ((int16_t)(Y_HIGH_Gyroscope << 8 | Y_LOW_Gyroscope));
        Z_RAW_Gyroscope = ((int16_t)(Z_HIGH_Gyroscope << 8 | Z_LOW_Gyroscope));
    }

    void Gyroscope_Convertdata()
    {
        X_Gyroscope = (((float)X_RAW_Gyroscope) / 131);
        Y_Gyroscope = (((float)Y_RAW_Gyroscope) / 131);
        Z_Gyroscope = (((float)Z_RAW_Gyroscope) / 131);
    }
    void Gyroscope_Printdata()
    {
        cout << "Gyroscope" << fixed << setprecision(4) << "\tX : " << X_Gyroscope << fixed << setprecision(4) << "\tY : " << Y_Gyroscope << fixed << setprecision(4) << "\tZ : " << Z_Gyroscope << endl;
    }

    void Gyroscope_ConvertToSignint()
    {
        Gyroscope_X_16bit = static_cast<int16_t>(X_Gyroscope * 100);
        Gyroscope_Y_16bit = static_cast<int16_t>(Y_Gyroscope * 100);
        Gyroscope_Z_16bit = static_cast<int16_t>(Z_Gyroscope * 100);
    }

    void Gyroscope_Status()
    {
        if ((X_Gyroscope == Y_Gyroscope) && (Y_Gyroscope == Z_Gyroscope))
        {
            int Error_check = IMU_Error_Counter.fetch_add(1);
        }
    }

    void Gyroscope_Readrawdata_print()
    {
        cout << "READ X HIGH Gyroscope: " << bitset<8>(X_HIGH_Gyroscope) << endl;
        cout << "READ X LOW: Gyroscope" << bitset<8>(X_LOW_Gyroscope) << endl;
        cout << "READ Y HIGH: Gyroscope" << bitset<8>(Y_HIGH_Gyroscope) << endl;
        cout << "READ Y LOW: Gyroscope" << bitset<8>(Y_LOW_Gyroscope) << endl;
        cout << "READ Z HIGH: Gyroscope" << bitset<8>(Z_HIGH_Gyroscope) << endl;
        cout << "READ Z LOW: Gyroscope" << bitset<8>(Z_LOW_Gyroscope) << endl;
    }

    void Gyroscope_Mergedata_print()
    {
        cout << "MERGE X Gyroscope: " << bitset<16>(X_RAW_Gyroscope) << endl;
        cout << "MERGE Y Gyroscope: " << bitset<16>(Y_RAW_Gyroscope) << endl;
        cout << "MERGE Z Gyroscope: " << bitset<16>(Z_RAW_Gyroscope) << endl;
    }
};

class magnetometer
{

private:
    // int AK8963_Interface = wiringPiI2CSetup(AK8963_SLAVE_ADDRESS); //! It initializes the I2C interface and returns a file descriptor that can be used for subsequent I2C operations. (for default  bus , default  bus is 1)
    int AK8963_Interface = wiringPiI2CSetupInterface(bus_ID, AK8963_SLAVE_ADDRESS); //! It initializes the I2C interface and returns a file descriptor that can be used for subsequent I2C operations. (for another bus)

    uint8_t X_HIGH_Magnetometer, X_LOW_Magnetometer, Y_HIGH_Magnetometer, Y_LOW_Magnetometer, Z_HIGH_Magnetometer, Z_LOW_Magnetometer;
    int16_t X_RAW_Magnetometer, Y_RAW_Magnetometer, Z_RAW_Magnetometer;
    int32_t X_32bit_Magnetometer, Y_32bit_Magnetometer, Z_32bit_Magnetometer;
    float X_Magnetometer, Y_Magnetometer, Z_Magnetometer;

    // For calibrate
    float mres, X_Caribate, Y_Caribate, Z_Caribate, Magnetometer_scale, avg_rad;
    int magnetometer_max = -32767;
    int magnetometer_min = 32767;
    int magnetometer_Temperature;

    pthread_t magnetometer_thread;

    static void *helper(void *arg)
    {
        magnetometer *mt = reinterpret_cast<magnetometer *>(arg);
        mt->magnetometer_active();
        return 0;
    }

    void magnetometer_active()
    {
        Magnetometer_Readrawdata();  // Read raw data from sensor
        Magnetometer_Mergedata();    // Merge high bit and low bit from  Magnetometer_Readrawdata()
        Magnetometer_Convertdata();  // Convert data from   Magnetometer_Mergedata() to physical value
        Magnetometer_Printdata();    // Print physical value
        Magnetometer_ConvertToSignint(); // Convert physical value to int 32 bit
        Magnetometer_Status();

        // Magnetometer_Readrawdata_print(); // Print data from  Magnetometer_Readrawdata()
        // Magnetometer_Mergedata_print(); // Print data from  Magnetometer_Mergedata()
    }

public:
    void start_magnetometer()
    {
        pthread_create(&magnetometer_thread, 0, helper, this);
    }

    void join_magnetometer()
    {
        pthread_join(magnetometer_thread, 0);
    }

    void Magnetometer_Readrawdata()
    {

        X_HIGH_Magnetometer = wiringPiI2CReadReg8(AK8963_Interface, MTM_XOUT_H);
        X_LOW_Magnetometer = wiringPiI2CReadReg8(AK8963_Interface, MTM_XOUT_L);
        Y_HIGH_Magnetometer = wiringPiI2CReadReg8(AK8963_Interface, MTM_YOUT_H);
        Y_LOW_Magnetometer = wiringPiI2CReadReg8(AK8963_Interface, MTM_YOUT_L);
        Z_HIGH_Magnetometer = wiringPiI2CReadReg8(AK8963_Interface, MTM_ZOUT_H);
        Z_LOW_Magnetometer = wiringPiI2CReadReg8(AK8963_Interface, MTM_ZOUT_L);
    }

    void Magnetometer_Mergedata()
    {
        X_RAW_Magnetometer = ((int16_t)(X_HIGH_Magnetometer << 8 | X_LOW_Magnetometer));
        Y_RAW_Magnetometer = ((int16_t)(Y_HIGH_Magnetometer << 8 | Y_LOW_Magnetometer));
        Z_RAW_Magnetometer = ((int16_t)(Z_HIGH_Magnetometer << 8 | Z_LOW_Magnetometer));
    }

    void Magnetometer_Convertdata()
    {
        mres = 4912.0 / 32760.0;
        X_Caribate = ((float)(wiringPiI2CReadReg8(AK8963_Interface, 0x10) - 128) / 256 + 1.0);
        Y_Caribate = ((float)(wiringPiI2CReadReg8(AK8963_Interface, 0x11) - 128) / 256 + 1.0);
        Z_Caribate = ((float)(wiringPiI2CReadReg8(AK8963_Interface, 0x12) - 128) / 256 + 1.0);
        magnetometer_Temperature = ((int16_t)(X_HIGH_Magnetometer << 8 | Y_LOW_Magnetometer));

        if (magnetometer_Temperature > magnetometer_max)
            magnetometer_max = magnetometer_Temperature;

        if (magnetometer_Temperature < magnetometer_min)
            magnetometer_min = magnetometer_Temperature;

        Magnetometer_scale = (magnetometer_max + magnetometer_min) / 2;
        avg_rad = (Magnetometer_scale + 1 + 1) / 3;
        Magnetometer_scale = avg_rad / Magnetometer_scale;

        X_Magnetometer = (((float)X_RAW_Magnetometer * mres * X_Caribate));
        Y_Magnetometer = (((float)Y_RAW_Magnetometer * mres * Y_Caribate));
        Z_Magnetometer = (((float)Z_RAW_Magnetometer * mres * Z_Caribate * Magnetometer_scale));

        wiringPiI2CReadReg8(AK8963_Interface, MTM_Status); // Clear data ready flag by reading ST2 register
    }

    void Magnetometer_Printdata()
    {
        cout << "Magnetometer" << fixed << setprecision(4) << "\tX : " << X_Magnetometer << fixed << setprecision(4) << "\tY : " << Y_Magnetometer << fixed << setprecision(4) << "\tZ : " << Z_Magnetometer << endl;
    }

    void Magnetometer_ConvertToSignint()
    {
        Magnetometer_X_32bit = static_cast<int32_t>(X_Magnetometer * 100000);
        Magnetometer_Y_32bit = static_cast<int32_t>(Y_Magnetometer * 100000);
        Magnetometer_Z_32bit = static_cast<int32_t>(Z_Magnetometer * 100000);
    }

    void Magnetometer_Status()
    {
        if ( X_Magnetometer == Y_Magnetometer )
        {
            int Error_check = IMU_Error_Counter.fetch_add(1);
        }
    }

    void Magnetometer_Readrawdata_print()
    {
        cout << "READ X HIGH Magnetometer: " << bitset<8>(X_HIGH_Magnetometer) << endl;
        cout << "READ X LOW Magnetometer: " << bitset<8>(X_LOW_Magnetometer) << endl;
        cout << "READ Y HIGH Magnetometer: " << bitset<8>(Y_HIGH_Magnetometer) << endl;
        cout << "READ Y LOW Magnetometer: " << bitset<8>(Y_LOW_Magnetometer) << endl;
        cout << "READ Z HIGH Magnetometer: " << bitset<8>(Z_HIGH_Magnetometer) << endl;
        cout << "READ Z LOW Magnetometer: " << bitset<8>(Z_LOW_Magnetometer) << endl;
    }

    void Magnetometer_Mergedata_print()
    {
        cout << "MERGE X Magnetometer: " << bitset<16>(X_RAW_Magnetometer) << endl;
        cout << "MERGE Y Magnetometer: " << bitset<16>(Y_RAW_Magnetometer) << endl;
        cout << "MERGE Z Magnetometer: " << bitset<16>(Z_RAW_Magnetometer) << endl;
    }
};

class bmp280
{
private:
    // int BMP280_Interface = wiringPiI2CSetup(0x76); //! It initializes the I2C interface and returns a file descriptor that can be used for subsequent I2C operations. (for default  bus , default  bus is 1)
    int BMP280_Interface = wiringPiI2CSetupInterface(bus_ID, BMP280_SLAVE_ADDRESS); //! It initializes the I2C interface and returns a file descriptor that can be used for subsequent I2C operations. (for another  bus)

    uint8_t Pressure_msb, Pressure_lsb, Pressure_xlb;
    int32_t Raw_Preesure, Raw_Temperature;
    double Pressure;
    float Real_Pressure;

    uint8_t Temperature_xlsb, Temperature_lsb, Temperature_msb;
    int32_t Data_32bit_temperature, pressure32bit, altitude32bit;
    float Temperature;

    double adc_T;
    double Real_Temperature;
    double QNH, Altitude;
    int baseline_count = 0, firstround = 1;

    pthread_t bmp_thread;

    static void *helper(void *arg)
    {
        bmp280 *mt = reinterpret_cast<bmp280 *>(arg);
        mt->bmp_active();
        return 0;
    }

    void bmp_active()
    {

        // Magnetometer_Readrawdata_print(); // Print data from  Magnetometer_Readrawdata()
        // Magnetometer_Mergedata_print(); // Print data from  Magnetometer_Mergedata()

        Temperature_Readrawdata();  // Read raw data from sensor
        Temperature_Mergerawdata(); // Merge high bit and low bit from  Temperature_Readrawdata()
        Temperature_Convertdata();  // Convert data from   Temperature_Mergerawdata() to physical value

        Pressure_Readdata();        // Read raw data from sensor
        Pressure_Mergedata();       // Merge high bit and low bit from   Pressure_Readdata()
        Pressure_Convertdata();     // Convert data from   Pressure_Mergedata() to physical value
        Pressure_ConvertAltitude(); // Convert Pressure to Altitude
        BMP_Print();                // Print all physical value
        BMP_ConverttoSignint();
    }

public:
    void start_bmp()
    {
        pthread_create(&bmp_thread, 0, helper, this);
    }

    void join_bmp()
    {
        pthread_join(bmp_thread, 0);
    }

    void baseline_set()
    { // This function will compare baseline_count and baseline_time if baaseline_count < baseline_time we will use pressure_baseline() for store pressure value in QNH but  if baseline_count equl baseline_time we will get average of QNH
        Temperature_Readrawdata();
        Temperature_Mergerawdata();
        Temperature_Convertdata();
        if (baseline_count < baseline_time)
        {
            pressure_baseline();
        }
        else
        {
            if (baseline_count == baseline_time)
            {
                // QNH is value for altimeter setting
                QNH = QNH / baseline_time;
                baseline_count++;
                cout << "Real QNH : " << QNH << endl;
            }
        }
    }

    void Temperature_Readrawdata()
    {
        Temperature_msb = wiringPiI2CReadReg8(BMP280_Interface, TEMP_MSB);
        Temperature_lsb = wiringPiI2CReadReg8(BMP280_Interface, TEMP_LSB);
        Temperature_xlsb = wiringPiI2CReadReg8(BMP280_Interface, TEMP_XLSB);
    }

    void Temperature_Mergerawdata()
    {
        Raw_Temperature = ((int32_t)((Temperature_msb << 16) | (Temperature_lsb << 8) | Temperature_xlsb) >> 4);
    }

    void Temperature_Convertdata()
    {
        double var1, var2;
        var1 = (Raw_Temperature / 16384.0 - 28251 / 1024.0) * 26662;
        var2 = Raw_Temperature / 131072.0 - 28251 / 8192.0;
        var2 = var2 * var2 * -1000;
        adc_T = (var1 + var2);
        Real_Temperature = adc_T / 5120;
    }

    void pressure_baseline()
    {
        //! This function will get Pressure to add in QNH
        Pressure_Readdata();
        Pressure_Mergedata();
        Pressure_Convertdata();
        // if this is first round we will not store that value because first value is erorr
        if (firstround == 1)
        {
            firstround = 0;
        }
        else
        {
            QNH = QNH + Real_Pressure;
            cout << "QNH : " << QNH << endl;
            baseline_count++;
        }
    }

    void Pressure_Readdata()
    {
        Pressure_msb = wiringPiI2CReadReg8(BMP280_Interface, PRESSURE_MSB);
        Pressure_lsb = wiringPiI2CReadReg8(BMP280_Interface, PRESSURE_LSB);
        Pressure_xlb = wiringPiI2CReadReg8(BMP280_Interface, PRESSURE_XLSB);
    }

    void Pressure_Mergedata()
    {
        Raw_Preesure = ((int32_t)((Pressure_msb << 16) | (Pressure_lsb << 8) | Pressure_xlb) >> 4);
    }

    void Pressure_Convertdata()
    {
        double var1, var2, var3;
        //! Edit here
        var1 = adc_T / 2.0 - 64000.0;
        var2 = var1 * var1 * -7 / 32768.0;
        var2 = var2 + var1 * 200 * 2;
        var2 = var2 / 4.0 + 3591 * 65536.0;
        var1 = (3024 * var1 * var1 / 524288.0 + -10936 * var1) / 524288.0;
        var1 = (1.0 + var1 / 32768.0) * 38955;
        Pressure = (1048576.0 - Raw_Preesure) * 1.0;

        Pressure = (Pressure - var2 / 4096.0) * 6250.0 / var1;
        var1 = 6000 * Pressure * Pressure / 2147483648.0;
        var2 = Pressure * -14600 / 32768.0;
        Pressure = (double)(Pressure + (var1 + var2 + 15500) / 16.0);

        Real_Pressure = Pressure / 100;
    }

    void Pressure_ConvertAltitude()
    {
        Altitude = ((pow((QNH / Real_Pressure), (1.0 / 5.257)) - 1) * (Real_Temperature + 273.15)) / 0.0065;
    }

    void BMP_Print()
    {
        cout << "Pressure : \t" << Real_Pressure << "\ttemperature : " << Real_Temperature << endl;
        cout << "Altitude : \t" << Altitude << endl;
    }

    void BMP_ConverttoSignint()
    {
        Temperature_16bit = static_cast<int32_t>(Real_Temperature * 100);
        Pressure_32bit = static_cast<int32_t>(Real_Pressure * 100000);
        Altitude_32bit = static_cast<int32_t>(Altitude * 100000);
        
    }
};

void IMU(accelerometer acc, gyroscope gyro, magnetometer mag, bmp280 bmp)
{
    // Start thread
    acc.start_accelerometer();
    gyro.start_gyroscope();
    mag.start_magnetometer();
    bmp.start_bmp();
    //

    // thread join
    acc.join_acc();
    gyro.join_gyroscope();
    mag.join_magnetometer();
    bmp.join_bmp();
    //
}

std::atomic<uint8_t> thread_status = 0;
std::atomic<int> IMU_Error_Counter(0);
std::atomic<uint8_t> IMU_status = 0; //IMU Status
std::atomic<int32_t> Accelerometer_X_32bit = 0; //X Accelerometer
std::atomic<int32_t> Accelerometer_Y_32bit = 0; //Y Accelerometer
std::atomic<int32_t> Accelerometer_Z_32bit = 0; //Z Accelerometer
std::atomic<int16_t> Gyroscope_X_16bit = 0; //Z Gyroscope
std::atomic<int16_t> Gyroscope_Y_16bit = 0; //Y Gyroscope
std::atomic<int16_t> Gyroscope_Z_16bit = 0; //Z Gyroscope
std::atomic<int32_t> Magnetometer_X_32bit = 0; //X Magnetometer
std::atomic<int32_t> Magnetometer_Y_32bit = 0; //Y Magnetometer
std::atomic<int32_t> Magnetometer_Z_32bit = 0; //Z Magnetometer
std::atomic<int16_t> Temperature_16bit = 0; //Temperature
std::atomic<int32_t> Pressure_32bit = 0; //Pressure
std::atomic<int32_t> Altitude_32bit = 0; //Altitude


int IMU_TC_1 (const TTCInternal* ttc,const Services* services) {
    //working some thing here
    printf("working TC_1");
    thread_status = 1;
    IMU_status = 0; // Reset Error
    std::thread t1([=](){
        TTCInternal ttc = ttc;
        ttc.ctrlType = CtrlType::TC_ret;
        services->MQ_Return->Send(ttc);

    int AK8963_Interface = wiringPiI2CSetupInterface(bus_ID, AK8963_SLAVE_ADDRESS);
    int BMP280_Interface = wiringPiI2CSetupInterface(bus_ID, BMP280_SLAVE_ADDRESS);
    int MPU9250_Interface = wiringPiI2CSetupInterface(bus_ID, MPU9250_ADDRESS);

    wiringPiI2CWriteReg8(MPU9250_Interface, INT_PIN_CFG, 0x02); // Enable Pass-Through mode for AK8963
    wiringPiI2CWriteReg8(AK8963_Interface, CNTL1, 0x12);        // Enable magnetometernetometer
    // Config MPU9250
    wiringPiI2CWriteReg8(MPU9250_Interface, CONFIG, 0x03);
    wiringPiI2CWriteReg8(MPU9250_Interface, SMPLRT_DIV, 0x04);
    // Config BMP280
    wiringPiI2CWriteReg8(BMP280_Interface, CTRL_MEAS, 0x27);

    // Create Object
    accelerometer acc;
    gyroscope gyro;
    magnetometer mag;
    bmp280 bmp;

    int baseline_count = 0;
    int maintime_count = 0;
    if (baseline_count == 0)
        {
            for (baseline_count = 0; baseline_count <= baseline_time + 1; baseline_count++) // You can set loop round by baseline_time
            {
                cout << baseline_count << "/" << baseline_time << " S" << endl;
                bmp.baseline_set();
                delay(1000);
                //! Note in baseline_set() We will lose 1 round becase first round to run this code preesure value will error value that's why for this loop baseline_time + 1
            }
        }

    IMU_status = 0; // Ready Status

        while(thread_status == 1){
            // write a debug log
            //store data to datapool as [tmID] = [value]
            
        cout << "IMU DATA : " << endl;
        IMU(acc, gyro, mag, bmp); // Start all sensor
        IMU_status = 1; // Sampling Status
        cout << "-------------------------------------------------------------------------" << endl;
          
            if (IMU_Error_Counter == 3)
            {
                IMU_status = 2; // Disconnect Status
            }

            (*services->Pool->tm)[1] = IMU_status;
            (*services->Pool->tm)[2] = Accelerometer_X_32bit;
            (*services->Pool->tm)[3] = Accelerometer_Y_32bit;
            (*services->Pool->tm)[4] = Accelerometer_Z_32bit;
            (*services->Pool->tm)[5] = Gyroscope_X_16bit;
            (*services->Pool->tm)[6] = Gyroscope_Y_16bit;
            (*services->Pool->tm)[7] = Gyroscope_Z_16bit;
            (*services->Pool->tm)[8] = Magnetometer_X_32bit;
            (*services->Pool->tm)[9] = Magnetometer_Y_32bit;
            (*services->Pool->tm)[10] = Magnetometer_Z_32bit;
            (*services->Pool->tm)[11] = Temperature_16bit;
            (*services->Pool->tm)[12] = Pressure_32bit;
            (*services->Pool->tm)[13] = Altitude_32bit;

            IMU_Error_Counter = 0; //Reset Error Counter
            //sleep thread to prevent over perform
           
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        // printf("thread closed");
        std::cout <<"thread TC_1 closed" << std::endl << std::flush;
        return 0;
    });
    t1.detach();
    return 0;
};

int IMU_TC_2 (const TTCInternal* ttc,const Services* services) {
    thread_status = 0;
    return 0;
};

int IMU_TM_1 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[1];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_2 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[2];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_3 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[3];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_4 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[4];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_5 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[5];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_6 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[6];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_7 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[7];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_8 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[8];
    services->MQ_Return -> Send(_ttc);
    return 0;
};
int IMU_TM_9 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[9];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_10 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[10];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_11 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[11];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_12 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[12];
    services->MQ_Return -> Send(_ttc);
    return 0;
};

int IMU_TM_13 (const TTCInternal* ttc,const Services* services) {
    TTCInternal _ttc = *ttc;
    _ttc.ctrlType = CtrlType::TM_ret;
    _ttc.value = (*services->Pool->tm)[13];
    services->MQ_Return -> Send(_ttc);
    return 0;
};


int main(int argc, char* argv[]) {
    Args args = parseArguments(argc, argv);
    Config* config = parseConfigFile(args.configFile);
    if (!config) {
        return 1;
    }

    Logger *logger = new Logger();
    logger -> setLogLevel(LogLevel::DEBUG);
    logger -> setModuleID(3021);

    Datapool *pool = new Datapool("/302.1");
    CMqueue *mq_request = new CMqueue("/302.1");
    CMqueue *mq_return = new CMqueue("/308");
    CMqueue *mq_log = new CMqueue("/log");

    if (mq_return->OpenWrite() == -1) {
        logger -> log(LogLevel::ERROR, "Failed to open message queue");
        return EXIT_FAILURE;
    }else{
        logger -> log(LogLevel::INFO, "connected MQ_Return");
    }

    if (mq_request->OpenRead() == -1) {
        logger -> log(LogLevel::ERROR, "Failed to open message queue");
        return EXIT_FAILURE;
    }else{
        logger -> log(LogLevel::INFO, "connected MQ_Request");
    }

    if (mq_log->OpenWrite() == -1) {
        logger -> log(LogLevel::ERROR, "Failed to open message queue");
        return EXIT_FAILURE;
    }else{
        logger -> log(LogLevel::INFO, "connected MQ_Logger");
    }
    // ======================= Start subsystem code ========================================

    std::unordered_map <int,std::function<void(TTCInternal*,Services*)>> tc_actions;
    std::unordered_map <int,std::function<void(TTCInternal*,Services*)>> tm_actions;
    // tc_actions[<<commandID>>] = function()
    tc_actions[1] = IMU_TC_1;
    tc_actions[2] = IMU_TC_2;
    tm_actions[1] = IMU_TM_1;
    tm_actions[2] = IMU_TM_2;
    tm_actions[3] = IMU_TM_3;
    tm_actions[4] = IMU_TM_4;
    tm_actions[5] = IMU_TM_5;
    tm_actions[6] = IMU_TM_6;
    tm_actions[7] = IMU_TM_7;
    tm_actions[8] = IMU_TM_8;
    tm_actions[9] = IMU_TM_9;
    tm_actions[10] = IMU_TM_10;
    tm_actions[11] = IMU_TM_11;
    tm_actions[12] = IMU_TM_12;
    tm_actions[13] = IMU_TM_13;


    // ===============================================================
    CommandThreadParams *params = new CommandThreadParams;
    params->mq_return = mq_return;
    params->mq_request = mq_request;
    params->mq_logservice = mq_log;
    params->tc_actions = &tc_actions;
    params->tm_actions = &tm_actions;
    params->pool = pool;
    params->logger = logger;

    // Start the CommandThread as a detached thread
    pthread_t commandThread;
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    pthread_create(&commandThread, &threadAttr, commandThreadFunc, params); 

    
    int input;
    while (true) {
        // Do other tasks if needed
        std::cin >> input;
    }

    pthread_attr_destroy(&threadAttr);
    return 0;
}
