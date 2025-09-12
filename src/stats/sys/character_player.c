// players are richer beings from the aether
void PlayerCharacterStatsSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    // zox_sys_in(PlayerLink);
    zox_sys_out(StatLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        // zox_sys_i(PlayerLink, player);
        zox_sys_o(StatLinks, stats);

        if (state->value != zox_dirty_end) {
            continue;
        }

        zox_geter(realm->value, StatLinks, realm_stats);

        // add all attributes as 0
        for (int j = 0; j < realm_stats->length; j++) {
            const entity stat = realm_stats->value[j];
            if (!zox_valid(stat)) {
                continue;
            }
            if (zox_has(stat, StatAttribute)) {
                const entity character_stat = spawn_user_stat(world, stat, e);
                add_to_StatLinks(stats, character_stat);
            }
        }

        // Link to StatPanel
        /*zox_geter_value(player->value, CanvasLink, entity, canvas);
        find_child_with_tag(canvas, MenuGame, game_menu)
        if (zox_valid(game_menu)) {
            find_child_with_tag(game_menu, ElementBar, healthbar);
            if (zox_valid(healthbar)) {
                zox_set(healthbar, StatLink, { stat_health });
                zox_geter(healthbar, Children, statbar2D_children);
                if (statbar2D_children->length >= 1) {
                    const entity text = statbar2D_children->value[1];
                    zox_set(text, StatLink, { stat_health });
                }
            }
        }*/
    }
} zoxd_system2(PlayerCharacterStatsSystem);