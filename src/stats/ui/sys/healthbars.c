zox_sys2(HealthbarSpawnerSystem) {
    float healthbar_trail_offset = 0.28f;
    float ui_scale = zox_ui_scale3 * 1.8f;
    byte font_size = 16;
    byte2 padding = (byte2) { 10, 8 };
    int2 bar_size = (int2) {
        font_size * 14 + padding.x * 2,
        font_size + padding.y * 2
    };
    color fill = (color) { 0, 0, 0, 88 };
    color outline = (color) { 22, 22, 22, 88 };
    color fill2 = (color) { 66, 5, 5, 155 };
    color outline2 = (color) { 33, 33, 33, 155 };
    color font_fill = (color) { 200, 255, 255, 255 };
    color font_outline = (color) { 77, 122, 122, 255 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CombatState);
    zox_sys_out(ElementLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CombatState, combat);
        zox_sys_o(ElementLinks, elementLinks);
        // remove old
        if (combat->value == zox_combat_leaving) {
            // destroy healthbar here
            find_array_element_with_tag(elementLinks, Healthbar, healthbar)
            if (zox_valid(healthbar)) {
                remove_from_ElementLinks(elementLinks, healthbar);
                zox_delete(healthbar)
            } else {
                zox_logw("[%s] had no healthbar in combat", zox_get_name(e));
            }
            continue;
        }
        // when entering, spawn healthbar
        if (combat->value != zox_combat_enter_battle) {
            continue;
        }
        entity health = zox_get_child_by_id(
            world,
            e,
            zox_id(StatHealth));
        if (!zox_valid(health)) {
            continue;
        }
        float3 spawn_position = float3_zero;
        entity3 spawns = spawn_bar3(
            world,
            spawn_position,
            ui_scale,
            bar_size,
            font_size,
            fill,
            outline,
            fill2,
            outline2,
            font_fill,
            font_outline,
            e,
            healthbar_trail_offset);
        zox_add(spawns.x, Healthbar);
        add_to_ElementLinks(elementLinks, spawns.x);
        // Statbar stuff
        entity bar = spawns.y;
        zox_add(bar, Statbar);
        zox_setv(bar, StatLink, health);
        // Text
        zox_setv(spawns.z, StatLink, health);
        zox_add(spawns.z, StatsLabel);
    }
} zox_sys_end(HealthbarSpawnerSystem);
