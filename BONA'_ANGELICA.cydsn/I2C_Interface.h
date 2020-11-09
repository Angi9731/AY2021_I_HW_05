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

#ifndef I2C_Interface_H
    #define I2C_Interface_H
    
    #include "cytypes.h"
    #include "ErrorCode.h"
    
    
    ErrorCode I2C_Peripheral_Start(void); //Start I2C peripheral
    
    ErrorCode I2C_Peripheral_Stop(void); //Stop I2C peripheral
    
    ErrorCode I2C_Peripheral_ReadRegister(uint8_t device_address, //Read a register and save it in a variable
                                            uint8_t register_address,
                                            uint8_t* data);
    
    ErrorCode I2C_Peripheral_WriteRegister(uint8_t device_address, //Write the content of a variable in a register
                                            uint8_t register_address,
                                            uint8_t data);
    
#endif
/* [] END OF FILE */
