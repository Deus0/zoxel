// a small square frame ui with an icon in it
// used for game - action/skill/stat - uis
entity spawn_prefab_frame(ecs *world, entity p, color b, color o) {
    zox_prefab_child(p);
    zox_prefab_name("frame");
    zox_add_tag(e, FixToLayout);
    zox_add_tag(e, Frame);
    zox_prefab_set(e, Color, { b });
    zox_prefab_set(e, OutlineColor, { o });
    // zox_prefab_set(e, Children, { 0 });
    return e;
}
