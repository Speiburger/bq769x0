//
// Created by justus on 9/22/26.
//

#ifndef BQ769X0_BQ769X0_H
#define BQ769X0_BQ769X0_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** \brief Driver error codes
 *
 * Used to communicate the result of various actions. Will always be 0 on
 * success to be interoperable with external code.
 */
typedef enum {
    BQ769X0_OK        = 0, /**< No error*/
    BQ769X0_ERROR     = 1, /**< Generic error*/
    BQ769X0_NULLPTR   = 2, /**< Function received a null pointer*/
    BQ769X0_CRC       = 3, /**< Crc check failed*/
    BQ769X0_NOT_READY = 4  /**< Context has not been initialized*/
} bq769x0_ErrorCode_t;

/** \brief Undervoltage protection delay settings
 *
 * Selects the delay before the undervoltage protection trips.
 */
typedef enum {
    BQ769X0_UV_DELAY_1S  = 0x0, /**< 1 second delay*/
    BQ769X0_UV_DELAY_4S  = 0x1, /**< 4 second delay*/
    BQ769X0_UV_DELAY_8S  = 0x2, /**< 8 second delay*/
    BQ769X0_UV_DELAY_16S = 0x3  /**< 16 second delay*/
} bq769x0_UvDelay_t;

/** \brief Overvoltage protection delay settings
 *
 * Selects the delay before the overvoltage protection trips.
 */
typedef enum {
    BQ769X0_OV_DELAY_1S = 0x0, /**< 1 second delay*/
    BQ769X0_OV_DELAY_2S = 0x1, /**< 2 second delay*/
    BQ769X0_OV_DELAY_4S = 0x2, /**< 4 second delay*/
    BQ769X0_OV_DELAY_8S = 0x3  /**< 8 second delay*/
} bq769x0_OvDelay_t;

/** \name Safety check macros
 *
 * Helper macros used throughout the driver to perform common safety checks
 * @{
 */

/** \brief Return BQ769X0_NULLPTR from the calling function if ptr is NULL*/
#define BQ769X0_CHECK_NULLPTR(ptr) \
    if (ptr == NULL) {             \
        return BQ769X0_NULLPTR;    \
    }

/** \brief Return ret from the calling function if it does not equal
 * BQ769X0_OK*/
#define BQ769X0_CHECK_RETVAL(ret) \
    if (ret != BQ769X0_OK) {      \
        return ret;               \
    }

/** \brief Return BQ769X0_NOT_READY from the calling function if ctx has not
 * been initialized*/
#define BQ769X0_CHECK_READY(ctx)  \
    if (!ctx->ready) {            \
        return BQ769X0_NOT_READY; \
    }

/** @}*/

/** \name Register map
 *
 * Addresses of the chip registers used by the driver.
 * @{
 */
#define BQ769X0_SYS_STAT_REG 0x00 /**< System status register*/
#define BQ769X0_PROTECT3_REG \
    0x08 /**< Protection configuration register 3 (OV/UV delay)*/
#define BQ769X0_OV_TRIP_REG 0x09    /**< Overvoltage trip threshold register*/
#define BQ769X0_UV_TRIP_REG 0x0A    /**< Undervoltage trip threshold register*/
#define BQ769X0_ADC_GAIN1_REG 0x50  /**< ADC gain trim register (part 1)*/
#define BQ769X0_ADC_OFFSET_REG 0x51 /**< ADC offset trim register*/
#define BQ769X0_ADC_GAIN2_REG 0x59  /**< ADC gain trim register (part 2)*/
/** @}*/

/** \name SYS_STAT register bits
 *
 * Bit positions within the SYS_STAT register.
 * @{
 */
#define BQ769X0_SYS_STAT_OCD_BIT 0x0 /**< Overcurrent in discharge fault*/
#define BQ769X0_SYS_STAT_SCD_BIT 0x1 /**< Short circuit in discharge fault*/
#define BQ769X0_SYS_STAT_OV_BIT 0x2  /**< Overvoltage fault*/
#define BQ769X0_SYS_STAT_UV_BIT 0x3  /**< Undervoltage fault*/
#define BQ769X0_SYS_STAT_OVRD_ALERT_BIT 0x4 /**< Override alert condition*/
#define BQ769X0_SYS_STAT_DEVICE_XREADY_BIT \
    0x5 /**< Device XREADY fault (internal IC error)*/
#define BQ769X0_SYS_STAT_CC_READY_BIT \
    0x7 /**< Coulomb counter conversion ready*/
/** @}*/

typedef bq769x0_ErrorCode_t (*bq769x0_readPtr)(
    void *handle,
    uint8_t registerAddress,
    uint8_t *dataPtr,
    uint8_t length
);

typedef bq769x0_ErrorCode_t (*bq769x0_writePtr)(
    void *handle,
    uint8_t registerAddress,
    const uint8_t *dataPtr,
    uint8_t length
);

typedef bq769x0_ErrorCode_t (*bq769x0_lowLevelInitPtr)(
    void *enPort,
    uint16_t enPin
);

typedef struct {
    bq769x0_writePtr writePtr;
    bq769x0_readPtr readPtr;
    bq769x0_lowLevelInitPtr lowLevelInit;

    void *handle;
    void *enPort;
    uint16_t enPin;

    uint16_t adcGain;
    int8_t adcOffset;

    bool ready;
} bq769x0_Ctx_t;

bq769x0_ErrorCode_t bq769x0_readReg(
    const bq769x0_Ctx_t *ctx,
    uint8_t registerAddress,
    uint8_t *dataPtr,
    uint8_t length
);

bq769x0_ErrorCode_t bq769x0_writeReg(
    const bq769x0_Ctx_t *ctx,
    uint8_t registerAddress,
    const uint8_t *dataPtr,
    uint8_t length
);

bq769x0_ErrorCode_t
bq769x0_lowLevelInit(const bq769x0_Ctx_t *ctx, void *enPort, uint16_t enPin);

bq769x0_ErrorCode_t bq769x0_init(
    bq769x0_Ctx_t *ctx,
    bq769x0_writePtr writePtr,
    bq769x0_readPtr readPtr,
    bq769x0_lowLevelInitPtr lowLevelInit,
    void *handle,
    void *enPort,
    uint16_t enPin
);

bq769x0_ErrorCode_t bq769x0_handleAlert(const bq769x0_Ctx_t *ctx);

#endif // BQ769X0_BQ769X0_H