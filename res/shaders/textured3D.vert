layout(location=0) in vec3 vertex_position;
layout(location=1) in vec2 vertex_uv;
layout(location=2) in vec3 vertex_color;
uniform mat4 camera_matrix;
uniform mat4 transform_matrix;
out float fog_level;
out vec2 uv;
out vec3 vertex_color_frag;

void main() {
    gl_Position = camera_matrix * transform_matrix * vec4(vertex_position, 1);
    fog_level = gl_Position.z * gl_Position.z;
    uv = vertex_uv;
    vertex_color_frag = vertex_color;
}
