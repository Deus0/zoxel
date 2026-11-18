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
#include "dirty.c"

entity zox_ins_named(ecs* world, entity prefab) {
    entity e = zox_ins(world, prefab);
    zox_set_unique_name(e, zox_get_name(prefab));
    return e;
}


#define zox_each(world, id) ecs_each(world, id)
#define zox_each_next(it) ecs_each_next(&it)

// every entity with T, outside the frame's systems (a gpu dispose or restore): disabled ones too, not prefabs
//  - zox_each_begin(T); zox_each_o(T, name); ... zox_each_end(); e is the entity
#define zox_each_begin(T)\
    { iter each_it = zox_each(world, T);\
    while (zox_each_next(each_it)) {\
        for (int each_i = 0; each_i < each_it.count; each_i++) {\
            const entity e = each_it.entities[each_i];\
            if (zox_is_prefab(e)) continue

#define zox_each_o(T, name)\
    T* name = &ecs_field(&each_it, T, 0)[each_i]

#define zox_each_end() } } }

#define zox_each_add(T, tag) zox_each_add_id(world, ecs_id(T), ecs_id(tag))
