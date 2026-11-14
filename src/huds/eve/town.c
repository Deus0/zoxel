// a player's character entering a town (InTown, in_town_system) is greeted with its name (a toast)
void town_entered_toast(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity town = zox_get_link(world, e, TownLink);
        const entity player = zox_get_link(world, e, PlayerLink);
        if (!zox_valid(town) || !zox_valid(player)) {
            continue;
        }
        char text[96];
        const char* name = zox_has(town, ZoxName) ? zox_getv(town, ZoxName) : NULL;
        snprintf(text, sizeof(text), "Welcome to %s", name && name[0] ? name : "Town");
        spawn_toast(world, zox_get_link(world, player, CanvasLink), text);
    }
}
