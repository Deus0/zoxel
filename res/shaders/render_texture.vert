precision highp float;

uniform mat4 camera_matrix;
uniform mat4 transform_matrix;
layout(location=0) in vec2 vertex_position;
layout(location=1) in vec2 vertex_uv;
out vec2 uv;

void main() {
    gl_Position = camera_matrix * transform_matrix * vec4(vertex_position, 0.0, 1.0);
    uv = vertex_uv;
}
