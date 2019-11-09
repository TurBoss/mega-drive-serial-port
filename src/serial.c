#include "serial.h"

#define VDP_MODE_REG_3 0xB
#define VDP_IE2 0x08
#define INT_MASK_LEVEL_ENABLE_ALL 1

_voidCallback* readReadyCallback;

static void extIntCallback(void)
{
    readReadyCallback();
}

static void setSCtrl(u16 value)
{
    vs8* pb;
    pb = (s8*)PORT2_SCTRL;
    *pb = value;
}

static void setCtrl(u16 value)
{
    vs8* pb;
    pb = (s8*)PORT2_CTRL;
    *pb = value;
}

u8 serial_sctrl(void)
{
    vs8* pb;
    pb = (s8*)PORT2_SCTRL;
    return *pb;
}

bool serial_readyToReceive(void)
{
    vs8* pb = (s8*)PORT2_SCTRL;
    return *pb & SCTRL_RRDY;
}

u8 serial_receive(void)
{
    vs8* pb = (s8*)PORT2_RX;
    return *pb;
}

void serial_setReadyToReceiveCallback(_voidCallback* cb)
{
    readReadyCallback = cb;
}

void serial_init(u8 sctrlFlags)
{
    setSCtrl(sctrlFlags);
    setCtrl(CTRL_PCS_OUT);
    if (sctrlFlags & SCTRL_RINT) {
        VDP_setReg(VDP_MODE_REG_3, VDP_IE2);
        SYS_setExtIntCallback(&extIntCallback);
        SYS_setInterruptMaskLevel(INT_MASK_LEVEL_ENABLE_ALL);
    }
}

void serial_send(u8 data)
{
    vu8* pb = (vu8*)PORT2_TX;
    *pb = data;
}

bool serial_readyToSend(void)
{
    vu8* pb = (vu8*)PORT2_SCTRL;
    return !((*pb & SCTRL_TFUL) == SCTRL_TFUL);
}

void serial_sendWhenReady(u8 data)
{
    while (!serial_readyToSend())
        ;
    serial_send(data);
}
