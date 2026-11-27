in vec3 position;
in vec4 color;
in float size;
in float brightness;   // past 1 past white: it glows (blooms, doc/plans/hdr.md)
uniform mat4 camera_matrix;
uniform float thickness;
out vec4 frag_color;

void main() {
    gl_Position = camera_matrix * vec4(position, 1.0);
    // size vox model voxels: thickness is a voxel's pixels at depth 1, w the depth (in front of the near plane)
    gl_PointSize = size * thickness / max(gl_Position.w, 0.05);
    frag_color = vec4(color.rgb * brightness, color.a);
}
