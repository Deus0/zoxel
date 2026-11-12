// Dialogue voices: the speaker babbles a syllable as each one appears (sounds/bop/voice.c)
//  - a syllable starts at a vowel opening a word, or at a consonant (not after another consonant)
//      with a vowel ahead in its word: "hello there" says he-llo the-re
//  - a closing consonant (the t in cat) is quiet, a word without vowels (hmm) says its first letter
//  - a word before a ? rises, before a ! is louder
//  - a speaker's voice is from its seed, so every npc sounds like themself
typedef struct {
    char consonant;     // 0 for none
    char vowel;         // 0 for a schwa
    float inflection;   // semitones, rising for questions
    float volume;
} DialogueSyllable;

static inline char dialogue_lower(char c) {
    return c >= 'A' && c <= 'Z' ? c + 32 : c;
}

static inline byte dialogue_is_letter(char c) {
    c = dialogue_lower(c);
    return c >= 'a' && c <= 'z';
}

// y is a vowel inside a word (my, happy), a consonant starting one (yes)
static inline byte dialogue_is_vowel_at(const char* text, int index) {
    const char c = dialogue_lower(text[index]);
    if (c == 'y') {
        return index > 0 && dialogue_is_letter(text[index - 1]);
    }
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

// does the letter at index start a syllable, and what does it say
byte dialogue_syllable_at(const char* text, int index, DialogueSyllable* syllable) {
    const int length = (int) strlen(text);
    if (index < 0 || index >= length || !dialogue_is_letter(text[index])) {
        return 0;
    }
    const byte after_letter = index > 0 && dialogue_is_letter(text[index - 1]);
    const byte after_vowel = after_letter && dialogue_is_vowel_at(text, index - 1);
    *syllable = (DialogueSyllable) { 0, 0, 0, 1 };
    if (dialogue_is_vowel_at(text, index)) {
        if (after_letter) {
            return 0;   // said with the letter before it
        }
        syllable->vowel = dialogue_lower(text[index]);
    } else {
        if (after_letter && !after_vowel) {
            return 0;   // a cluster (pr, st), said by its first consonant
        }
        int ahead = index + 1;
        while (ahead < length && dialogue_is_letter(text[ahead]) && !dialogue_is_vowel_at(text, ahead)) {
            ahead++;
        }
        if (ahead < length && dialogue_is_letter(text[ahead])) {
            syllable->vowel = dialogue_lower(text[ahead]);
        } else if (after_letter) {
            return 0;   // a closing consonant
        }
        syllable->consonant = dialogue_lower(text[index]);
    }
    // the end of its word: a question rises, an exclamation is louder
    int end = index;
    while (end < length && dialogue_is_letter(text[end])) {
        end++;
    }
    if (end < length && text[end] == '?') {
        syllable->inflection = 4;
    } else if (end < length && text[end] == '!') {
        syllable->volume = 1.15f;
    }
    return 1;
}

// a speaker's voice, from its seed (or its name), a stranger's from nothing
Voice get_speaker_voice(ecs* world, entity speaker) {
    uint seed = 1;
    if (zox_valid(speaker) && zox_has(speaker, Seed)) {
        seed = (uint) zox_getv(speaker, Seed);
    } else if (zox_valid(speaker) && zox_has(speaker, ZoxName)) {
        const char* name = zox_getv(speaker, ZoxName);
        for (int i = 0; name && name[i]; i++) {
            seed = seed * 31 + (byte) name[i];
        }
    }
    return voice_from_seed(seed);
}
