#include <DL_PAC_NK76.h>

dl_ac_msg dl_msg;


void setup() {
  dl_msg.on = false;
  dl_msg.use_F = false;
  dl_msg.temperature = 25;
  dl_msg.mode=0;
  dl_msg.fan=0;
  dl_msg.tmr_on=false;
  dl_msg.tmr_value=0;

  long data = dl_build_msg(&dl_msg);
  sendNEC_DL(data, 32);
}

void loop() {

}
