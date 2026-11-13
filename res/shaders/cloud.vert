layout(location=0) in vec3 vertex_position;
uniform mat4 mvp_matrix;
out float fog_level;
out vec3 local_position;

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    // clouds are far up: a lighter fog than the ground's, near ones clear, far ones fade into the sky
    float depth = gl_Position.z * 0.17;
    fog_level = depth * depth;
    local_position = vertex_position;
}
