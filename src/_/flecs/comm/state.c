#define zoxc_state(T) \
    zoxc_byte(T); \
    zox_increment_system_with_reset(T, zox_dirty_end)

#define zoxc_state_remove(T) \
    zoxc_byte(T); \
    zox_increment_system_with_remove(T, zox_dirty_end)

#define zoxd_state(T, ...) \
    zoxd_byte(T); \
    zoxd_system_increment(T, __VA_ARGS__)
