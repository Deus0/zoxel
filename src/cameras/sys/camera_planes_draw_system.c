extern void draw_planes(ecs *world, const plane *planes, const int plane_count)

zox_sys2(CameraPlanesDrawSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CameraPlanes);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CameraPlanes, planes);

        draw_planes(world, planes->value, 6);
    }
} zox_sys_end(CameraPlanesDrawSystem);
