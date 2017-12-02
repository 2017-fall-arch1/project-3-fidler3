/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#define GREEN_LED BIT6


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

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */
MovLayer ml2 = { &layer2, {1,1}, 0 }; //paddle2
MovLayer ml1 = { &layer1, {1,2}, 0 }; //paddle1
MovLayer ml0 = { &layer0, {2,1}, 0 }; //ball 

void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */

  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  



//Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
int mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  int used = 0;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] <= fence->topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] >= fence->botRight.axes[axis]) ) {
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity);
	if((shapeBoundary.topLeft.axes[0] == fence->topLeft.axes[0]) ||
	   (shapeBoundary.botRight.axes[0] == fence->botRight.axes[0])){
	  used = 1;
	}
      }	/**< if outside of fence */
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
  return used;
}

int bounceLeft(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  int used = 0; //true if a bounce occured
  int tlx = fence->topLeft.axes[0];
  int tly = fence->topLeft.axes[1];
  int brx = fence->botRight.axes[0];
  int bry = fence->botRight.axes[1]; 
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    if ((shapeBoundary.topLeft.axes[0] >= tlx) &&
	 ( shapeBoundary.topLeft.axes[0] <= brx)) {
      if((shapeBoundary.topLeft.axes[1] >= tly) &&
	 (shapeBoundary.topLeft.axes[1] <= bry)){
      int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
      newPos.axes[axis] += (2*velocity);
      used = 1;
      }
   }/**< if outside of fence */ /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */

  return used;
}

int bounceRight(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  int used = 0;
  int tlx = fence->topLeft.axes[0];
  int tly = fence->topLeft.axes[1];
  int brx = fence->botRight.axes[0];
  int bry = fence->botRight.axes[1]; 
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    if ((shapeBoundary.botRight.axes[0] >= tlx) &&
	 ( shapeBoundary.botRight.axes[0] <= brx)) {
      if((shapeBoundary.botRight.axes[1] >= tly) &&
	 (shapeBoundary.botRight.axes[1] <= bry)){
      int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
      newPos.axes[axis] += (2*velocity);
      used = 1;
      }
   }/**< if outside of fence */ /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */

  return used;
}


u_int bgColor = COLOR_BLUE;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */
Region paddle1;
Region paddle2;
Region ball;

/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(1);

  shapeInit();
  char score[3] = {'0','0', '\0'};
  
  layerInit(&layer0);
  layerDraw(&layer0);
  drawString5x7(50, 152, score, COLOR_BLACK, COLOR_BLUE);
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
		 
  for(;;) {
    while (0) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
    movLayerDraw(&ml1, &layer1);
    layerGetBounds(&fieldLayer, &fieldFence);
    layerGetBounds(&layer1, &paddle1);
    layerGetBounds(&layer2, &paddle2);
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
    if(mlAdvance(&ml0, &fieldFence)){
      k =1;
    }
    if(bounceLeft(&ml0, &paddle1)){
      k=1;
    }
    if(bounceRight(&ml0, &paddle2)){
      k=1;
    }
    if (!p2sw_read()){
    MovLayer *a = &ml1;
    a->layer->posNext.axes[1]--;
    movLayerDraw(a, &layer1); 
    }
    count = 0;
    k=0;
  } 
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}

