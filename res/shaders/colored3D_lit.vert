layout(location=0) in vec3 vertex_position;
layout(location=1) in vec4 vertex_color;     // its colour, its alpha its glow
uniform mat4 mvp_matrix;
uniform mat4 model_matrix;
out vec4 vertex_color_frag;
out vec3 world_position;

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    world_position = (model_matrix * vec4(vertex_position, 1)).xyz;
    vertex_color_frag = vertex_color;
}
