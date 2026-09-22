byte syllabells_count = 23;
const char *syllabells[] = {
    "mo",
    "monn",
    "fay",
    "shi",
    "zag",
    "zen",
    "tex",
    "zel",
    "pie",
    "ze",
    "zi",
    "me",
    "mi",
    "el",
    "te",
    "ex",
    "te",
    "bi",
    "si",
    "le",
    "ga",
    "ta",
    "ba"
};

char ascii_to_upper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32; // Convert lowercase to uppercase
    }
    return c; // If already uppercase or not a letter, return the character unchanged
}

char* generate_name(uint seed, char* name) {
    if (name == NULL) {
        return NULL;
    }
    // byte max_syllable_length = 4;
    byte count = seed_range(seed, 2, 5);
    name[0] = '\0';
    for (byte i = 0; i < count; i++) {
        uint seedier = seed + i + 1;
        byte syllable_index = seed_range(seedier, 0, syllabells_count);
        strcat(name, syllabells[syllable_index]);
    }
    name[0] = ascii_to_upper(name[0]);
    return name;
}

char* generate_name_malloc(uint seed) {
    byte max_syllable_length = 4;
    byte count = seed_range(seed, 2, 5);
    char* name = (char*) malloc((count * max_syllable_length) + 1); // Assuming max length of syllable is 3
    if (name == NULL) {
        return NULL;
    }
    generate_name(seed, name);
    return name;
}

zox_sys2(GenerateNameSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_out(GenerateName);
    zox_sys_out(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_o(GenerateName, generate);
        zox_sys_o(ZoxName, name);
        if (!generate->value) {
            continue;
        }
        generate_name(seed->value, name->value);
        generate->value = 0;
        if (dbg_log) {
            zox_log("+ [%s] Generated Name [%lu]:[%s]",
                zox_get_name(e),
                seed->value,
                name->value);
        }
    }
} zox_sys_end(GenerateNameSystem);
