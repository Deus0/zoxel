// Containers like groups of elements, Grids, Lists, etc
#ifndef zoxm_containers
#define zoxm_containers

#include "com.c"
#include "sys/_.c"

zox_begin_module(Containers) {
    zox_components_containers(world);
    zox_systems_containers(world);
} zox_end_module(Containers);

#endif
