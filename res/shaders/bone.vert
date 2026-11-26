layout(location=0) in vec3 vertex_position;
layout(location=1) in vec4 vertex_color;     // its colour, its alpha its glow
const float GLOW = 3.0;   // a vertex's glow (its colour's alpha, doc/plans/hdr.md): up to this much past white

layout(location=2) in vec4 bone_index;    // two bones, the second's weight (bones/sys/generate.c), 0-1 bytes
uniform mat4 mvp_matrix;
uniform mat4 bone_matrix[32];
uniform vec3 bone_positions[32];
uniform vec4 fog_data;
uniform float paint_weights;  // 1: coloured by bone, mixed at joints by weight (the Paint Weights setting)
out vec3 vertex_color_frag;
out float fog_blend;

// a bone's paint: hues spread by the golden ratio, so neighbouring bones differ
vec3 bone_paint(int bone) {
    float hue = fract(float(bone) * 0.618034);
    return clamp(abs(fract(hue + vec3(0.0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - 3.0) - 1.0, 0.0, 1.0) * 0.8 + 0.2;
}

void main() {
    // blended at joints: moved by both bones, by the second's weight
    int a = int(bone_index.x * 255.0 + 0.5);
    int b = int(bone_index.y * 255.0 + 0.5);
    vec4 position_a = bone_matrix[a] * vec4(vertex_position - bone_positions[a], 1);
    vec4 position_b = bone_matrix[b] * vec4(vertex_position - bone_positions[b], 1);
    vec4 final_position = mix(position_a, position_b, bone_index.z);
    gl_Position = mvp_matrix * final_position;
    vertex_color_frag = paint_weights > 0.5 ? mix(bone_paint(a), bone_paint(b), bone_index.z) : vertex_color.rgb * (1.0 + vertex_color.a * GLOW);
    float fog_level = gl_Position.z * gl_Position.z;
    fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
}
