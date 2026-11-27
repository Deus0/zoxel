// Vox Instance - static vox meshes drawn many times (minivoxes, characters)
// NOTE: matrices are model matrices, camera_matrix is applied here
// NOTE: a model matrix's bottom row is unused (0 0 0 1), [0][3] carries the instance brightness
//  - set by vox_instance_render_system from the entity's Brightness (i.e. the light at a minivox)
layout(location=0) in vec3 vertex_position;
layout(location=1) in vec4 vertex_color;     // its colour, its alpha its glow
const float GLOW = 3.0;   // a vertex's glow (its colour's alpha, doc/plans/hdr.md): up to this much past white

uniform InstanceMatrices {
    mat4 matrices[zox_ubo_max];
};
uniform mat4 camera_matrix;
out vec3 vertex_color_frag;
out float fog_blend;

void main() {
    mat4 model_matrix = matrices[gl_InstanceID];
    float instance_brightness = model_matrix[0][3];
    model_matrix[0][3] = 0.0;
    gl_Position = camera_matrix * (model_matrix * vec4(vertex_position, 1.0));
    vertex_color_frag = vertex_color.rgb * instance_brightness * (1.0 + vertex_color.a * GLOW);
    float fog_level = gl_Position.z * gl_Position.z;
    fog_blend = zox_fog_blend(fog_level);
}
