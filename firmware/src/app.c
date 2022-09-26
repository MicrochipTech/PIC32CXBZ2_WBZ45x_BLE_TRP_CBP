// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "app.h"
#include "definitions.h"
#include "app_ble.h"
#include "blecb_pipe.h"



// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define APP_KEY_PB4  0x10
bool USERKEY_PRESSED;
TaskHandle_t xBUTTON_Task;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/

/**
 * CHECK IF USER BUTTON IS PRESSED
 * @return 
 */
bool CheckUserKeyPressed( void )
{
    if(!USERKEY_PRESSED)
    {
        if ((GPIOB_REGS->GPIO_PORT & APP_KEY_PB4) == 0)
        {
            USERKEY_PRESSED = true;
            return true;
        }
    }else
    {
        if ((GPIOB_REGS->GPIO_PORT & APP_KEY_PB4) != 0)
        {
            USERKEY_PRESSED = false;
        }
    }
    return false;
}

/**
 * USER BUTTON CHECK TASK
 * @param pvParameters
 */
void _BUTTON_Task(  void *pvParameters  )
{   
    while(1)
    {
        if(CheckUserKeyPressed())
        {
            Debug_Uart_Write_blocking((uint8_t *)"\n-> Pressed", 11);
            BLECB_Pipe_SendData((uint8_t *)"Message from WBZ451\r\n",21);
        }
    }
}


/**
 * DEBUG UART BLOCKING WRITE
 * @param data
 * @param length
 */
void Debug_Uart_Write_blocking(uint8_t *data, uint16_t length)
{
    SERCOM0_USART_Write(data, length);
    while(SERCOM0_USART_WriteIsBusy());
}

/**
 * DEBUG UART NON-BLOCKING WRITE
 * @param data
 * @param length
 */
void Debug_Uart_Write(uint8_t *data, uint16_t length)
{
    SERCOM0_USART_Write(data, length);    
}

/**
 * CALLBACK FOR PROCESSING EACH RECEIVED MESSAGE
 * @param data
 * @param length
 */
void BLE_Rx_Callback(uint8_t *data, uint16_t length)
{
    if (length >= 64)
    {   // print '*' if length >=64
        SERCOM0_USART_Write((uint8_t *)"*", 1);
    }
    else
    {
        SERCOM0_USART_Write(data, length);        
    }
    while(SERCOM0_USART_WriteIsBusy());
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;


    appData.appQueue = xQueueCreate( 64, sizeof(APP_Msg_T) );
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    
    xTaskCreate((TaskFunction_t) _BUTTON_Task,
            "BUTTON_Task",
            1024,
            NULL,
            1,
            &xBUTTON_Task);
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    APP_Msg_T    appMsg[1];
    APP_Msg_T   *p_appMsg;
    p_appMsg=appMsg;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            //--- USER KEY INIT
            GPIOB_REGS->GPIO_ANSELCLR = APP_KEY_PB4;
            GPIOB_REGS->GPIO_TRISSET =  APP_KEY_PB4;
            USERKEY_PRESSED = false;

            //appData.appQueue = xQueueCreate( 10, sizeof(APP_Msg_T) );
            APP_BleStackInit();

            // Configure Bluetooth device address
            BLE_GAP_Addr_T devAddr;
            devAddr.addrType = BLE_GAP_ADDR_TYPE_PUBLIC;
            devAddr.addr[0] = 0xB1;
            devAddr.addr[1] = 0xB2;
            devAddr.addr[2] = 0xA3;
            devAddr.addr[3] = 0xA4;
            devAddr.addr[4] = 0xA5;
            devAddr.addr[5] = 0xA6;
            BLE_GAP_SetDeviceAddr(&devAddr); 
            
            BLECB_Pipe_Init(BLE_Rx_Callback);
            Debug_Uart_Write_blocking((uint8_t *)"\n-> Initialized", 15);
            BLE_GAP_SetAdvEnable(0x01, 0x00);
            Debug_Uart_Write_blocking((uint8_t *)"\n-> Start Advertising", 21);

            if (appInitialized)
            {

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            if (OSAL_QUEUE_Receive(&appData.appQueue, &appMsg, OSAL_WAIT_FOREVER))
            {
                if(p_appMsg->msgId==APP_MSG_BLE_STACK_EVT)
                {
                    bool eventConsumed = BLECB_Pipe_Event_Handler((STACK_Event_T *)p_appMsg->msgData);
                    if (eventConsumed) return;
                    
                    // Pass BLE Stack Event Message to User Application for handling
                    APP_BleStackEvtHandler((STACK_Event_T *)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_MSG_BLECB_PIPE_CONNECTED)
                {
                    SERCOM0_USART_Write((uint8_t *)"\n-> BLECB PIPE OPENED", 21);
                }
                else if(p_appMsg->msgId==APP_MSG_BLECB_PIPE_DISCONNECTED)
                {
                    SERCOM0_USART_Write((uint8_t *)"\n-> BLECB PIPE CLOSED", 21);
                }
                else if(p_appMsg->msgId==APP_MSG_BLECB_PIPE_PHY_UPDATED)
                {
                    if(phyInUse==BLE_GAP_PHY_OPTION_2M){
                        SERCOM0_USART_Write((uint8_t *)"\n-> BLE PHY Updated to 2M", 25);
                    }
                    else{
                        SERCOM0_USART_Write((uint8_t *)"\n-> BLE PHY Updated to 1M", 25);
                    }
                }
                else if(p_appMsg->msgId==APP_MSG_BLECB_PIPE_FILE_TX_START)
                {
                    SERCOM0_USART_Write((uint8_t *)"\n",1);
                }
                else if(p_appMsg->msgId==APP_MSG_BLECB_PIPE_FILE_TX_END)
                {
                    SERCOM0_USART_Write((uint8_t *)p_appMsg->msgData,51);
                }
                else if(p_appMsg->msgId==APP_MSG_BLE_STACK_LOG)
                {
                    // Pass BLE LOG Event Message to User Application for handling
                    APP_BleStackLogHandler((BT_SYS_LogEvent_T *)p_appMsg->msgData);
                }
            }
            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
