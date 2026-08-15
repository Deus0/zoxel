#define camera_filtering_begin() byte filtering = zox_has(renderer_camera, CameraFilter)

#define camera_filtering_check()\
    entity fe = it->entities[i];\
    byte filtered = zox_has(fe, CameraRenderer) ? zox_getv(fe, CameraRenderer) != renderer_camera : 1;\
    if (filtering) {\
        if (filtered) {\
            continue;\
        }\
    } else if (zox_has(fe, CameraRenderer) && filtered) {\
        /* if the object has a filter, but not filtering, we skip rendering it */ \
        continue;\
    }
