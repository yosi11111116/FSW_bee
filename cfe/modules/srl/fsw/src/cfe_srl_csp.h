#ifndef CFE_SRL_CSP_H
#define CFE_SRL_CSP_H

#include "cfe.h"

#include "cfe_srl_basic.h"

#define CSP_CAN_DEV_NAME     "can0"

#define CSP_NODE_FIX_MASK        5
typedef enum {
    CSP_NODE_EPS = 2,   // EPS Dock p31u Node
    CSP_NODE_OBC = 3,
    CSP_NODE_UTRX = 5,
    CSP_NODE_STRX = 6,
    CSP_NODE_GS_KISS = 8,
    CSP_NODE_GSTRX = 20
    
} CFE_SRL_CSP_Node_t;
#endif /* CFE_SRL_CSP_H */