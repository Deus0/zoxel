zox_sys2(HealthbarSpawnerSystem) {
    float healthbar_trail_offset = 0.22f;
    byte font_size = 16;
    byte2 padding = (byte2) { 10, 8 };
    int2 bar_size = (int2) { font_size * 12 + padding.x * 2, font_size + padding.y * 2 };
    // Colors
    color fill = (color) { 30, 5, 8, 130 };
    color outline = (color) { 0, 0, 0, 220 };
    color fill2 = (color) { 35, 150, 45, 210 };
    color outline2 = (color) { 90, 200, 90, 170 };
    color font_fill = (color) { 240, 220, 170, 220 };
    color font_outline = (color) { 0, 0, 0, 240 };
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
        entity health = zox_get_child_by_id(world, e, zox_id(StatHealth));
        if (!zox_valid(health)) {
            continue;
        }
        float3 spawn_position = float3_zero;
        entity3 spawns = spawn_bar3(world, spawn_position, zox_ui_scale3, bar_size, font_size, fill, outline, fill2, outline2, font_fill, font_outline, e, healthbar_trail_offset);
        zox_add_tag(spawns.x, Healthbar);
        add_to_ElementLinks(elementLinks, spawns.x);
        // Statbar stuff
        entity bar = spawns.y;
        zox_add_tag(bar, Statbar);
        zox_set(bar, StatLink, { health });
        // Text
        zox_set(spawns.z, StatLink, { health });
        zox_add_tag(spawns.z, StatsLabel);
    }
} zox_sys_end(HealthbarSpawnerSystem);
