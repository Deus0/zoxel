zox_sys2(Skeleton3RenderSystem) {
    entity rmaterial = transparent_meshes ? material_bonet : material_bone;
    if (!rmaterial) {
        return;
    }
    byte has_set_material = 0;
    zox_sys_world();
    zox_geter_value(rmaterial, MaterialGPULink, uint, material_link);
    zox_geter(rmaterial, MaterialBone, material_attributes);
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(BoneIndexGPULink);
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    zox_sys_in(BoneLinks);
    camera_filtering_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, colors);
        zox_sys_i(BoneIndexGPULink, bone_indicies);
        zox_sys_i(BoneLinks, bones);
        zox_sys_i(TransformMatrix, matrix);
        if (disabled->value || !indicies->length || !mesh->value.x || !mesh->value.y || !colors->value || !bone_indicies->value || !bones->length) {
            continue;
        }
        camera_filtering_check();
        if (!has_set_material) {
            has_set_material = 1;
            if (transparent_meshes) {
                zox_gpu_enable_blend();
                zox_gpu_disable_culling();
            }
            zox_gpu_material(material_link);
            zox_gpu_float4x4(material_attributes->camera_matrix, render_camera_matrix);
            zox_gpu_float4(material_attributes->fog_data, get_fog_value());
            zox_gpu_float(material_attributes->brightness, 1);
        }
        // get local bone matrix, local to transform
        // Cache bone data to send to GPU
        float4x4 inverse = float4x4_inverse(matrix->value);
        float4x4 transforms[bones->length];
        float3 bone_positions[bones->length];
        for (int j = 0; j < bones->length; j++) {
            entity bone = bones->value[j];
#ifdef zox_safety_checks
            if (!zox_valid(bone)) {
                zox_loge("Bone at [%i] is invalid on Skeleton [%s]", j, zox_get_name(e));
                continue;
            }
#endif
            // Idk whats going on here
            if (!zox_has(bone, TransformMatrix)) {
                // zox_loge("Bone at [%s] has no TransformMatrix on Skeleton [%s]", zox_getn(bone), zox_getn(e));
                continue;
            }
            float4x4 bone_matrix = zox_getv(bone, TransformMatrix);
            transforms[j] = float4x4_multiply(bone_matrix, inverse);
            bone_positions[j] = zox_get_value(bone, BonePosition);
        }
        zox_gpu_float4x4_array(material_attributes->bone_matrix, transforms, bones->length);
        zox_gpu_float3_array(material_attributes->bone_positions, bone_positions, bones->length);
        zox_gpu_array_buffer_byte(material_attributes->bone_index, bone_indicies->value);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(material_attributes->vertex_position, mesh->value.y);
        opengl_enable_color_buffer(material_attributes->vertex_color, colors->value);
        zox_gpu_float4x4(material_attributes->transform_matrix, matrix->value);
        zox_gpu_render(indicies->length);
    }
    if (has_set_material) {
        zox_gpu_disable_attribute(material_attributes->vertex_color);
        zox_gpu_disable_attribute(material_attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_disable_material();
        if (transparent_meshes) {
            zox_gpu_disable_blend();
            zox_gpu_enable_culling();
        }
    }
} zox_sys_end(Skeleton3RenderSystem);
