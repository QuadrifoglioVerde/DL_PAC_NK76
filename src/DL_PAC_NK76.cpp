#include "DL_PAC_NK76.h"
#include <IRremote.h>

//
//  bit
//  2-6   timer value (reversed 5bit)
//  7     timer ON
//  9     C/F
// 13-14  MODE: 0-AC; 1-FAN; 2-DEH
// 15     ON
// 16-17  FAN: 0-Auto; 1-Low; 2-Med; 3-High (reversed 2bit)
// 18-23  Temperature value (reversed 6bit)
//

#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     500
#define NEC_ONE_SPACE   1660
#define NEC_ZERO_SPACE   600

// Original function from IRremote lib, added DL stuff
void sendNEC_DL(unsigned long data,  int nbits)
{
  IRsend irsend;
  // Set IR carrier frequency
  irsend.enableIROut(38);

  // Header
  irsend.mark(NEC_HDR_MARK);
  irsend.space(NEC_HDR_SPACE);

  // Data
  for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
    if (data & mask) {
      irsend.mark(NEC_BIT_MARK);
      irsend.space(NEC_ONE_SPACE);
    } else {
      irsend.mark(NEC_BIT_MARK);
      irsend.space(NEC_ZERO_SPACE);
    }
  }

  // DL stuff
  for (int i = 0; i < 6; i++) {
    irsend.mark(NEC_BIT_MARK);
    irsend.space(NEC_ZERO_SPACE);
  }

  for (int i = 0; i < 2; i++) {
    irsend.mark(NEC_BIT_MARK);
    irsend.space(NEC_ONE_SPACE);
  }

  // Footer
  irsend.mark(NEC_BIT_MARK);
  irsend.space(0);  // Always end with the LED off
}

long dl_build_msg(dl_ac_msg* msg) {
  long buf = 0xDA000103;

  if (msg->tmr_on) {                                           // use timer?
    buf |= 1L << 7;                                            // set Timer ON bit
    msg->tmr_value = constrain(msg->tmr_value, 1, 24);         // timer range 1-24h
    buf |= (long)bit_reverse(msg->tmr_value, 5) << 2;          // reverse 5bits
  }

  if (msg->mode == 0) {                                         // only in AC we need set temperature
    if (msg->use_F) {                                           // use F
      buf |= 1L << 9;                                           // set bit 9 to true
      msg->temperature = constrain(msg->temperature, 64, 90);   // temperature range 64-90F
      buf |= (long)bit_reverse(msg->temperature-54, 6) << 18;   // reverse 6bits, subtract 54 for F
    } else {                                                    // use C
      msg->temperature = constrain(msg->temperature, 18, 30);   // temperature range 18-30C
      buf |= (long)bit_reverse(msg->temperature-12, 6) << 18;   // reverse 6bits, subtract 12 for C
    }
  } else {                                                      // in DEH/FAN is temperature fixed to 23C
    buf |= (long)bit_reverse(23-12, 6) << 18;                   //
  }

  buf |= (long)msg->on << 15;                                   // ON-OFF

  buf |= (long)msg->mode << 13;                                 // set MODE

  if (msg->mode == 0) {                                         // if AC allow all FAN modes
    buf |= (long)bit_reverse(msg->fan, 2) << 16;                // reversed for proper order
  } else if (msg->mode == 1) {                                  // if BLOW only LO-MI-HI
    msg->fan = constrain(msg->fan, 1, 3);                       //
    buf |= (long)bit_reverse(msg->fan, 2) << 16;                //
  }                                                             // in DEH mode keep FAN AUTO

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
