// -----------------------------------------------------------------
// Fixed-length string component macros
// -----------------------------------------------------------------

// Define a component with fixed buffer (for use in a module)
#define zoxc_fixed_string(T, l) \
\
uint T##_length = l;\
\
typedef struct { \
    char value[l]; \
} T; \
\
zoxc_custom(T); \
\
void set_##T(ecs* world, entity e, const char* value) { \
    T com = { }; \
    size_t len = strlen(value); \
    if (len >= l) { \
        len = l - 1; \
    } \
    memcpy(com.value, value, len); \
    com.value[len] = '\0'; \
    zox_set_ptr(e, T, com); \
}

// Declare/use component in world (in module)
#define zoxd_fixed_string(T) \
    zoxd(T)
