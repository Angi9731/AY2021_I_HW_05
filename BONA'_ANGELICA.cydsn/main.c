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
#include "I2C_Interface.h"
#include "stdio.h"
#include "InterruptRoutines.h"
#include "Variables.h"

int main(void)
{
    CyGlobalIntEnable; 
    UART_Start();
    I2C_Peripheral_Start();
    CyDelay(5);
    EEPROM_Start();
    
    flag_button = 0; //initialised as 'button not pressed'
    count = 0; //initialised to the value corresponding to f = 1Hz
    
    //Initial setting of EEPROM and SETTING REGISTERS
    
    frequency = EEPROM_ReadByte(EEPROM); //read the value already present in EEPROM
    
    switch(frequency) //if the value corresponds to one of the allowed frequencies keep it and set the corresponding count
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
        default: //otherwise write the first frequency f = 1Hz
           EEPROM_UpdateTemperature();
           EEPROM_WriteByte(CTRL_REG1_FREQ1,EEPROM);
           break;
    }
    
    frequency = EEPROM_ReadByte(EEPROM); //read again the frequency in the EEPROM
    
    
    I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS, //set the register with the value read
                                             CTRL_REG1,
                                             frequency);
    
    
    I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS, //set the high resolution mode
                                             CTRL_REG4,
                                             CTRL_REG4_HR);
    
        
    ISR_BUTTON_StartEx(BUTTON_ISR);
    Clock_DEB_Start();
    
    ACC_TOT[0] = 0xA0; //Start byte
    ACC_TOT[BUFFER_SIZE - 1] = 0xC0; //Stop byte
    
    //ACQUISITION and DATA MANAGEMENT
    
    for(;;)
    {
        
        if(flag_button == 0) //if the button has not been pressed
        {
        
            I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS, //read the value of the Status Register
                                                STATUS_REG,
                                                &status_register);
            
            if(status_register & STATUS_REG_ZYXDA) //if a new set of data is available
            {
                //Read the LSB and MSB acceleration for each axis
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
                    //HR mode: 12 bit data --> ricomposition and 16 bit casting
                    
                    Acc_X = (int16)((ACC_X[0]) | (ACC_X[1]<<8))>>4;
                    Acc_Y = (int16)((ACC_Y[0]) | (ACC_Y[1]<<8))>>4;
                    Acc_Z = (int16)((ACC_Z[0]) | (ACC_Z[1]<<8))>>4;
                    
                    //X AXIS
                    
                    Acc_X_conv = (M_digit_TO_ms2 * Acc_X) * 1000; //conversion: digits --> m/s2
                    
                    Acc_X_new = (int16)(Acc_X_conv); //float --> int
                    
                    ACC_TOT[1] = (uint8)(Acc_X_new & 0xFF); //bytes to send
                    ACC_TOT[2] = (uint8)(Acc_X_new >> 8);
                    
                    //Y AXIS
                    
                    Acc_Y_conv = (M_digit_TO_ms2 * Acc_Y) * 1000;
                    
                    Acc_Y_new = (int16)(Acc_Y_conv);
                    
                    ACC_TOT[3] = (uint8)(Acc_Y_new & 0xFF);
                    ACC_TOT[4] =(uint8)(Acc_Y_new >> 8);
                    
                    //Z AXIS
                    
                    Acc_Z_conv = (M_digit_TO_ms2 * Acc_Z) * 1000;
                    
                    Acc_Z_new = (int16)(Acc_Z_conv);
                   
                    ACC_TOT[5] = (uint8)(Acc_Z_new & 0xFF);
                    ACC_TOT[6] = (uint8)(Acc_Z_new >> 8);
                    
                    UART_PutArray(ACC_TOT,BUFFER_SIZE); //send buffer
                }
            }
        }
        
        if(flag_button == 1) //If the button has been pressed
        {
            flag_button = 0;
            
            if(count < 6)//if the selected frequency is one of the allowed
            {
                frequency = EEPROM_ReadByte(EEPROM) + 0x10; //select the following frequency
                EEPROM_UpdateTemperature();
                EEPROM_WriteByte(frequency, EEPROM); //update the value in EEPROM
                I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG1,
                                             frequency); //set the register
            }
            else //restart the cycle starting from f = 1Hz
            {
                count = 0;
                EEPROM_UpdateTemperature();
                EEPROM_WriteByte(CTRL_REG1_FREQ1, EEPROM);
                I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG1,
                                             CTRL_REG1_FREQ1);
            }
        }    
    }
}

/* [] END OF FILE */
