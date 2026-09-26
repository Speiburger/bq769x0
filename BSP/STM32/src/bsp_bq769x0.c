//
// Created by justus on 9/23/26.
//

#include "bsp_bq769x0.h"

/* Private helper functions*/

uint8_t bq769x0_crc(const uint8_t *data, size_t len) {
    // TODO: Null pointer check
    uint8_t crc = 0;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80U) {
                crc = (uint8_t)((crc << 1) ^
                                0x07U); /* CRC polynomial of x^8 + x^2 + x + 1*/
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}

/* Public functions*/

/** \brief Read one or more registers over I2C
 *
 * Reads the requested registers from the chip via I2C and verifies each
 * received byte against its accompanying CRC byte.
 *
 * \param handle I2C_HandleTypeDef used for the transfer
 * \param registerAddress Address of the first register to read
 * \param dataPtr Buffer to store the read data in
 * \param length Number of registers to read
 * \return BQ769X0_OK on success, error code otherwise
 */
bq769x0_ErrorCode_t bsp_bq769x0_readFunc(
    void *handle,
    uint8_t registerAddress,
    uint8_t *dataPtr,
    uint8_t length
) {
    RROR;
    if ((handle == NULL) || (dataPtr == NULL)) {
        return BQ769X0_NULLPTR;
    }
    if (length < 1) {
        return BQ769X0_E
    }

    I2C_HandleTypeDef *i2cHandle = (I2C_HandleTypeDef *)handle;
    uint8_t rawI2CData[length * 2];
    uint8_t crcBuf[2];

    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(
        i2cHandle,
        BAT_I2C_ADDRESS << 1,
        &registerAddress,
        1,
        HAL_MAX_DELAY
    );
    ret += HAL_I2C_Master_Receive(
        i2cHandle,
        BAT_I2C_ADDRESS << 1,
        rawI2CData,
        length * 2,
        HAL_MAX_DELAY
    );

    if (ret != HAL_OK) {
        return BQ769X0_ERROR;
    }

    crcBuf[0]        = (BAT_I2C_ADDRESS << 1) | 1;
    crcBuf[1]        = rawI2CData[0];
    uint8_t crcValue = bq769x0_CRC(crcBuf, 2);
    if (crcValue != rawI2CData[1]) {
        return BQ769X0_CRC;
    }
    dataBuf[0] = rawI2CData[0];

    for (int i = 2; i < length * 2; i += 2) {
        crcValue = bq769x0_CRC(&rawI2CData[i], 1);
        if (crcValue != rawI2CData[i + 1]) {
            return BQ769X0_CRC;
        }
        dataBuf[i / 2] = rawI2CData[i];
    }

    return BQ769X0_OK;
}

/** \brief Write one or more registers over I2C
 *
 * Writes the given data to the chip via I2C, appending a CRC byte after each
 * data byte as required by the protocol.
 *
 * \param handle I2C_HandleTypeDef used for the transfer
 * \param registerAddress Address of the first register to write
 * \param dataPtr Data to write
 * \param length Number of registers to write
 * \return BQ769X0_OK on success, error code otherwise
 */
bq769x0_ErrorCode_t bsp_bq769x0_writeFunc(
    void *handle,
    uint8_t registerAddress,
    const uint8_t *dataPtr,
    uint8_t length
) {
    if ((handle == NULL) || (dataPtr == NULL)) {
        return BQ769X0_NULLPTR;
    }
    if (length < 1) {
        return BQ769X0_ERROR;
    }

    I2C_HandleTypeDef *i2cHandle = (I2C_HandleTypeDef *)handle;
    uint8_t rawI2CData[1 + length * 2];
    uint8_t crcBuf[3];

    rawI2CData[0] = registerAddress;

    crcBuf[0]     = BAT_I2C_ADDRESS << 1;
    crcBuf[1]     = registerAddress;
    crcBuf[2]     = dataBuf[0];
    rawI2CData[1] = dataBuf[0];
    rawI2CData[2] = bq769x0_CRC(crcBuf, 3);

    for (uint8_t i = 1; i < length; i++) {
        rawI2CData[1 + i * 2] = dataBuf[i];
        rawI2CData[2 + i * 2] = bq769x0_CRC(&dataBuf[i], 1);
    }

    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(
        i2cHandle,
        BAT_I2C_ADDRESS << 1,
        rawI2CData,
        1 + length * 2,
        HAL_MAX_DELAY
    );

    if (ret != HAL_OK) {
        return BQ769X0_ERROR;
    }

    return BQ769X0_OK;
}

/** \brief Wake the chip from SHIP mode via the enable pin
 *
 * Toggles the chip enable pin with the delays required by the datasheet to
 * bring the chip out of SHIP mode and ready it for the first voltage read.
 *
 * \param enPort Port of the chip enable pin
 * \param enPin Pin number of the chip enable pin
 * \return BQ769X0_OK on success, error code otherwise
 */
bq769x0_ErrorCode_t bsp_bq769x0_lowLevelInitFunc(void *enPort, uint16_t enPin) {
    GPIO_TypeDef *enPortGpio = (GPIO_TypeDef *)enPort;

    /* Wake the chip from SHIP mode*/
    HAL_GPIO_WritePin(enPortGpio, enPin, GPIO_PIN_SET);
    BQ769X0_DELAY(5); /* Datasheet specifies at least 2ms time*/
    HAL_GPIO_WritePin(enPortGpio, enPin, GPIO_PIN_RESET);
    BQ769X0_DELAY(250); /* Datasheet specifies 250ms before first voltage read,
                         * can also be placed somewhere else*/

    return BQ769X0_OK;
}