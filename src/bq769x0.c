//
// Created by justus on 9/22/26.
//
#include "bq769x0.h"

/* Private helper functions*/

/** \brief Read ADC calibration parameters from the chip
 *
 * Reads the ADC gain and offset trim values and stores them in the context
 * for use in voltage conversions.
 *
 * \param ctx Driver context
 * \return BQ769X0_OK on success, error code otherwise
 */
static bq769x0_ErrorCode_t bq769x0_readAdcParams(bq769x0_Ctx_t *ctx) {
    BQ769X0_CHECK_NULLPTR(ctx);
    uint8_t adcGain1;
    uint8_t adcGain2;
    uint8_t adcOffset;

    bq769x0_ErrorCode_t ret =
        bq769x0_readReg(ctx, BQ769X0_ADC_GAIN1_REG, &adcGain1, 1);
    BQ769X0_CHECK_RETVAL(ret);
    ret = bq769x0_readReg(ctx, BQ769X0_ADC_GAIN2_REG, &adcGain2, 1);
    BQ769X0_CHECK_RETVAL(ret);
    ret = bq769x0_readReg(ctx, BQ769X0_ADC_OFFSET_REG, &adcOffset, 1);
    BQ769X0_CHECK_RETVAL(ret);

    ctx->adcGain = 365;
    ctx->adcGain += ((adcGain1 & 0xc) << 1);
    ctx->adcGain += ((adcGain2 & 0x60) >> 5);
    ctx->adcOffset = (int8_t)adcOffset;

    return BQ769X0_OK;
}

/** \brief Set the overvoltage protection trip threshold
 *
 * Converts the given voltage to the raw register value and writes it to the
 * OV_TRIP register.
 *
 * \param ctx Driver context
 * \param voltageMV Trip threshold in millivolts
 * \return BQ769X0_OK on success, error code otherwise
 */
static bq769x0_ErrorCode_t
bq769x0_setOvTrip(const bq769x0_Ctx_t *ctx, const uint32_t voltageMV) {
    /* Null pointer check*/
    BQ769X0_CHECK_NULLPTR(ctx);

    /* Calculate the value to be written to the register from the raw millivolt
     * value*/
    const uint16_t ovTripFull =
        (((long)voltageMV - ctx->adcOffset) * 1000 / ctx->adcGain);

    /* Check if calculated value fits into the register on the chip*/
    if (ovTripFull > 0x3FF8) {
        return BQ769X0_ERROR;
    }
    if (ovTripFull < 0x2008) {
        return BQ769X0_ERROR;
    }

    /* Write the value to the register*/
    const uint8_t rawValue = (ovTripFull >> 4) & 0xFF;
    const bq769x0_ErrorCode_t ret =
        bq769x0_writeReg(ctx, BQ769X0_OV_TRIP_REG, &rawValue, 1);
    BQ769X0_CHECK_RETVAL(ret);

    return BQ769X0_OK;
}

/** \brief Set the undervoltage protection trip threshold
 *
 * Converts the given voltage to the raw register value and writes it to the
 * UV_TRIP register.
 *
 * \param ctx Driver context
 * \param voltageMV Trip threshold in millivolts
 * \return BQ769X0_OK on success, error code otherwise
 */
static bq769x0_ErrorCode_t
bq769x0_setUvTrip(const bq769x0_Ctx_t *ctx, const uint32_t voltageMV) {
    /* Null pointer check*/
    BQ769X0_CHECK_NULLPTR(ctx);

    /* Calculate the value to be written to the register from the raw millivolt
     * value*/
    const uint16_t uvTripFull =
        (((long)voltageMV - ctx->adcOffset) * 1000 / ctx->adcGain);

    /* Check if calculated value fits into the register on the chip*/
    if (uvTripFull > 0x1FF0) {
        return BQ769X0_ERROR;
    }
    if (uvTripFull < 0x1000) {
        return BQ769X0_ERROR;
    }

    /* Write the value to the register*/
    const uint8_t rawValue = (uvTripFull >> 4) & 0xFF;
    const bq769x0_ErrorCode_t ret =
        bq769x0_writeReg(ctx, BQ769X0_UV_TRIP_REG, &rawValue, 1);
    BQ769X0_CHECK_RETVAL(ret);

    return BQ769X0_OK;
}

// TODO: Set UV / OV delay
// TODO: Set OCD / SCD threshold
// TODO: Set OCD / SCD delay
// TODO: CC settings
// TODO: Get SYS_STAT
// TODO: Set DSG_ON
// TODO: Set CHG_ON
// TODO: Shutdown
// TODO: Temperature stuff

/* Public functions*/

/** \brief Read one or more registers from the chip
 *
 * Weak function calling the read callback configured in the context. Can be
 * overridden with a hardware-specific implementation.
 *
 * \param ctx Driver context
 * \param registerAddress Address of the first register to read
 * \param dataPtr Buffer to store the read data in
 * \param length Number of registers to read
 * \return BQ769X0_OK on success, error code otherwise
 */
__attribute__((weak)) bq769x0_ErrorCode_t bq769x0_readReg(
    const bq769x0_Ctx_t *ctx,
    uint8_t registerAddress,
    uint8_t *dataPtr,
    uint8_t length
) {
    BQ769X0_CHECK_NULLPTR(ctx);
    return ctx->readPtr(ctx->handle, registerAddress, dataPtr, length);
}

/** \brief Write one or more registers on the chip
 *
 * Weak function calling the write callback configured in the context. Can be
 * overridden with a hardware-specific implementation.
 *
 * \param ctx Driver context
 * \param registerAddress Address of the first register to write
 * \param dataPtr Data to write
 * \param length Number of registers to write
 * \return BQ769X0_OK on success, error code otherwise
 */
__attribute__((weak)) bq769x0_ErrorCode_t bq769x0_writeReg(
    const bq769x0_Ctx_t *ctx,
    uint8_t registerAddress,
    const uint8_t *dataPtr,
    uint8_t length
) {
    BQ769X0_CHECK_NULLPTR(ctx);
    return ctx->writePtr(ctx->handle, registerAddress, dataPtr, length);
}

/** \brief Perform hardware-specific low level initialization
 *
 * Weak function calling the low level init callback configured in the
 * context. Can be overridden with a hardware-specific implementation.
 *
 * \param ctx Driver context
 * \param enPort Port of the chip enable pin
 * \param enPin Pin number of the chip enable pin
 * \return BQ769X0_OK on success, error code otherwise
 */
__attribute__((weak)) bq769x0_ErrorCode_t
bq769x0_lowLevelInit(const bq769x0_Ctx_t *ctx, void *enPort, uint16_t enPin) {
    BQ769X0_CHECK_NULLPTR(ctx);
    return ctx->lowLevelInit(enPort, enPin);
}

/** \brief Initialize the driver context
 *
 * Stores the given callbacks and handles in the context, performs the low
 * level initialization and reads the ADC calibration parameters from the
 * chip.
 *
 * \param ctx Driver context
 * \param writePtr Callback used to write registers
 * \param readPtr Callback used to read registers
 * \param lowLevelInit Callback used for hardware-specific initialization
 * \param handle Handle passed to the read/write callbacks
 * \param enPort Port of the chip enable pin
 * \param enPin Pin number of the chip enable pin
 * \return BQ769X0_OK on success, error code otherwise
 */
bq769x0_ErrorCode_t bq769x0_init(
    bq769x0_Ctx_t *ctx,
    bq769x0_writePtr writePtr,
    bq769x0_readPtr readPtr,
    bq769x0_lowLevelInitPtr lowLevelInit,
    void *handle,
    void *enPort,
    uint16_t enPin
) {
    /* Null pointer checks*/
    BQ769X0_CHECK_NULLPTR(ctx);
    BQ769X0_CHECK_NULLPTR(writePtr);
    BQ769X0_CHECK_NULLPTR(readPtr);
    BQ769X0_CHECK_NULLPTR(lowLevelInit);
    BQ769X0_CHECK_NULLPTR(handle);
    BQ769X0_CHECK_NULLPTR(enPort);

    ctx->writePtr     = writePtr;
    ctx->readPtr      = readPtr;
    ctx->lowLevelInit = lowLevelInit;
    ctx->handle       = handle;
    ctx->enPort       = enPort;
    ctx->enPin        = enPin;

    bq769x0_ErrorCode_t ret = ctx->lowLevelInit(ctx->enPort, ctx->enPin);
    BQ769X0_CHECK_RETVAL(ret);

    ret = bq769x0_readAdcParams(ctx);
    BQ769X0_CHECK_RETVAL(ret);

    ctx->ready = true;
    return BQ769X0_OK;
}
