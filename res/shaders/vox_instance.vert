layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_color;
uniform InstanceMatrices {
    mat4 matrices[zox_ubo_max];
};
uniform mat4 camera_matrix;
uniform vec4 fog_data;
out vec3 vertex_color_frag;
out float fog_blend;

void main() {
    mat4 transform_matrix = matrices[gl_InstanceID];
    gl_Position = camera_matrix * transform_matrix * vec4(vertex_position, 1);
    vertex_color_frag = vertex_color;
    float fog_level = gl_Position.z * gl_Position.z;
    fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
}
