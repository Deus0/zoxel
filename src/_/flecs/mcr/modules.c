#define zox_add_module(T) \
    ecs_import_c(world, import_##T, #T)

#define zox_module(T) \
    ECS_MODULE(world, T)

#define zox_import_module(T) \
    ECS_IMPORT(world, T)

#define zox_module_dispose(function) \
    ecs_atfini(world, function, NULL);

void zox_debug_module(ecs *world, const char *name, entity module_type) {
#ifdef zox_debug_modules
    zox_log(" + module [%s] > [%s]\n", name, zox_get_name(module_type))
#else
    (void)world;
    (void)name;
    (void)module_type;
#endif
}

#define zox_begin_module(T)\
    void T##Import(ecs* world) {\
        zox_module(T);\
        zox_statistics_modules++;\
        zox_debug_module(world, #T, zox_id(T));

#define zox_end_module(T)\
    /* end timings here */\
}
