// Only works in Start Menu
void menu_start_triggered(
    ecs *world,
    entity player,
    entity canvas)
{
    entity start_menu = zox_get_link(world, player, MenuStart);
    if (!zox_valid(start_menu)) {
        zox_loge("Start Menu not found on player");
        return;
    }
    zox_delete(start_menu);
    spawn_main_menu(
        world,
        player,
        game_name);
    spawn_sound_effect(world, sound_effect_start, 0);
}

void button_event_menu_start(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, CanvasLink);
    menu_start_triggered(world, event.clicker, canvas);
}
