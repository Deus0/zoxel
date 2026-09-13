// stores camera data to use in camera pass systems
entity renderer_camera;
float4x4 render_camera_matrix;
float3 render_camera_position;
color_rgb renderer_fog_color;
byte renderer_layer = 0;
byte render_camera_fov;

// fog
byte is_render_fog = 1;
float fog_density = 0.03f; // 0326f;

static inline float get_fog_density() {
    return is_render_fog ?
        fog_density * fog_density :
        0;
}

static inline float4 get_fog_value() {
    return float4_from_float3(color_rgb_to_float3(renderer_fog_color), get_fog_density());
}

void set_camera_fog_color(ecs* world, color_rgb fog) {
    for (int i = 0; i < main_cameras_count; i++) {
        entity camera = main_cameras[i];
        if (zox_valid(camera)) {
            zox_setv(camera, FogColor, fog);
        } else {
            zox_logw("Main Camera Invalid at [%i]", i)
        }
    }
}