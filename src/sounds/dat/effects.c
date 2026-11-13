// Sound effects: what the world's actions sound like, all in one place (tuned by tst sound_balance)
//  - an effect is an instrument, a range of notes (note_frequencies), lengths and volumes (of the sfx volume)
//  - each spawn picks within its ranges, plays from its source (3D) or in the middle (no source)
//  - dialogue is spoken in voices (sounds/bop/voice.c), not effects
//  - hits are impacts (flesh on flesh), swings whooshes, soft not sharp (sounds/bop/impacts.c)
//  - small speakers barely play under ~150hz, so effects keep their body above that, or nobody hears them
typedef enum {
    sound_effect_jump,
    sound_effect_land,
    sound_effect_hit,           // a character takes damage
    sound_effect_swing,         // a punch at nothing
    sound_effect_out_of_range,
    sound_effect_no_resource,
    sound_effect_block_hit,
    sound_effect_block_hard,    // a block that can't break
    sound_effect_consume,
    sound_effect_place,
    sound_effect_aura,
    sound_effect_block_damage,  // a block hit by a punch or tool (block_damage.c)
    sound_effect_block_break,
    sound_effect_level_up,
    sound_effect_click,         // ui, in the middle
    sound_effect_click_rare,
    sound_effect_start,         // the start screen, on to the main menu
    sound_effect_end
} SoundEffectType;

typedef struct {
    const char* name;
    byte instrument;
    byte note_min;
    byte note_max;
    float seconds_min;
    float seconds_max;
    float volume_min;
    float volume_max;
} SoundEffect;

