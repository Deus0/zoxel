#define zox_malloc(size) \
    ecs_os_malloc(size)

#define zox_free(v) \
    ecs_os_free(v)

#define zox_realloc(v, size) \
    ecs_os_realloc(v, size)
