in vec3 position;
in vec4 color;
uniform mat4 camera_matrix;
uniform float thickness;
out float fog_level;
out vec4 frag_color;

void main() {
    gl_Position = camera_matrix * vec4(position, 1.0);
    fog_level = gl_Position.z;
    float distance_to_camera = distance(vec3(0, 0, 0), gl_Position.xyz);
    gl_PointSize = thickness / distance_to_camera;
    frag_color = color;
}
