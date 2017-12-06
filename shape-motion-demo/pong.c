#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shapemotion.h"
#include "buzzer.h"
#include "stateMachine.h"

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
int score = 00;
char sc[12];
char num = 0;
int highscore = 0;
char hs[12];

//statemachine for buttons
void stateMachine(){
  u_int switches = p2sw_read();
  layerGetBounds(&layer1, &paddle1);
  layerGetBounds(&layer2, &paddle2);

  if(!(switches & (1))){ //s1
    if(p1->topLeft.axes[1] == fence->topLeft.axes[1])
      return;   
    MovLayer *a = &ml1;
    a->layer->posNext.axes[1]-=10;
    return;
  }
  if(!(switches & (2))){ //s2
    if(p1->botRight.axes[1] == fence->botRight.axes[1])
      return;
    MovLayer *a = &ml1;
    a->layer->posNext.axes[1]+=10;
    return;
  }
  if(!(switches & (4))){ //s3
    if(p2->topLeft.axes[1] == fence->topLeft.axes[1])
      return;
    MovLayer *a = &ml2;
    a->layer->posNext.axes[1]-=10;
    return;
  }
  
  if(!(switches & (8))){ //s4
    if(p2->botRight.axes[1] == fence->botRight.axes[1])
      return;
    MovLayer *a = &ml2;
    a->layer->posNext.axes[1]+=10;
    //movLayerDraw(a, &layer2);
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
  buzzer_init1();
  buzzerState(2);
  itoa(score, sc, 10);
  layerGetBounds(&fieldLayer, &fieldFence);
  drawString5x7(0, 152, "Score:", COLOR_BLACK, COLOR_BLUE);
  itoa(highscore, hs, 10);
  drawString5x7(75, 152, hs, COLOR_RED, COLOR_BLACK);
  
  for(;;) {
    while(0){
      P1OUT &= ~GREEN_LED;
      or_sr(0x10);
    }
    P1OUT |= GREEN_LED; /**< Green led on when CPU on */
    redrawScreen = 0;
    drawString5x7(50, 152, sc, COLOR_BLACK, COLOR_BLUE);
    drawString5x7(75, 152, hs, COLOR_RED, COLOR_BLACK);
    movLayerDraw(&ml0, &layer0);
    movLayerDraw(&ml1, &layer1);
    movLayerDraw(&ml2, &layer2);

  }
}

void add1(){ //add1 to score and redraw
  score++;
  itoa(score, sc, 10);  
}

void startOver(){
  if(score > highscore){
    highscore = score;
    itoa(highscore, hs, 10);
  }
  score = 0;
  itoa(score, sc, 10);
  
}
/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  char k = 0;
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  if (count == 15) {
    buzzerState(0);
    if(mlAdvance(&ml0, &fieldFence)){
      buzzerState(2);
      startOver();
    }
    if((bounceLeft(&ml0, &paddle1)) ||
       (bounceRight(&ml0, &paddle2))){
      add1(); //adjust score
      buzzerState(1); //beep when you get a point
    }
    if(p2sw_read()){
      stateMachine();
    }
    count = 0;
    button = 5;
  }
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
