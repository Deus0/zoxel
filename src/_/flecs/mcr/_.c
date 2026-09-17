#include "memory.c"
#include "reflect.c"
#include "ids.c"
#include "get.c"
#include "set.c"
#include "mut.c"
#include "tag.c"
#include "delete.c"
#include "override.c"
#include "prefab.c"
#include "instance.c"
#include "components.c"
#include "dest.c"
#include "names.c"
#include "observer.c"
#include "modules.c"
#include "systems.c"
#include "fields.c"
#include "filters.c"
#include "entities.c"
#include "query.c"
#include "hooks.c"
#include "children.c"
#include "disable.c"
#include "pair.c"

entity zox_ins_named(ecs* world, entity prefab) {
    entity e = zox_ins(world, prefab);
    zox_set_unique_name(e, zox_get_name(prefab));
    return e;
}


#define zox_each(world, id) ecs_each(world, id)
#define zox_each_next(it) ecs_each_next(&it)
