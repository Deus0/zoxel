// Also supports no models, for skeleton body gen
entity spawn_character3(ecs* world, entity prefab, entity realm, entity terrain, lint seed, byte render_distance, byte render_disabled, float3 position, float4 rotation, const char* name) {
    byte dbg_log = 0;
    // If Model, get the Vox (its lodded)
    byte max_depth = 0;
    entity model = zox_getv(prefab, ModelLink);
    entity vox = model;
    if (zox_valid(model)) {
        // If a ModelGroup, pick a random sub model
        if (zox_has(model, ModelLinks)) {
            zox_geter(model, ModelLinks, models)
            if (models->length) {
                uint mindex = rand() % models->length;
                model = models->value[mindex];
            }
        }
        if (zox_has(model, MaxRenderDepth)) {
            max_depth = zox_getv(model, MaxRenderDepth);
            /*if (render_depth > max_depth) {
                render_depth = max_depth;
            }*/
        }
    }
    byte render_depth = camera_distance_to_npc_render_depth(render_distance, max_depth);
    if (zox_valid(model)) {
        if (zox_has(model, ModelLods)) {
            zox_geter(model, ModelLods, lods);
            vox = lods->value[render_depth];
        } else {
            vox = model;
        }
    }
    // zox_log("Spawning Character [%s] with Model [%s] of render type [%i]", name, zox_get_name(model), type);
    if (dbg_log) {
        zox_log("Character [%s] At [%fx%fx%f]", zox_get_name(prefab), position.x, position.y, position.z);
        if (zox_valid(model)) {
            zox_log("Model [%s]", zox_get_name(model));
        } else {
            zox_log("No Model");
        }
        if (zox_valid(vox)) {
            zox_log("Vox [%s]", zox_get_name(vox));
        } else {
            zox_log(" No Vox");
        }
    }
    zox_instance(prefab);
    zox_remove_tag(e, RealmCharacter);
    zox_set_parent(world, e, realm);
    zox_set(e, RealmLink, { realm });
    zox_set(e, TerrainLink, { terrain });
    zox_name("character3");
    if (name) {
        set_ZoxName(world, e, name);
    }
    zox_set(e, Seed, { seed });
    zox_set(e, Position3D, { position });
    zox_set(e, Rotation3D, { rotation });
    zox_set(e, LastPosition3D, { position });
    zox_set(e, RenderDisabled, { render_disabled });
    if (zox_valid(model)) {
        zox_set(e, ModelLink, { model });
        zox_set(e, RenderDepth, { render_depth });
        zox_set(e, MaxRenderDepth, { max_depth });
        // zox_log("Render Depth of character set to [%i] of [%i]", render_depth, max_depth);
    }
    if (zox_has(prefab, CharacterGeneric)) {
        if (zox_valid(vox)) {
            zox_set(e, InstanceLink, { vox });
        }
        if (zox_valid(vox) && zox_has(vox, BlockScale)) {
            zox_geter_value(vox, BlockScale, float, bscale);
            zox_geter_value(vox, ChunkSize, int3, csize);
            float3 bounds = calculate_vox_bounds(csize, bscale);
            zox_set(e, BlockScale, { bscale });
            zox_set(e, Bounds3D, { bounds });
            if (dbg_log) {
                zox_log("Bounds Generated [%fx%fx%f]", bounds.x, bounds.y, bounds.z);
            }
        } else if (zox_valid(vox)) {
            zox_loge("Model has no BlockScale [%s]", zox_get_name(vox));
        } /*else {
            zox_loge("Generic Character has no Vox [%s]", zox_get_name(model));
        }*/
    } else if (zox_has(prefab, CharacterUnique)) {
        if (zox_valid(vox)) {
            zox_set(e, CloneVoxLink, { vox });
            zox_set(e, CloneVox, { 1 });
        }
        if (dbg_log) {
            zox_log("Unique Character");
        }
    }
    return e;
}
