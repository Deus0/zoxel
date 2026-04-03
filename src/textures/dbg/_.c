// extern entity spawn_element_texture(ecs*, entity, entity, entity, int2, int2);
extern entity get_linked_canvas(ecs*, entity);
extern entity spawn_element2(ecs *world, entity p, entity canvas, entity parent, int2 position, int2 size, int2 tsize, float2 anchor, byte layer, color fcolor, color ocolor);

#include "tilemap.c"
#include "generated.c"
