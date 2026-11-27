#ifdef zox_outlines
// outlines (doc/plans/outlines.md): the mask, its outline id and its window depth, drawn only while outlined models draw
layout(location=0) out vec4 frag_color;
layout(location=1) out uvec2 frag_mask;
uniform float outline;
#else
out vec4 frag_color;
#endif
in vec3 vertex_color_frag;
uniform float brightness;
uniform float alpha;    // 1 drawn solid, less for a see through mesh (drawn after, blended)

void main() {
    vec3 color = vertex_color_frag * brightness;
    frag_color = vec4(zox_fog(color), alpha);
#ifdef zox_outlines
    // its depth exactly as the depth buffer has it: the pass drops it where something nearer hides it
    frag_mask = uvec2(uint(outline * 255.0 + 0.5), floatBitsToUint(gl_FragCoord.z));
#endif
}
