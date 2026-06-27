// #define zox_debug_modules

// #define zoxd_module(T) ECS_COMPONENT_DECLARE(T)
#define zoxd_module(T) ECS_DECLARE(T)

#define zox_module(T) ECS_MODULE(world, T)

#define zox_import_module(T) ECS_IMPORT(world, T)

#define zox_module_dispose(function) ecs_atfini(world, function, NULL);

#define zox_begin_module(T)\
    void T##Import(ecs* world) {\
        zox_module(T);\
        zox_statistics_modules++;\
        zox_debug_module(world, #T, zox_id(T));

#define zox_end_module(T)\
    /* end timings here */\
}

void zox_debug_module(ecs *world, const char *name, entity module_type) {
#ifdef zox_debug_modules
    zox_log(" + module [%s] > [%s]\n", name, zox_get_name(module_type))
#else
    (void)world;
    (void)name;
    (void)module_type;
#endif
}

void zox_debug_module2(ecs *world, const char *name, const entity module_type) {
    zox_log(" + module [%s] > [%s]\n", name, zox_get_name(module_type))
}

#define zox_debug_single_module(T) zox_debug_module2(world, #T, zox_id(T));
