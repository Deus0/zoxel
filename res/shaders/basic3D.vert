layout(location=0) in vec3 vertex_position;
uniform mat4 mvp_matrix;
out float fog_level;

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    fog_level = gl_Position.z * gl_Position.z;
}
