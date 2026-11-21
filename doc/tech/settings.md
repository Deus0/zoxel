# Settings

Settings are entities, children of the app (`main_app`), each a `Setting` with a `ZoxName` and one value:
`SettingByte`, `SettingInt` or `SettingFloat`. The options menu shows them, a file saves them.

## A module's settings

Each module owns its own, in its `settings.c` (i.e. src/sounds/sys/settings.c):

```c
// spawned on the app
void sounds_settings(ecs* world, entity e) {
    spawn_setting_float(world, e, "Music", volume_music, (float2) { 0, 1 });
}

// a setting changed: set the module's value
void sounds_settings_on_dirty(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (zox_has(e, SettingFloat) && !strcmp(name->value, "Music")) {
            volume_music = zox_getv(e, SettingFloat);
        }
    }
}

void sounds_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, sounds_settings);
    zox_on_add(sounds_settings_on_dirty, [in] core.ZoxName, [none] settings.Setting, [none] core.Dirty);
}
```

and its systems function hooks them: `add_hook_spawn_prefabs(sounds_settings_events)` (it needs `prefab_app`).

- Spawn: `spawn_setting_byte`, `spawn_setting_byte_slider`, `spawn_setting_float` (with a range), on the app's
  `InitializeEvent`. No system polls for it.
- Apply: an observer on `Dirty` added to a `Setting`. Match by name, set the module's global.
- Settings only in dev builds go behind `#ifdef zox_dev` (Terrain Res).

## Changing one

`set_app_setting_byte / _int / _float(world, app, name, value)` sets the value and adds `Dirty` (returns 0 if no
setting has that name). The options menu's toggles and sliders do the same (huds/ins/settings.c).

- **Why `Dirty`, not `Update`:** `Dirty` comes off every frame (`dirty_system`), so a slider dragged adds it again
  each frame and the observer fires each frame. `Update` stays on while an entity is dirtied again, so an observer on
  it fired only for a drag's first value (tst settings_drag).
- Set the value before (or in the same deferred batch as) adding `Dirty`, the observer reads it then.

## Save and load

- **Save:** `setting_save_system` runs on settings with `Update` (the frame after their `Dirty`), and writes them
  all once: `~/.config/<game>/settings.cfg`, or the saves folder with `--nosaves` (tests). Lines are
  `name:type:value`, i.e. `Music:float:0.5`.
- **Load:** the app's `LoadSettings` goes start, spawn, run, end; at run it reads the file and sets each by name.
  Unknown names and bad lines are skipped; ones set by launch args (`--tiny` sets Far RD) are kept.
  Loading dirties every setting, so the save rewrites the file a frame later.

## Tests

| Test | Checks |
|---|---|
| settings_spawn | every module's settings on the app, each once |
| settings_apply | one setting of each module reaches its global, and the file |
| settings_drag | one changed every frame reaches the game every frame |
| settings_load | a file loaded sets the game's values, bad lines skipped |
| options_toggle, xr_options | the menus that change them |

All in src/npcs/tst (the lowest module that sees every setting's module).
