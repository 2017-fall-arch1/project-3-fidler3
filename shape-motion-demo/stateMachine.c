#include "shapemotion.h"

void stateMachine(){
  u_int switches = p2sw_read();
  layerGetBounds(&layer1, &paddle1);
  layerGetBounds(&layer2, &paddle2);

  if(!(switches & (1))){
    if(p1->topLeft.axes[1] == fence->topLeft.axes[1])
      return;   
    MovLayer *a = &ml1;
    a->layer->posNext.axes[1]-=10;
    movLayerDraw(a, &layer1);
    return;
  }
  if(!(switches & (2))){
    if(p1->botRight.axes[1] == fence->botRight.axes[1])
      return;
    MovLayer *a = &ml1;
    a->layer->posNext.axes[1]+=10;
    movLayerDraw(a, &layer1);
    return;
  }
  if(!(switches & (4))){
    if(p2->topLeft.axes[1] == fence->topLeft.axes[1])
      return;
    MovLayer *a = &ml2;
    a->layer->posNext.axes[1]-=10;
    movLayerDraw(a, &layer2);
    return;
  }
  
  if(!(switches & (8))){
    if(p2->botRight.axes[1] == fence->botRight.axes[1])
      return;
    MovLayer *a = &ml2;
    a->layer->posNext.axes[1]+=10;
    movLayerDraw(a, &layer2);
    return;
  }
}
