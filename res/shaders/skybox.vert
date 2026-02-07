precision highp float;

layout(location=0) in vec3 vertex_position;
uniform mat4 camera_matrix;
uniform mat4 transform_matrix;
out float fog_level;
out vec3 mesh_pos;

void main() {
    gl_Position = camera_matrix * transform_matrix * vec4(vertex_position, 1);
    fog_level = gl_Position.z * gl_Position.z;
    mesh_pos = vertex_position;
}
