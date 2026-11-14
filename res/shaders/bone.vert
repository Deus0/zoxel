layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_color;
layout(location=2) in vec4 bone_index;    // two bones, the second's weight (bones/sys/generate.c), 0-1 bytes
uniform mat4 mvp_matrix;
uniform mat4 bone_matrix[32];
uniform vec3 bone_positions[32];
uniform vec4 fog_data;
out vec3 vertex_color_frag;
out float fog_blend;

void main() {
    // blended at joints: moved by both bones, by the second's weight
    int a = int(bone_index.x * 255.0 + 0.5);
    int b = int(bone_index.y * 255.0 + 0.5);
    vec4 position_a = bone_matrix[a] * vec4(vertex_position - bone_positions[a], 1);
    vec4 position_b = bone_matrix[b] * vec4(vertex_position - bone_positions[b], 1);
    vec4 final_position = mix(position_a, position_b, bone_index.z);
    gl_Position = mvp_matrix * final_position;
    vertex_color_frag = vertex_color;
    float fog_level = gl_Position.z * gl_Position.z;
    fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
}
