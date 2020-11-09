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
#include "project.h"

    ErrorCode I2C_Peripheral_Start(void) 
    {
        // Start I2C peripheral
        I2C_MASTER_Start();  
        
        // Return no error since start function does not return any error
        return NO_ERROR;
    }
    
    ErrorCode I2C_Peripheral_Stop(void)
    {
        // Stop I2C peripheral
        I2C_MASTER_Stop();
        
        // Return no error since stop function does not return any error
        return NO_ERROR;
    }
    
    ErrorCode I2C_Peripheral_ReadRegister(uint8_t device_address, 
                                            uint8_t register_address,
                                            uint8_t* data)
    {
        // Send start condition
        uint8_t error = I2C_MASTER_MasterSendStart(device_address,I2C_MASTER_WRITE_XFER_MODE);
        if (error == I2C_MASTER_MSTR_NO_ERROR)
        {
            // Write address of register to be read
            error = I2C_MASTER_MasterWriteByte(register_address);
            if (error == I2C_MASTER_MSTR_NO_ERROR)
            {
                // Send restart condition
                error = I2C_MASTER_MasterSendRestart(device_address, I2C_MASTER_READ_XFER_MODE);
                if (error == I2C_MASTER_MSTR_NO_ERROR)
                {
                    // Read data without acknowledgement
                    *data = I2C_MASTER_MasterReadByte(I2C_MASTER_NAK_DATA);
                }
            }
        }
        // Send stop condition
        I2C_MASTER_MasterSendStop();
        // Return error code
        return error ? ERROR : NO_ERROR;
    }
    
    
     ErrorCode I2C_Peripheral_WriteRegister(uint8_t device_address,
                                            uint8_t register_address,
                                            uint8_t data)
    {
        // Send start condition
        uint8_t error = I2C_MASTER_MasterSendStart(device_address, I2C_MASTER_WRITE_XFER_MODE);
        if (error == I2C_MASTER_MSTR_NO_ERROR)
        {
            // Write register address
            error = I2C_MASTER_MasterWriteByte(register_address);
            if (error == I2C_MASTER_MSTR_NO_ERROR)
            {
                // Write byte of interest
                error = I2C_MASTER_MasterWriteByte(data);
            }
        }
        // Send stop condition
        I2C_MASTER_MasterSendStop();
        // Return error code
        return error ? ERROR : NO_ERROR;
    }
    
/* [] END OF FILE */
