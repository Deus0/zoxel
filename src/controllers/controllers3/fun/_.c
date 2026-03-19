#include "detatch.c"
#include "attach.c"
#include "labels.c"

// returns success
byte get_player_linked_things(ecs* world, entity player, entity* realm_out, entity* terrain_out, entity* camera_out) {

    zox_geter_value(player, GameLink, entity, game);
    if (!zox_valid(game)) {
        zox_log_error("Invalid [game]");
        return 0;
    }

    zox_geter_value(game, RealmLink, entity, realm);
    if (!zox_valid(realm) || !zox_has(realm, TerrainLink)) {
        if (!zox_valid(realm)) {
            zox_log_error("Invalid [realm]");
        } else {
            zox_log_error("[realm] has No TerrainLink");
        }
        return 0;
    }

    zox_geter_value(realm, TerrainLink, entity, terrain);
    if (!zox_valid(terrain)) {
        zox_log_error("Invalid [terrain]");
        return 0;
    }

    zox_geter_value(player, CameraLink, entity, camera);
    if (!zox_valid(camera)) {
        zox_log_error("Invalid [camera]");
        return 0;
    }

    *realm_out = realm;
    *terrain_out = terrain;
    *camera_out = camera;
    return 1;
}
