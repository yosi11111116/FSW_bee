#ifndef CFE_SRL_EXTERN_TYPEDEF_H
#define CFE_SRL_EXTERN_TYPEDEF_H

/**
 * @file
 *
 * Declarations and prototypes for cfe_srl_extern_typedefs module
 */

#include "common_types.h"
#include "cfe_mission_cfg.h"

/* Foward declaration of struct */
typedef struct CFE_SRL_IO_Handle_s    CFE_SRL_IO_Handle_t;
typedef struct CFE_SRL_IO_Param_s     CFE_SRL_IO_Param_t;

typedef int32 (*CFE_SRL_Write_Function_t)(CFE_SRL_IO_Handle_t *Handle, CFE_SRL_IO_Param_t *Param);
typedef int32 (*CFE_SRL_Read_Function_t)(CFE_SRL_IO_Handle_t *Handle, CFE_SRL_IO_Param_t *Param);

typedef struct {
    CFE_SRL_Write_Function_t TxFunc;
    CFE_SRL_Read_Function_t RxFunc;
} CFE_SRL_Function_t;


struct CFE_SRL_IO_Handle_s {
    int FD;
    int __errno;
    uint32_t TxCount;
    uint32_t RxCount;
    uint32_t TxErrCnt;
    uint32_t RxErrCnt;
    CFE_SRL_Function_t Func;
};

struct CFE_SRL_IO_Param_s {
    void *TxData;
    size_t TxSize;
    void *RxData;
    size_t RxSize;

    /**
     * Used for ApiRead
     */
    uint32_t Timeout;
    /**
     * Used for I2C, CAN
     * I2C : Slave Addr
     * CAN : CAN frame ID
     */
    uint32_t Addr;
};



typedef struct {
    struct gpiod_chip *Chip;
    struct gpiod_line *Line;
} CFE_SRL_GPIO_Handle_t;

#endif /* CFE_SRL_EXTERN_TYPEDEF_H */