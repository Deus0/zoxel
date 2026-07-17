typedef ecs_entity_t entity;
typedef ecs_world_t ecs;
typedef ecs_iter_t iter;

#include "new.c"
#include "ids.c"
#include "add.c"
#include "get.c"
#include "set.c"
#include "has.c"
#include "mut.c"
#include "tag.c"
#include "delete.c"
#include "override.c"
#include "prefab.c"
#include "instance.c"
#include "observer.c"
#include "components.c"
#include "names.c"
#include "modules.c"
#include "systems.c"
#include "fields.c"
#include "filters.c"
#include "entities.c"
#include "query.c"
#include "hooks.c"
#include "children.c"
#include "disable.c"

entity zox_ins_named(ecs* world, entity prefab) {
    entity e = zox_ins(world, prefab);
    zox_set_unique_name(e, zox_get_name(prefab));
    return e;
}
