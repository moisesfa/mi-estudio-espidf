
#ifndef sub_rotenc_h_
#define sub_rotenc_h_

#include "freertos/task.h"

extern "C"
{
    void rotenc_init(int clk, int dt);

    typedef void (*posChangedCb_t)(void *);
    void rotenc_setPosChangedCallback(posChangedCb_t);
    int rotenc_getPos(void);
}

#endif