//
// Created by justus on 9/22/26.
//

#ifndef BQ769X0_BQ769X0_H
#define BQ769X0_BQ769X0_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/** \brief Driver error codes
 *
 * Used to communicate the result of various actions. Will always be 0 on success to be interoperable with external code.
 */
typedef enum {
    BQ769X0_OK = 0, /**< No error*/
    BQ769X0_ERROR = 1,/**< Generic error*/
    BQ769X0_NULLPTR = 2/**< Function received a null pointer */
} bq769x0_ErrorCode_t;

typedef bq769x0_ErrorCode_t (*bq769x0_readPtr)(void *handle, uint8_t registerAddress, uint8_t *dataPtr, uint8_t length);
typedef bq769x0_ErrorCode_t (*bq769x0_writePtr)(void *handle, uint8_t registerAddress, const uint8_t *dataPtr, uint8_t length);
typedef bq769x0_ErrorCode_t (*bq769x0_lowLevelInitPtr)(void *enPort, uint16_t enPin);

typedef struct {
    bq769x0_writePtr writePtr;
    bq769x0_readPtr readPtr;
    bq769x0_lowLevelInitPtr lowLevelInit;

    void *handle;
    void *enPort;
    uint16_t enPin;

    bool ready;
} bq769x0_Ctx_t;

bq769x0_ErrorCode_t bq769x0_readReg(const bq769x0_Ctx_t *ctx, uint8_t registerAddress, uint8_t *dataPtr, uint8_t length);
bq769x0_ErrorCode_t bq769x0_writeReg(const bq769x0_Ctx_t *ctx, uint8_t registerAddress, const uint8_t *dataPtr, uint8_t length);
bq769x0_ErrorCode_t bq769x0_lowLevelInit(const bq769x0_Ctx_t *ctx, void *enPort, uint16_t enPin);


#endif //BQ769X0_BQ769X0_H
