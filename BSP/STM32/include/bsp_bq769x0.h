//
// Created by justus on 9/23/26.
//

/* The BSP files are not part of the core project but instead provide a starting
 * point for developing the hardware specific code the driver needs to operate
 */
1

#ifndef BQ769X0_BSP_BQ769X0_C_H
#define BQ769X0_BSP_BQ769X0_C_H

#include "bq769x0.h"
#include <stdint.h>
#include "main.h"

#ifdef FREERTOS
#define BQ769X0_DELAY(delay) vTaskDelay(delay);
#else
#define BQ769X0_DELAY(delay) HAL_Delay(delay);
#endif

#define BAT_I2C_ADDRESS 0x08

    bq769x0_ErrorCode_t bsp_bq769x0_readFunc(
    void *handle,
    uint8_t registerAddress,
    uint8_t *dataPtr,
    uint8_t length
);

bq769x0_ErrorCode_t bsp_bq769x0_writeFunc(
    void *handle,
    uint8_t registerAddress,
    const uint8_t *dataPtr,
    uint8_t length
);

bq769x0_ErrorCode_t bsp_bq769x0_lowLevelInitFunc(void *enPort, uint16_t enPin);

#endif // BQ769X0_BSP_BQ769X0_C_H
