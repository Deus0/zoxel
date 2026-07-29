void add_prefab_render_camera(ecs* world, entity e) {
    if (!e) {
        return;
    }
    zox_prefab_set(e, FrameBufferLink, { 0 });
    zox_prefab_set(e, RenderBufferLink, { 0 });
}

void spawn_prefabs_rendering_cameras(ecs *world) {
    add_prefab_render_camera(world, prefab_camera3);
    add_prefab_render_camera(world, prefab_camera_game);
}
