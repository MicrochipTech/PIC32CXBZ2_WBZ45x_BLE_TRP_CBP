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

/*******************************************************************************
  BLE Transparent Credit Based Server Profile Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_trcbps.h

  Summary:
    This file contains the BLE Transparent Credit Based Server functions for application user.

  Description:
    This file contains the BLE Transparent Credit Based Server functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_TRCBPS
 * @{
 * @brief Header file for the BLE Transparent Credit Based Profile Server role library.
 * @note Definitions and prototypes for the BLE Transparent Credit Based profile stack layer application programming interface.
 */
#ifndef BLE_TRCBPS_H
#define BLE_TRCBPS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "gatt.h"
#include "stack_mgr.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

/**@addtogroup BLE_TRCBPS_DEFINES Defines
 * @{ */

/**@defgroup BLE_TRCBPS_MAX_CONN_NBR Maximum connection number
 * @brief The definition of maximum  connection number of BLE Transparent Credit Based Profile.
 * @{ */
#define BLE_TRCBPS_MAX_CONN_NBR                           BLE_GAP_MAX_LINK_NBR    /**< Maximum allowing Connection Numbers. */
/** @} */

/**@defgroup BLE_TRCBPS_L2CAP_ID Definition of unassigned L2CAP ID
 * @brief The definition of unassigned L2CAP ID.
 * @{ */
#define BLE_TRCBPS_L2CAP_UNASSIGNED_ID                    0xFF    /**< Unassigned BLE L2CAP ID. */
/** @} */

/**@defgroup BLE_TRCBPS_STATUS TRCPBS status
 * @brief The definition of BLE Transparent Credit Based Profile connection status.
 * @{ */
#define BLE_TRCBPS_STATUS_STANDBY                         0x00    /**< BLE TRCBPS is standby. */
#define BLE_TRCBPS_STATUS_CONNECTING                      0x01    /**< BLE TRCBPS is connecting. */
#define BLE_TRCBPS_STATUS_CONNECTED                       0x02    /**< BLE TRCBPS is connected. */
#define BLE_TRCBPS_STATUS_DISCONNECTING                   0x03    /**< BLE TRCBPS is disconnecting. */
/** @} */

/**@defgroup BLE_TRCBPS_PARA Definition of TRCBPS parameters
 * @brief The definition of BLE Transparent Credit Based Profile parameters.
 * @{ */
#define BLE_TRCBPS_SDU_LEN_FIELD_LEN          0x02                                  /**< Length of the SDU length field. */
#define BLE_TRCBPS_CTRL_MTU                   BLE_ATT_MAX_MTU_LEN - BLE_TRCBPS_SDU_LEN_FIELD_LEN               /**< MTU size of control channel. */
#define BLE_TRCBPS_DATA_MTU                   BLE_ATT_MAX_MTU_LEN - BLE_TRCBPS_SDU_LEN_FIELD_LEN               /**< MTU size of data channel. */
#define BLE_TRCBPS_CTRL_MPS                   BLE_L2CAP_MAX_PDU_SIZE                /**< MPS size of control channel. */
#define BLE_TRCBPS_DATA_MPS                   BLE_L2CAP_MAX_PDU_SIZE                /**< MPS size of data channel. */

#define BLE_TRCBPS_CTRL_MAX_CREDITS           0x0002                                /**< Maximum credit value of control channel. */
#define BLE_TRCBPS_CTRL_MAX_ACCU_CREDITS      0x0001                                /**< Maximum accumulation credits which will be sent to the peer device of control channel. */
#define BLE_TRCBPS_DATA_MAX_CREDITS           0x0008                                /**< Maximum credit value of data channel. */
#define BLE_TRCBPS_DATA_MAX_ACCU_CREDITS      0x0005                                /**< Maximum accumulation credits which will be sent to the peer device of data channel. */
#define BLE_TRCBPS_PERMISSION                 0x00                                  /**< Permission setting. */

/** @} */


/**@defgroup BLE_TRCBPS_MAX_BUF Maximum buffer list
 * @brief The definition of maximum buffer list.
 * @{ */
#define BLE_TRCBPS_CTRL_MAX_BUF_IN           BLE_TRCBPS_CTRL_MAX_CREDITS             /**< Maximum number of PacketIn buffer list for receive data on control channel. */ 
#define BLE_TRCBPS_DATA_MAX_BUF_IN           BLE_TRCBPS_DATA_MAX_CREDITS             /**< Maximum number of PacketIn buffer list for receive data on data channel. */
/** @} */


/**@defgroup BLE_TRCBPS_VENDOR_OPCODE Definition of vendor opcode
 * @brief The definition of BLE Transparent Credit Based Profile vendor command opcodes over control channel
 * @{ */
#define BLE_TRCBPS_VENDOR_OPCODE_MIN         0x20    /**< Minimum value of Opcode. */
#define BLE_TRCBPS_VENDOR_OPCODE_MAX         0xFF    /**< Maximum value of Opcode. */
/** @} */


/**@} */ //BLE_TRCBPS_DEFINES


/**@addtogroup BLE_TRCBPS_ENUMS Enumerations
 * @{ */

/**@brief Enumeration type of BLE Transparent Credit Based Profile callback events. */
typedef enum BLE_TRCBPS_EventId_T
{
    BLE_TRCBPS_EVT_CONNECTION_STATUS = 0x00,                              /**< Transparent Credit Based Profile Data/Control Channel connection status update event. See @ref BLE_TRCBPS_EvtConnStatus_T for event details. */
    BLE_TRCBPS_EVT_RECEIVE_DATA,                                          /**< Transparent Credit Based Profile Data Channel received notification event. See @ref BLE_TRCBPS_EvtReceiveData_T for event details. */
    BLE_TRCBPS_EVT_VENDOR_CMD,                                            /**< Transparent Credit Based Profile vendor command received notification event. See @ref BLE_TRCBPS_EvtVendorCmd_T for event details. */
    BLE_TRCBPS_EVT_ERR_NO_MEM                                             /**< Profile internal error occurs due to insufficient heap memory. */
} BLE_TRCBPS_EventId_T;

/**@brief Enumeration type of BLE Transparent Credit Based Profile channel type. */
typedef enum BLE_TRCBPS_ChanType_T
{
    BLE_TRCBPS_UNKNOWN_CHAN = 0x00,                                      /**< Unknown Channel. */
    BLE_TRCBPS_CTRL_CHAN,                                                /**< Control Channel. */
    BLE_TRCBPS_DATA_CHAN                                                 /**< Data Channel. */
} BLE_TRCBPS_ChanType_T;

/**@} */ //BLE_TRCBPS_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/**@addtogroup BLE_TRCBPS_STRUCTS Structures
 * @{ */

/**@brief Data structure for @ref BLE_TRCBPS_EVT_CONNECTION_STATUS event. */
typedef struct BLE_TRCBPS_EvtConnStatus_T
{
    uint16_t                connHandle;                                  /**< Connection handle associated with this connection. */
    BLE_TRCBPS_ChanType_T   chanType;                                    /**< Channel Type. See @ref BLE_TRCBPS_ChanType_T. */
    uint8_t                 leL2capId;                                   /**< Instance of L2CAP session. Invaild when it was assigned as BLE_TRCBPS_L2CAP_UNASSIGNED_ID. Only available for data channel. */
    uint16_t                peerMtu;                                     /**< Maximum MTU size of peer device. Only available for data channel. */
    uint16_t                peerMps;                                     /**< Maximum PDU size of peer device. Only available for data channel. */
    uint8_t                 status;                                      /**< Connection status. See @ref BLE_TRCBPS_STATUS. */
} BLE_TRCBPS_EvtConnStatus_T;


/**@brief Data structure for @ref BLE_TRCBPS_EVT_RECEIVE_DATA event. */
typedef struct BLE_TRCBPS_EvtReceiveData_T
{
    uint16_t                connHandle;                                  /**< Connection handle associated with this connection. */
    BLE_TRCBPS_ChanType_T   chanType;                                    /**< Channel Type. See @ref BLE_TRCBPS_ChanType_T. */
    uint8_t                 leL2capId;                                   /**< Instance of L2CAP session. */
} BLE_TRCBPS_EvtReceiveData_T;

/**@brief Data structure for @ref BLE_TRCBPS_EVT_VENDOR_CMD event. */
typedef struct BLE_TRCBPS_EvtVendorCmd_T
{
    uint16_t         connHandle;                                           /**< Connection handle associated with this connection. */
    uint16_t         length;                                               /**< Length of Vendor command. */
    uint8_t          *p_payLoad;                                           /**< Pointer to Vendor command payload. */
} BLE_TRCBPS_EvtVendorCmd_T;

/**@brief The union of BLE Transparent Credit Based profile event types. */
typedef union BLE_TRCBPS_EventField_T
{
    BLE_TRCBPS_EvtConnStatus_T        connStatus;                          /**< Handle @ref BLE_TRCBPS_EVT_CONNECTION_STATUS. */
    BLE_TRCBPS_EvtReceiveData_T       onReceiveData;                       /**< Handle @ref BLE_TRCBPS_EVT_RECEIVE_DATA. */
    BLE_TRCBPS_EvtVendorCmd_T         onVendorCmd;                         /**< Handle @ref BLE_TRCBPS_EVT_VENDOR_CMD. */
} BLE_TRCBPS_EventField_T;

/**@brief BLE Transparent Credit Based profile callback event. */
typedef struct BLE_TRCBPS_Event_T
{
    BLE_TRCBPS_EventId_T       eventId;                                   /**< Event ID. See @ref BLE_TRCBPS_EventId_T. */
    BLE_TRCBPS_EventField_T    eventField;                                /**< Event field. */
} BLE_TRCBPS_Event_T;


/**@brief The structure contains information about BLE Transparent Credit Based Profile connection parameters. */
typedef struct BLE_TRCBPS_ConnPara_T
{
    uint16_t spsm;                                                        /**< Simplified Protocol/Service Multiplexer. */
    uint16_t mtu;                                                         /**< Maximum Transmission Unit. */
    uint16_t mps;                                                         /**< Maximum PDU Payload Size. */
    uint16_t initCredits;                                                 /**< Initial Credits. It should not exceed BLE_TRCBPS_DATA_MAX_CREDITS. */
    uint8_t permission;                                                   /**< Permission of the SPSM. */
} BLE_TRCBPS_ConnPara_T;

/**@brief BLE Transparent Credit Based profile callback type. This callback function sends BLE Transparent Credit Based profile events to the application. */
typedef void(*BLE_TRCBP_EventCb_T)(BLE_TRCBPS_Event_T *p_event);

/**@} */ //BLE_TRCBPS_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_TRCBPS_FUNS Functions
 * @{ */

/**
 *@brief Register BLE Transparent Credit Based profile callback.
 *
 *@param[in] bleTrcbpHandler                 Client callback function.
 *
 */
void BLE_TRCBPS_EventRegister(BLE_TRCBP_EventCb_T bleTrcbpHandler);


/**@brief Initialize BLE Transparent Credit Based Profile.
 * 
 * @retval MBA_RES_SUCCESS                   Successfully Initialize BLE Transparent Credit Based Profile.
 * @retval MBA_RES_INVALID_PARA              Invalid MTU parameter usage.
 * @retval MBA_RES_FAIL                      Fail to register SPSM or fail to register the service..
 *
 */
uint16_t BLE_TRCBPS_Init(void);


/**@brief Query local L2CAP CoC PSM value of TRCBP Data Channel.
 *@param[out] dataPsm                         Pointer to the PSM of data channel.
 *
 * @retval MBA_RES_SUCCESS                    Successfully query the PSM value.
 *
 */
uint16_t BLE_TRCBPS_QueryPsm(uint16_t *dataPsm);


/**@brief IIssue a L2CAP CoC connection request for Transparent Credit Based Profile Data pipe establishment.
 * @param[in] connHandle                       Connection handle.
 *
 * @retval MBA_RES_SUCCESS                    Successfully issue a connection request.
 * @retval MBA_RES_OOM                        No available memory.
 * @retval MBA_RES_INVALID_PARA               The connection handle or SPSM doesn't exist.
 * @retval MBA_RES_NO_RESOURCE                No Transmit buffers available for sending connection request.
 *
 */
uint16_t BLE_TRCBPS_ConnReq(uint16_t connHandle);


/**@brief Issue a L2CAP CoC disconnect request for Transparent Credit Based Profile Data pipe.
 * @param[in] connHandle                       Connection handle.
 *
 * @retval MBA_RES_SUCCESS                    Successfully send a disconnect request.
 * @retval MBA_RES_OOM                        No available memory.
 * @retval MBA_RES_INVALID_PARA               The L2CAP link doesn't exist.
 * @retval MBA_RES_NO_RESOURCE                No Transmit buffers available for sending a disconnect request.
 * @retval MBA_RES_FAIL                       Fail to send disconnect request by incorrect connection handle or SPSM.
 *
 */
uint16_t BLE_TRCBPS_DisconnectReq(uint16_t connHandle);

/**@brief Send data via BLE Transparent Credit Based Profile Data pipe.
 *
 * @param[in] connHandle                      Connection handle.
 * @param[in] len                             Data length.
 * @param[in] p_data                          Pointer to the transparent data.
 *
 * @retval MBA_RES_SUCCESS                    Successfully issue a send data command.
 * @retval MBA_RES_OOM                        No available memory.
 * @retval MBA_RES_INVALID_PARA               The L2CAP link doesn't exist or parameter does not meet the spec such as the data length exceeds the maximum MTU size of remote device.
 * @retval MBA_RES_NO_RESOURCE                There is no avaliable credit of the remote device.
 * @retval MBA_RES_BAD_STATE                  The connection of Transparent Credit Based Profile is not established.
 *
 */
uint16_t BLE_TRCBPS_SendData(uint16_t connHandle, uint16_t len, uint8_t *p_data);

/**@brief Send vendor command via BLE Transparent Credit Based Profile Control pipe.
 *
 * @param[in] connHandle                      Connection handle.
 * @param[in] cmdId                           Vendor command Opcode. See @ref BLE_TRCBPS_VENDOR_OPCODE.
 * @param[in] len                             Length of vendor command
 * @param[in] p_payload                       Pointer to the payload of vendor command
 *
 * @retval MBA_RES_SUCCESS                    Successfully issue a send vendor command.
 * @retval MBA_RES_OOM                        No available memory.
 * @retval MBA_RES_INVALID_PARA               The L2CAP link doesn't exist or parameter does not meet the spec such as the data length exceeds the maximum MTU size of remote device.
 * @retval MBA_RES_BAD_STATE                  The connection of Transparent Credit Based Profile is not established.
 *
 */
uint16_t BLE_TRCBPS_SendVendorCommand(uint16_t connHandle, uint8_t cmdId, uint16_t len, uint8_t *p_payload);

/**@brief Get the length of queued data in Data pipe by connection handle.
 *
 * @param[in] connHandle                      Connection handle.
 * @param[out] p_dataLength                   Poniter to the length of the queued data
 *
 * @retval MBA_RES_SUCCESS                    Get the length successfully.
 * @retval MBA_RES_INVALID_PARA               The L2CAP link doesn't exist.
 *
 */
uint16_t BLE_TRCBPS_GetDataLength(uint16_t connHandle, uint16_t *p_dataLength);


/**@brief Get the queued data in Data pipe by connection handle.
 *
 * @param[in] connHandle                      Connection handle.
 * @param[out] p_data                         Pointer to the data buffer
 *
 * @retval MBA_RES_SUCCESS                    Get the data successfully.
 * @retval MBA_RES_INVALID_PARA               The L2CAP link doesn't exist.
 * @retval MBA_RES_FAIL                       Failed to get data due to the queue is empty .
 *
 */
uint16_t BLE_TRCBPS_GetData(uint16_t connHandle, uint8_t *p_data);


/**@brief Handle BLE_Stack events.
 *        This API should be called in the application while catching BLE_Stack events
 *
 * @param[in] p_stackEvent                    Pointer to BLE_Stack events buffer.
 *
*/
void BLE_TRCBPS_BleEventHandler(STACK_Event_T *p_stackEvent);


/**@} */ //BLE_TRCBPS_FUNS

#endif

/**
  @}
 */ 
