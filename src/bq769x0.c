//
// Created by justus on 9/22/26.
//
#include "bq769x0.h"

// Private helper functions

// TODO: Doxygen comment
static uint8_t bq769x0_CRC(const uint8_t *data, const size_t len) {
    uint8_t crc = 0;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80U) {
                crc = (uint8_t)((crc << 1) ^ 0x07U); // CRC polynomial of x^8 + x^2 + x + 1
            }
            else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}


// Public functions

// TODO: Doxygen comment
__attribute__((weak)) bq769x0_ErrorCode_t bq769x0_readReg(const bq769x0_Ctx_t *ctx, uint8_t registerAddress, uint8_t *dataPtr, uint8_t length) {
    if (ctx == NULL) {
        return BQ769X0_NULLPTR;
    }
    return ctx->readPtr(ctx->handle, registerAddress, dataPtr, length);
}

// TODO: Doxygen comment
__attribute__((weak)) bq769x0_ErrorCode_t bq769x0_writeReg(const bq769x0_Ctx_t *ctx, uint8_t registerAddress, const uint8_t *dataPtr, uint8_t length) {
    if (ctx == NULL) {
        return BQ769X0_NULLPTR;
    }
    return ctx->writePtr(ctx->handle, registerAddress, dataPtr, length);
}

// TODO: Doxygen comment
__attribute__((weak)) bq769x0_ErrorCode_t bq769x0_lowLevelInit(const bq769x0_Ctx_t *ctx, void *enPort, uint16_t enPin) {
    if (ctx == NULL) {
        return BQ769X0_NULLPTR;
    }
    return ctx->lowLevelInit(enPort, enPin);
}