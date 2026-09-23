//
// Created by justus on 9/22/26.
//
#include "bq769x0.h"

/* Private helper functions*/

/* Public functions*/

// TODO: Doxygen comment
__attribute__((weak)) bq769x0_ErrorCode_t bq769x0_readReg(
    const bq769x0_Ctx_t *ctx,
    uint8_t registerAddress,
    uint8_t *dataPtr,
    uint8_t length
) {
    if (ctx == NULL) {
        return BQ769X0_NULLPTR;
    }
    return ctx->readPtr(ctx->handle, registerAddress, dataPtr, length);
}

// TODO: Doxygen comment
__attribute__((weak)) bq769x0_ErrorCode_t bq769x0_writeReg(
    const bq769x0_Ctx_t *ctx,
    uint8_t registerAddress,
    const uint8_t *dataPtr,
    uint8_t length
) {
    if (ctx == NULL) {
        return BQ769X0_NULLPTR;
    }
    return ctx->writePtr(ctx->handle, registerAddress, dataPtr, length);
}

// TODO: Doxygen comment
__attribute__((weak)) bq769x0_ErrorCode_t
bq769x0_lowLevelInit(const bq769x0_Ctx_t *ctx, void *enPort, uint16_t enPin) {
    if (ctx == NULL) {
        return BQ769X0_NULLPTR;
    }
    return ctx->lowLevelInit(enPort, enPin);
}

// TODO: Doxygen comment
bq769x0_ErrorCode_t bq769x0_init(
    bq769x0_Ctx_t *ctx,
    bq769x0_writePtr writePtr,
    bq769x0_readPtr readPtr,
    bq769x0_lowLevelInitPtr lowLevelInit,
    void *handle,
    void *enPort,
    uint16_t enPin
) {
    return BQ769X0_OK;
}