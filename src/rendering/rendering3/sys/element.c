// a 3D ui's layers (its frame, a bar's fill, its text) each nearer the camera than the one under it, in the world:
//  element3_layer_offset, or element3_layer_offset_far of its distance when further (depth precision falls off with
//  distance, fixed layers fought far off); a depth offset (glPolygonOffset, 5000 a layer) pulled far uis through walls
const float element3_layer_offset = 0.01f;
const float element3_layer_offset_far = 0.002f;

// NOTE: Renders our 3D uis, depth tested like the world, each layer nudged toward the camera
void element3_render_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    byte dbg_gl = 0;
    entity material = material_textured3D;
    if (!material) {
        // zox_loge("[material_textured3D] missing in element3_render_system.");
        return;
    }
    zox_sys_world();
    byte has_set_material = 0;
    guint material_link = 0;
    const MaterialTextured3D* attributes = NULL;
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(Layer);
    zox_sys_in(TransformMatrix);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(Layer, layer);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(ColorsGPULink, colors);
        zox_sys_i(TextureGPULink, texture);
        if (disabled->value) {
            continue;
        }
// #ifdef zox_safety_checks
        if (!mesh->value.x || !mesh->value.y) {
            // zox_loge("Gpu links [mesh] broken on Element3 [%s]", zox_getn(e));
            continue;
        }
        if (!uvs->value) {
            // zox_loge("Gpu links [uvs] broken on Element3 [%s]", zox_getn(e));
            continue;
        }
        if (!colors->value) {
            // zox_loge("Gpu links [colors] broken on Element3 [%s]", zox_getn(e));
            continue;
        }
// #endif
        if (!has_set_material) {
            has_set_material = 1;
            material_link = zox_getv(material, MaterialGPULink);
            attributes = zox_get(material, MaterialTextured3D);
            zox_gpu_enable_blend();
            // depth tested and written like the world (left from other passes, a layer could draw over the one above it)
            zox_gpu_enable_depth_test();
            zox_gpu_set_depth_mask(1);
            zox_gpu_material(material_link);
            //zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
            zox_gpu_float4(attributes->fog_data, get_fog_value());
            zox_gpu_float(attributes->brightness, 1);
            if (dbg_log >= 2) {
                entity shader = zox_get_link(world, material, ShaderLink);
                zox_log("Rendering Element3 Material [%s]: %i - Shader [%s]", zox_getn(material), material_link, zox_getn(shader));
            }
        }
        // its layer, nudged toward the camera
        float4x4 model = matrix->value;
        const float3 at = matrix_to_position(model);
        const float3 to_camera = float3_subtract(render_camera_position, at);
        const float distance = float3_length(to_camera);
        const float nudge = fmaxf(element3_layer_offset, element3_layer_offset_far * distance) * layer->value / fmaxf(distance, 0.0001f);
        model.w.x += to_camera.x * nudge;
        model.w.y += to_camera.y * nudge;
        model.w.z += to_camera.z * nudge;
        float4x4 mvp_matrix = float4x4_multiply(
            model,
            render_camera_matrix);
        zox_gpu_float4x4(
            attributes->mvp_matrix,
            mvp_matrix);
        //zox_gpu_float4x4(attributes->transform_matrix, matrix->value);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(attributes->vertex_position, mesh->value.y);
        opengl_enable_uv_buffer(attributes->vertex_uv, uvs->value);
        opengl_enable_color_buffer(attributes->vertex_color, colors->value);
        zox_gpu_bind_texture(texture->value);
        zox_gpu_render3(6);
        if (dbg_gl) {
            if (check_opengl_error_unlogged()) {
                zox_loge("element3_render_system");
            }
        }
        if (dbg_log) {
            float3 position = matrix_to_position(matrix->value);
            float4 rotation = matrix_to_rotation(matrix->value);
            float3 scale = matrix_to_scale(matrix->value);
            zox_log("Rendered Element3 [%s]", zox_getn(e));
            zox_log(" - Layer [%i]", layer->value);
            zox_log(" - P [%fx%fx%f]", position.x, position.y, position.z);
            zox_log(" - R [%fx%fx%fx%f]", rotation.x, rotation.y, rotation.z, rotation.w);
            zox_log(" - S [%fx%fx%f]", scale.x, scale.y, scale.z);
            zox_log(" - GPU: Mesh [%ix%i] UVs [%i] Colors [%i] Texture [%i]", mesh->value.x, mesh->value.y, uvs->value, colors->value, texture->value);
        }
    }
    if (has_set_material) {
        zox_gpu_disable_blend();
        zox_gpu_disable_attribute(attributes->vertex_color);
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_texture();
        zox_gpu_reset_mesh();
        zox_disable_material();
    }
    zox_sys_on_end();
} zoxd_system(element3_render_system);
