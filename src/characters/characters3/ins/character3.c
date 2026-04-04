entity spawn_character3(ecs *world, spawn_character3D_data data) {
    entity vox = data.model;

    // if model, we use lodded for vox
    if (zox_valid(vox) && zox_has(vox, ModelLods)) {
        zox_geter(vox, ModelLods, modelLods);
        entity vox_lod = modelLods->value[data.render_depth];

        if (zox_valid(vox_lod)) {
            vox = vox_lod;
        } else {
            zox_log_error("[spawn_character3]: Invalid Vox Model Lod [%s] Depth [%i]", zox_get_name(vox), data.render_depth);
            return 0;
        }
        // zox_log("+ set model [%s] vox [%s]", zox_get_name(model), zox_get_name(vox))
    }

    zox_geter_value(data.prefab, Character3Type, byte, type);

    zox_instance(data.prefab);
    zox_name("character3");

    char *name = generate_name();
    set_ZoxName(world, e, name);
    free(name);

    zox_set(e, RealmLink, { data.realm });

    if (data.meta) {
        zox_set(e, CharacterMetaLink, { data.meta });
    }

    zox_set(e, Position3D, { data.position });
    zox_set(e, LastPosition3D, { data.position });

    if (!float4_equals(data.rotation, quaternion_identity)) {
        zox_set(e, Rotation3D, { data.rotation });
    }

    if (!float3_equals(data.euler, float3_zero)) {
        zox_set(e, Euler, { data.euler });
    }

    // rendering
    zox_set(e, RenderDepth, { data.render_depth });

    if (zox_valid(vox) && zox_has(vox, MaxRenderDepth)) {
        zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);
        zox_set(e, MaxRenderDepth, { max_render_depth });
    }

    if (!data.render_disabled) {
        zox_set(e, RenderDisabled, { data.render_disabled });
    }

    // voxels
    /*if (data.terrain) {
        zox_set(e, TerrainLink, { data.terrain });
    }*/
    if (data.terrain_chunk) {
        zox_set(e, ChunkLink, { data.terrain_chunk });
        zox_set(e, ChunkPosition, { data.chunk_position });
    }

    zox_set(e, ModelLink, { data.model });

    if (type == zox_character_type_instanced) {

        zox_set(e, InstanceLink, { vox });

        if (zox_has(vox, BlockScale)) {

            zox_geter_value(vox, BlockScale, float, meta_vox_scale);
            zox_geter_value(vox, ChunkSize, int3, meta_chunk_size);

            float3 meta_bounds = calculate_vox_bounds(meta_chunk_size, meta_vox_scale);

            zox_set(e, BlockScale, { meta_vox_scale });
            zox_set(e, Bounds3D, { meta_bounds });

        } else {
            zox_log_error("vox has no BlockScale [%s]", zox_get_name(vox));
        }

    } else {
        if (zox_valid(vox)) {
            zox_set(e, CloneVoxLink, { vox });
            zox_set(e, CloneVox, { 1 });
        } else {
              zox_set(e, DisableMovement, { 1 });
        }
    }

    // move gpu functions to new system
    if (type != zox_character_type_instanced) {
        spawn_gpu_mesh(world, e);
        spawn_gpu_colors(world, e);
    }

    if (type == zox_character_type_skeleton) {
        spawn_gpu_bone_index(world, e);
        if (is_paint_skeletons) {
            zox_add_tag(e, PaintedSkeleton);
        }
        /*if (!data.player) {
            BoneLinks bones = (BoneLinks) { 0 };
            spawn_skeleton_bones(world, e, &bones, 0.01f);
            zox_set_ptr(e, BoneLinks, bones);
        }*/
        //float head_move_y = data.player ? 0.01f : 0.1f;
        //spawn_skeleton_bones(world, e, head_move_y);
    }

    return e;
}
