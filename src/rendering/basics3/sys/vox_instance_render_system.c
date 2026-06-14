// todo:
//   create vbo for instance stuff like particles
//   create instanced mesh prefab and spawn for grass - just has to have a tag, transform data, and point to a mesh to render
// atm renders about 2k per frame
// when registering a new instance entity:
// create a renderer data:
//      - material
//      - ubo link
//      - transform list
// extern string_hashmap *files_hashmap_voxes;

// Renders Characters and Mimuvoxes
zox_sys2(VoxInstanceRenderSystem) {
#ifndef zox_disable_ubos
    if (!material_vox_instance) {
        return;
    }
    uint max_meshes = 16;
    uint max_transforms = 2048;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(InstanceLink);
    zox_sys_in(RenderDisabled);
    camera_filtering_begin();
    zox_geter_value(material_vox_instance, MaterialGPULink, uint, material_link);
    if (!material_link) {
        return;
    }
    zox_geter(material_vox_instance, UboGPULink, ubo);
    if (!ubo->value) {
        // zox_log(" ! error with material_vox_instance uboGPULink.\n")
        return;
    }
    zox_geter(material_vox_instance, MaterialVoxInstance, material_attributes);
    InstanceRenderCommand_array_d* commands = create_InstanceRenderCommand_array_d(max_meshes);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(InstanceLink, instance);
        zox_sys_i(TransformMatrix, matrix);
        if (!zox_valid(instance->value) || disabled->value) {
            continue;
        }
        camera_filtering_check();
        int index = 0;
        if (has_mesh(commands, instance->value, &index)) {
            InstanceRenderCommand command = commands->data[index];
            add_to_float4x4_array_d(command.transforms, matrix->value);
            commands->data[index] = command;
        } else {
            InstanceRenderCommand command = {
                .mesh = instance->value,
                .transforms = create_float4x4_array_d(max_transforms)
            };
            if (!command.transforms) {
                // errored out
                break;
            }
            add_to_float4x4_array_d(command.transforms, matrix->value);
            add_to_InstanceRenderCommand_array_d(commands, command);
        }
    }
    zox_gpu_material(material_link);
    zox_gpu_float4x4(material_attributes->camera_matrix, render_camera_matrix);
    zox_gpu_float4(material_attributes->fog_data, get_fog_value());
    zox_gpu_float(material_attributes->brightness, 1);
    for (size_t i = 0; i < commands->size; i++) {
        InstanceRenderCommand command = commands->data[i];
        if (!command.transforms) {
            continue;
        }
        entity mesh = command.mesh;
        if (!zox_valid(mesh)){
            continue;
        }
        if (!zox_has(mesh, MeshIndicies)) {
            zox_loge("Invalid Instance Mesh [%s]: No MeshIndicies", zox_get_name(mesh));
            continue;
        }
        // Ignore if no indicies
        zox_geter(mesh, MeshIndicies, meshIndicies);
        if (!meshIndicies->length) {
            continue;
        }
        if (!zox_has(mesh, MeshGPULink)) {
            zox_loge("Invalid Instance Mesh [%s]: No MeshGPULink", zox_get_name(mesh));
            continue;
        }
        zox_geter(mesh, MeshGPULink, gpumesh);
        if (!gpumesh->value.x || !gpumesh->value.y) {
            zox_loge("Invalid Instance Mesh [%s]: GPU Mesh is 0.", zox_get_name(mesh));
            continue;
        }
        zox_geter(mesh, ColorsGPULink, gpucolors);
        if (!gpucolors->value) {
            zox_loge("Invalid Instance Mesh [%s]: GPU Color is 0.", zox_get_name(mesh));
            continue;
        }
        zox_gpu_bind_buffer_element(gpumesh->value.x);
        opengl_enable_vertex_buffer(material_attributes->vertex_position, gpumesh->value.y);
        opengl_enable_color_buffer(material_attributes->vertex_color, gpucolors->value);
        int render_count = command.transforms->size;
        zox_gpu_ubo_set_matricies(ubo->value, render_count, command.transforms->data);
        zox_gpu_render_triangles_instanced(meshIndicies->length, render_count);
        zox_gpu_ubo_reset();
        zox_gpu_disable_attribute(material_attributes->vertex_color);
        zox_gpu_disable_attribute(material_attributes->vertex_position);
        zox_gpu_reset_mesh();
    }
    // cleanup
    for (size_t i = 0; i < commands->size; i++) {
        InstanceRenderCommand command = commands->data[i];
        dispose_float4x4_array_d(command.transforms);
    }
    dispose_InstanceRenderCommand_array_d(commands);
    zox_disable_material();
    catch_basic3D_errors("VoxInstanceRenderSystem");
#endif
} zox_sys_end(VoxInstanceRenderSystem);
