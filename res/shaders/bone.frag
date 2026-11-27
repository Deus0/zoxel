#ifdef zox_outlines
// outlines (doc/plans/outlines.md): the mask, its outline id and its window depth, drawn only while outlined models draw
layout(location=0) out vec4 frag_color;
layout(location=1) out uvec2 frag_mask;
uniform float outline;
#else
out vec4 frag_color;
#endif
in vec3 vertex_color_frag;
in float fog_blend;
uniform float brightness;

void main() {
    frag_color.rgb = vertex_color_frag * brightness;
    // frag_color = mix(frag_color, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend);
    frag_color.rgb = mix(frag_color.rgb, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend);
    frag_color.a = 1.0;
#ifdef zox_outlines
    // its depth exactly as the depth buffer has it: the pass drops it where something nearer hides it
    frag_mask = uvec2(uint(outline * 255.0 + 0.5), floatBitsToUint(gl_FragCoord.z));
#endif
}
