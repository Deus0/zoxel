// Also supports no models, for skeleton body gen
entity spawn_character3(ecs* world, entity prefab, entity realm, entity terrain, entity model, byte render_depth, byte render_disabled, float3 position, float4 rotation, const char* name) {
    // If Model, get the Vox (its lodded)
    entity vox = 0;
    byte mdepth = render_depth;
    if (zox_valid(model)) {
        // If a ModelGroup, pick a random sub model
        if (zox_has(model, ModelLinks)) {
            zox_geter(model, ModelLinks, models)
            if (models->length) {
                uint mindex = rand() % models->length;
                model = models->value[mindex];
            }
        }
        if (zox_has(model, ModelLods)) {
            zox_geter(model, ModelLods, modelLods);
            vox = modelLods->value[render_depth];
        } else {
            vox = model;
        }
        if (zox_has(model, MaxRenderDepth)) {
            mdepth = zox_gett_value(model, MaxRenderDepth);
        }
    }
    if (render_depth > mdepth) {
        render_depth = mdepth;
    }
    // zox_log("Spawning Character [%s] with Model [%s] of render type [%i]", name, zox_get_name(model), type);
    zox_instance(prefab);
    zox_name("character3");
    set_ZoxName(world, e, name);
    zox_set(e, Position3D, { position });
    zox_set(e, Rotation3D, { rotation });
    zox_set(e, LastPosition3D, { position });
    zox_set(e, RenderDisabled, { render_disabled });
    zox_set(e, RealmLink, { realm });
    zox_set(e, TerrainLink, { terrain });
    if (zox_valid(model)) {
        zox_set(e, ModelLink, { model });
        zox_set(e, RenderDepth, { render_depth });
        zox_set(e, MaxRenderDepth, { mdepth });
    }
    zox_geter_value(prefab, Character3Type, byte, rtype);
    if (rtype == zox_character_type_instanced) {
        if (zox_valid(vox) && zox_has(vox, BlockScale)) {
            zox_geter_value(vox, BlockScale, float, bscale);
            zox_geter_value(vox, ChunkSize, int3, csize);
            float3 bounds = calculate_vox_bounds(csize, bscale);
            zox_set(e, InstanceLink, { vox });
            zox_set(e, BlockScale, { bscale });
            zox_set(e, Bounds3D, { bounds });
        } else if (zox_valid(vox)) {
            zox_log_error("Model has no BlockScale [%s]", zox_get_name(vox));
        }
    } else {
        // Unique Meshes
        if (zox_valid(vox)) {
            zox_set(e, CloneVoxLink, { vox });
            zox_set(e, CloneVox, { 1 });
        }
        spawn_gpu_mesh(world, e);
        spawn_gpu_colors(world, e);
    }
    return e;
}
