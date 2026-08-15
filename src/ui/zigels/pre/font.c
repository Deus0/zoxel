entity spawn_font_prefab(ecs *world) {
    zox_prefab();
    zox_prefab_name("font");
    zox_add(e, Font);
    zox_prefab_add(e, FontData);
    return e;
}

entity spawn_font(ecs *world, entity parent, entity prefab, const byte2 *points, byte length) {
    if (!points || length == 0) {
        zox_log("! [spawn_font] cannot spawn null points\n")
        return 0;
    }
    zox_instance(prefab);
    zox_name("font");
    zox_set_parent(world, e, parent);
    FontData data = (FontData) { 0 };
    initialize_FontData(&data, length);
    memcpy(data.value, points, length * sizeof(byte2));
    zox_set_ptr(e, FontData, data);
    return e;
}

// doesn't clone points
entity spawn_font_direct(ecs *world, entity prefab, byte2 *points, byte length) {
    zox_instance(prefab);
    zox_name("font");
    zox_muter(e, FontData, fontData);
    fontData->length = length;
    fontData->value = points;
    return e;
}
