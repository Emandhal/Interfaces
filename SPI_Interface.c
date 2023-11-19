/*!*****************************************************************************
 * @file    SPI_Interface.c
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.1.1
 * @date    18/11/2023
 * @brief   SPI interface for drivers
 * @details This SPI interface that can be used to communicate with devices
 *          for all the https://github.com/Emandhal drivers and developments.
 *          This implements SPI communication with Arduino, STM32cubeIDE
 ******************************************************************************/

/* Revision history:
 * 1.1.1    Add STM32cubeIDE
 * 1.1.0    Add Arduino
 * 1.0.0    Release version
 *****************************************************************************/

//-----------------------------------------------------------------------------
#include "SPI_Interface.h"
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
// SPI Interface initialization implementations
//********************************************************************************************************************
#ifdef ARDUINO
//=============================================================================
// Function for SPI driver initialization with Arduino
//=============================================================================
eERRORRESULT Interface_SPIinit(SPI_Interface *pIntDev, uint8_t chipSelect, eSPIInterface_Mode mode, const uint32_t sckFreq)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;
#endif

  if (SPI_PIN_COUNT_GET(mode) > 1u) return ERR__NOT_SUPPORTED;
  pIntDev->_SPIsettings = SPISettings(sckFreq, (SPI_IS_LSB_FIRST(mode) ? LSBFIRST : MSBFIRST), SPI_MODE_GET(mode));
  return ERR_NONE;
}
#endif // #ifdef ARDUINO

//-----------------------------------------------------------------------------


#ifdef USE_HAL_DRIVER // STM32cubeIDE
//=============================================================================
// Function for SPI driver initialization with STM32cubeIDE
//=============================================================================
eERRORRESULT Interface_SPIinit(SPI_Interface *pIntDev, uint8_t chipSelect, eSPIInterface_Mode mode, const uint32_t sckFreq)
{
#ifdef CHECK_NULL_PARAM
  if (pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;
#endif

  (void)pIntDev;
  (void)chipSelect;
  (void)mode;
  (void)sckFreq;
  return ERR_NONE;
}
#endif // #ifdef USE_HAL_DRIVER // STM32cubeIDE

//-----------------------------------------------------------------------------





//********************************************************************************************************************
// SPI Interface transfer implementations
//********************************************************************************************************************
#ifdef ARDUINO
//=============================================================================
// Function for SPI transfer with Arduino
//=============================================================================
eERRORRESULT Interface_SPItransfer(SPI_Interface *pIntDev, SPIInterface_Packet* const pPacketDesc)
{
#ifdef CHECK_NULL_PARAM
  if ((pIntDev == NULL) || (pPacketDesc == NULL)) return ERR__SPI_PARAMETER_ERROR;
#endif

  pIntDev->aSPI.beginTransaction(pIntDev->SPISettings);
  //--- Disable interrupts ---
  if (SPI_IS_BLOCK_INTERRUPTS_ON_TRANSFER(pPacketDesc->Config.Value))
  {
# ifdef ARDUINO_ARCH_ESP32
    taskDISABLE_INTERRUPTS();
# else
    noInterrupts();
# endif
  }
  //--- SPI transfer ---
  digitalWrite(pPacketDesc->ChipSelect, LOW); // Set CS at low level
  for (size_t zIdx = 0; zIdx < size; ++zIdx)
  {
    uint8_t RxValue = pIntDev->aSPI.transfer(pPacketDesc->TxData[zIdx]);
    if (pPacketDesc->RxData != NULL) pPacketDesc->RxData[zIdx] = RxValue;
  }
  if (pPacketDesc->Terminate)
  {
    digitalWrite(pPacketDesc->ChipSelect, HIGH); // Set CS at high level
    //--- Enable interrupts ---
    if (SPI_IS_BLOCK_INTERRUPTS_ON_TRANSFER(pPacketDesc->Config.Value))
    {
# ifdef ARDUINO_ARCH_ESP32
      taskENABLE_INTERRUPTS();
# else
      interrupts();
# endif
    }
  }
  pIntDev->aSPI.endTransaction();
  return ERR_NONE;
}
#endif // #ifdef ARDUINO

//-----------------------------------------------------------------------------


#ifdef USE_HAL_DRIVER // STM32cubeIDE
//=============================================================================
// Function for SPI transfer with STM32cubeIDE
//=============================================================================
eERRORRESULT Interface_SPItransfer(SPI_Interface *pIntDev, SPIInterface_Packet* const pPacketDesc)
{ // It's a weak function, the user need to create the same function in his project and implement things, thus this function will be discarded
#ifdef CHECK_NULL_PARAM
  if ((pIntDev == NULL) || (pPacketDesc == NULL)) return ERR__SPI_PARAMETER_ERROR;
#endif

  HAL_StatusTypeDef HALstatus;
  if (SPI_IS_BLOCK_INTERRUPTS_ON_TRANSFER(pPacketDesc->Config.Value)) __disable_irq();  // Disable IRQ if asked
  HAL_GPIO_WritePin(pIntDev->pGPIOx, pIntDev->GPIOpin, GPIO_PIN_RESET);                 // Clear CS pin
  //--- Transfer data ---
  if (pPacketDesc->RxData != NULL)
       HALstatus = HAL_SPI_TransmitReceive(pIntDev->pHSPI, pPacketDesc->TxData, pPacketDesc->RxData, pPacketDesc->DataSize, pIntDev->SPItimeout);
  else HALstatus = HAL_SPI_Transmit(pIntDev->pHSPI, pPacketDesc->TxData, pPacketDesc->DataSize, pIntDev->SPItimeout);
  if (pPacketDesc->Terminate || (HALstatus != HAL_OK))                                  // If terminate or transfer error...
  {
    HAL_GPIO_WritePin(pIntDev->pGPIOx, pIntDev->GPIOpin, GPIO_PIN_SET);                 // Set CS pin
    if (SPI_IS_BLOCK_INTERRUPTS_ON_TRANSFER(pPacketDesc->Config.Value)) __enable_irq(); // Enable IRQ if asked
  }
  //--- Check for errors ---
  if (HALstatus == HAL_ERROR  ) return ERR__SPI_COMM_ERROR;
  if (HALstatus == HAL_BUSY   ) return ERR__SPI_BUSY;
  if (HALstatus == HAL_TIMEOUT) return ERR__SPI_TIMEOUT;
  return ERR_NONE;
}
#endif // #ifdef USE_HAL_DRIVER // STM32cubeIDE

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------