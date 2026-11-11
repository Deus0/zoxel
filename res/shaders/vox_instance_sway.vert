// Vox Instance Sway - like vox_instance, but sways top verts with noise (grass)
// NOTE: matrices are model matrices here, camera_matrix is applied after sway
// NOTE: [0][3] of a model matrix carries the instance brightness, like vox_instance.vert
layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_color;
uniform InstanceMatrices {
    mat4 matrices[zox_ubo_max];
};
uniform mat4 camera_matrix;
uniform vec4 fog_data;
uniform vec4 sway_data; // x: time, y: strength, z: mesh bottom y, w: mesh height
out vec3 vertex_color_frag;
out float fog_blend;

float sway_hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// value noise, 0 to 1
float sway_noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    float a = sway_hash(i);
    float b = sway_hash(i + vec2(1.0, 0.0));
    float c = sway_hash(i + vec2(0.0, 1.0));
    float d = sway_hash(i + vec2(1.0, 1.0));
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

void main() {
    mat4 model_matrix = matrices[gl_InstanceID];
    float instance_brightness = model_matrix[0][3];
    model_matrix[0][3] = 0.0;
    vec4 world_position = model_matrix * vec4(vertex_position, 1.0);
    // 0 at mesh bottom, 1 at top - bottom verts stay planted
    float top = clamp((vertex_position.y - sway_data.z) / max(sway_data.w, 0.0001), 0.0, 1.0);
    top = top * top;
    // wind moving across the world
    vec2 wind = world_position.xz * 0.35 + vec2(sway_data.x * 0.7, sway_data.x * 0.45);
    vec2 sway = vec2(
        sway_noise(wind),
        sway_noise(wind + vec2(17.3, 5.1))) * 2.0 - 1.0;
    world_position.xz += sway * sway_data.y * top;
    gl_Position = camera_matrix * world_position;
    vertex_color_frag = vertex_color * instance_brightness;
    float fog_level = gl_Position.z * gl_Position.z;
    fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
}
