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
  if (pIntDev == NULL) return ERR__I2C_PARAMETER_ERROR;
#endif

  return ERR_NONE;
}
#endif // #ifdef ARDUINO

//-----------------------------------------------------------------------------


#if defined(USE_HAL_DRIVER) || defined(USE_FULL_LL_DRIVER) // STM32cubeIDE
//=============================================================================
// Function for I2C driver initialization with STM32cubeIDE
//=============================================================================
eERRORRESULT Interface_I2Cinit(I2C_Interface* pIntDev, const uint32_t sclFreq)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__I2C_PARAMETER_ERROR;
#endif
  (void)pIntDev;
  (void)sclFreq;
  return ERR_NONE;
}
#endif // #if defined(USE_HAL_DRIVER) || defined(USE_FULL_LL_DRIVER) // STM32cubeIDE

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
  if (pIntDev == NULL) return ERR__I2C_PARAMETER_ERROR;
#endif

  return ERR_NONE;
}
#endif // #ifdef ARDUINO

//-----------------------------------------------------------------------------





#if defined(USE_HAL_DRIVER) && defined(STM32G4xx_HAL_I2C_H) // STM32cubeIDE with HAL
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
// Function for I2C transfer with STM32cubeIDE and HAL driver
//=============================================================================
eERRORRESULT Interface_I2Ctransfer(I2C_Interface *pIntDev, I2CInterface_Packet* const pPacketDesc)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__I2C_PARAMETER_ERROR;
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
#endif // #if defined(USE_HAL_DRIVER) && defined(STM32G4xx_HAL_I2C_H) // STM32cubeIDE with HAL

#if defined(USE_FULL_LL_DRIVER) && defined(STM32G4xx_LL_I2C_H) // STM32cubeIDE with LL
//=============================================================================
// Function for I2C transfer with STM32cubeIDE and Low Level driver
//=============================================================================
eERRORRESULT Interface_I2Ctransfer(I2C_Interface *pIntDev, I2CInterface_Packet* const pPacketDesc)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__I2C_PARAMETER_ERROR;
#endif
  const bool DeviceWrite = ((pPacketDesc->ChipAddr & 0x01) == 0);
  const uint16_t ChipAddr = (pPacketDesc->ChipAddr & (I2C_IS_10BITS_ADDRESS(pPacketDesc->ChipAddr) ? I2C_ONLY_ADDR10_Mask : I2C_ONLY_ADDR8_Mask));
  const uint32_t ChipAddrSize = (I2C_IS_10BITS_ADDRESS(pPacketDesc->ChipAddr) ? LL_I2C_ADDRSLAVE_10BIT : LL_I2C_ADDRSLAVE_7BIT);
  const uint32_t EndMode = (pPacketDesc->Stop ? LL_I2C_MODE_SOFTEND : LL_I2C_MODE_RELOAD);
  size_t RemainingBytes = pPacketDesc->BufferSize;
  uint32_t Timeout = pIntDev->I2Ctimeout;

  //--- Device polling? ---
  if ((pPacketDesc->pBuffer == NULL) || (pPacketDesc->BufferSize <= 0))                             // Device polling only
  {
    Timeout = pIntDev->I2Ctimeout;
    LL_I2C_HandleTransfer(pIntDev->pHI2C, ChipAddr, ChipAddrSize, 0, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
    while (true)                                                                                    // Wait the polling to finish
    {
      if (LL_I2C_IsActiveFlag_NACK(pIntDev->pHI2C) > 0) return ERR__I2C_NACK;                       // If NACK received, return the error
      if (LL_I2C_IsActiveFlag_STOP(pIntDev->pHI2C) > 0) break;                                      // Wait STOP condition detected
      if (Timeout == 0) return ERR__I2C_TIMEOUT;                                                    // Timeout? return an error
      --Timeout;
    }
    return ERR_NONE;
  }

  //--- Endianness configuration for data striding ---
  const eI2C_EndianTransform EndianTransform = I2C_ENDIAN_TRANSFORM_GET(pPacketDesc->Config.Value); // Only the endianness configuration is important for this driver
  const size_t BlockSize = (EndianTransform == I2C_NO_ENDIAN_CHANGE ? 1 : (size_t)EndianTransform); // Get block size. No endian change = 8-bits data
  if ((pPacketDesc->BufferSize % BlockSize) > 0) return ERR__DATA_MODULO;                           // Data block size shall be a multiple of data size
  size_t CurrentBlockPos = BlockSize;

  //--- Transfer data ---
  uint8_t* pBuffer = &pPacketDesc->pBuffer[BlockSize - 1];                                          // Adjust the start of data for endianness
  if (DeviceWrite) // Device write
  {
    const uint32_t RequestMode = (pPacketDesc->Start ? LL_I2C_GENERATE_START_WRITE : LL_I2C_GENERATE_NOSTARTSTOP);
    LL_I2C_HandleTransfer(pIntDev->pHI2C, ChipAddr, ChipAddrSize, RemainingBytes, EndMode, RequestMode);
    while (true)
    {
      if (LL_I2C_IsActiveFlag_NACK(pIntDev->pHI2C) > 0) return ERR__I2C_NACK_DATA;                  // If NACK received, return the error
      if (LL_I2C_IsActiveFlag_STOP(pIntDev->pHI2C) > 0) break;                                      // STOP condition detected? break
      if (Timeout == 0) return ERR__I2C_TIMEOUT;                                                    // Timeout? return an error
      --Timeout;
      if (LL_I2C_IsActiveFlag_TXE(pIntDev->pHI2C) == 0) continue;                                   // TX not empty? Do another loop
      Timeout = pIntDev->I2Ctimeout;                                                                // Reset timeout

      if (RemainingBytes == 0) break;                                                               // No data remaining to send, then break the loop
      LL_I2C_TransmitData8(pIntDev->pHI2C, *pBuffer);                                               // Send next data byte
      --RemainingBytes;
      //--- Adjust buffer address with data striding ---
      --CurrentBlockPos;
      if (CurrentBlockPos == 0)
      {
        pBuffer += (2 * BlockSize) - 1;
        CurrentBlockPos = BlockSize;
      }
      else --pBuffer;
    }
  }
  else // Device read
  {
    const uint32_t RequestMode = (pPacketDesc->Start ? LL_I2C_GENERATE_START_READ : LL_I2C_GENERATE_NOSTARTSTOP);
    LL_I2C_HandleTransfer(pIntDev->pHI2C, ChipAddr, ChipAddrSize, RemainingBytes, EndMode, RequestMode);
    while (RemainingBytes > 0)
    {
      Timeout = pIntDev->I2Ctimeout;                                                                // Reset timeout
      while (true)
      {
        if (LL_I2C_IsActiveFlag_RXNE(pIntDev->pHI2C) > 0) break;                                    // Data received
        if (Timeout == 0) return ERR__I2C_TIMEOUT;                                                  // Timeout? return an error
        --Timeout;
      }

      *pBuffer = LL_I2C_ReceiveData8(pIntDev->pHI2C);                                               // Get next data byte
      --RemainingBytes;
      //--- Adjust buffer address with data striding ---
      --CurrentBlockPos;
      if (CurrentBlockPos == 0)
      {
        pBuffer += (2 * BlockSize) - 1;
        CurrentBlockPos = BlockSize;
      }
      else --pBuffer;
    }
  }
  if (pPacketDesc->Stop)
  {
    Timeout = pIntDev->I2Ctimeout;                                                                  // Reset timeout
    LL_I2C_HandleTransfer(pIntDev->pHI2C, ChipAddr, ChipAddrSize, 0, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_STOP); // Send a stop
    while (true)                                                                                    // Wait the stop to finish
    {
      if (LL_I2C_IsActiveFlag_STOP(pIntDev->pHI2C) > 0) break;                                      // Wait STOP condition detected
      if (Timeout == 0) return ERR__I2C_TIMEOUT;                                                    // Timeout? return an error
      --Timeout;
    }
    LL_I2C_ClearFlag_STOP(pIntDev->pHI2C);                                                          // Clear STOP flag
  }

  //--- Endianness result ---
  pPacketDesc->Config.Value &= ~I2C_ENDIAN_RESULT_Mask;
  pPacketDesc->Config.Value |= I2C_ENDIAN_RESULT_SET(EndianTransform);                              // Indicate that the endian transform have been processed
  return ERR_NONE;
}
#endif // #if defined(USE_FULL_LL_DRIVER) && defined(STM32G4xx_LL_I2C_H) // STM32cubeIDE with LL

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
