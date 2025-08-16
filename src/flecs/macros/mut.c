#define zox_modified(e, T)\
    ecs_modified(world, e, T)

#define zox_gett_mut(e, T)\
    ecs_get_mut(world, e, T)

#define zox_mut(e, T)\
    ecs_get_mut(world, e, T)


#define zox_get_mut(e, T)\
    ecs_get_mut(world, e, T);

#define zox_mut_begin(e, T, name)\
    T *name = zox_get_mut(e, T)

#define zox_mut_end(e, T)\
    ecs_modified(world, e, T);

#define zox_get_mutt(e, T, name)\
    T *name = zox_get_mut(e, T)

#define zox_muter(e, T, name)\
    zox_get_mutt(e, T, name)\
    zox_modified(e, T);

#define zox_set_mut(e, T, valuer) {\
    T *component = zox_get_mut(e, T)\
    if (component->value != valuer) {\
        component->value = valuer;\
        zox_modified(e, T)\
    }\
}
