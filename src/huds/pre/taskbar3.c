// 3D huds' taskbar (spawn_taskbar3): its frames toggleable as a 2D taskbar's (prefab_frame_selectable: ActiveState
//  outlines it, element_active_system), its icons textured from files (clone_texture_file_to_entity)
entity spawn_prefab_taskbar_frame3(ecs* world, entity prefab) {
    entity e = spawn_prefab_frame_toggleable(world, prefab, default_outline_color_frame, button_outline_active);
    zox_prefab_name("taskbar_frame3");
    zox_add(e, Element);
    zox_setv(e, ElementFillColor, default_fill_color_frame);
    zox_setv(e, ActiveStateDirty, 0);
    zox_setv(e, Layer, 1);
    return e;
}

entity spawn_prefab_taskbar_icon3(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("taskbar_icon3");
    zox_setv(e, Layer, 2);
    zox_setv(e, ClickEvent, NULL);
    return e;
}
