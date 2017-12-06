#include <msp430.h>
#include "buzzer.h"

void buzzerState(char c){
  if(c==0){
    buzzer_set_period(0);
    return;
  }if(c==1){
    buzzer_set_period(5000);
    return;
  }if(c==2){
     buzzer_set_period(1000);
     return;
  }
}
