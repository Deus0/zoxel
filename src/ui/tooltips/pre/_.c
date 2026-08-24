entity spawn_prefab_tooltip(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_add(e, Tooltip);
    zox_add(e, ZextRenderEnabler);
    zox_add(e, MouseElement);
    zox_setv(e, RenderDisabled, 1);
    zox_setv(e, MeshAlignment, zox_alignment_top_right);
    zox_setv(e, ZeviceLink, 0);
    zox_setv(e, TooltipperLink, 0);
    return e;
}
