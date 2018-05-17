#include <arduino.h>

#ifndef DL_PAC_NK76_h
#define DL_PAC_NK76_h

typedef struct {
  bool on;
  bool tmr_on;
  byte temperature;
  byte tmr_value;
  byte mode;
  byte fan;
} dl_ac_msg;

long dl_build_msg(dl_ac_msg* msg);
byte bit_reverse(byte num);

#endif
