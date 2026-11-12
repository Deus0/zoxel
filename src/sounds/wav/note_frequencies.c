// equal temperament from C0 (16.35hz), A4 (index 57) is 440hz, up to B7
const float note_frequencies[] = {
    // 0: octave 0
    16.35f,     // C0
    17.32f,     // C#0
    18.35f,     // D0
    19.45f,     // D#0
    20.60f,     // E0
    21.83f,     // F0
    23.12f,     // F#0
    24.50f,     // G0
    25.96f,     // G#0
    27.50f,     // A0
    29.14f,     // A#0
    30.87f,     // B0
    // 12: octave 1
    32.70f,     // C1
    34.65f,     // C#1
    36.71f,     // D1
    38.89f,     // D#1
    41.20f,     // E1
    43.65f,     // F1
    46.25f,     // F#1
    49.00f,     // G1
    51.91f,     // G#1
    55.00f,     // A1
    58.27f,     // A#1
    61.74f,     // B1
    // 24: octave 2
    65.41f,     // C2
    69.30f,     // C#2
    73.42f,     // D2
    77.78f,     // D#2
    82.41f,     // E2
    87.31f,     // F2
    92.50f,     // F#2
    98.00f,     // G2
    103.83f,    // G#2
    110.00f,    // A2
    116.54f,    // A#2
    123.47f,    // B2
    // 36: octave 3
    130.81f,    // C3
    138.59f,    // C#3
    146.83f,    // D3
    155.56f,    // D#3
    164.81f,    // E3
    174.61f,    // F3
    185.00f,    // F#3
    196.00f,    // G3
    207.65f,    // G#3
    220.00f,    // A3
    233.08f,    // A#3
    246.94f,    // B3
    // 48: octave 4
    261.63f,    // C4
    277.18f,    // C#4
    293.66f,    // D4
    311.13f,    // D#4
    329.63f,    // E4
    349.23f,    // F4
    369.99f,    // F#4
    392.00f,    // G4
    415.30f,    // G#4
    440.00f,    // A4
    466.16f,    // A#4
    493.88f,    // B4
    // 60: octave 5
    523.25f,    // C5
    554.37f,    // C#5
    587.33f,    // D5
    622.25f,    // D#5
    659.26f,    // E5
    698.46f,    // F5
    739.99f,    // F#5
    783.99f,    // G5
    830.61f,    // G#5
    880.00f,    // A5
    932.33f,    // A#5
    987.77f,    // B5
    // 72: octave 6
    1046.50f,   // C6
    1108.73f,   // C#6
    1174.66f,   // D6
    1244.51f,   // D#6
    1318.51f,   // E6
    1396.91f,   // F6
    1479.98f,   // F#6
    1567.98f,   // G6
    1661.22f,   // G#6
    1760.00f,   // A6
    1864.66f,   // A#6
    1975.53f,   // B6
    // 84: octave 7
    2093.00f,   // C7
    2217.46f,   // C#7
    2349.32f,   // D7
    2489.02f,   // D#7
    2637.02f,   // E7
    2793.83f,   // F7
    2959.96f,   // F#7
    3135.96f,   // G7
    3322.44f,   // G#7
    3520.00f,   // A7
    3729.31f,   // A#7
    3951.07f,   // B7
};
#define notes_max 96

// Function to find the closest note index for a given frequency
int find_note_index(float frequency) {
    int closest_index = 0;
    float min_diff = fabs(note_frequencies[0] - frequency);
    // Iterate through the note_frequencies array to find the closest match
    for (int i = 1; i < notes_max; i++) {
        float diff = fabs(note_frequencies[i] - frequency);
        if (diff < min_diff) {
            min_diff = diff;
            closest_index = i;
        }
    }
    return closest_index;
}
