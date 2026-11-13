// Sets up a character made from a prefab (an instance, or a prefab variant of it)
//  - also supports no models, for skeleton body gen
static inline void setup_character3(
    ecs* world,
    entity e,
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
    entity model = zox_get_link(world, prefab, ModelLink);
    if (zox_valid(model)) {
        // If a ModelGroup, pick a random sub model
        model = get_model_variant(world, model, rand());
        if (zox_has(model, MaxRenderDepth)) {
            max_depth = zox_getv(model, MaxRenderDepth);
        }
    }
    byte render_depth = camera_distance_to_npc_render_depth(
        render_distance,
        max_depth);
    // NOTE: Render children spawn in character_children_spawn_system
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
    }
    if (name) {
        zox_name(name);
        set_ZoxName(world, e, name);
    } else {
        zox_name("character3");
    }
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
    zox_setv(e, Seed, seed);
    // what it sounds like hit, its kind's realm sound (sounds/bop/hits.c)
    //  (only when it has one: a removal here, on a kind made this frame, outlived the link added after it)
    const entity hit_sound = zox_get_link(world, prefab, HitSoundLink);
    if (hit_sound) {
        zox_link(world, e, HitSoundLink, hit_sound);
    }
    zox_setv(e, Position3D, position);
    zox_setv(e, Rotation3D, rotation);
    zox_setv(e, LastPosition3D, position);
    zox_setv(e, RenderDisabled, render_disabled);
    if (zox_valid(model)) {
        zox_set_link(world, e, ModelLink, model);
        zox_setv(e, RenderDepth, render_depth);
        zox_setv(e, MaxRenderDepth, max_depth);
        // zox_log("Render Depth of character set to [%i] of [%i]", render_depth, max_depth);
    }
    // NOTE: Bounds from the model, skeletons get theirs from their combined vox
    float block_scale;
    float3 bounds;
    if (get_model_bounds(world, model, &block_scale, &bounds)) {
        zox_setv(e, BlockScale, block_scale);
        zox_setv(e, Bounds3D, bounds);
    }
}

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
    zox_instance(prefab);
    setup_character3(world, e, prefab, realm, terrain, seed, render_distance, render_disabled, position, rotation, name);
    return e;
}
