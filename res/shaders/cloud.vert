layout(location=0) in vec3 vertex_position;
uniform mat4 mvp_matrix;
out vec3 local_position;

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    local_position = vertex_position;
}
