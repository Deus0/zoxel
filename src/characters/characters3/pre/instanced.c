// we link our instances to model links
//      - set ModelLink->value per lod change -  RenderDepthDirty
entity spawn_prefab_character3_instanced(ecs *world, entity prefab) {
    entity e = spawn_prefab_character3(
        world,
        prefab,
        zox_character_type_instanced);
    zox_make_prefab(e);
    zox_prefab_name("character3_instanced");
    zox_prefab_set(e, Rotation3D, { quaternion_identity });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_add(e, CharacterGeneric);
    return e;
}
