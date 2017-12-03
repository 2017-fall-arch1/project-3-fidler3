#ifndef shapemotion_included
#define shapemotion_included

#define GREEN_LED BIT6

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

void state_update(); //handles states

void movLayerDraw(MovLayer *movLayers, Layer *layers);

char mlAdvance(MovLayer *ml, Region *fence);

char bounceLeft(MovLayer *ml, Region *fence);

char bounceRight(MovLayer *ml, Region *fence);

#endif
