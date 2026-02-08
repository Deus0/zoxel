uniform mat4 camera_matrix;
uniform vec3 position;
uniform float angle;
uniform float scale;
layout(location=0) in vec2 vertex_position;
layout(location=1) in vec2 vertex_uv;
out vec2 uv;

void main() {
    mat2 rotate = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    gl_Position = camera_matrix * vec4(vec2(position.x, position.y) + (rotate * vertex_position) * scale, position.z, 1.0);
    uv = vertex_uv;
}
