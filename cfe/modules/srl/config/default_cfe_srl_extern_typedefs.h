#ifndef CFE_SRL_EXTERN_TYPEDEF_H
#define CFE_SRL_EXTERN_TYPEDEF_H

/**
 * @file
 *
 * Declarations and prototypes for cfe_srl_extern_typedefs module
 */

#include "common_types.h"
#include "cfe_mission_cfg.h"

typedef struct {
    int FD;
    int __errno;
    uint32_t TxCount;
    uint32_t RxCount;
    uint32_t TxErrCnt;
    uint32_t RxErrCnt;
} CFE_SRL_IO_Handle_t;

typedef struct {
    struct gpiod_chip *Chip;
    struct gpiod_line *Line;
} CFE_SRL_GPIO_Handle_t;

#endif /* CFE_SRL_EXTERN_TYPEDEF_H */