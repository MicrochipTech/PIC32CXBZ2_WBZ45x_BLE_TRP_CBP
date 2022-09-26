/**************************************************************************
  Crypto Framework Library Source

  Company:
    Microchip Technology Inc.

  File Name:
    CryptoLib_Rc_pb.h

  Summary:
    Crypto Framework Libarary interface file for hardware Cryptography

  Description:
    This file provides an example for interfacing with the PUKCC module
    on the SAME5x device family.
**************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2019 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

//DOM-IGNORE-END

#ifndef _CRYPTOLIB_RC_PB_INCLUDED
#define _CRYPTOLIB_RC_PB_INCLUDED

// Standard Return and Severity Codes
#define PUKCL_SEVERE(a) (a | 0xC000)
#define PUKCL_WARNING(a) (a | 0x8000)
#define PUKCL_INFO(a) (a | 0x4000)
#define PUKCL_SEVERITY_MASK(a) (a | 0xC000)

// Generic Return Codes
#define PUKCL_OK 0x0000
#define PUKCL_COMPUTATION_NOT_STARTED PUKCL_SEVERE(0x0001)
#define PUKCL_UNKNOWN_SERVICE PUKCL_SEVERE(0x0002)
#define PUKCL_UNEXPLOITABLE_OPTIONS PUKCL_SEVERE(0x0003)
#define PUKCL_HARDWARE_ISSUE PUKCL_SEVERE(0x0004)
#define PUKCL_WRONG_HARDWARE PUKCL_SEVERE(0x0005)
#define PUKCL_LIBRARY_MALFORMED PUKCL_SEVERE(0x0006)
#define PUKCL_ERROR PUKCL_SEVERE(0x0007)
#define PUKCL_UNKNOWN_SUBSERVICE PUKCL_SEVERE(0x0008)

// Preliminary tests Return Codes (when not in release)
#define PUKCL_OVERLAP_NOT_ALLOWED PUKCL_SEVERE(0x0010)
#define PUKCL_PARAM_NOT_IN_PUKCCRAM PUKCL_SEVERE(0x0011)
#define PUKCL_PARAM_NOT_IN_RAM PUKCL_SEVERE(0x0012)
#define PUKCL_PARAM_NOT_IN_CPURAM PUKCL_SEVERE(0x0013)
#define PUKCL_PARAM_WRONG_LENGTH PUKCL_SEVERE(0x0014)
#define PUKCL_PARAM_BAD_ALIGNEMENT PUKCL_SEVERE(0x0015)
#define PUKCL_PARAM_X_BIGGER_THAN_Y PUKCL_SEVERE(0x0016)
#define PUKCL_PARAM_LENGTH_TOO_SMALL PUKCL_SEVERE(0x0017)

// Run time errors (even when in release)
#define PUKCL_DIVISION_BY_ZERO PUKCL_SEVERE(0x0101)
#define PUKCL_MALFORMED_MODULUS PUKCL_SEVERE(0x0102)
#define PUKCL_FAULT_DETECTED PUKCL_SEVERE(0x0103)
#define PUKCL_MALFORMED_KEY PUKCL_SEVERE(0x0104)
#define PUKCL_APRIORI_OK PUKCL_SEVERE(0x0105)
#define PUKCL_WRONG_SERVICE PUKCL_SEVERE(0x0106)

// Run time events (not obviously severe)
#define PUKCL_POINT_AT_INFINITY PUKCL_WARNING(0x0001)
#define PUKCL_WRONG_SIGNATURE PUKCL_WARNING(0x0002)
#define PUKCL_WRONG_SELECTNUMBER PUKCL_WARNING(0x0003)
#define PUKCL_POINT_IS_NOT_ON_CURVE PUKCL_WARNING(0x0004)
// Run time informations (even when in release)
#define PUKCL_NUMBER_IS_NOT_PRIME PUKCL_INFO(0x0001)
#define PUKCL_NUMBER_IS_PRIME PUKCL_INFO(0x0002)

#endif // _CRYPTOLIB_RC_PB_INCLUDED
