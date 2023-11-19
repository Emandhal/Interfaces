/*!*****************************************************************************
 * @file    I2C_Interface.h
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.1.1
 * @date    02/10/2021
 * @brief   I2C interface for drivers
 * @details This I2C interface that can be used to communicate with devices
 *          for all the https://github.com/Emandhal drivers and developments.
 *          This implements I2C communication with Arduino, STM32cubeIDE
 ******************************************************************************/

/* Revision history:
 * 1.1.1    Add STM32cubeIDE
 * 1.1.0    Add Arduino
 * 1.0.0    Release version
 *****************************************************************************/

//-----------------------------------------------------------------------------
#include "I2C_Interface.h"
#include "ErrorsDef.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------
#ifdef USE_HAL_DRIVER // STM32cubeIDE
#  include <Main.h> // To get the MCU general defines
#endif
//-----------------------------------------------------------------------------





//********************************************************************************************************************
// I2C Interface initialization implementations
//********************************************************************************************************************
#ifdef ARDUINO
//=============================================================================
// Function for I2C driver initialization with Arduino
//=============================================================================
eERRORRESULT Interface_I2Cinit(I2C_Interface* pIntDev, const uint32_t sclFreq)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;
#endif

  return ERR_NONE;
}
#endif // #ifdef ARDUINO

//-----------------------------------------------------------------------------


#ifdef USE_HAL_DRIVER // STM32cubeIDE
//=============================================================================
// Function for I2C driver initialization with STM32cubeIDE
//=============================================================================
eERRORRESULT Interface_I2Cinit(I2C_Interface* pIntDev, const uint32_t sclFreq)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;
#endif
  (void)pIntDev;
  (void)sclFreq;
  return ERR_NONE;
}
#endif // #ifdef USE_HAL_DRIVER // STM32cubeIDE

//-----------------------------------------------------------------------------





//********************************************************************************************************************
// I2C Interface transfer implementations
//********************************************************************************************************************
#ifdef ARDUINO
//=============================================================================
// Function for I2C transfer with Arduino
//=============================================================================
eERRORRESULT Interface_I2Ctransfer(I2C_Interface *pIntDev, I2CInterface_Packet* const pPacketDesc)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;
#endif

  return ERR_NONE;
}
#endif // #ifdef ARDUINO

//-----------------------------------------------------------------------------


#ifdef USE_HAL_DRIVER // STM32cubeIDE
//=============================================================================
// [STATIC] Wait for a proper I2C status
//=============================================================================
static eERRORRESULT __Interface_I2Cstatus(I2C_HandleTypeDef *hi2c)
{
  HAL_I2C_StateTypeDef I2Cstate;
  do
  {
    I2Cstate = HAL_I2C_GetState(hi2c);
    switch (I2Cstate)
    {
      case HAL_I2C_STATE_BUSY          :
      case HAL_I2C_STATE_BUSY_TX       :
      case HAL_I2C_STATE_BUSY_RX       :
      case HAL_I2C_STATE_READY         : break;

      default:
      case HAL_I2C_STATE_ERROR         :
      case HAL_I2C_STATE_ABORT         :
      case HAL_I2C_STATE_RESET         : return ERR__I2C_COMM_ERROR;

      case HAL_I2C_STATE_LISTEN        :
      case HAL_I2C_STATE_BUSY_TX_LISTEN:
      case HAL_I2C_STATE_BUSY_RX_LISTEN: return ERR__I2C_OTHER_BUSY;

      case HAL_I2C_STATE_TIMEOUT       : return ERR__I2C_TIMEOUT;
    }
  }
  while (I2Cstate != HAL_I2C_STATE_READY);
  return ERR_NONE;
}


//=============================================================================
// [STATIC] Convert I2C error code to eERRORRESULT
//=============================================================================
static eERRORRESULT __I2CerrorCodeToERRORRESULT(uint32_t errorCode)
{
  switch (errorCode)
  {
    case HAL_I2C_ERROR_NONE     : break;
    case HAL_I2C_ERROR_DMA      : return ERR__DMA_ERROR;
    case HAL_I2C_ERROR_TIMEOUT  : return ERR__I2C_TIMEOUT;
    case HAL_I2C_ERROR_SIZE     : return ERR__I2C_CONFIG_ERROR;
    case HAL_I2C_ERROR_DMA_PARAM: return ERR__DMA_PARAMETER_ERROR;

    default:
    case HAL_I2C_ERROR_BERR     :
    case HAL_I2C_ERROR_ARLO     :
    case HAL_I2C_ERROR_AF       :
    case HAL_I2C_ERROR_OVR      : return ERR__I2C_COMM_ERROR;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
    case HAL_I2C_ERROR_INVALID_CALLBACK:
#endif // USE_HAL_I2C_REGISTER_CALLBACKS
    case HAL_I2C_ERROR_INVALID_PARAM: return ERR__I2C_PARAMETER_ERROR;
  }
  return ERR_NONE;
}


//=============================================================================
// [STATIC] Convert HAL_StatusTypeDef to eERRORRESULT
//=============================================================================
static eERRORRESULT __I2CHALstatusToERRORRESULT(HAL_StatusTypeDef halError)
{
  switch (halError)
  {
    case HAL_OK     : break;
    default:
    case HAL_ERROR  : return ERR__I2C_COMM_ERROR;
    case HAL_BUSY   : return ERR__I2C_NACK;
    case HAL_TIMEOUT: return ERR__I2C_TIMEOUT;
  }
  return ERR_NONE;
}


//=============================================================================
// Function for I2C transfer with STM32cubeIDE
//=============================================================================
eERRORRESULT Interface_I2Ctransfer(I2C_Interface *pIntDev, I2CInterface_Packet* const pPacketDesc)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;
#endif
  const bool DeviceWrite = ((pPacketDesc->ChipAddr & 0x01) == 0);
  const uint16_t ChipAddr = (pPacketDesc->ChipAddr & I2C_ONLY_ADDR_Mask);
  const uint32_t XferOption = (pPacketDesc->Stop ? I2C_AUTOEND_MODE : I2C_SOFTEND_MODE);
  HAL_StatusTypeDef HALerror;
  eERRORRESULT Error;

  //--- Device polling? ---
  if ((pPacketDesc->pBuffer == NULL) || (pPacketDesc->BufferSize <= 0)) // Device polling only
  {
    HALerror = HAL_I2C_IsDeviceReady(pIntDev->pHI2C, ChipAddr, 1, 2);
    switch (HALerror)
    {
      case HAL_OK     : break;
      default:
      case HAL_ERROR  : return ERR__I2C_COMM_ERROR;
      case HAL_BUSY   : return ERR__I2C_NACK;
      case HAL_TIMEOUT: return ERR__I2C_TIMEOUT;
    }
    return ERR_NONE;
  }

  //--- Transfer data ---
  Error = __Interface_I2Cstatus(pIntDev->pHI2C); // Wait for a I2C ready
  if (Error != ERR_NONE) return Error;           // If the status of the I2C is not ready, return the error
  if (DeviceWrite)
  {
    Error = __I2CHALstatusToERRORRESULT(HAL_I2C_Master_Seq_Transmit_IT(pIntDev->pHI2C, ChipAddr, pPacketDesc->pBuffer, pPacketDesc->BufferSize, XferOption));
    if (Error != ERR_NONE) return Error;         // If there is an error while calling HAL_I2C_Master_Seq_Transmit_IT() then return the error
  }
  else
  {
    Error = __I2CHALstatusToERRORRESULT(HAL_I2C_Master_Seq_Receive_IT(pIntDev->pHI2C, ChipAddr, pPacketDesc->pBuffer, pPacketDesc->BufferSize, XferOption));
    if (Error != ERR_NONE) return Error;         // If there is an error while calling HAL_I2C_Master_Seq_Receive_IT() then return the error
  }
  return __I2CerrorCodeToERRORRESULT(HAL_I2C_GetError(pIntDev->pHI2C));
}
#endif // #ifdef USE_HAL_DRIVER // STM32cubeIDE

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
