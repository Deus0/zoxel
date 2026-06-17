// #define zox_debug_bones
byte render_bones = 0;
byte transparent_meshes = 0;
byte paint_bone_weights = 0;

void on_set_transparent_mesh(ecs* world, void* value) {
    transparent_meshes = *(byte*) value;
}

void on_set_bone_rendering(ecs* world, void* value) {
    render_bones = (*(byte*) value);
}

void on_set_paint_bone_weights(ecs* world, void* value) {
    paint_bone_weights = (*(byte*) value);
}

// TODO: also pass in function
void spawn_settings_bones(ecs* world) {
    // spawn_setting_byte(world, prefab_setting, "transparent mesh", transparent_meshes, on_set_transparent_mesh);
    // spawn_setting_byte(world, prefab_setting, "render nones", !render_bones, on_set_bone_rendering);
}

// Temp use old way
void initialize_settings_bones(ecs* world) {
#ifndef zox_debug
    zoxs_new_byte("transparent meshes", on_set_transparent_mesh, transparent_meshes);
    zoxs_new_byte("render bones", on_set_bone_rendering, render_bones);
    zoxs_new_byte("paint bone weights", on_set_paint_bone_weights, paint_bone_weights);
#endif
}
