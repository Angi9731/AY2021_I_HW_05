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
#include "I2C_Interface.h"
#include "stdio.h"
#include "ErrorCode.h"
#include "InterruptRoutines.h"

#define LIS3DH_DEVICE_ADDRESS 0x18

#define CTRL_REG1_FREQ1 0x17
#define CTRL_REG1_FREQ10 0x27
#define CTRL_REG1_FREQ25 0x37
#define CTRL_REG1_FREQ50 0x47
#define CTRL_REG1_FREQ100 0x57
#define CTRL_REG1_FREQ200 0x67

#define CTRL_REG1 0x20
#define CTRL_REG4 0x23
#define CTRL_REG0 0x1E
#define STATUS_REG 0x27

#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#define CTRL_REG4_HR 0x08 //High Resolution Mode

#define M_digit_TO_ms2 4*9.8/4095
#define Q_digit_TO_ms2 0



ErrorCode error;
uint8_t ACC_X[2];
uint8_t ACC_Y[2];
uint8_t ACC_Z[2];

int16 Acc_X;
int16 Acc_Y;
int16 Acc_Z;

int16 Acc_X_new;
int16 Acc_Y_new;
int16 Acc_Z_new;

float Acc_X_conv;
float Acc_Y_conv;
float Acc_Z_conv;

uint8 ACC_TOT[8];
uint8 frequency;
uint8 check;


int main(void)
{
    CyGlobalIntEnable; 
    UART_Start();
    I2C_Peripheral_Start();
    CyDelay(5);
    EEPROM_Start();
    flag_button = 0;
    count = 0;
    
    ///////////////////////////////
    
    frequency = EEPROM_ReadByte(0x0000);
    
    switch(frequency)
    {
        case CTRL_REG1_FREQ1:
            count = 0;
        break;
        case CTRL_REG1_FREQ10:
            count = 1;
        break;
        case CTRL_REG1_FREQ25:
            count = 2;
        break;
        case CTRL_REG1_FREQ50:
            count = 3;
        break;
        case CTRL_REG1_FREQ100:
            count = 4;
        break;
        case CTRL_REG1_FREQ200:
            count = 5;
        break;
        default:
           EEPROM_UpdateTemperature();
           EEPROM_WriteByte(CTRL_REG1_FREQ1,0x0000);
           break;
    }
    
    frequency = EEPROM_ReadByte(0x0000);
    
    
    I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG4,
                                             CTRL_REG4_HR);
    
        
    I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG1,
                                             frequency);
    

    ISR_BUTTON_StartEx(BUTTON_ISR);
    Clock_DEB_Start();
    
    ACC_TOT[0] = 0xA0;
    ACC_TOT[7] = 0xC0;
    
    /////////////////////////////////////////
    
    for(;;)
    {
        
        if(flag_button == 0)
        {
        
            I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                STATUS_REG,
                                                &check);
            if(check & 0b00001000)
            {
                
                error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                OUT_X_L,
                                                &ACC_X[0]);
                if (error == NO_ERROR)
                {
                    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                OUT_X_H,
                                                &ACC_X[1]);
                    if (error == NO_ERROR)
                    {
                        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                OUT_Y_L,
                                                &ACC_Y[0]);
                        if (error == NO_ERROR)
                        {
                            error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                OUT_Y_H,
                                                &ACC_Y[1]);
                            if (error == NO_ERROR)
                            {
                                error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                OUT_Z_L,
                                                &ACC_Z[0]);
                                if (error == NO_ERROR)
                                {
                                    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                OUT_Z_H,
                                                &ACC_Z[1]);
                                }
                            }
                        }
                    }
                }
                if(error == NO_ERROR)
                {
                    Acc_X = (int16)((ACC_X[0]) | (ACC_X[1]<<8))>>4;
                    Acc_Y = (int16)((ACC_Y[0]) | (ACC_Y[1]<<8))>>4;
                    Acc_Z = (int16)((ACC_Z[0]) | (ACC_Z[1]<<8))>>4;
                    
                    //ACC X
                    
                    if((Acc_X >= 0) && (Acc_X <= 2047))
                    {
                        Acc_X_conv = (M_digit_TO_ms2 * Acc_X) * 1000;
                    }
                    else
                    {
                        Acc_X_conv =(M_digit_TO_ms2 * Acc_X - Q_digit_TO_ms2) * 1000;
                    }
                    
                    Acc_X_new = (int16)(Acc_X_conv);
                    
                    ACC_TOT[1] = (uint8_t)(Acc_X_new & 0xFF);
                    ACC_TOT[2] = (uint8_t)(Acc_X_new >> 8);
                    
                    //ACC Y
                    
                    if((Acc_Y >= 0) && (Acc_Y <= 2047))
                    {
                        Acc_Y_conv = (M_digit_TO_ms2 * Acc_Y) * 1000;
                    }
                    else
                    {
                        Acc_Y_conv =(M_digit_TO_ms2 * Acc_Y - Q_digit_TO_ms2) * 1000;
                    }
                    
                    Acc_Y_new = (int16)(Acc_Y_conv);
                    
                    ACC_TOT[3] = (uint8_t)(Acc_Y_new & 0xFF);
                    ACC_TOT[4] =(uint8_t)(Acc_Y_new >> 8);
                    
                    //ACC Z
                    
                    if((Acc_Z >= 0) && (Acc_Z <= 2047))
                    {
                        Acc_Z_conv = (M_digit_TO_ms2 * Acc_Z) * 1000;
                    }
                    else
                    {
                        Acc_Z_conv =(M_digit_TO_ms2 * Acc_Z - Q_digit_TO_ms2) * 1000;
                    }
                    
                    Acc_Z_new = (int16)(Acc_Z_conv);
                   
                    ACC_TOT[5] = (uint8_t)(Acc_Z_new & 0xFF);
                    ACC_TOT[6] = (uint8_t)(Acc_Z_new >> 8);
                    
                    UART_PutArray(ACC_TOT,8);
                }
            }
            
        }
        
        if(flag_button == 1)
        {
            flag_button = 0;
            
            if(count < 6)
            {
                frequency = EEPROM_ReadByte(0x0000) + 0x10;
                EEPROM_UpdateTemperature();
                EEPROM_WriteByte(frequency, 0x0000);
                I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG1,
                                             frequency);
            }
            else
            {
                count = 0;
                EEPROM_UpdateTemperature();
                EEPROM_WriteByte(CTRL_REG1_FREQ1, 0x0000);
                I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG1,
                                             CTRL_REG1_FREQ1);
            }
        }    
    }
}

/* [] END OF FILE */
