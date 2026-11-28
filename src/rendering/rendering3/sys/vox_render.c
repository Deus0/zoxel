// a see through mesh's alpha without a material (its voxels' colours, blended over what's behind)
#define zox_see_through_alpha 0.55f

// a see through mesh's alpha: its material's (MaterialLink to a TransparentMaterial, its Alpha), else the default
static inline float instance_see_through_alpha(ecs* world, entity mesh) {
    const entity material = zox_get_link(world, mesh, MaterialLink);
    return zox_valid(material) && zox_has(material, TransparentMaterial) && zox_has(material, Alpha) ?
        zox_getv(material, Alpha) : zox_see_through_alpha;
}

#define vox_render_materials_max 4

// the material a mesh draws with: its MaterialLink's when that's a lit one (colored3D_lit), else colored3D (also when it
//  links a data material, a slime's gel's TransparentMaterial)
static inline entity vox_render_material(ecs* world, entity e) {
    const entity link = zox_get_link(world, e, MaterialLink);
    return zox_valid(link) && zox_has(link, MaterialColoredLit3D) ? link : material_colored3D;
}

// binds a material for a group of meshes: its colored3D uniforms, the lit ones if it has them; 0 if it can't draw
static inline const MaterialColored3D* vox_render_bind(ecs* world, entity material, byte see_through) {
    const guint gpu = zox_valid(material) ? zox_getv(material, MaterialGPULink) : 0;
    if (!gpu) {
        return NULL;
    }
    const MaterialColoredLit3D* lit = zox_has(material, MaterialColoredLit3D) ? zox_get(material, MaterialColoredLit3D) : NULL;
    const MaterialColored3D* attributes = lit ? &lit->colored : zox_get(material, MaterialColored3D);
    zox_gpu_material(gpu);
    zox_gpu_float(attributes->alpha, 1);
    if (lit) {
        zox_gpu_float3(lit->sun_direction, float3_normalize(render_sun_direction));
        zox_gpu_float3(lit->sun_light, render_sun_light);
        zox_gpu_float3(lit->ambient_light, render_ambient_light);
    }
    outline_mask_begin(attributes->outline);
    if (see_through) {
        zox_gpu_enable_blend();
        zox_gpu_set_depth_mask(0);
    }
    return attributes;
}

static inline void vox_render_unbind(const MaterialColored3D* attributes, byte see_through) {
    zox_gpu_disable_attribute(attributes->vertex_color);
    zox_gpu_disable_attribute(attributes->vertex_position);
    zox_gpu_reset_mesh();
    outline_mask_end();
    if (see_through) {
        zox_gpu_set_depth_mask(1);
        zox_gpu_disable_blend();
    }
    zox_disable_material();
}

// draws colored meshes (VoxMesh: a vox's own mesh, a colored chunk's), solid ones, or see through ones (TransparentMesh:
//  after, blended at their material's alpha, testing depth without writing it)
//  - grouped by the material they draw with (vox_render_material), then each group drawn with its material bound once
static inline void vox_render_meshes(iter* it, byte see_through) {
    zox_sys_world();
    if (!zox_valid(material_colored3D)) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    entity materials[vox_render_materials_max];
    byte materials_count = 0;
    byte groups[it->count];     // each row's material, or 255 not drawn
    int counts[it->count];
    camera_filtering_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, colors);
        groups[i] = 255;
        // a gpu mesh (chunks3/sys/vox_gpu.c) has its count, its cpu arrays empty
        counts[i] = zox_has(e, GpuMesh) ? mesh_index_count(world, e) : indicies->length;
        if (disabled->value || !counts[i] || !mesh->value.x || !mesh->value.y || !colors->value) {
            continue;
        }
        camera_filtering_check();
        const entity material = vox_render_material(world, e);
        byte g = 0;
        while (g < materials_count && materials[g] != material) {
            g++;
        }
        if (g == materials_count) {
            if (materials_count == vox_render_materials_max) {
                continue;
            }
            materials[materials_count++] = material;
        }
        groups[i] = g;
    }
    for (byte g = 0; g < materials_count; g++) {
        const MaterialColored3D* attributes = vox_render_bind(world, materials[g], see_through);
        if (!attributes) {
            continue;
        }
        const MaterialColoredLit3D* lit = zox_has(materials[g], MaterialColoredLit3D) ? zox_get(materials[g], MaterialColoredLit3D) : NULL;
        float last_brightness = -1, last_block = -1;
        for (int i = 0; i < it->count; i++) {
            if (groups[i] != g) {
                continue;
            }
            zox_sys_e();
            zox_sys_i(MeshGPULink, mesh);
            zox_sys_i(ColorsGPULink, colors);
            zox_sys_i(TransformMatrix, matrix);
            if (see_through) {
                zox_gpu_float(attributes->alpha, instance_see_through_alpha(world, e));
            }
            // i.e. a block vox lit by its chunk, not every VoxMesh has one
            const float brightness = zox_has(e, Brightness) ? zox_getv(e, Brightness) : 1;
            if (brightness != last_brightness) {
                last_brightness = brightness;
                zox_gpu_float(attributes->brightness, brightness);
            }
            const float block = zox_has(e, BlockBrightness) ? zox_getv(e, BlockBrightness) : 0;    // blocks' light on it
            if (block != last_block) {
                last_block = block;
                zox_gpu_float(attributes->block_brightness, block);
            }
            zox_gpu_float4x4(attributes->mvp_matrix, float4x4_multiply(matrix->value, render_camera_matrix));
            if (lit) {
                zox_gpu_float4x4(lit->model_matrix, matrix->value);
            }
            zox_gpu_bind_buffer_element(mesh->value.x);
            if (zox_has(e, GpuMesh)) {
                // the compute shader's: vec4 vertices, rgba8 colours
                zox_gpu_array_buffer_float3_stride(attributes->vertex_position, mesh->value.y, sizeof(float4));
                zox_gpu_array_buffer_byte4(attributes->vertex_color, colors->value);
            } else {
                opengl_enable_vertex_buffer(attributes->vertex_position, mesh->value.y);
                opengl_enable_color_buffer(attributes->vertex_color, colors->value);
            }
            zox_gpu_render3(counts[i]);
        }
        vox_render_unbind(attributes, see_through);
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
