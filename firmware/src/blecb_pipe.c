/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    blecb_pipe.c

  @Summary
    BLE CREDIT BASE PIPE Source file

  @Description
    Implements the BLE Credit Base pipe service that can be added to a
    WBZ451 FreeRTOS project.
    It creates a L2CAP data channel that allow high speed data transfer over BLE
 
  @Usage
    STEP1: Configure a project in MCC to support BLE Transparent Uart Credit Base profile
    
    STEP2: Include in the project the 2 files blecb_pipe.h and blecb_pipe.c
    
    STEP3: Define data queue length and max allocation space in bytes with the 2
    definitions you find in blecb_pipe.h:
    BLECB_Pipe_DATA_QUEUE_MAX_ELEMENTS and BLECB_Pipe_DATA_QUEUE_MAX_ALLOC
    
    STEP4: Add BLECB_Pipe_Task() before SYS_TASK() in main.c:
        SYS_Initialize ( NULL );
        while ( true )
        {
            // Maintain state machines of all polled MPLAB Harmony modules. 
            ----> BLECB_Pipe_Task();
            SYS_Tasks ( );
        }
 
    STEP5: Define in app.c a callback function to receive data from the pipe as for:
    typedef void (* pipedatarecived_callback)(uint8_t *, uint16_t);
    defined in blecb_pipe.h. E.G.:
        void BLERXCallback(uint8_t * data, uint16_t length){
            ----> print data on a uart
        }

    STEP6: Add BLECB_Pipe_Init(BLERXCallback); just after BLE stack initialization in
    app.c (into APP init function):
        case APP_STATE_INIT:
               {
                    APP_BleStackInit();
                ------>BLECB_Pipe_Init(BLERXCallback);
 
    STEP7: Add BLECB_Pipe_Event_Handler before the default ble stack handler in app.c
    APP_STATE_SERVICE_TASKS - APP_MSG_BLE_STACK_EVT. To make the handler intercept messages
    and exit correctly from the task, you can check the returned value of this function that
    will report if the event has been consumed. If it has been consumed you can exit the function 
    preventing the default handler to process the event again. E.G.:
                    bool eventconsumed = BLECB_Pipe_Event_Handler((STACK_Event_T *)p_appMsg->msgData);
                    if(eventconsumed) return;
                    APP_BleStackEvtHandler((STACK_Event_T *)p_appMsg->msgData);
 
    STEP8: You are pretty much done. If you want to send data on the pipe you can use the BLECB_Pipe_SendData
    API that takes as input the pointer to the buffer you want to send and its length
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>                      // Defines sprintf
#include "definitions.h"                // SYS function prototypes
#include "configuration.h"
#include "app_ble_handler.h"
#include "blecb_pipe.h"
#include "ble_gap.h"

//--- DATA QUEUE TASK HANDLER
TaskHandle_t xblecb_pipe_QUEUE_Tasks;

//--- DATA QUEUE GLOBALS
uint16_t    MESSAGE_L;
uint8_t *   MESSAGE_BUFFER;
uint16_t    MESSAGE_PARTIAL_L;
uint8_t *   MESSAGE_INCOMPLETE;
long start;
long edgestart;
long edgeamount;
bool STARTED;
long reck;
BLECB_Pipe_DATA_QUEUE_CircQueue BLEDATA_RECEIVEQUEUE;
BLECB_Pipe_DATA_QUEUE_CircQueue BLEDATA_TRANSMITQUEUE;
pipedatarecived_callback BLECB_Pipe_ReceivedDataCallback;

//--- CONNECTION HANDLE
uint16_t ActiveConnectionHandle;

//--- BLE PHY
#define DEFAULTPHY  BLE_GAP_PHY_OPTION_2M
uint8_t phyInUse;

/**
 * BLECB PIPE Data Queue get valid
 * @param p_circQueue_t
 * @return 
 */
uint8_t BLECB_Pipe_DATA_QUEUE_GetValidCircQueueNum(BLECB_Pipe_DATA_QUEUE_CircQueue *p_circQueue_t)
{
    uint8_t validNum = 0;
    
    if (p_circQueue_t != NULL)
    {
        if (p_circQueue_t->usedNum < BLECB_Pipe_DATA_QUEUE_MAX_ELEMENTS && p_circQueue_t->currentAlloc < BLECB_Pipe_DATA_QUEUE_MAX_ALLOC)
            validNum = BLECB_Pipe_DATA_QUEUE_MAX_ELEMENTS - p_circQueue_t->usedNum;
    }
    
    return validNum;
}


/**
 * BLECB PIPE Data Queue Insert 
 * @param dataLeng
 * @param p_data
 * @param p_circQueue_t
 * @return 
 */
int BLECB_Pipe_DATA_QUEUE_InsertDataToCircQueue(uint16_t dataLeng, uint8_t *p_data, BLECB_Pipe_DATA_QUEUE_CircQueue *p_circQueue_t)
{
    
    if ((dataLeng > 0) && (p_data != NULL) && (p_circQueue_t != NULL))
    {
        if (BLECB_Pipe_DATA_QUEUE_GetValidCircQueueNum(p_circQueue_t) > 0)
        {
            p_circQueue_t->queueElem[p_circQueue_t->writeIdx].dataLeng = dataLeng;
            p_circQueue_t->currentAlloc  += dataLeng;
            p_circQueue_t->queueElem[p_circQueue_t->writeIdx].p_data = p_data;
            p_circQueue_t->queueElem[p_circQueue_t->writeIdx].processedUpTo = 0;
            p_circQueue_t->usedNum++;
            p_circQueue_t->writeIdx++;
            if (p_circQueue_t->writeIdx >= BLECB_Pipe_DATA_QUEUE_MAX_ELEMENTS)
                p_circQueue_t->writeIdx = 0;
        }
        else
            return -1;
    }
    else
        return -1;
    
    return 0;
}


/**
 * BLECB PIPE Data Queue Check if empty
 * @param p_circQueue_t
 * @return 
 */
bool BLECB_Pipe_DATA_QUEUE_Is_Empty(BLECB_Pipe_DATA_QUEUE_CircQueue *p_circQueue_t)
{
    if (p_circQueue_t != NULL)
    {
        if (p_circQueue_t->usedNum > 0)
            return false;
        else
            return true;
    }else
        return true;
}


/**
 * BLECB PIPE Data Queue Get Element
 * @param p_circQueue_t
 * @return 
 */
BLECB_Pipe_DATA_QUEUE_QueueElement * BLECB_Pipe_DATA_QUEUE_GetElemCircQueue(BLECB_Pipe_DATA_QUEUE_CircQueue *p_circQueue_t)
{
    BLECB_Pipe_DATA_QUEUE_QueueElement *p_queueElem_t;

    if (p_circQueue_t != NULL)
    {
        if (p_circQueue_t->usedNum > 0)
        {
            p_queueElem_t = (BLECB_Pipe_DATA_QUEUE_QueueElement *) &(p_circQueue_t->queueElem[p_circQueue_t->readIdx].dataLeng);
        }
        else
            return NULL;
    }
    else
        return NULL;
    
    return p_queueElem_t;
}


/**
 * BLECB PIPE Data Queue Set Element processed amount
 * @param p_circQueue_t
 * @param procam
 */
void BLECB_Pipe_DATA_QUEUE_SetElemProcessedAmount(BLECB_Pipe_DATA_QUEUE_CircQueue *p_circQueue_t, uint16_t procam)
{
    BLECB_Pipe_DATA_QUEUE_QueueElement *p_queueElem_t;

    if (p_circQueue_t != NULL)
    {
        if (p_circQueue_t->usedNum > 0)
        {
            p_queueElem_t = (BLECB_Pipe_DATA_QUEUE_QueueElement *) &(p_circQueue_t->queueElem[p_circQueue_t->readIdx].dataLeng);
            p_queueElem_t->processedUpTo = procam;
        }
    } 
}


/**
 * BLECB PIPE Data Queue Free element
 * @param p_circQueue_t
 */
void BLECB_Pipe_DATA_QUEUE_FreeElemCircQueue(BLECB_Pipe_DATA_QUEUE_CircQueue *p_circQueue_t)
{
    if (p_circQueue_t != NULL)
    {
        p_circQueue_t->currentAlloc  -= p_circQueue_t->queueElem[p_circQueue_t->readIdx].dataLeng;
        p_circQueue_t->queueElem[p_circQueue_t->readIdx].dataLeng = 0;
        if (p_circQueue_t->queueElem[p_circQueue_t->readIdx].p_data != NULL)
            OSAL_Free(p_circQueue_t->queueElem[p_circQueue_t->readIdx].p_data);
        if (p_circQueue_t->usedNum > 0)
            p_circQueue_t->usedNum--;
        p_circQueue_t->readIdx++;
        if (p_circQueue_t->readIdx >= BLECB_Pipe_DATA_QUEUE_MAX_ELEMENTS)
            p_circQueue_t->readIdx = 0;       
    }
}


/**
 * BLECB PIPE Data Queue Clear Queue
 * @param p_circQueue_t
 */
void BLECB_Pipe_DATA_QUEUE_ClearQueue(BLECB_Pipe_DATA_QUEUE_CircQueue *p_circQueue_t){
    int i;
    for(i=0;i<BLECB_Pipe_DATA_QUEUE_MAX_ELEMENTS;i++){
        if(p_circQueue_t->queueElem[i].p_data != NULL)
            OSAL_Free(p_circQueue_t->queueElem[i].p_data);
    }
    memset(p_circQueue_t,0,sizeof(BLECB_Pipe_DATA_QUEUE_CircQueue));
}


/**
 * BLECB PIPE Data Queue Init
 * @param rxcallback
 */
void BLECB_Pipe_dataqueue_Init( pipedatarecived_callback rxcallback ){
    
    //--- INIT DATA QUEUES
    memset(&BLEDATA_RECEIVEQUEUE,0,sizeof(BLECB_Pipe_DATA_QUEUE_CircQueue));
    memset(&BLEDATA_TRANSMITQUEUE,0,sizeof(BLECB_Pipe_DATA_QUEUE_CircQueue));
    
    //--- ASSIGN RX CALLBACK
    BLECB_Pipe_ReceivedDataCallback = rxcallback;
    
    //--- INIT RX BUFFERS
    MESSAGE_L = 0;
    MESSAGE_PARTIAL_L = 0;
    MESSAGE_BUFFER = NULL;
    MESSAGE_INCOMPLETE = NULL;
    
    STARTED = false;
    
}


/**
 * BLECB PIPE Data Queue INSERT IN RX QUEUE
 * @param p_event
 */
void BLECB_Pipe_dataqueue_InsertInRXQueue(BLE_TRCBPS_Event_T *p_event){
    uint8_t * newbuffer;
    uint16_t dataLength = 0 ;
    APP_Msg_T appMsg;
    
    
    BLE_TRCBPS_GetDataLength(p_event->eventField.onReceiveData.connHandle,&dataLength);
    newbuffer = OSAL_Malloc(dataLength);
    BLE_TRCBPS_GetData(p_event->eventField.onReceiveData.connHandle,newbuffer);
    if(BLECB_Pipe_DATA_QUEUE_InsertDataToCircQueue(dataLength,newbuffer,&BLEDATA_RECEIVEQUEUE)==-1){
            appData.state = APP_STATE_RXQUEUE_FULL;
            appMsg.msgId = APP_MSG_IDLE;
            OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
            return;
        }
}


/**
 * BLECB PIPE Data Queue INSERT IN TX QUEUE
 * @param message
 * @param message_l
 * @return 
 */
bool BLECB_Pipe_dataqueue_InsertInTXQueue(uint8_t * message, uint16_t message_l){
    if (BLECB_Pipe_DATA_QUEUE_GetValidCircQueueNum(&BLEDATA_TRANSMITQUEUE) > 0){
       uint8_t * new_buffer = OSAL_Malloc(message_l + 2);
       new_buffer[0] = message_l & 0xFF;
       new_buffer[1] = (message_l >> 8) & 0xFF;
       memcpy(&new_buffer[2],message,message_l);
       if(BLECB_Pipe_DATA_QUEUE_InsertDataToCircQueue(message_l + 2,new_buffer,&BLEDATA_TRANSMITQUEUE)==-1){
           OSAL_Free(new_buffer);
           return false;
       } 
       return true;
    }else{
        return false;
    }
    return false;
}


/**
 * BLECB PIPE NOTIFY MAIN APP
 * @param msgid
 */
void BLECB_Pipe_Notify_APP(int msgid){
    APP_Msg_T appMsg;
    appMsg.msgId = msgid;
    OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
}


/**
 * BLECB PIPE NOTIFY MAIN APP with Data
 * @param msgid
 */
void BLECB_Pipe_Notify_APP_with_Data(int msgid, uint8_t * data, uint8_t size){
    APP_Msg_T appMsg;
    appMsg.msgId = msgid;
    memcpy((uint8_t *)appMsg.msgData,data,size);
    OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
}


/**
 * BLECB PIPE Data Queue PROCESS TX QUEUE
 */
void BLECB_Pipe_ProcessTXQueue( void ){
    if(appData.state!=APP_STATE_SERVICE_TASKS) return;
    if(BLECB_Pipe_DATA_QUEUE_Is_Empty(&BLEDATA_TRANSMITQUEUE)) return;
    BLECB_Pipe_DATA_QUEUE_QueueElement * element = BLECB_Pipe_DATA_QUEUE_GetElemCircQueue(&BLEDATA_TRANSMITQUEUE);
    if(element!=NULL){
        BLE_TRCBPS_SendData(ActiveConnectionHandle,element->dataLeng,element->p_data);
        BLECB_Pipe_DATA_QUEUE_FreeElemCircQueue(&BLEDATA_TRANSMITQUEUE);
    }
}


/**
 * CHECK IF SPEED TEST IN COURSE
 * @param rx
 */
void BLECB_Pipe_CheckIfSpeedTest( uint16_t rx ){
    if(memcmp(MESSAGE_BUFFER,"\nSTART\n",7)==0) {
        BLECB_Pipe_Notify_APP(APP_MSG_BLECB_PIPE_FILE_TX_START);
        start = xTaskGetTickCount();reck = 0;return;
    }
    if(memcmp(MESSAGE_BUFFER,"\nSTOP \n",7)==0) {
        long elapsedTicks = xTaskGetTickCount() - start;
        reck += 7;
        uint8_t speed[52];
        long s = (reck * 1000) / elapsedTicks;
        uint16_t integers = s / 1000 ;
        uint16_t decimals = s - (integers * 1000);
        sprintf((char *)speed,"\nTX SPEED: %-2d.%03d KBytes/sec (%-7ldB in %-5ldms)\n",integers,decimals,reck,elapsedTicks);
        BLECB_Pipe_Notify_APP_with_Data(APP_MSG_BLECB_PIPE_FILE_TX_END,(uint8_t *)speed, 51);
        return;
    }
    reck += rx;
    
}


/**
 * BLECB PIPE Data Queue PROCESS RX QUEUE
 */
void BLECB_Pipe_ProcessRXQueue( void ){
    uint16_t elementbytesCopied = 0;
    if(appData.state!=APP_STATE_SERVICE_TASKS) return;
    if(BLECB_Pipe_DATA_QUEUE_Is_Empty(&BLEDATA_RECEIVEQUEUE)) return;
    BLECB_Pipe_DATA_QUEUE_QueueElement * element = BLECB_Pipe_DATA_QUEUE_GetElemCircQueue(&BLEDATA_RECEIVEQUEUE);
    if(element!=NULL){
        
        if(MESSAGE_L==0)
        {
            uint16_t elementbytesMinusSize = element->dataLeng - 2;
            elementbytesCopied = elementbytesMinusSize;
            MESSAGE_L = (((element->p_data[1]&0xFF)<<8) | (element->p_data[0]&0xFF)) & 0xFFFF;
            MESSAGE_BUFFER = OSAL_Malloc(MESSAGE_L);
            if(elementbytesMinusSize>MESSAGE_L) elementbytesCopied = MESSAGE_L;
            memcpy(MESSAGE_BUFFER,&element->p_data[2],elementbytesCopied);
            MESSAGE_PARTIAL_L = elementbytesCopied;
            BLECB_Pipe_DATA_QUEUE_SetElemProcessedAmount(&BLEDATA_RECEIVEQUEUE,elementbytesCopied+2);

            if(MESSAGE_PARTIAL_L == MESSAGE_L) {
                // fire callback
                BLECB_Pipe_CheckIfSpeedTest(MESSAGE_L);
                if(BLECB_Pipe_ReceivedDataCallback!=NULL) BLECB_Pipe_ReceivedDataCallback(MESSAGE_BUFFER,MESSAGE_L);
                MESSAGE_L = 0;
                MESSAGE_PARTIAL_L = 0;
                OSAL_Free(MESSAGE_BUFFER);
                MESSAGE_BUFFER = NULL;
            }
            if(element->processedUpTo == element->dataLeng){
                BLECB_Pipe_DATA_QUEUE_FreeElemCircQueue(&BLEDATA_RECEIVEQUEUE);
            }
        } else {
           
            uint16_t bytestocompletemessage = MESSAGE_L - MESSAGE_PARTIAL_L;
            uint16_t bytesInElement = element->dataLeng - element->processedUpTo;
            elementbytesCopied = bytesInElement;
            if(bytesInElement>bytestocompletemessage) elementbytesCopied = bytestocompletemessage;
            memcpy(&MESSAGE_BUFFER[MESSAGE_PARTIAL_L],&element->p_data[element->processedUpTo],elementbytesCopied);
            MESSAGE_PARTIAL_L += elementbytesCopied;
            uint16_t processed = element->processedUpTo + elementbytesCopied;
            BLECB_Pipe_DATA_QUEUE_SetElemProcessedAmount(&BLEDATA_RECEIVEQUEUE,processed);
            
            if(MESSAGE_PARTIAL_L == MESSAGE_L) {
                // fire callback
                BLECB_Pipe_CheckIfSpeedTest(MESSAGE_L);
                if(BLECB_Pipe_ReceivedDataCallback!=NULL) BLECB_Pipe_ReceivedDataCallback(MESSAGE_BUFFER,MESSAGE_L);
                MESSAGE_L = 0;
                MESSAGE_PARTIAL_L = 0;
                OSAL_Free(MESSAGE_BUFFER);
                MESSAGE_BUFFER = NULL;
            }
            if(element->processedUpTo == element->dataLeng){
                BLECB_Pipe_DATA_QUEUE_FreeElemCircQueue(&BLEDATA_RECEIVEQUEUE);
            }   
        }      
    }
}


/**
 * BLECB PIPE QUEUES TASK HANDLER
 * @param pvParameters
 */
void _blecb_pipe_QUEUE_Task(  void *pvParameters  )
{   
    while(1)
    {
        BLECB_Pipe_ProcessRXQueue();
        BLECB_Pipe_ProcessTXQueue();
    }
}


/**
 * BLECB PIPE FREERTOS TASK CREATION
 */
void BLECB_Pipe_Task(void){
    xTaskCreate((TaskFunction_t) _blecb_pipe_QUEUE_Task,
                "QUEUES_Task",
                1024,
                NULL,
                1,
                &xblecb_pipe_QUEUE_Tasks);
}




/**
 * BLECB PIPE TRCBPS EVENT Consumer
 * @param p_event
 */
void BLECB_Pipe_Process_TRCB_Event(BLE_TRCBPS_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        
        case BLE_TRCBPS_EVT_RECEIVE_DATA:
        {
            BLECB_Pipe_dataqueue_InsertInRXQueue(p_event);
        }
        break;
        default:
        break;
    }
}

/**
 * BLECB PIPE Initialization
 * @param rxcallback callback function for received data
 */
void BLECB_Pipe_Init(pipedatarecived_callback rxcallback){
    phyInUse = DEFAULTPHY;
    BLE_TRCBPS_EventRegister(BLECB_Pipe_Process_TRCB_Event);
    BLECB_Pipe_dataqueue_Init(rxcallback);
}


/**
 * BLECB PIPE Send Data
 * @param msg
 * @param size
 */
void BLECB_Pipe_SendData(uint8_t * msg, uint16_t size){
    BLECB_Pipe_dataqueue_InsertInTXQueue((uint8_t *)msg,size);
}

/**
 * BLECB PIPE GAP EVENT Consumer
 * @param p_event
 * @return 
 */
bool BLECB_Pipe_Process_GAP_Event(BLE_GAP_Event_T * p_event){
    switch(p_event->eventId)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            ActiveConnectionHandle = p_event->eventField.evtConnect.connHandle;
            if(phyInUse==BLE_GAP_PHY_OPTION_2M)
            {
                //--- UPDATE PHY TO 2M
                uint8_t txPhys = 0, rxPhys = 0, phyOptions = 0;
                txPhys = BLE_GAP_PHY_OPTION_2M;
                rxPhys = BLE_GAP_PHY_OPTION_2M;
                BLE_GAP_SetPhy(p_event->eventField.evtConnect.connHandle, txPhys, rxPhys, phyOptions);
            }
            return true;
        }
        break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            //--- CLEAN-UP DATA QUEUES
            BLECB_Pipe_DATA_QUEUE_ClearQueue(&BLEDATA_RECEIVEQUEUE);
            BLECB_Pipe_DATA_QUEUE_ClearQueue(&BLEDATA_TRANSMITQUEUE);
             
            //--- RE-START ADVERTISING 
            BLE_GAP_SetAdvEnable(0x01, 0x00);
            
            return true;
        }
        break;
        case BLE_GAP_EVT_PHY_UPDATE:
        {
            phyInUse = p_event->eventField.evtPhyUpdate.rxPhy;
            BLECB_Pipe_Notify_APP(APP_MSG_BLECB_PIPE_PHY_UPDATED);
            return true;
        }
        break;
        default:
        break;
    }

    return false;
}


/**
 * BLECB PIPE L2CAP EVENT Consumer
 * @param p_event
 * @return 
 */
bool BLECB_Pipe_Process_L2CAP_Event(BLE_L2CAP_Event_T *p_event){
    
    switch(p_event->eventId)
    {
        
        case BLE_L2CAP_EVT_CB_CONN_IND:
        {
            BLECB_Pipe_Notify_APP(APP_MSG_BLECB_PIPE_CONNECTED);
            return true;
        }
        break;

        
        case BLE_L2CAP_EVT_CB_DISC_IND:
        {
            BLECB_Pipe_Notify_APP(APP_MSG_BLECB_PIPE_DISCONNECTED);
            return true;
        }
        break;        

        default:
        break;
    }
    return false;
    
}


/**
 * BLECB PIPE Global Event Handler
 * @param event
 * @return 
 */
bool BLECB_Pipe_Event_Handler(STACK_Event_T * p_stackEvt){
    
    bool caught = false;
    switch(p_stackEvt->groupId)
    {
        case STACK_GRP_BLE_GAP:
        {
            caught = BLECB_Pipe_Process_GAP_Event((BLE_GAP_Event_T *)p_stackEvt->p_event);
        }
        break;
        
        case STACK_GRP_BLE_L2CAP:
        {
            caught = BLECB_Pipe_Process_L2CAP_Event((BLE_L2CAP_Event_T *)p_stackEvt->p_event);
         }
        break;
        
        default:
        break;
    }
    
    if(!caught) return false;
    
    BLE_DM_BleEventHandler(p_stackEvt);
    BLE_TRCBPS_BleEventHandler(p_stackEvt);
    OSAL_Free(p_stackEvt->p_event);
    return true;
}