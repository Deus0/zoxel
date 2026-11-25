out vec4 frag_color;
in vec3 vertex_color_frag;
in float fog_blend;
uniform vec4 fog_data;
uniform float brightness;
uniform float alpha;    // 1 drawn solid, less for a see through mesh (drawn after, blended)

void main() {
    vec3 color = vertex_color_frag * brightness;
    frag_color = vec4(mix(color, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend), alpha);
}
