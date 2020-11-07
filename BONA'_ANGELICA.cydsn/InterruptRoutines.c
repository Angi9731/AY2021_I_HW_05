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

#include "InterruptRoutines.h"
#include "project.h"
#include "I2C_Interface.h"

uint8 freq;

CY_ISR(BUTTON_ISR)
{
    //flag_button = 1;
    freq = EEPROM_ReadByte(0x0000);
    
    switch(freq)
    {
        case CTRL_REG1_FREQ200:
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(CTRL_REG1_FREQ1, 0x0000);
            break;
        default:
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(freq + 0x10, 0x0000);
            break;
            
    }
    
    freq = EEPROM_ReadByte(0x0000);
    
    I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             CTRL_REG1,
                                             freq);
    
}
/* [] END OF FILE */
