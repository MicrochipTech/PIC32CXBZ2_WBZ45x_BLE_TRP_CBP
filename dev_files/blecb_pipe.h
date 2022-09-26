/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    blecb_pipe.h

  @Summary
    BLE CREDIT BASE PIPE Header file.

  @Description
    Contains the declarations needed for the BLE Credit Base pipe to be integrated in a
    WBZ451 FreeRTOS project.
    For a complete description see blecb_pipe.c
 */
/* ************************************************************************** */

#ifndef _EXAMPLE_FILE_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_FILE_NAME_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stdbool.h>                    // Defines true
#include "ble_trcbps/ble_trcbps.h"

#ifdef __cplusplus
extern "C" {
#endif
    
        #define BLECB_Pipe_DATA_QUEUE_MAX_ELEMENTS     255
        #define BLECB_Pipe_DATA_QUEUE_MAX_ALLOC        10240

        typedef struct 
        {
            uint16_t                   dataLeng;            /**< Data length. */
            uint8_t                    *p_data;             /**< Pointer to the data buffer */
            uint16_t                   processedUpTo;       /**< Data already processed for this element */
        } BLECB_Pipe_DATA_QUEUE_QueueElement;

        typedef struct 
        {
            uint8_t                                 usedNum;                                /**< The number of data list in circular queue. */
            uint8_t                                 writeIdx;                               /**< The Index of data, written in circular queue. */
            uint8_t                                 readIdx;                                /**< The Index of data, read in circular queue. */
            BLECB_Pipe_DATA_QUEUE_QueueElement      queueElem[BLECB_Pipe_DATA_QUEUE_MAX_ELEMENTS];     /**< The circular data queue. @ref APP_UTILITY_QueueElem_T.*/
            uint16_t                                currentAlloc;                           /**< The current memory allocated for this queue */
        } BLECB_Pipe_DATA_QUEUE_CircQueue;
    
        
        
        typedef void (* pipedatarecived_callback)(uint8_t *, uint16_t);
        extern pipedatarecived_callback BLECB_Pipe_ReceivedDataCallback;
        void BLECB_Pipe_Task(void);
        void BLECB_Pipe_Init(pipedatarecived_callback rxcallback);
        bool BLECB_Pipe_Event_Handler(STACK_Event_T * event);
        void BLECB_Pipe_SendData(uint8_t * msg, uint16_t size);
    
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
