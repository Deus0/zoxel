const int inspector_component_size_buffer = 128;

void set_editor_window(ecs* world, entity e) {
    byte layer = get_game_overlay_layer() + 2;
    zox_add(e, EditorElement);
    zox_add(e, IgnoreCanvasStack);
    zox_add(e, WindowLayerDirty);
    zox_setv(e, BonusLayer, layer);
}

// the debug player's canvas, for the editor's key toggles, 0 if not ready
entity get_editor_canvas(ecs* world) {
    const entity player = dbg_player;
    if (!zox_valid(player)) {
        return 0;
    }
    const entity canvas = zox_get_link(world, player, CanvasLink);
    return zox_valid(canvas) ? canvas : 0;
}

static inline void play_editor_toggle_sound(ecs* world, byte opened) {
    spawn_sound_from_file_index(world, prefab_sound, opened ? 0 : 1);
}

// spawns the ui if e is 0, otherwise deletes e, returns the spawned ui
entity canvas_toggle_ui(
    ecs *world,
    entity canvas,
    entity e,
    entity (*spawn_ui)(ecs*, entity))
{
    if (!zox_valid(e)) {
        return (*spawn_ui)(world, canvas);
    }
    zox_delete(e);
    return 0;
}

#include "list.c"
#include "tooltip.c"
#include "component.c"
#include "inspector.c"
