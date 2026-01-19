#define realm_clear_system(T)\
zox_sys2(T##RealmClearSystem) {\
    zox_sys_world();\
    zox_sys_begin();\
    zox_sys_in(GenerateRealm);\
    zox_sys_out(T);\
    for (int i = 0; i < it->count; i++) {\
        zox_sys_i(GenerateRealm, state);\
        zox_sys_o(T, data);\
        \
        if (state->value != zox_generate_realm_clear) {\
            continue;\
        }\
        \
        for (int j = 0; j < data->length; j++) {\
            entity e = data->value[j];\
            if (zox_valid(e)) {\
                zox_delete(e);\
            }\
        }\
        \
        resize_##T(data, 0);\
    }\
} zox_sys_end(T##RealmClearSystem)

#define realm_clear_systemd(M, T)\
zox_system(\
    T##RealmClearSystem,\
    EcsOnLoad,\
    [in] realms.GenerateRealm,\
    [out] M.T,\
    [none] realms.Realm\
)
