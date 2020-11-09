/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "ErrorCode.h"

#define LIS3DH_DEVICE_ADDRESS 0x18

//FREQUENCIES
#define CTRL_REG1_FREQ1 0x17
#define CTRL_REG1_FREQ10 0x27
#define CTRL_REG1_FREQ25 0x37
#define CTRL_REG1_FREQ50 0x47
#define CTRL_REG1_FREQ100 0x57
#define CTRL_REG1_FREQ200 0x67

//REGISTERS
#define CTRL_REG1 0x20
#define CTRL_REG4 0x23
#define CTRL_REG0 0x1E
#define STATUS_REG 0x27
#define EEPROM 0x0000

#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#define CTRL_REG4_HR 0x08 //High Resolution Mode

#define M_digit_TO_ms2 4*9.8/4095 //Angular coefficient of the conversion line

#define BUFFER_SIZE 8
#define STATUS_REG_ZYXDA 0b00001000 //mask to check the value of bit ZYXDA

ErrorCode error;

//Vectors to save the content of OUT_X,Y,Z register
uint8 ACC_X[2];
uint8 ACC_Y[2];
uint8 ACC_Z[2];

//Ricomposed values
int16 Acc_X;
int16 Acc_Y;
int16 Acc_Z;

//Converted values
float Acc_X_conv;
float Acc_Y_conv;
float Acc_Z_conv;

//Values after cast
int16 Acc_X_new;
int16 Acc_Y_new;
int16 Acc_Z_new;

uint8 ACC_TOT[BUFFER_SIZE];
uint8 frequency;
uint8 status_register;

/* [] END OF FILE */
