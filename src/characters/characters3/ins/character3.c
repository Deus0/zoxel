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
    /*if (!float3_equals(data.euler, float3_zero)) {
        zox_set(e, Euler, { data.euler });
    }*/

    // rendering
    zox_set(e, RenderDepth, { data.render_depth });
    zox_set(e, RenderDisabled, { data.render_disabled });

    if (zox_valid(vox) && zox_has(vox, MaxRenderDepth)) {
        zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);
        zox_set(e, MaxRenderDepth, { max_render_depth });
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
    }

    return e;
}

// , entity tchunk, int3 cposition

entity spawn_character3_new(ecs *world, entity prefab, entity meta, entity realm, entity terrain, byte render_depth, float3 position, float4 rotation, const char* name) {
    if (!zox_valid(meta)) {
        zox_loge("[spawn_character3_new] Invalid Meta");
        return 0;
    }

    zox_geter_value(meta, ModelLink, entity, model);
    if (!zox_valid(model)) {
        zox_log_error("Meta Model Invalid [%s]", zox_get_name(meta));
        return 0;
    }

    // If a ModelGroup, pick a random sub model
    if (zox_has(model, ModelLinks)) {
        zox_geter(model, ModelLinks, models)
        if (models->length) {
            uint mindex = rand() % models->length;
            model = models->value[mindex];
        }
    }

    if (!zox_has(model, MaxRenderDepth)) {
        zox_log_error("Model Invalid [%s]", zox_get_name(model));
        return 0;
    }

    zox_geter_value(model, MaxRenderDepth, byte, max_render_depth);
    if (render_depth > max_render_depth) {
        render_depth = max_render_depth;
    }

    // If Model, get the Vox (its lodded)
    if (zox_has(model, ModelLods)) {
        zox_geter(model, ModelLods, modelLods);
        entity vox_lod = modelLods->value[render_depth];
        if (zox_valid(vox_lod)) {
            model = vox_lod;
        } else {
            zox_log_error("[spawn_character3]: Invalid Vox Model Lod [%s] Depth [%i]", zox_get_name(model), render_depth);
            return 0;
        }
    }

    zox_geter_value(prefab, Character3Type, byte, type);
    if (type == zox_character_type_instanced) {
        if (!zox_has(model, BlockScale)) {
            zox_log_error("Model has no BlockScale [%s]", zox_get_name(model));
            return 0;
        }
    }

    zox_log("Spawning Character [%s] with Model [%s] of render type [%i]", name, zox_get_name(model), type);

    zox_instance(prefab);
    zox_name("character3");
    set_ZoxName(world, e, name);
    zox_set(e, CharacterMetaLink, { meta });
    zox_set(e, RealmLink, { realm });
    zox_set(e, TerrainLink, { terrain });
    zox_set(e, Position3D, { position });
    zox_set(e, LastPosition3D, { position });
    if (!float4_equals(rotation, quaternion_identity)) {
        zox_set(e, Rotation3D, { rotation });
    }
    // this should just be found automatically? using terrain link?
    zox_set(e, RenderDepth, { render_depth });
    zox_set(e, MaxRenderDepth, { max_render_depth });
    zox_set(e, RenderDisabled, { 0 });
    zox_set(e, ModelLink, { model });

    if (type == zox_character_type_instanced) {
        zox_geter_value(model, BlockScale, float, bscale);
        zox_geter_value(model, ChunkSize, int3, csize);
        float3 bounds = calculate_vox_bounds(csize, bscale);
        zox_set(e, InstanceLink, { model });
        zox_set(e, BlockScale, { bscale });
        zox_set(e, Bounds3D, { bounds });
    } else {
        // Unique Meshes
        zox_log("   - Unique Mesh Spawning");
        zox_set(e, CloneVoxLink, { model });
        zox_set(e, CloneVox, { 1 });
        spawn_gpu_mesh(world, e);
        spawn_gpu_colors(world, e);
        // For Skeletons
        if (type == zox_character_type_skeleton) {
            spawn_gpu_bone_index(world, e);
            if (is_paint_skeletons) {
                zox_add_tag(e, PaintedSkeleton);
            }
        }
    }

    return e;
}
