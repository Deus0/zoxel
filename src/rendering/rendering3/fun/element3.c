// drawing 3D ui elements (textured quads), canvas by canvas (ui/elements3/sys/render.c): depth tested against the
//  world, their layers in order, see doc/tech/uis.md

float element3_brightness = 1;   // the 3D uis' brightness, all of them (a label fading in from the dark: XR loading)

// starts drawing them: the textured material, blending, depth tested; returns its attributes, NULL without it
const MaterialTextured3D* element3_draw_begin(ecs* world) {
    if (!material_textured3D) {
        return NULL;
    }
    const MaterialTextured3D* attributes = zox_get(material_textured3D, MaterialTextured3D);
    zox_gpu_enable_blend();
    zox_gpu_enable_depth_test();
    zox_gpu_material(zox_getv(material_textured3D, MaterialGPULink));
    zox_gpu_float4(attributes->fog_data, get_fog_value());
    zox_gpu_float(attributes->brightness, element3_brightness);
    zox_gpu_float(attributes->alpha_cutoff, 0);
    return attributes;
}

// a billboard turned from the camera it faces (billboard_system, the closest) to the one drawing it (split screen):
//  its world back into its frame, then out with this camera's rotation; 0 if it faces this one already
static inline byte element3_billboard_turn(ecs* world, entity canvas, float4x4* turn) {
    if (!zox_has(canvas, ElementBillboard) || !zox_valid(renderer_camera) || !zox_has(renderer_camera, Rotation3D)) {
        return 0;
    }
    const float4 built = zox_getv(canvas, Rotation3D);
    const float4 camera = quaternion_without_roll(zox_getv(renderer_camera, Rotation3D));
    if (float4_equals(built, camera)) {
        return 0;
    }
    const float3 position = zox_getv(canvas, Position3D);
    *turn = float4x4_multiply(float4x4_inverse(float4x4_transform(position, built)), float4x4_transform(position, camera));
    return 1;
}

// one element, if it's shown and on the gpu; turn: a billboard's, to face this camera (NULL none)
void element3_draw(ecs* world, entity e, const MaterialTextured3D* attributes, const float4x4* turn) {
    if (zox_getv(e, RenderDisabled)) {
        return;
    }
    const MeshGPULink* mesh_link = zox_get(e, MeshGPULink);
    const uint mesh_x = mesh_link->value.x, mesh_y = mesh_link->value.y;
    const uint uvs = zox_getv(e, UvsGPULink);
    const uint colors = zox_getv(e, ColorsGPULink);
    if (!mesh_x || !mesh_y || !uvs || !colors) {
        return;
    }
    float4x4 model = zox_getv(e, TransformMatrix);
    if (turn) {
        model = float4x4_multiply(model, *turn);
    }
    zox_gpu_float4x4(attributes->mvp_matrix, float4x4_multiply(model, render_camera_matrix));
    // its own brightness (a selected icon brighter: SelectedBrighter, as a 2D one's) on all of theirs
    const float brightness = zox_has(e, Brightness) ? zox_getv(e, Brightness) : 1;
    zox_gpu_float(attributes->brightness, element3_brightness * brightness);
    zox_gpu_bind_buffer_element(mesh_x);
    opengl_enable_vertex_buffer(attributes->vertex_position, mesh_y);
    opengl_enable_uv_buffer(attributes->vertex_uv, uvs);
    opengl_enable_color_buffer(attributes->vertex_color, colors);
    zox_gpu_bind_texture(zox_getv(e, TextureGPULink));
    zox_gpu_render3(6);
}

// layers painting over each other (depth writes off), or only depth (colour off, its solid parts: alpha over half)
void element3_draw_pass(const MaterialTextured3D* attributes, byte depth_only) {
    zox_gpu_set_depth_mask(depth_only);
    zox_gpu_set_color_mask(!depth_only);
    zox_gpu_float(attributes->alpha_cutoff, depth_only ? 0.5f : 0);
}

void element3_draw_end(const MaterialTextured3D* attributes) {
    zox_gpu_set_depth_mask(1);
    zox_gpu_set_color_mask(1);
    zox_gpu_float(attributes->alpha_cutoff, 0);
    zox_gpu_disable_blend();
    zox_gpu_disable_attribute(attributes->vertex_color);
    zox_gpu_disable_attribute(attributes->vertex_uv);
    zox_gpu_disable_attribute(attributes->vertex_position);
    zox_gpu_reset_texture();
    zox_gpu_reset_mesh();
    zox_disable_material();
}
