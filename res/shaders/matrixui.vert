uniform mat4 camera_matrix;
uniform mat4 matrix;
layout(location=0) in vec2 vertex_position;
layout(location=1) in vec2 vertex_uv;
out vec2 uv;

void main() {
    vec4 pos = vec4(vertex_position, 0.0, 1.0);
    gl_Position = camera_matrix * matrix * pos;
    uv = vertex_uv;
}
