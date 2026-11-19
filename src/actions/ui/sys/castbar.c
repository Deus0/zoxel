// A castbar fills over its character's cast: its active action's warmup (WarmupAt, over WarmupTime), shown while it
//  casts (poked, AutoHide hides it after), its name set as a cast begins; instant actions (no warmup) show nothing
void castbar_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(CastAction);
    zox_sys_out(BarLevel);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(CastAction, cast);
        zox_sys_o(BarLevel, level);
        const entity character = zox_get_link(world, e, CharacterLink);
        const entity action = zox_valid(character) && zox_has(character, ActiveAction) ? zox_getv(character, ActiveAction) : 0;
        if (!zox_valid(action) || !zox_has(action, WarmupAt) || !zox_has(action, WarmupTime)) {
            continue;
        }
        const double at = zox_getv(action, WarmupAt);
        const double time = zox_getv(action, WarmupTime);
        if (!at || time <= 0) {
            continue;
        }
        const float done = (float) ((zox_current_time - at) / time);
        level->value = done < 0 ? 0 : (done > 1 ? 1 : done);
        ui_poke(world, e);
        if (cast->value != action) {
            cast->value = action;
            const entity text = zox_get_child_by_id(world, e, zox_id(CastbarText));
            if (zox_valid(text)) {
                set_entity_text(world, text, zox_has(action, ZoxName) ? zox_getv(action, ZoxName) : zox_get_name(action));
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(castbar_system);
