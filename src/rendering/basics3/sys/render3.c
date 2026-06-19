zox_sys2(Basic3RenderSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(Color);
    zox_sys_in(Alpha);
    zox_sys_in(Brightness);
    zox_sys_in(MeshGPULink);
    zox_sys_in(MeshIndicies);
    zox_sys_in(MaterialGPULink);
    camera_filtering_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, transformMatrix);
        zox_sys_i(Color, fill);
        zox_sys_i(Alpha, alpha);
        zox_sys_i(Brightness, brightness);
        zox_sys_i(MeshGPULink, meshGPULink);
        zox_sys_i(MeshIndicies, meshIndicies);
        zox_sys_i(MaterialGPULink, materialGPULink);
        if (!meshIndicies->length || !meshGPULink->value.x || !meshGPULink->value.y || !materialGPULink->value) {
            continue;
        }
        camera_filtering_check();
        if (alpha->value < 1) {
            zox_gpu_enable_blend();
        }
        MaterialBasic3D attributes = create_MaterialBasic3D(materialGPULink->value);
        zox_gpu_material(materialGPULink->value);
        zox_gpu_float4x4(attributes.camera_matrix, render_camera_matrix);
        zox_gpu_float4(attributes.fog_data, get_fog_value());
        float4 colorf = color_to_float4(fill->value);
        colorf.x *= brightness->value;
        colorf.y *= brightness->value;
        colorf.z *= brightness->value;
        colorf.w *= alpha->value;
        zox_gpu_float4(attributes.color, colorf);
        // zox_gpu_float(attributes.brightness, brightness->value);
        zox_gpu_float4x4(attributes.transform_matrix, transformMatrix->value);
        zox_gpu_bind_buffer_element(meshGPULink->value.x);
        opengl_enable_vertex_buffer(attributes.vertex_position, meshGPULink->value.y);
        zox_gpu_render(meshIndicies->length);
        if (dbg_log) {
            zox_log("Rendering Basic3D Mesh [%s] [%i]: %s, Brightness [%f], Alpha [%f]", zox_get_name(e), meshIndicies->length, alpha->value < 1 ? "Transparent" : "Opaque", brightness->value, alpha->value);
        }
        zox_gpu_disable_attribute(attributes.vertex_position);
        zox_gpu_reset_mesh();
        zox_disable_material();
        if (alpha->value < 1) {
            zox_gpu_disable_blend();
        }
    }
} zox_sys_end(Basic3RenderSystem);
