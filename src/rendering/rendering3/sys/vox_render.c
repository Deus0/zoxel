// a see through mesh's alpha without a material (its voxels' colours, blended over what's behind)
#define zox_see_through_alpha 0.55f

// a see through mesh's alpha: its material's (MaterialLink to a TransparentMaterial, its Alpha), else the default
static inline float instance_see_through_alpha(ecs* world, entity mesh) {
    const entity material = zox_get_link(world, mesh, MaterialLink);
    return zox_valid(material) && zox_has(material, TransparentMaterial) && zox_has(material, Alpha) ?
        zox_getv(material, Alpha) : zox_see_through_alpha;
}

// draws colored meshes (VoxMesh: a vox's own mesh, a colored chunk's), solid ones, or see through ones (TransparentMesh:
//  after, blended at their material's alpha, testing depth without writing it)
static inline void vox_render_meshes(iter* it, byte see_through) {
    byte dbg_log = 0;
    zox_sys_world();
    if (!zox_valid(material_colored3D)) {
        return;
    }
    guint material_link = zox_getv(material_colored3D, MaterialGPULink);
    if (!material_link) {
        return;
    }
    const MaterialColored3D* material_attributes = zox_get(material_colored3D, MaterialColored3D);
    byte has_set_material = 0;
    float last_brightness = -1;
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    camera_filtering_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, colors);
        zox_sys_i(TransformMatrix, matrix);
        // a gpu mesh (chunks3/sys/vox_gpu.c) has its count, its cpu arrays empty
        const byte gpu = zox_has(e, GpuMesh);
        const int count = gpu ? mesh_index_count(world, e) : indicies->length;
        if (disabled->value ||
            !count ||
            !mesh->value.x ||
            !mesh->value.y ||
            !colors->value)
        {
            continue;
        }
        camera_filtering_check();
        if (!has_set_material) {
            has_set_material = 1;
/*#ifdef zox_transparent_voxes
            zox_gpu_enable_blend();
            zox_gpu_disable_culling();
#endif*/
            zox_gpu_material(material_link);
            zox_gpu_float4(
                material_attributes->fog_data,
                get_fog_value());
            zox_gpu_float(material_attributes->alpha, 1);
            if (see_through) {
                zox_gpu_enable_blend();
                zox_gpu_set_depth_mask(0);
            }
        }
        if (see_through) {
            zox_gpu_float(material_attributes->alpha, instance_see_through_alpha(world, e));
        }
        // i.e. a block vox lit by its chunk, not every VoxMesh has one
        const float brightness = zox_has(e, Brightness) ?
            zox_getv(e, Brightness) :
            1;
        if (brightness != last_brightness) {
            last_brightness = brightness;
            zox_gpu_float(material_attributes->brightness, brightness);
        }
        float4x4 mvp_matrix = float4x4_multiply(
            matrix->value,
            render_camera_matrix);
        zox_gpu_float4x4(
            material_attributes->mvp_matrix,
            mvp_matrix);
        zox_gpu_bind_buffer_element(mesh->value.x);
        if (gpu) {
            // the compute shader's: vec4 vertices, rgba8 colours
            zox_gpu_array_buffer_float3_stride(material_attributes->vertex_position, mesh->value.y, sizeof(float4));
            zox_gpu_array_buffer_byte4(material_attributes->vertex_color, colors->value);
        } else {
            opengl_enable_vertex_buffer(material_attributes->vertex_position, mesh->value.y);
            opengl_enable_color_buffer(material_attributes->vertex_color, colors->value);
        }
        zox_gpu_render3(count);
        if (dbg_log) {
            zox_log("Rendering Vox Mesh [%s] - Tris [%i]",
                zox_sys_e_name,
                indicies->length / 3);
        }
    }
    if (has_set_material) {
        zox_gpu_disable_attribute(material_attributes->vertex_color);
        zox_gpu_disable_attribute(material_attributes->vertex_position);
        zox_gpu_reset_mesh();
        if (see_through) {
            zox_gpu_set_depth_mask(1);
            zox_gpu_disable_blend();
        }
        zox_disable_material();
/*#ifdef zox_transparent_voxes
        zox_gpu_disable_blend();
        zox_gpu_enable_culling();
#endif*/
    }
}

void vox_render_system(iter* it) {
    zox_sys_on_begin();
    vox_render_meshes(it, 0);
    zox_sys_on_end();
} zoxd_system(vox_render_system);

void vox_transparent_render_system(iter* it) {
    zox_sys_on_begin();
    vox_render_meshes(it, 1);
    zox_sys_on_end();
} zoxd_system(vox_transparent_render_system);
