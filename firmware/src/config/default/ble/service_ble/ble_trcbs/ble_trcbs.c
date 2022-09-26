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
  BLE Transparent Service Source File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_trcbs.c

  Summary:
    This file contains the BLE Transparent Credit Based Service functions for application user.

  Description:
    This file contains the BLE Transparent Credit Based Service functions for application user.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include <stdint.h>
#include "mba_error_defs.h"
#include "gatt.h"
#include "ble_util/byte_stream.h"
#include "ble_trcbs/ble_trcbs.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

#define BLE_TRCBS_MAX_MTU_LEN                                      BLE_ATT_MAX_MTU_LEN
#define BLE_TRCBS_CCCD_NUM                                         0x01


// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
/* BLE Transparent Credit Based Service Declaration */
static uint8_t s_svcUuidBleTrcb[] = {UUID_MCHP_PROPRIETARY_SERVICE_TRCB_16};
static const uint16_t s_svcUuidBleTrcbLen = sizeof (s_svcUuidBleTrcb);

/* BLE Transparent Credit Based Ctrl Point Characteristic Declaration */
static uint8_t s_charBleTrcbCtrl[] = {(ATT_PROP_WRITE_CMD | ATT_PROP_WRITE_REQ | ATT_PROP_NOTIFY), UINT16_TO_BYTES(BLE_TRCB_HDL_CHARVAL_CTRL), UUID_MCHP_TRCB_CTRL_16};
static const uint16_t s_charBleTrcbCtrlLen = sizeof (s_charBleTrcbCtrl);

/* BLE Transparent Credit Based Ctrl Point Characteristic Value */
static uint8_t s_chUuidBleTrcbCtrl[] = {UUID_MCHP_TRCB_CTRL_16};
static uint8_t s_bleTrcbCtrlVal[1] = {0};
static uint16_t s_bleTrcbCtrlValLen = 1;

/* BLE Transparent Credit Based Ctrl Point Client Characteristic Configuration Descriptor */
static uint8_t s_descCccBleTrcbCtrl[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_descCccBleTrcbCtrlLen = sizeof (s_descCccBleTrcbCtrl);


/* BLE Transparent Credit Based PSM Characteristic Declaration */
static const uint8_t s_charBleTrcbPsm[] = {ATT_PROP_READ, UINT16_TO_BYTES(BLE_TRCB_HDL_CHARVAL_L2CAP_PSM), UUID_MCHP_TRCB_L2CAP_PSM_16};
static const uint16_t s_charBleTrcbPsmLen = sizeof(s_charBleTrcbPsm);

/* BLE Transparent Credit Based PSM Characteristic Value */
static uint8_t s_chUuidBleTrcbPsm[] = {UUID_MCHP_TRCB_L2CAP_PSM_16};
static uint8_t s_bleTrcbPsmVal[2] = {UINT16_TO_BYTES_BE(BLE_TRCB_DATA_PSM)};
static uint16_t s_bleTrcbPsmValLen = sizeof(s_bleTrcbPsmVal);


/* Attribute list for Transparent service */
static GATTS_Attribute_T s_bleTrcbList[] = {
    /* Service Declaration */
    {
        (uint8_t *) g_gattUuidPrimSvc,
        (uint8_t *) s_svcUuidBleTrcb,
        (uint16_t *) & s_svcUuidBleTrcbLen,
        sizeof (s_svcUuidBleTrcb),
        0,
        PERMISSION_READ
    },
    /* Characteristic Declaration */
    {
        (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_charBleTrcbCtrl,
        (uint16_t *) & s_charBleTrcbCtrlLen,
        sizeof (s_charBleTrcbCtrl),
        0,
        PERMISSION_READ
    },
    /* Characteristic Value */
    {
        (uint8_t *) s_chUuidBleTrcbCtrl,
        (uint8_t *) s_bleTrcbCtrlVal,
        (uint16_t *) & s_bleTrcbCtrlValLen,
        BLE_TRCBS_MAX_MTU_LEN,
        (SETTING_MANUAL_WRITE_RSP | SETTING_UUID_16 | SETTING_VARIABLE_LEN),
        PERMISSION_WRITE
    },
    /* Client Characteristic Configuration Descriptor */
    {
        (uint8_t *) g_descUuidCcc,
        (uint8_t *) s_descCccBleTrcbCtrl,
        (uint16_t *) & s_descCccBleTrcbCtrlLen,
        sizeof (s_descCccBleTrcbCtrl),
        (SETTING_MANUAL_WRITE_RSP | SETTING_CCCD),
        (PERMISSION_READ | PERMISSION_WRITE)
    },
    /* Characteristic Declaration */
    {
        (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_charBleTrcbPsm,
        (uint16_t *) & s_charBleTrcbPsmLen,
        sizeof (s_charBleTrcbPsm),
        0,
        PERMISSION_READ
    },
    /* Characteristic Value */
    {
        (uint8_t *) s_chUuidBleTrcbPsm,
        (uint8_t *) s_bleTrcbPsmVal,
        (uint16_t *) & s_bleTrcbPsmValLen,
        sizeof(s_bleTrcbPsmVal),
        0,
        PERMISSION_READ
    }
};

static const GATTS_CccdSetting_T s_trcbCccdSetting[] = 
{
    {BLE_TRCB_HDL_CCCD_CTRL, (NOTIFICATION)}
};

/* BLE Transparent Credit Based Service structure */
static GATTS_Service_T s_svcBleTrcb = 
{
    NULL,
    (GATTS_Attribute_T *) s_bleTrcbList,
    (GATTS_CccdSetting_T const *)s_trcbCccdSetting,
    BLE_TRCB_START_HDL,
    BLE_TRCB_END_HDL,
    BLE_TRCBS_CCCD_NUM
};

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

uint16_t BLE_TRCBS_Add(void)
{
    return GATTS_AddService(&s_svcBleTrcb, (BLE_TRCB_END_HDL - BLE_TRCB_START_HDL + 1));
}
