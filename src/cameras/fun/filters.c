#define camera_filtering_begin() \
    byte filtering = renderer_camera && zox_has(renderer_camera, CameraFilter)

#define camera_filtering_check()\
    entity fe = it->entities[i];\
    entity fe_camera = zox_get_link(world, fe, CameraRenderer);\
    byte filtered = renderer_camera && fe_camera ? \
            fe_camera != renderer_camera : \
            1; \
    if (filtering) {\
        if (filtered) {\
            continue;\
        }\
    } else if (fe_camera && filtered) {\
        /* if the object has a filter, but not filtering, we skip rendering it */ \
        continue;\
    }
