/*!*****************************************************************************
 * @file    UART_Interface.h
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.0.0
 * @date    16/01/2022
 * @brief   UART interface for driver
 *
 * This UART interface definitions for all the https://github.com/Emandhal drivers
 * and developments
 ******************************************************************************/
 /* @page License
 *
 * Copyright (c) 2020-2023 Fabien MAILLY
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

/* Revision history:
 * 1.0.0    Release version
 *****************************************************************************/
#ifndef __UART_INTERFACE_H_INC
#define __UART_INTERFACE_H_INC
//=============================================================================

//-----------------------------------------------------------------------------
#include <stdio.h>
#include "ErrorsDef.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
   extern "C" {
#  define UART_MEMBER(name)
#else
#  define UART_MEMBER(name)  .name =
#endif
//-----------------------------------------------------------------------------

#define UART_NO_ERROR  0

//-----------------------------------------------------------------------------





//********************************************************************************************************************
// UART Interface functions definitions
//********************************************************************************************************************

typedef struct UART_Interface UART_Interface; //! Typedef of UART_Interface device object structure

//-----------------------------------------------------------------------------

/*! @brief Interface function for UART transmit
 *
 * This function will be called to try to transmit data over the UART
 * This function only try to transmit and it is not intend to transmit all the data. To transmit all the data, repeat calling this function until size == 0
 * @param[in] *pIntDev Is the UART interface container structure used for the UART transmit
 * @param[in] *data Is the data array to send to the UART transmiter through the transmit FIFO
 * @param[in] size Is the count of data to send to the UART transmitter through the transmit FIFO
 * @param[out] *actuallySent Is the count of data actually sent to the transmit FIFO
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*UARTtransmit_Func)(UART_Interface *pIntDev, uint8_t *data, size_t size, size_t *actuallySent);


/*! @brief Interface function for UART receive
 *
 * @param[in] *pIntDev Is the UART interface container structure used for the UART receive
 * @param[out] *data Is where the data will be stored
 * @param[in] size Is the count of data that the data buffer can hold
 * @param[out] *actuallyReceived Is the count of data actually received from the received FIFO
 * @param[out] *lastCharError Is the last char received error. Set to UART_NO_ERROR (0) if no errors
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*UARTreceive_Func)(UART_Interface *pIntDev, uint8_t *data, size_t size, size_t *actuallyReceived, uint8_t *lastCharError);

//-----------------------------------------------------------------------------

//! @brief UART interface container structure
struct UART_Interface
{
  void* InterfaceDevice;             //!< This is the pointer that will be in the first parameter of all interface call functions
  UARTtransmit_Func fnUART_Transmit; //!< This function will be called when a driver/library needs to transmit data
  UARTreceive_Func fnUART_Receive;   //!< This function will be called when a driver/library needs to receive data
  uint8_t Channel;                   //!< UART channel of the interface device
};

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif /* __UART_INTERFACE_H_INC */