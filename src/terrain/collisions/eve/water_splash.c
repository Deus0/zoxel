// a splash going into water (falling fast) and a small one coming out: particles and a sound at the water's surface
// and InWater while in it (it swims: characters/jump/sys/swim.c)
// on InsideBlockDirty (inside.c), InsideBlockLast keeps the block it was in before
// counted for one character, for tst water_splash
entity zox_water_splash_watch = 0;
int zox_water_splashes_in = 0;
int zox_water_splashes_out = 0;
#define zox_water_splash_speed 2.0f

static inline void water_splash_spawn(ecs* world, float3 position, int count, byte alpha) {
    const entity emitter = spawn_particle3D_emitter_at(world, position, count, (float3) { 0.6f, 0.2f, 0.6f }, (color) { 120, 170, 240, alpha });
    zox_setv(emitter, DestroyInTime, 0.3f);
    spawn_sound_effect_at(world, sound_effect_splash, position);
}

void water_splash_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InsideBlock);
    zox_sys_in(InsideBlockPosition);
    zox_sys_in(Velocity3D);
    zox_sys_out(InsideBlockLast);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InsideBlock, inside);
        zox_sys_i(InsideBlockPosition, iposition);
        zox_sys_i(Velocity3D, velocity);
        zox_sys_o(InsideBlockLast, last);
        if (last->value == inside->value) {
            continue;
        }
        const byte was = last->value;
        last->value = inside->value;
        const entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain)) {
            continue;
        }
        const entity water = zox_get_child_by_id(world, zox_get_parent(world, terrain), zox_id(BlockWater));
        if (!zox_valid(water)) {
            continue;
        }
        const byte water_index = zox_getv(water, BlockIndex);
        const byte entering = was != water_index && inside->value == water_index;
        const byte leaving = was == water_index && inside->value != water_index;
        if (!entering && !leaving) {
            continue;
        }
        if (entering) {
            zox_add(e, InWater);
        } else {
            zox_remove(e, InWater);
        }
        // the top of the block it went in (or came out of, the one under it)
        const float scale = zox_getv(terrain, BlockScale);
        const int3 b = iposition->value;
        const float3 surface = { (b.x + 0.5f) * scale, (b.y + (entering ? 1 : 0)) * scale, (b.z + 0.5f) * scale };
        if (entering && velocity->value.y < -zox_water_splash_speed) {
            zox_log("[splash] into water at [%.1f] a second", velocity->value.y);
            water_splash_spawn(world, surface, 12, 200);
            zox_water_splashes_in += e == zox_water_splash_watch;
        } else if (leaving) {
            zox_log("[splash] out of water");
            water_splash_spawn(world, surface, 6, 100);
            zox_water_splashes_out += e == zox_water_splash_watch;
        }
    }
}
