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

char* generate_name(uint seed) {
    byte max_syllable_length = 4;
    byte count = seed_range(seed, 2, 5);
    // 2 + (rand() % 3); // Random number between 2 and 4
    char* name = (char*) malloc((count * max_syllable_length) + 1); // Assuming max length of syllable is 3
    if (name == NULL) {
        return NULL;
    }
    name[0] = '\0';
    for (byte i = 0; i < count; i++) {
        uint seedier = seed + i + 1;
        byte syllable_index = seed_range(seedier, 0, syllabells_count);
        strcat(name, syllabells[syllable_index]);
    }
    name[0] = ascii_to_upper(name[0]);
    return name;
}
