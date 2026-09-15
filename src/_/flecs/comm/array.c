#define zoxc_array(name, type, length)\
    const byte name##_length = length;\
    typedef struct {\
        type value[length];\
    } name;\
    zoxc_custom(name)

// placeholder for later
#define zox_define_component_array(T)\
    zoxd(T)

#define zoxc_array_l(T, type, capacity) \
    const ulong T##_length = capacity; \
    typedef struct { \
        ulong length; \
        type value[capacity]; \
    } T; \
    zoxc_custom(T)

// placeholder for later
#define zox_define_component_array_l(T)\
    zoxd(T)
