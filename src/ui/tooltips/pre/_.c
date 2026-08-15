entity spawn_prefab_tooltip(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_add(e, Tooltip);
    zox_add(e, ZextRenderEnabler);
    zox_add(e, MouseElement);
    zox_prefab_set(e, RenderDisabled, { 1 });
    zox_prefab_set(e, MeshAlignment, { zox_alignment_top_right });
    zox_prefab_set(e, ZeviceLink, { 0 });
    zox_prefab_set(e, TooltipperLink, { 0 });
    return e;
}
