// tst: pickup_pop, a dropped item pops: it hops a little away from where it dropped, then rests
//  - drops an item beside the player (spawn_item_world_from_meta, what block and character drops use)
//  - mid pop it's above where it dropped (the hop)
//  - after the pop it rests at the drop's height, moved sideways but within its block, PickupPop removed
//  - run: bash bsh/tst.sh pickup_pop
int zox_tst_pop_stage = 0;
int zox_tst_pop_frames = 0;
entity zox_tst_pop_pickup = 0;
float3 zox_tst_pop_start;
double zox_tst_pop_time = 0;
float zox_tst_pop_highest = 0;

byte zox_tst_pickup_pop_ready(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(character)) {
        zox_tst_waiting = "the player's character";
        return 0;
    }
    zox_tst_pop_frames++;
    if (zox_tst_pop_stage == 0) {
        zox_tst_waiting = "playing";
        if (zox_getv(player, PlayerState) != zox_player_state_playing || zox_tst_pop_frames < 10) {
            return 0;
        }
        const entity realm = zox_get_link(world, character, RealmLink);
        entity meta = 0;
        iter it2 = zox_children(world, realm);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count && !meta; j++) {
                if (zox_has(it2.entities[j], Item)) {
                    meta = it2.entities[j];
                }
            }
        }
        if (!meta) {
            zox_tst_check(0, "realm has no items to drop");
            return 1;
        }
        zox_tst_pop_start = zox_getv(character, Position3D);
        zox_tst_pop_start.x += 1;
        zox_tst_pop_pickup = spawn_item_world_from_meta(world, realm, meta, zox_tst_pop_start, item_pickup_scale);
        zox_tst_pop_time = zox_current_time;
        zox_tst_pop_highest = zox_tst_pop_start.y;
        zox_tst_pop_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the pop to finish";
    const entity pickup = zox_tst_pop_pickup;
    if (!zox_valid(pickup)) {
        zox_tst_check(0, "the dropped item is gone");
        return 1;
    }
    const float3 position = zox_getv(pickup, Position3D);
    if (position.y > zox_tst_pop_highest) {
        zox_tst_pop_highest = position.y;
    }
    if (zox_current_time - zox_tst_pop_time < pickup_pop_seconds + 0.3) {
        return 0;
    }
    const float dx = position.x - zox_tst_pop_start.x;
    const float dz = position.z - zox_tst_pop_start.z;
    const float moved = sqrtf(dx * dx + dz * dz);
    zox_log("[tst] popped [%.3f] sideways, hopped [%.3f] high", moved, zox_tst_pop_highest - zox_tst_pop_start.y);
    zox_tst_check(!zox_has(pickup, PickupPop), "the pop didn't finish");
    zox_tst_check(zox_tst_pop_highest > zox_tst_pop_start.y + 0.01f, "the item didn't hop up");
    zox_tst_check(fabsf(position.y - zox_tst_pop_start.y) < 0.001f, "the item rests at [%.3f], dropped at [%.3f]", position.y, zox_tst_pop_start.y);
    zox_tst_check(moved > 0.001f, "the item didn't move away from where it dropped");
    zox_tst_check(moved <= item_pickup_scale * 1.5f + 0.001f, "the item moved [%.3f], further than its block", moved);
    return 1;
}

void zox_tst_pickup_pop_check(ecs* world) {
    (void) world;
}
