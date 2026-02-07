precision highp float;

layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_color;
layout(location=2) in float bone_index;
uniform mat4 camera_matrix;
uniform mat4 transform_matrix;
uniform mat4 bone_matrix[32];
uniform vec3 bone_positions[32];
uniform vec4 fog_data;
out vec3 vertex_color_frag;
out float fog_blend;

void main() {
    int bone_index2 = int(bone_index * 255.0);
    mat4 bone = bone_matrix[bone_index2];
    vec3 bone_position = bone_positions[bone_index2];
    vec4 transformed_position = bone * vec4(vertex_position - bone_position, 1);
    vec4 final_position = transformed_position;
    gl_Position = camera_matrix * transform_matrix * final_position;
    vertex_color_frag = vertex_color;
    float fog_level = gl_Position.z * gl_Position.z;
    fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
}
