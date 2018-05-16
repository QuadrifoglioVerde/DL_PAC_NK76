#include "DL_PAC_NK76.h"

long dl_build_msg(dl_ac_msg* msg){
  long buf = 0xDA000103;

  msg->temperature = constrain(msg->temperature, 18, 30);       // range 18-30C
  buf |= (long)bit_reverse(msg->temperature-12) << 18;

  buf |= (long)msg->on << 15;                                   // ON-OFF
  
  buf |= (long)msg->mode << 13;                                 // set MODE

  if (msg->mode == 0 ){                                         // if AC allow all FAN modes
    buf |= (long)msg->fan << 16;
  } else if (msg->mode == 1 ){                                  // if BLOW only LO-MI-HI
    if (msg->fan > 0  ){ buf |= (long)msg->fan << 16; }
    if (msg->fan == 0 ){ buf |= 1L << 16; }
  }                                                             // in DEH mode set FAN AUTO

  return buf;
}

byte bit_reverse(byte num) {
  byte var = 0;     
  int i, x, y, p;
  int s = 6;    // number of bits in 'num'.
 
  for (i = 0; i < (s / 2); i++) {
    // extract bit on the left, from MSB
    p = s - i - 1;
    x = num & (1 << p);
    x = x >> p;
    // extract bit on the right, from LSB
    y = num & (1 << i);
    y = y >> i;
 
    var = var | (x << i);       // apply x
    var = var | (y << p);       // apply y
  }
  return var;
}
