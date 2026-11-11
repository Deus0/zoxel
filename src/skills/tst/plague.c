// --blackplague: damages every npc every few seconds, for crash testing hits and deaths
//  - uses damage_character, the same path as melee and projectiles
byte zox_blackplague = 0;
const double zox_blackplague_interval = 5.0;
const float zox_blackplague_damage = 2.0f;

void process_arguments_skills(ecs* world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--blackplague") == 0) {
            zox_blackplague = 1;
            zox_log("+ [blackplague] enabled");
        }
    }
}

zox_sys2(BlackPlagueSystem) {
    // NOTE: a system runs once per matched table, so the strike time is shared for the frame
    static double last_strike = 0;
    static double strike_frame = -1;
    if (!zox_blackplague) {
        return;
    }
    if (strike_frame != zox_current_time) {
        if (zox_current_time - last_strike < zox_blackplague_interval) {
            return;
        }
        last_strike = zox_current_time;
        strike_frame = zox_current_time;
    }
    zox_sys_world();
    // the player is the attacker, so combat_on_hit runs like a punch
    entity attacker = zox_valid(dbg_player) ?
        zox_get_link(world, dbg_player, CharacterLink) :
        0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        damage_character(
            world,
            attacker,
            e,
            0,
            zox_blackplague_damage,
            (float3) { 0, 1, 0 });
    }
    zox_log("+ [blackplague] struck [%i] npcs", it->count);
} zox_sys_end(BlackPlagueSystem);

void zox_systems_blackplague(ecs* world) {
    zox_system_1(
        BlackPlagueSystem,
        zoxp_spawn,
        [none] characters3.Character3,
        [none] !players.PlayerCharacter,
        [none] !core.Dead,
    );
}
