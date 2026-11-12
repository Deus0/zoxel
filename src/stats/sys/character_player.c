// NOTE: Players are richer beings from the aether
//  - its stats from the same helper as npcs' prefabs (fill_character_base_stats), and attributes and currencies
void PlayerCharacterStatsSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity realm = zox_get_link(world, e, RealmLink);
        fill_character_base_stats(world, e, realm, 1, player_health_base, 1);
    }
    zox_sys_on_end();
} zoxd_system(PlayerCharacterStatsSystem);
