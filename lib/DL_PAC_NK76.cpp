#include "DL_PAC_NK76.h"

//
//  bit
//  2-6   timer value
//  7     timer ON
//  9     C/F
// 13-14  MODE: 0-AC; 1-FAN; 2-DEH
// 15     ON
// 16-17  FAN: 0-Auto; 1-Low; 2-Med; 3-High
// 18-23  Temperature value
//

long dl_build_msg(dl_ac_msg* msg) {
  long buf = 0xDA000103;

  if (msg->tmr_on) {
    buf |= 1L << 7;                                            // set Timer ON bit
    msg->tmr_value = constrain(msg->tmr_value, 1, 24);         // timer range 1-24h
    buf |= (long)bit_reverse(msg->tmr_value, 5) << 2;          // reverse 5bits
  }

  if (msg->use_F) {
    buf |= 1L << 9;
    msg->temperature = constrain(msg->temperature, 64, 90);      // temperature range 64-90F
    buf |= (long)bit_reverse(msg->temperature-54, 6) << 18;      // reverse 6bits, subtract 54 for F
  } else {
    msg->temperature = constrain(msg->temperature, 18, 30);      // temperature range 18-30C
    buf |= (long)bit_reverse(msg->temperature-12, 6) << 18;      // reverse 6bits, subtract 12 for C
  }

  buf |= (long)msg->on << 15;                                  // ON-OFF

  buf |= (long)msg->mode << 13;                                // set MODE

  if (msg->mode == 0) {                                        // if AC allow all FAN modes
    buf |= (long)bit_reverse(msg->fan, 2) << 16;               // reversed for proper order
  } else if (msg->mode == 1) {                                 // if BLOW only LO-MI-HI
    msg->fan = constrain(msg->fan, 1, 3);                       //
    buf |= (long)bit_reverse(msg->fan, 2) << 16;               //
  }                                                            // in DEH mode set FAN AUTO

  return buf;
}

byte bit_reverse(byte data, byte n) {
  byte var = 0;
  byte i, x, y, p;

  for (i = 0; i <= (n / 2); i++) {
    // extract bit on the left, from MSB
    p = n - i - 1;
    x = data & (1 << p);
    x = x >> p;
    // extract bit on the right, from LSB
    y = data & (1 << i);
    y = y >> i;

    var = var | (x << i);  // apply x
    var = var | (y << p);  // apply y
  }
  return var;
}
