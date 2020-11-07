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



#define LIS3DH_DEVICE_ADDRESS 0x18

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
#define CTRL_REG1_ODR 0x57 //f sampling 100Hz

#define M_digit_TO_ms2 4*9.8/4095
#define Q_digit_TO_ms2 0

ErrorCode error;
char message[50];
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

uint8_t ACC_TOT[8];



int main(void)
{
    CyGlobalIntEnable; 
    UART_Start();
    I2C_Peripheral_Start();
    
    CyDelay(5);
   /* EEPROM_Start();
    EEPROM_UpdateTemperature();
    EEPROM_WriteByte(1,0x0000);*/
    
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG4,
                                             CTRL_REG4_HR);
    
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 4 successfully written as: 0x%02X\r\n", CTRL_REG4_HR);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set control register 4\r\n");   
        }
        
     error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG1,
                                             CTRL_REG1_ODR);
    
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", CTRL_REG1_ODR);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set control register 1\r\n");   
        }

    ACC_TOT[0] = 0xA0;
    ACC_TOT[7] = 0xC0;
    
    for(;;)
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

/* [] END OF FILE */
