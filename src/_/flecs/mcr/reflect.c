#ifdef flecs_profiler

extern char* convert_zext_to_text(const byte*, byte);

#define zoxd_reflect_1(T, Type) \
    ecs_struct(world, { \
        .entity = zox_id(T), \
        .members = { \
            { .name = "value", .type = zox_id(Type) } \
        } \
    })

#define zoxd_reflect_2(T, Type) \
    ecs_struct(world, { \
        .entity = zox_id(T), \
        .members = { \
            { .name = "x", .type = zox_id(Type) }, \
            { .name = "y", .type = zox_id(Type) } \
        } \
    })

#define zoxd_reflect_3(T, Type) \
    ecs_struct(world, { \
        .entity = zox_id(T), \
        .members = { \
            { .name = "x", .type = zox_id(Type) }, \
            { .name = "y", .type = zox_id(Type) }, \
            { .name = "z", .type = zox_id(Type) } \
        } \
    })

#define zoxd_reflect_4(T, Type) \
    ecs_struct(world, { \
        .entity = zox_id(T), \
        .members = { \
            { .name = "x", .type = zox_id(Type) }, \
            { .name = "y", .type = zox_id(Type) }, \
            { .name = "z", .type = zox_id(Type) }, \
            { .name = "w", .type = zox_id(Type) } \
        } \
    })

static int zox_text_serialize(
    const ecs_serializer_t* serializer,
    const void* ptr
) {
    const text* t = ptr;
    char* string = convert_zext_to_text(
        t->value,
        t->length);
    int result = serializer->value(
        serializer,
        ecs_id(ecs_string_t),
        &string);
    free(string);
    return result;
}

#define zoxd_text_reflect(T) \
    ecs_opaque(world, { \
        .entity = zox_id(T), \
        .type = { \
            .as_type = ecs_id(ecs_string_t),\
            .serialize = zox_text_serialize \
        }, \
    })



#else
    #define zoxd_reflect_1(T, Type)
    #define zoxd_reflect_2(T, Type)
    #define zoxd_reflect_3(T, Type)
    #define zoxd_reflect_4(T, Type)
    #define zoxd_text_reflect(T)
#endif