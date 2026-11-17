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

// 0 to 1, the 3D view faded to black (XR loading: huds3_darkness): the fog darkens and thickens, the far first
//  - 3D uis render without it (the loading label stays lit)
float render_darkness = 0;
#define render_darkness_density 60.0f   // the fog's density fully dark: black a few cm out

static inline float4 get_fog_value() {
    const float dark = render_darkness;
    const float3 fog = float3_scale(color_rgb_to_float3(renderer_fog_color), 1 - dark);
    return float4_from_float3(fog, get_fog_density() + render_darkness_density * dark * dark * dark * dark);
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