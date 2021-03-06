//=====================================================================================================
// IMU.c
// S.O.H. Madgwick
// 25th September 2010
//
// IMU.cpp
// Modified as class for arduino by Lestofante
// 27th December 2010
//=====================================================================================================
// Description:
//
// Quaternion implementation of the 'DCM filter' [Mayhony et al].
//
// User must define 'halfT' as the (sample period / 2), and the filter gains 'Kp' and 'Ki'.
//
// Global variables 'q0', 'q1', 'q2', 'q3' are the quaternion elements representing the estimated
// orientation.  See my report for an overview of the use of quaternions in this application.
//
// User must call 'IMUupdate()' every sample period and parse calibrated gyroscope ('gx', 'gy', 'gz')
// and accelerometer ('ax', 'ay', 'ay') data.  Gyroscope units are radians/second, accelerometer 
// units are irrelevant as the vector is normalised.
//
//=====================================================================================================

//----------------------------------------------------------------------------------------------------
// Header files

#include "IMU.h"
//#include <math.h>

//----------------------------------------------------------------------------------------------------
// Definitions

#define Kp 2.0f			// proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.005f		// integral gain governs rate of convergence of gyroscope biases
#define halfT 0.004f		// half the sample period

//---------------------------------------------------------------------------------------------------
// Variable definitions

//float q0 = 1, q1 = 0, q2 = 0, q3 = 0;	// quaternion elements representing the estimated orientation
//float exInt = 0, eyInt = 0, ezInt = 0;	// scaled integral error

//====================================================================================================
// Function
//====================================================================================================

IMU::IMU(){
  exInt=0;
  eyInt=0;
  ezInt=0;
  q0=1;
  q1=0;
  q2=0;
  q3=0;
}

void IMU::IMUupdate(float gx, float gy, float gz, float ax, float ay, float az) {
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;         
	
        //eliminate bad data:
        if (ax==0 && ay==0 && az==0){
          Serial.println("Accelerometer bad data");
          return;
        }

	// normalise the measurements
	norm = sqrt(ax*ax + ay*ay + az*az);
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;      	

	// estimated direction of gravity
	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        
	// error is sum of cross product between reference direction of field and direction measured by sensor
	ex = (ay*vz - az*vy);
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);

	// integral error scaled integral gain
	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki;
	ezInt = ezInt + ez*Ki;

	// adjusted gyroscope measurements
	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;
        
	// integrate quaternion rate and normalise
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  
	
	// normalise quaternion
	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;
}

/*
void IMU::IMUupdateVerbose(float gx, float gy, float gz, float ax, float ay, float az) {
        Serial.print("IMU");
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;         
	
        Serial.print("accel");
	Serial.print(ax);
        Serial.print(" ");
        Serial.print(ay);
        Serial.print(" ");
        Serial.print(az);
        Serial.print(" ");

	// normalise the measurements
	norm = sqrt(ax*ax + ay*ay + az*az);
        Serial.print("norma");
        Serial.print(norm);
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;      
        Serial.print("accel normlized");
	Serial.print(ax);
        Serial.print(" ");
        Serial.print(ay);
        Serial.print(" ");
        Serial.print(az);
        Serial.print(" ");	

	// estimated direction of gravity
	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        Serial.print("gravity");
	Serial.print(vx);
        Serial.print(" ");
        Serial.print(vy);
        Serial.print(" ");
        Serial.print(vz);
        Serial.print(" ");
        
	// error is sum of cross product between reference direction of field and direction measured by sensor
	ex = (ay*vz - az*vy);
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);
        Serial.print("error");
	Serial.print(ex);
        Serial.print(" ");
        Serial.print(ey);
        Serial.print(" ");
        Serial.print(ez);
        Serial.print(" ");

	// integral error scaled integral gain
	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki;
	ezInt = ezInt + ez*Ki;
	Serial.print("integral e");
	Serial.print(exInt);
        Serial.print(" ");
        Serial.print(eyInt);
        Serial.print(" ");
        Serial.print(ezInt);
        Serial.print(" ");

	// adjusted gyroscope measurements
	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;
	Serial.print("new gyro");
	Serial.print(gx);
        Serial.print(" ");
        Serial.print(gy);
        Serial.print(" ");
        Serial.print(gz);
        Serial.print(" ");
        
	// integrate quaternion rate and normalise
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  
	
	// normalise quaternion
	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
        Serial.print("norma");
        Serial.print(norm);
        Serial.println();
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;
}
*/
//====================================================================================================
// END OF CODE
//====================================================================================================
 
