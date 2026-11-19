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
float fog_settings_mul = 0.048f;   // the Fog setting (0 to 1) to fog_density

static inline float get_fog_density() {
    return is_render_fog ?
        fog_density * fog_density :
        0;
}

// the camera drawing now is under water (cameras.Underwater, set in camera_render3_system): a thick blue fog
byte render_underwater = 0;
const color_rgb render_underwater_color = { 28, 72, 120 };
#define render_underwater_density 0.15f   // fully blue about 5m out, 9 blocks (fog is 1.1 - 2^(-density * z^2))

static inline float4 get_fog_value() {
    const color_rgb color = render_underwater ? render_underwater_color : renderer_fog_color;
    const float density = render_underwater ? render_underwater_density : get_fog_density();
    return float4_from_float3(color_rgb_to_float3(color), density);
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