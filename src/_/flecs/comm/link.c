#define zoxc_link(name, type, links_name, ...)\
    zoxc(name, type); \
    void on_destroyed_##name(iter *it) {\
        zox_sys_world(); \
        zox_sys_begin(); \
        zox_sys_out(name); \
        for (int i = 0; i < it->count; i++) {\
            zox_sys_e(); \
            zox_sys_o(name, component); \
            if (component->value && zox_valid(component->value)) { \
                zox_muter(component->value, links_name, links_component); \
                remove_from_##links_name(links_component, e); \
            } \
            component->value = 0; \
        } \
    }

#define zoxd_links2(name, ...)\
    zoxd(name);\
    zox_observe(on_destroyed_##name, EcsOnRemove, __VA_ARGS__)

#define zoxd_links(name)\
    zoxd_links2(name, [out] name)
