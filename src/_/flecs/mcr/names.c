// #define zox_debug_spawning

#ifdef zox_debug
    #define zox_get_name(e)\
        (zox_valid(e) && zox_alive(e)) ? ecs_get_name(world, e) : (zox_valid(e) ? "Dead" : "Invalid")
    #define zox_sys_get_name(e) \
        (ecs_is_valid(it->world, e) && ecs_is_alive(it->world, e)) ?\
            ecs_get_name(it->world, e) : "Invalid"

#else
    #define zox_get_name(e) ecs_get_name(world, e)
    #define zox_sys_get_name(e) ecs_get_name(it->world, e)
#endif

#define zox_sys_e_name zox_sys_get_name(it->entities[i])

#define zox_getn(e)\
    (zox_valid(e) ? ecs_get_name(world, e) : "Invalid")

#define zox_sys_name() ecs_get_name(it->world, it->system)

#define zox_set_name(e, name)\
    ecs_set_name(world, e, name);

#define zox_get_name_type(type)\
    zox_get_name(ecs_id(type))

#define zox_get_namespace(e)\
    ecs_get_fullpath(world, e)



// -- ? --

void zox_set_entity_name(ecs *world, entity e, const char* name) {
    if (!e || !name) return;

    //char name_buffer[128]; // Adjust size if needed
    //get_entity_string_stack(name_buffer, sizeof(name_buffer), e, name);

    char buffer[128]; // ample stack buffer, tune size
    int nlen = snprintf(buffer, sizeof(buffer), "%s_%lu", name, (unsigned long)e);
    if (nlen < 0 || nlen >= (int)sizeof(buffer)) {
        zox_log_error("Entity name too long");
        return;
    }

    ecs_set_name(world, e, buffer);
}

void zox_set_name_spawned(ecs *world, entity e, const char* name) {
#ifndef zox_disable_names
    zox_set_entity_name(world, e, name);
#endif
}

#define zox_set_unique_name(e, name)\
    zox_set_name_spawned(world, e, name)

#define zox_prefab_name2(label, label2)\
    zox_set_name_spawned(world, e, label2 label);

#define zox_prefab_name(label)\
    zox_prefab_name2(label, "prefab_")

#define zox_name(label)\
    zox_set_name_spawned(world, e, label);

#define zox_set_name_e(e, name)\
    zox_set_entity_name(world, e, name);
