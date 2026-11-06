#define zoxc_dirty(T) \
    zox_tag(T##Dirty); \
    zox_tag(T##Update)

#define zoxd_dirty(T) \
    zoxd_tag_event(T##Dirty); \
    zoxd_tag_event(T##Update)

// Triggers Update Tags
#define zoxc_system_dirty(T) \
\
void T##_dirty_system(iter* it) { \
    zox_sys_world(); \
    for (int i = 0; i < it->count; i++) { \
        zox_sys_e(); \
        zox_remove(e, T##Dirty); \
        zox_add(e, T##Update); \
    } \
} zoxd_system(T##_dirty_system); \
\
void T##_update_system(iter* it) { \
    zox_sys_world(); \
    for (int i = 0; i < it->count; i++) { \
        zox_sys_e(); \
        zox_remove(e, T##Update); \
    } \
} zoxd_system(T##_update_system)

#define zoxd_system_dirty(T) \
    zox_system(\
        T##_dirty_system,\
        zoxp_remove,\
        [none] T##Dirty\
    ); \
    zox_system(\
        T##_update_system,\
        zoxp_remove,\
        [none] T##Update\
    )
