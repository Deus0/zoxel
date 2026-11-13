// tst: quest_dialogue, a quest giver offers its quest to a character without it, then remembers it gave it
//  - talks to a quest giving npc the way dialogue_begin_system does, pressing next as each line finishes
//  - the first talk: "Welcome traveler." then "I have a quest you may like.", and the character gets the quest
//      (it said "You did well..." to a character without the quest: NodeLink was exclusive, a branch kept
//      only its true node, and the character started with the quest it offers)
//  - checks and actions aren't said (they flashed ".....")
//  - the second talk: "You have already been commanded."
//  - slays its slimes (check_quest_slay, as many as its target), the quest is done
//  - the third talk: "You did well..." then "I shall reward thee.", the quest is handed in, "Go on, giitt."
//  - the fourth talk: only "You did well..." (handed in already, no second reward)
//  - the npc babbles its lines in its voice (voice sounds spawn)
//  - run: bash bsh/tst.sh quest_dialogue
#define zox_tst_qd_max_lines 12
int zox_tst_qd_stage = 0;
int zox_tst_qd_frames = 0;
int zox_tst_qd_settled = 0;
entity zox_tst_qd_npc = 0;
entity zox_tst_qd_process = 0;
entity zox_tst_qd_ui = 0;
#define zox_tst_qd_talks 4
char zox_tst_qd_lines[zox_tst_qd_talks][zox_tst_qd_max_lines][128];
int zox_tst_qd_lines_count[zox_tst_qd_talks];
int zox_tst_qd_slain = 0;
int zox_tst_qd_jingle_notes = 0;    // delayed sounds after taking the quest (its jingle)
int zox_tst_qd_voices = 0;

// a quest giving npc with its dialogue
static inline entity zox_tst_qd_find_npc(ecs* world) {
    ecs_iter_t it = ecs_each_id(world, zox_id(QuestGiver));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const entity npc = it.entities[i];
            if (zox_valid(zox_get_link(world, npc, Dialogue))) {
                ecs_iter_fini(&it);
                return npc;
            }
        }
    }
    return 0;
}

// any npc, made a quest giver like character_dialogue_system does (quest givers are random)
static inline entity zox_tst_qd_make_npc(ecs* world, entity character) {
    const entity realm = zox_get_link(world, character, RealmLink);
    const entity tree = zox_get_child_by_id(world, realm, zox_id(QuestDialogue));
    const entity quest = zox_get_child_by_id(world, realm, zox_id(SlayQuest));
    if (!zox_valid(tree) || !zox_valid(quest)) {
        return 0;
    }
    ecs_iter_t it = ecs_each_id(world, zox_id(Character));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const entity npc = it.entities[i];
            if (npc != character && zox_valid(zox_get_link(world, npc, Dialogue)) && !zox_has(npc, Dead)) {
                ecs_iter_fini(&it);
                zox_link(world, npc, Dialogue, tree);
                zox_add(npc, QuestGiver);
                spawn_quest_giving(world, npc, quest);
                return npc;
            }
        }
    }
    return 0;
}

// the quest the quest dialogue gives (gam/zoxel/sys/dialogues.c, the realm's slay quest)
static inline entity zox_tst_qd_quest(ecs* world, entity npc) {
    const entity realm = zox_get_link(world, npc, RealmLink);
    return zox_valid(realm) ? zox_get_child_by_id(world, realm, zox_id(SlayQuest)) : 0;
}

static inline void zox_tst_qd_begin(ecs* world, entity player, entity character, entity canvas) {
    const entity tree = zox_get_link(world, zox_tst_qd_npc, Dialogue);
    zox_tst_qd_process = spawn_process_dialogue(world, prefab_process_dialogue, tree, character, zox_tst_qd_npc);
    zox_link(world, character, DialogueProcess, zox_tst_qd_process);
    zox_tst_qd_ui = spawn_dialogue_ui(world, canvas, character, zox_tst_qd_npc);
    link_dialogue_run_to_ui(world, zox_tst_qd_process, zox_tst_qd_ui);
    zox_setv(player, PlayerState, zox_player_state_dialogue_begin);
    follow_target(world, zox_tst_qd_npc, character, 0.9f);   // as dialogue_begin_system: the npc turns to them
    zox_tst_qd_settled = 0;
}

extern char zox_screenshot_request[256];

// heads: both speakers look at each other while they talk (LookTarget, bones/sys/head_look.c), ahead after
int zox_tst_qd_looking = 0;     // moments both heads had the other as LookTarget

static inline entity zox_tst_qd_look(ecs* world, entity character) {
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    return zox_valid(head) ? zox_getv(head, LookTarget) : 0;
}

// how directly a character's head faces a target's, 1 straight at it
static inline float zox_tst_qd_facing(ecs* world, entity character, entity target) {
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    if (!zox_valid(head)) {
        return 0;
    }
    const float3 forward = float4_rotate_float3(zox_getv(head, Rotation3D), float3_forward);
    return float3_dot(forward, float3_normalize(float3_sub(look_point(world, target), zox_getv(head, Position3D))));
}

// characters across the dialogue box, its lines wrap to it
int zox_tst_qd_columns = 0;
int zox_tst_qd_overflows = 0;     // checks every frame, a failed wrap counts many times

// records each new line, presses next once it's typed out, returns 1 when the talk ended
static inline byte zox_tst_qd_talk(ecs* world, int talk) {
    if (!zox_valid(zox_tst_qd_ui) || !zox_valid(zox_tst_qd_process)) {
        return 1;
    }
    const entity label = zox_get_link(world, zox_tst_qd_ui, DialogueLabel);
    if (!zox_valid(label)) {
        return 0;
    }
    const char* typed = zox_getv(label, TargetText);
    int* count = &zox_tst_qd_lines_count[talk];
    zox_tst_qd_columns = text_columns(world, label);
    // wrapped to the box: its new lines read as spaces, a wrapped line wider than the box (with a space to break at) is
    //  a wrap that failed
    char line[512];
    snprintf(line, sizeof(line), "%s", typed);
    int width = 0;
    byte spaced = 0;
    for (int i = 0; ; i++) {
        if (!line[i] || line[i] == '\n') {
            zox_tst_qd_overflows += width > zox_tst_qd_columns && spaced;
            width = 0;
            spaced = 0;
            if (!line[i]) {
                break;
            }
            line[i] = ' ';
            continue;
        }
        width++;
        spaced |= line[i] == ' ';
    }
    if (line[0] && (*count == 0 || strcmp(zox_tst_qd_lines[talk][*count - 1], line)) && *count < zox_tst_qd_max_lines) {
        snprintf(zox_tst_qd_lines[talk][*count], 128, "%s", line);
        zox_log("[tst] talk [%i] line [%i]: %s", talk, *count, line);
        (*count)++;
        zox_tst_qd_settled = 0;
    }
    const TextData* shown = zox_get(label, TextData);
    if (line[0] && shown->length >= (int) strlen(typed)) {
        // typed out, a moment later press next, the first talk's second line (it wraps) saved to look at
        if (zox_tst_qd_settled == 5) {
            const entity character = zox_get_link(world, dbg_player, CharacterLink);
            zox_tst_qd_looking += zox_tst_qd_look(world, character) == zox_tst_qd_npc && zox_tst_qd_look(world, zox_tst_qd_npc) == character;
            zox_log("[tst] [metric] heads facing: the player [%.2f] the npc [%.2f] (1 straight at the other)",
                zox_tst_qd_facing(world, character, zox_tst_qd_npc), zox_tst_qd_facing(world, zox_tst_qd_npc, character));
        }
        if (zox_tst_qd_settled == 5 && talk == 0 && *count == 2) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("dialogue.ppm"));
        }
        if (++zox_tst_qd_settled == 10) {
            zox_add(zox_tst_qd_process, TriggerEnd);
        }
    }
    return *count >= zox_tst_qd_max_lines;
}

byte zox_tst_quest_dialogue_ready(ecs* world) {
    nosounds = 0;   // voices spawn (tests run --background, no audio device plays them)
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(character) || !zox_valid(canvas)) {
        zox_tst_waiting = "the player's character";
        return 0;
    }
    zox_tst_qd_frames++;
    zox_tst_qd_voices += zox_count_types(Sound) > 0 && zox_tst_qd_stage > 0;
    switch (zox_tst_qd_stage) {
        case -1:
            zox_tst_waiting = "the made quest giver";
            if (zox_tst_qd_frames < 3) {
                return 0;
            }
            zox_tst_qd_stage = 0;
            zox_tst_qd_frames = 11;
            return 0;
        case 0:
            zox_tst_waiting = "playing, near a quest giver";
            if (zox_getv(player, PlayerState) != zox_player_state_playing || zox_tst_qd_frames < 10) {
                return 0;
            }
            if (!zox_tst_qd_npc) {
                zox_tst_qd_npc = zox_tst_qd_find_npc(world);
            }
            if (!zox_tst_qd_npc && zox_tst_qd_frames > 60) {
                zox_tst_qd_npc = zox_tst_qd_make_npc(world, character);
                zox_tst_qd_frames = 0;
                zox_tst_qd_stage = -1;  // the giving quest spawns deferred, wait a frame
                return 0;
            }
            if (!zox_tst_qd_npc) {
                return 0;
            }
            {
                const entity quest = zox_tst_qd_quest(world, zox_tst_qd_npc);
                zox_tst_check(zox_valid(quest), "the quest giver has no quest to give");
                zox_tst_check(!zox_valid(quest) || !get_user_quest(world, character, quest), "the character has the quest before talking");
            }
            zox_tst_qd_begin(world, player, character, canvas);
            break;
        case 1:
            zox_tst_waiting = "the first talk";
            if (!zox_tst_qd_talk(world, 0)) {
                return 0;
            }
            break;
        default: {
            // between talks: playing again, then the next talk (the third after the quest is done)
            const int talk = (zox_tst_qd_stage - 2) / 2 + 1;
            if (zox_tst_qd_stage % 2 == 0) {
                zox_tst_waiting = "playing again";
                if (zox_getv(player, PlayerState) != zox_player_state_playing || zox_valid(zox_tst_qd_process)) {
                    return 0;
                }
                if (talk == 2) {
                    // slay its slimes, as a character's kills do (zox_event_quest_slay)
                    const entity user_quest = get_user_quest(world, character, zox_tst_qd_quest(world, zox_tst_qd_npc));
                    if (!zox_valid(user_quest)) {
                        zox_tst_check(0, "no quest to complete");
                        return 1;
                    }
                    const entity slime = zox_get_link(world, user_quest, CharacterLink);
                    const int target = zox_getv(user_quest, QuestTarget);
                    for (int k = 0; k < target + 2; k++) {
                        check_quest_slay(world, user_quest, slime, 0);
                        zox_tst_qd_slain++;
                    }
                    if (zox_tst_qd_frames < 3) {
                        return 0;   // QuestDone is added deferred
                    }
                    zox_tst_check(zox_has(user_quest, QuestDone), "slaying [%i] of [%i] slimes didn't finish the quest", zox_tst_qd_slain, target);
                }
                zox_tst_qd_begin(world, player, character, canvas);
                break;
            }
            if (talk == 1 && !zox_tst_qd_jingle_notes) {
                zox_tst_qd_jingle_notes = zox_count_types(SoundDelay);
            }
            zox_tst_waiting = "a talk";
            if (!zox_tst_qd_talk(world, talk)) {
                return 0;
            }
            if (talk == zox_tst_qd_talks - 1) {
                return 1;
            }
            break;
        }
    }
    zox_tst_qd_stage++;
    zox_tst_qd_frames = 0;
    return 0;
}

void zox_tst_quest_dialogue_check(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    zox_tst_check(zox_tst_qd_lines_count[0] >= 2 && !strcmp(zox_tst_qd_lines[0][0], "Welcome traveler."),
        "the first talk didn't start with a welcome");
    zox_tst_check(zox_tst_qd_lines_count[0] >= 2 && !strcmp(zox_tst_qd_lines[0][1], "I have a quest you may like."),
        "a character without the quest heard [%s], not the quest offer", zox_tst_qd_lines_count[0] >= 2 ? zox_tst_qd_lines[0][1] : "nothing");
    const entity quest = zox_tst_qd_quest(world, zox_tst_qd_npc);
    zox_tst_check(zox_valid(get_user_quest(world, character, quest)), "the character didn't get the quest");
    zox_tst_check(zox_tst_qd_lines_count[1] >= 2 && !strcmp(zox_tst_qd_lines[1][1], "You have already been commanded."),
        "the second talk said [%s], not that it gave the quest", zox_tst_qd_lines_count[1] >= 2 ? zox_tst_qd_lines[1][1] : "nothing");
    const entity user_quest = get_user_quest(world, character, quest);
    zox_tst_check(zox_tst_qd_lines_count[2] >= 3 && !strcmp(zox_tst_qd_lines[2][1], "You did well...") &&
        !strcmp(zox_tst_qd_lines[2][2], "I shall reward thee."), "the third talk didn't reward the done quest");
    zox_tst_check(zox_valid(user_quest) && zox_has(user_quest, QuestHandedin), "the done quest wasn't handed in");
    zox_tst_check(zox_tst_qd_lines_count[3] == 2 && !strcmp(zox_tst_qd_lines[3][1], "You did well..."),
        "the fourth talk said [%i] lines, not only that it was done well", zox_tst_qd_lines_count[3]);
    zox_tst_check(zox_tst_qd_columns > 0, "the dialogue box has no wrap width");
    zox_tst_check(!zox_tst_qd_overflows, "dialogue lines passed the box's [%i] characters [%i] times", zox_tst_qd_columns, zox_tst_qd_overflows);
    zox_tst_check(zox_tst_qd_looking > 0, "the speakers' heads didn't look at each other while talking");
    // the npc may glance at the player after (ai/sys/glance.c), the player looks ahead
    zox_tst_check(!zox_tst_qd_look(world, character), "the player's head still looks at the npc after the talks");
    zox_tst_check(zox_tst_qd_voices > 0, "the npc said its lines without a voice");
    zox_tst_check(zox_tst_qd_jingle_notes > 0, "taking the quest played no jingle");
}
