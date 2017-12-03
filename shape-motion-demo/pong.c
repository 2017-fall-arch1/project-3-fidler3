#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "shapemotion.h"
#include "switches.h"

AbRect rect10 = {abRectGetBounds, abRectCheck, {2,10}}; /**< 10x10 rectangle */
AbRArrow rightArrow = {abRArrowGetBounds, abRArrowCheck, 30};

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2 - 10, screenHeight/2 - 10}
};

Layer fieldLayer = {		/**< Layer with an orange circle */
  (AbShape *)&fieldOutline,
  {(screenWidth/2), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  0,
};


Layer layer2 = {		/* playing field as a layer */
  (AbShape *)&rect10,
  {(screenWidth/2)+52, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &fieldLayer,
};

Layer layer1 = {		/**< Layer with a red square */
  (AbShape *)&rect10,
  {(screenWidth/2)-52, screenHeight/2}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &layer2,
};

Layer layer0 = {		/**< Layer with an orange circle */
  (AbShape *)&circle2,
  {(screenWidth/2), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &layer1,
};

MovLayer ml2 = { &layer2, {1,1}, 0 }; //paddle2
MovLayer ml1 = { &layer1, {1,2}, 0 }; //paddle1
MovLayer ml0 = { &layer0, {2,1}, 0 }; //ball 

u_int bgColor = COLOR_BLUE;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */
Region paddle1;
Region paddle2;
Region ball;
Region *fence = &fieldFence;
Region *p1 = &paddle1;
Region *p2 = &paddle2;
char button = 5;

void stateMachine(){
  u_int switches = p2sw_read();
  layerGetBounds(&fieldLayer, fence);
  layerGetBounds(&layer1, p1);
  layerGetBounds(&layer1, p2);

  if(!(switches & (1<<0))){
    if(p1->topLeft.axes[1] == fence->topLeft.axes[1])
      return;   
    MovLayer *a = &ml1;
    a->layer->posNext.axes[1]-=5;
    movLayerDraw(a, &layer1);
    return;
  }
  if(!(switches & (1<<1))){
    if(p1->botRight.axes[1] == fence->botRight.axes[1])
      return;
    MovLayer *a = &ml1;
    a->layer->posNext.axes[1]+=5;
    movLayerDraw(a, &layer1);
    return;
  }
  if(!(switches & (1<<2))){
    if(p2->topLeft.axes[1] > fence->topLeft.axes[1]){
      MovLayer *a = &ml2;
      a->layer->posNext.axes[1]-=5;
      movLayerDraw(a, &layer2);
    }
    return;
  }
  if(!(switches & (1<<3))){
    if(p2->botRight.axes[1] < fence->botRight.axes[1]){
      MovLayer *a = &ml2;
      a->layer->posNext.axes[1]+=5;
      movLayerDraw(a, &layer2);
    }
    return;
  }
}

/* initial value of {0,0} will be overwritten */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  
  p2sw_init(15);
  
  layerInit(&layer0);
  layerDraw(&layer0);
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);  /**< GIE (enable interrupts) */
 

  char score[3] = {'0','0', '\0'};
  
  drawString5x7(50, 152, score, COLOR_BLACK, COLOR_BLUE);
  
		 
  for(;;) {
    while(0){
      P1OUT &= ~GREEN_LED;
      or_sr(0x10);
    }
    P1OUT |= GREEN_LED; /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
    layerGetBounds(&fieldLayer, &fieldFence);
    layerGetBounds(&layer1, &paddle1);
    layerGetBounds(&layer1, &paddle2);
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  char k = 0;
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  if (count == 15) {
    mlAdvance(&ml0, &fieldFence);
    bounceLeft(&ml0, &paddle1);
    bounceRight(&ml0, &paddle2);
    if(p2sw_read()){
      stateMachine();
    }
    count = 0;
    button = 5;
  } 
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
