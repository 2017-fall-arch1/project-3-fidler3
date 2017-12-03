#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "shapemotion.h"

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
char mlAdvance(MovLayer *ml, Region *fence)
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

char bounceLeft(MovLayer *ml, Region *fence)
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

char bounceRight(MovLayer *ml, Region *fence)
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



