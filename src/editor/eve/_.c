#include "menubar.c"

static void editor_menubar_spawn_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, EditorMenubarDirty);
        if (!zox_get_child_by_id(world, e, zox_id(Menubar))) {
            spawn_editor_menubar(world, e);
        }
    }
}

static void editor_theme_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        editor_theme(world, it->entities[i]);
        zox_remove(it->entities[i], EditorThemeDirty);
    }
}

static void editor_window_menu_hover_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        editor_window_menu_refresh(world, it->entities[i]);
    }
}

// the "FPS" setting (Video tab): the fps display top right, in every build
void app_initialize_editor(ecs* world, entity app) {
    setting_tags(world, spawn_setting_byte(world, app, "FPS", zox_fps_display), zox_id(SettingTabVideo),
        zox_id(SettingFPSDisplay));
}

void editor_settings_observer_callback(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        if (zox_has(e, SettingFPSDisplay) && zox_has(e, SettingByte)) {
            zox_fps_display = zox_getv(e, SettingByte);
        }
    }
}

void zox_events_editor(ecs* world) {
    zox_on_add(editor_menubar_spawn_event, [none] layouts.Canvas, [none] editor.EditorMenubarDirty);
    zox_on_add(editor_theme_event, [none] editor.EditorThemeDirty);
    zox_on_add(editor_window_menu_hover_event, [none] editor.EditorWindowMenu, [none] interactions.TriggerSelect);
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_editor);
    zox_on_add(
        editor_settings_observer_callback,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}
