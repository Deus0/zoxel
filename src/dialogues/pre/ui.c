entity spawn_prefab_dialogue_text(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("dialogue_text");
    zox_add(e, DialogueLabel);
    zox_setv(e, TargetText, 0);
    zox_setv(e, AnimateTextBegin, 0);
    zox_setv(e, AnimateTextTime, 0.1f);
    zox_set(e, AnimateTextTimeLimits, {{ 0.1f, 0.2f }});
    zox_setv(e, GlyphSpawnedDirty, 0);
    zox_setv(e, AnimateTextEnded, 0);
    return e;
}
