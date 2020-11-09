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

#ifndef INTERRUPT_ROUTINES_H
    #define INTERRUPT_ROUTINES_H
    
    #include "project.h"
    #include "cytypes.h"
    
    CY_ISR_PROTO(BUTTON_ISR);
    
    volatile uint8 flag_button; //variable set to 1 when the button is pressed
    volatile uint8 count; //variable that keeps track of the selected frequency (see in main)
    
#endif

/* [] END OF FILE */
