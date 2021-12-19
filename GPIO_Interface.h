/*******************************************************************************
 * @file    GPIO_Interface.h
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.0.0
 * @date    23/10/2021
 * @brief   GPIO interface for driver
 *
 * This GPIO interface definitions for all the https://github.com/Emandhal drivers
 * and developments
 ******************************************************************************/
 /* @page License
 *
 * Copyright (c) 2020-2021 Fabien MAILLY
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
#ifndef __GPIO_INTERFACE_H_INC
#define __GPIO_INTERFACE_H_INC
//=============================================================================

//-----------------------------------------------------------------------------
#include "ErrorsDef.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#  define GPIO_MEMBER(name)
#else
#  define GPIO_MEMBER(name)  .name =
#endif
//-----------------------------------------------------------------------------

#define GPIO_AS_OUTPUT  0
#define GPIO_AS_INPUT   1

//-----------------------------------------------------------------------------





//********************************************************************************************************************
// GPIO Interface functions definitions
//********************************************************************************************************************
typedef struct GPIO_Interface GPIO_Interface; //! Typedef of GPIO_Interface device object structure


/*! @brief Interface function for GPIO set direction
 *
 * This function will be called to change the direction of the GPIO
 * @param[in] *pIntDev Is the GPIO interface container structure used for the GPIO set direction
 * @param[in] pinsDirection Set the IO pin direction, if bit is '1' then the corresponding GPIO is input else it's output
 * @param[in] pinsChangeMask If the bit is set to '1', then the corresponding GPIO must be modified (this is the GPIO_Interface.PinBitMask)
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*GPIOSetDirection_Func)(GPIO_Interface *pIntDev, const uint32_t pinDirection, const uint32_t pinChangeMask);


/*! @brief Interface function for getting GPIO pin input level
 *
 * @param[in] *pIntDev Is the GPIO interface container structure used for the GPIO get input level
 * @param[out] *pinsState Return the actual level of the I/O pin. If bit is '1' then the corresponding GPIO is level high else it's level low
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*GPIOGetInputLevel_Func)(GPIO_Interface *pIntDev, uint32_t *pinsState);


/*! @brief Interface function for setting GPIO pins output level
 *
 * @param[in] *pIntDev Is the GPIO interface container structure used for the GPIO set output level
 * @param[in] pinLevel Set the IO pin output level, if bit is '1' then the corresponding GPIO is level high else it's level low
 * @param[in] pinsChangeMask If the bit is set to '1', then the corresponding GPIO must be modified (this is the GPIO_Interface.PinBitMask)
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*GPIOSetOutputLevel_Func)(GPIO_Interface *pIntDev, const uint32_t pinLevel, const uint32_t pinChangeMask);

//-----------------------------------------------------------------------------



//! @brief GPIO interface container structure
struct GPIO_Interface
{
  void *InterfaceDevice;                         //!< This is the pointer that will be in the first parameter of all interface call functions
  GPIOSetDirection_Func fnGPIO_SetDirection;     //!< This function will be called when the driver needs to change a pin direction
  GPIOGetInputLevel_Func fnGPIO_GetInputLevel;   //!< This function will be called when the driver needs to get the input level of a pin
  GPIOSetOutputLevel_Func fnGPIO_SetOutputLevel; //!< This function will be called when the driver needs to set the output level of a pin
  uint32_t PinBitMask;                           //!< GPIO pin bit index on the device port. THIS IS NOT THE PIN NUMBER
};

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif /* __GPIO_INTERFACE_H_INC */