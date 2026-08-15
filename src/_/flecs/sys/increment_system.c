#define zoxd_system_increment(component, ...)\
    zox_system(component##IncrementSystem, zoxp_state, [out] component, __VA_ARGS__)

#define zoxd_system_increment_pip(component, p, ...)\
    zox_system(component##IncrementSystem, p, [out] component, __VA_ARGS__)

#define zoxd_system_increment_external(component, full_path_component, pip, ...)\
    zox_system(component##IncrementSystem, pip, [out] full_path_component, __VA_ARGS__)

// if non zero, moves to target state
#define zox_increment_system(component_name, target) \
    zox_sys_untimed(component_name##IncrementSystem) { \
        zox_sys_begin();\
        zox_sys_out(component_name);\
        for (int i = 0; i < it->count; i++) {\
            zox_sys_o(component_name, component)\
            if (component->value) {\
                if (component->value < target) {\
                    component->value++;\
                }\
            }\
        }\
    } zox_sys_end_untimed(component_name##IncrementSystem);

// if non zero, moves to target state, then resets
#define zox_increment_system_with_reset(T, target)\
    zox_sys_untimed(T##IncrementSystem) {\
        zox_sys_begin();\
        zox_sys_out(T);\
        for (int i = 0; i < it->count; i++) {\
            zox_sys_o(T, component);\
            if (component->value) {\
                if (component->value < target) {\
                    component->value++;\
                } else if (component->value == target) {\
                    component->value = 0;\
                } \
            } \
        }\
    } zox_sys_end_untimed(T##IncrementSystem);

#define zox_increment_system_with_remove(T, target)\
    zox_sys_untimed(T##IncrementSystem) {\
        zox_sys_world(); \
        zox_sys_begin();\
        zox_sys_out(T);\
        for (int i = 0; i < it->count; i++) {\
            zox_sys_e(); \
            zox_sys_o(T, component);\
            if (component->value) {\
                if (component->value < target) {\
                    component->value++;\
                } else if (component->value == target) {\
                    zox_remove(e, T); \
                } \
            } \
        }\
    } zox_sys_end_untimed(T##IncrementSystem)

// if non zero, moves to target state
#define zox_increment_system_with_reset_extra( \
    component_name, \
    start1, target1, \
    start2, target2) \
        zox_sys_untimed(component_name##IncrementSystem) { \
            zox_sys_begin(); \
            zox_sys_out(component_name); \
            for (int i = 0; i < it->count; i++) { \
                zox_sys_o(component_name, component); \
                \
                if (component->value >= start1 && component->value < target1) {\
                    component->value++;\
                } else if (component->value >= start2 && component->value < target2) {\
                    component->value++;\
                } else if (component->value == target2) {\
                    component->value = 0;\
                }\
            }\
        } zox_sys_end_untimed(component_name##IncrementSystem);
