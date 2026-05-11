// Stats use name of the parent
entity zox_ins_named(ecs* world, entity prefab) {
    entity e = zox_ins(prefab);
    zox_set_unique_name(e, zox_get_name(prefab));
    return e;
}

entity spawn_stat(ecs* world, entity prefab, entity parent, const char* name, entity texture, color_rgb stat_color) {
    entity e = zox_ins_named(world, prefab);
    if (parent) {
        zox_set_parent(world, e, parent);
        zox_set(e, UserLink, { parent });
    }
    if (texture) {
        zox_set(e, TextureLink, { texture });
    }
    if (name) {
        zox_set_unique_name(e, name);
        set_ZoxName(world, e, name);
    }
    if (!color_rgb_equals(stat_color, color_rgb_white)) {
        zox_set(e, ColorRGB, { stat_color });
    }
    return e;
}

entity spawn_stat_attribute(ecs* world, entity prefab, entity parent, const char* name, float value, entity texture) {
    entity e = spawn_stat(world, prefab, parent, name, texture, color_rgb_white);
    zox_set(e, StatValue, { value });
    return e;
}

entity spawn_stat_state(ecs* world, entity prefab, entity parent, const char* name, float value, float max, entity texture) {
    entity e = spawn_stat(world, prefab, parent, name, texture, color_rgb_white);
    zox_set(e, StatValue, { value });
    zox_set(e, StatValueMax, { max });
    return e;
}

// Add rate later
entity spawn_stat_regen(ecs* world, entity prefab, entity parent, const char* name, float value, entity texture) {
    entity e = spawn_stat(world, prefab, parent, name, texture, color_rgb_white);
    zox_set(e, StatValue, { value });
    return e;
}

entity spawn_stat_level(ecs* world, entity prefab, entity parent, const char* name, float value, entity texture) {
    entity e = spawn_stat(world, prefab, parent, name, texture, color_rgb_white);
    zox_set(e, StatValue, { value });
    return e;
}
