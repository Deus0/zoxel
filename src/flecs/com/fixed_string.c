// -----------------------------------------------------------------
// Fixed-length string component macros
// -----------------------------------------------------------------

// Define a component with fixed buffer (for use in a module)
#define zoxc_fixed_string(T, l) \
    typedef struct { \
        char value[l]; \
    } T; \
    zoxc_custom(T)

// Declare/use component in world (in module)
#define zoxd_fixed_string(T) \
    zoxd(T)
