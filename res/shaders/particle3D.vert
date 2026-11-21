in vec3 position;
in vec4 color;
uniform mat4 camera_matrix;
uniform float thickness;
out float fog_level;
out vec4 frag_color;

void main() {
    gl_Position = camera_matrix * vec4(position, 1.0);
    // a vox model's voxel in size: thickness is its pixels at depth 1, w the depth (in front of the near plane)
    gl_PointSize = thickness / max(gl_Position.w, 0.05);
    fog_level = gl_Position.z;
    frag_color = color;
}
