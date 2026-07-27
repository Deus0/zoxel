#define zoxc_link(T, type, links_name, ...)\
    zoxc(T, type); \
    void on_destroyed_##T(iter *it) {\
        zox_sys_world(); \
        zox_sys_begin(); \
        zox_sys_out(T); \
        for (int i = 0; i < it->count; i++) {\
            zox_sys_e(); \
            zox_sys_o(T, component); \
            if (component->value && zox_valid(component->value)) { \
                zox_muter(component->value, links_name, links_component); \
                remove_from_##links_name(links_component, e); \
            } \
            component->value = 0; \
        } \
    }

#define zoxd_links2(T, ...)\
    zoxd(T);\
    zox_observe(on_destroyed_##T, EcsOnRemove, __VA_ARGS__)

#define zoxd_links(T)\
    zoxd_links2(T, [out] T)
