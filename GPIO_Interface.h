/*!*****************************************************************************
 * @file    GPIO_Interface.h
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.1.0
 * @date    17/06/2023
 * @brief   GPIO interface for driver
 * @details This PORT and GPIO interface definitions for all the
 * https://github.com/Emandhal drivers and developments
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
 * 1.1.0    Modify GPIO interface and add PORT interface
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
// PORT Interface functions definitions
//********************************************************************************************************************

typedef struct PORT_Interface PORT_Interface; //! Typedef of PORT_Interface device object structure

//-----------------------------------------------------------------------------


/*! @brief Interface function for PORT set pins direction
 *
 * This function will be called to change the direction of a whole PORT
 * @param[in] *pIntDev Is the PORT interface container structure used for the PORT set direction
 * @param[in] pinsDirection Set the PORT pin direction, if bit is '1' then the corresponding GPIO is input else it's output
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*PORTSetDirection_Func)(PORT_Interface *pIntDev, const uint32_t pinsDirection);


/*! @brief Interface function for getting PORT pins input level
 *
 * @param[in] *pIntDev Is the PORT interface container structure used to get input level of a whole PORT
 * @param[out] *pinsLevel Return the actual level of the PORT pins. If bit is '1' then the corresponding GPIO is level high else it's level low
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*PORTGetInputLevel_Func)(PORT_Interface *pIntDev, uint32_t *pinsLevel);


/*! @brief Interface function for setting PORT pins output level
 *
 * @param[in] *pIntDev Is the PORT interface container structure used to set output level of a whole PORT
 * @param[in] pinsLevel Set the PORT pins output level, if bit is '1' then the corresponding GPIO is level high else it's level low
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*PORTSetOutputLevel_Func)(PORT_Interface *pIntDev, const uint32_t pinsLevel);

//-----------------------------------------------------------------------------

//! @brief PORT interface container structure
struct PORT_Interface
{
  void *InterfaceDevice;                         //!< This is the pointer that will be in the first parameter of all interface call functions
  uint32_t UniqueID;                             //!< This is a protection for the #InterfaceDevice pointer. This value will be check when using the struct PORT_Interface in the driver which use the generic PORT interface
  PORTSetDirection_Func fnPORT_SetDirection;     //!< This function will be called when the driver needs to change a PORT direction
  PORTGetInputLevel_Func fnPORT_GetInputLevel;   //!< This function will be called when the driver needs to get the input level of a PORT
  PORTSetOutputLevel_Func fnPORT_SetOutputLevel; //!< This function will be called when the driver needs to set the output level of a PORT
  uint8_t PORTindex;                             //!< PORT index on the device port
};

//-----------------------------------------------------------------------------





//********************************************************************************************************************
// GPIO Interface functions definitions
//********************************************************************************************************************

typedef struct GPIO_Interface GPIO_Interface; //! Typedef of GPIO_Interface device object structure

//-----------------------------------------------------------------------------

//! I/O expander configuration flags (can be OR'ed)
typedef enum
{
  GPIO_STATE_OUTPUT = 0x00, //!< GPIO state change to output
  GPIO_STATE_INPUT  = 0x01, //!< GPIO state change to input
  GPIO_STATE_RESET  = 0x10, //!< GPIO output level change '0'
  GPIO_STATE_SET    = 0x11, //!< GPIO output level change '1'
  GPIO_STATE_TOGGLE = 0x20, //!< Toggle output level
} eGPIO_State;

//-----------------------------------------------------------------------------


/*! @brief Interface function for GPIO set direction
 *
 * This function will be called to change the direction of the GPIO
 * @param[in] *pIntDev Is the GPIO interface container structure used for the GPIO set direction
 * @param[in] pinState Set the GPIO state following #eGPIO_State enumerator
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*GPIOSetState_Func)(GPIO_Interface *pIntDev, const eGPIO_State pinState);


/*! @brief Interface function for getting GPIO pin input level
 *
 * @param[in] *pIntDev Is the GPIO interface container structure used for the GPIO get input level
 * @param[out] *pinLevel Return the actual level of the I/O pin
 * @return Returns an #eERRORRESULT value enum
 */
typedef eERRORRESULT (*GPIOGetInputLevel_Func)(GPIO_Interface *pIntDev, eGPIO_State *pinLevel);

//-----------------------------------------------------------------------------

//! @brief GPIO interface container structure
struct GPIO_Interface
{
  void *InterfaceDevice;                       //!< This is the pointer that will be in the first parameter of all interface call functions
  uint32_t UniqueID;                           //!< This is a protection for the #InterfaceDevice pointer. This value will be check when using the struct GPIO_Interface in the driver which use the generic GPIO interface
  GPIOSetState_Func fnGPIO_SetState;           //!< This function will be called when the driver needs to change a pin state
  GPIOGetInputLevel_Func fnGPIO_GetInputLevel; //!< This function will be called when the driver needs to get the input level of a pin
  uint32_t PinBitMask;                         //!< GPIO pin bit mask on the device port. THIS IS NOT THE PIN NUMBER
  uint8_t PORTindex;                           //!< PORT index on the device port
};

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif /* __GPIO_INTERFACE_H_INC */