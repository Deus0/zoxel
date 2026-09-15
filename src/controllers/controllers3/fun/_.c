#include "labels.c"

// returns success
byte get_player_linked_things(
    ecs* world,
    entity player,
    entity* realm_out,
    entity* terrain_out,
    entity* camera_out)
{
    entity game = zox_get_link(world, player, GameLink);
    if (!zox_valid(game)) {
        zox_log_error("Invalid [game]");
        return 0;
    }
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("Invalid [realm]");
        return 0;
    }
    entity terrain = zox_get_link(world, realm, TerrainLink);
    if (!zox_valid(terrain)) {
        zox_log_error("Invalid [terrain]");
        return 0;
    }
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(camera)) {
        zox_log_error("Invalid [camera]");
        return 0;
    }
    *realm_out = realm;
    *terrain_out = terrain;
    *camera_out = camera;
    return 1;
}
