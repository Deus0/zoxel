// Also supports no models, for skeleton body gen
entity spawn_character3(
    ecs* world,
    entity prefab,
    entity realm,
    entity terrain,
    lint seed,
    byte render_distance,
    byte render_disabled,
    float3 position,
    float4 rotation,
    const char* name)
{
    byte dbg_log = 0;
    // If Model, get the Vox (its lodded)
    byte max_depth = 0;
    entity model = zox_getv(prefab, ModelLink);
    entity vox = model;
    if (zox_valid(model)) {
        // If a ModelGroup, pick a random sub model
        if (zox_has(model, ModelLinks)) {
            zox_geter(model, ModelLinks, models);
            if (models->length) {
                uint mindex = rand() % models->length;
                model = models->value[mindex];
            }
        }
        if (zox_has(model, MaxRenderDepth)) {
            max_depth = zox_getv(model, MaxRenderDepth);
        }
    }
    byte render_depth = camera_distance_to_npc_render_depth(
        render_distance,
        max_depth);
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
        zox_log("Character [%s] At [%fx%fx%f]",
            zox_getn(prefab),
            position.x,
            position.y,
            position.z);
        if (zox_valid(model)) {
            zox_log("Model [%s]",
                zox_get_name(model));
        } else {
            zox_log("No Model");
        }
        if (zox_valid(vox)) {
            zox_log("Vox [%s]",
                zox_get_name(vox));
        } else {
            zox_log(" No Vox");
        }
    }
    zox_instance(prefab);
    zox_name("character3");
    if (zox_has(prefab, VoxelNodeLock)) {
        initialize_voxel_lock(world, e);
    }
    // TODO: SPawn mesh as sub entity here
    // new links
    if (realm) {
        zox_set_parent(world, e, realm);
        zox_link(world, e, RealmLink, realm);
    } else {
        zox_loge("No Realm in character spawn");
    }
    if (terrain) {
        zox_link(world, e, TerrainLink, terrain);
    }
    if (name) {
        set_ZoxName(world, e, name);
    }
    zox_setv(e, Seed, seed);
    zox_setv(e, Position3D, position);
    zox_setv(e, Rotation3D, rotation);
    zox_setv(e, LastPosition3D, position);
    zox_setv(e, RenderDisabled, render_disabled);
    if (zox_valid(model)) {
        zox_setv(e, ModelLink, model);
        zox_setv(e, RenderDepth, render_depth);
        zox_setv(e, MaxRenderDepth, max_depth);
        // zox_log("Render Depth of character set to [%i] of [%i]", render_depth, max_depth);
    }
    if (zox_has(prefab, CharacterGeneric)) {
        if (zox_valid(vox)) {
            zox_setv(e, InstanceLink, vox);
        }
        if (zox_valid(vox) && zox_has(vox, BlockScale)) {
            float block_scale = zox_getv(vox, BlockScale);
            int3 chunk_size = zox_getv(vox, ChunkSize);
            float3 bounds = calculate_vox_bounds(
                chunk_size,
                block_scale);
            zox_setv(e, BlockScale, block_scale);
            zox_setv(e, Bounds3D, bounds);
            if (dbg_log) {
                zox_log("Bounds Generated [%fx%fx%f]", bounds.x, bounds.y, bounds.z);
            }
        } else if (zox_valid(vox)) {
            zox_loge("Model has no BlockScale [%s]",
                zox_getn(vox));
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
