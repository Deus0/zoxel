static void ensure_editor_menubar(ecs* world, entity canvas) {
    if (!zox_get_child_by_id(world, canvas, zox_id(Menubar))) {
        zox_add(canvas, EditorMenubarDirty);
    }
}
