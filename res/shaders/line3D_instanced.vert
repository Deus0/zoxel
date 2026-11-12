// one line an instance: line_t is 0 or 1, the vertex is at its start or end
in float line_t;
in vec3 line_start;
in vec3 line_end;
in vec4 line_color;
uniform mat4 camera_matrix;
out float fog_level;
out vec4 vertex_color;

void main() {
    vec3 position = mix(line_start, line_end, line_t);
    gl_Position = camera_matrix * vec4(position, 1);
    fog_level = gl_Position.z * gl_Position.z;
    vertex_color = line_color;
}
