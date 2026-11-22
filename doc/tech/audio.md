# Audio

How sound works in Zoxel: from an effect, a voice or a song's note to your speakers.

## The path of a sound

```text
spawn_sound_effect / spawn_voice_syllable / a music note
→ a Sound entity (SoundData: mono samples, 44100hz)
→ generated (sound_generate_system) or loaded (sound_process_system)
→ TriggerSound → sound_play_system
→ zox_mixer_play (src/sounds/mix/mixer.c)
→ sdl's audio device pulls the mix (src/apps/sdl/wrp/audio.c)
```

- There's no sdl_mixer. Audio is sdl's own device, and the engine mixes.
- All sound data is **mono, 44100hz** (wavs are downmixed when loaded). The mixer makes it stereo.
- A Sound entity lives `sound_play_death_delay` seconds, then it's deleted. The mixer keeps its own copy.

## The mixer

[src/sounds/mix/mixer.c](../src/sounds/mix/mixer.c)

- 64 voices. Each voice is a playing sound: its samples, its position, and its left and right gains.
- `zox_mixer_render` runs on sdl's audio thread. It adds the voices up, then a soft limiter keeps
  the mix under 1 (no harsh clipping).
- When every voice is busy, the one closest to finishing makes way.
- Voices are guarded by `zox_audio_lock` / `zox_audio_unlock`. Finished voices are freed on the main thread.

## 3D sounds

[src/sounds/fun/spatial.c](../src/sounds/fun/spatial.c)

- A sound with a `SoundPosition` is 3D. Without one (ui, music) it plays in the middle.
- The listener is the entity tagged `SoundListener`, the game camera. `sound_listener_system` hands its
  position and rotation to the mixer every frame, so playing sounds follow you as you turn.
- Volume falls with distance like unity's logarithmic rolloff: full within `sound_min_distance` (3),
  then min / distance, silent by `sound_max_distance` (48).
- Pan is equal power, never fully into one ear, and barely pans close by (your own footsteps).
- Give a sound a place with `set_sound_source(world, sound, entity)` or `spawn_sound_effect_at(world, type, position)`.

## Sound effects

[src/sounds/fun/effects.c](../src/sounds/fun/effects.c)

Every world effect is a row in `sound_effects`: an instrument, a range of notes, lengths and volumes.

```c
spawn_sound_effect(world, sound_effect_jump, character);   // from the character (3D)
spawn_sound_effect(world, sound_effect_click, 0);          // ui, in the middle
```

- To add one, add a `SoundEffectType` and its row. `tst sound_balance` checks it straight away.
- **Small speakers barely play under ~150hz.** Laptops and most headphones lose anything lower, so an effect
  at 50hz is silent for most players however loud it is. Keep an effect's body above ~90hz.
- Hits are `instrument_impact` (flesh on flesh: a falling thump, a dull smack, a muffled slap), swings
  `instrument_whoosh` ([impacts.c](../src/sounds/bop/impacts.c)). `tst sound_balance` keeps attacks duller than a 1khz tone.
- Hits on blocks and characters sound like what they hit (stone knocks, dirt thuds, bodies grunt), see [Sounds](sounds.md).
- Spiky instruments (violin) hit their clip limit before they get loud. Fuller ones (square piano, flute,
  guitar, edm) carry better.

## Synths

[src/sounds/bop/synths.c](../src/sounds/bop/synths.c)

Analog style instruments, written as whole notes (`instrument_note_function`): they keep oscillator phases and a
filter's state through the note.

| Instrument | Sound |
|---|---|
| `instrument_synth_pad` | three detuned saws, a filter slowly opening as it swells |
| `instrument_synth_glass` | a struck fm tone, bright then a pure ring, for arpeggios |
| `instrument_synth_bass` | a saw and a sine, a resonant filter plucked open then closing |
| `instrument_synth_lead` | a hollow pulse, its width moving, scooping into its note, vibrato fading in |
| `instrument_synth_wind` | noise whistling through a narrow band at its note |

- Saws and pulses are band limited (polyblep), the filter is a state variable one (`synth_filter`, low and band pass).
- A pulse's offset from its width is taken off: it put the whole mix off centre.

## Voices

[src/sounds/bop/voice.c](../src/sounds/bop/voice.c), [src/dialogues/fun/voice.c](../src/dialogues/fun/voice.c)

NPCs babble their dialogue, a syllable as each one appears, from where they stand.

- A `Voice` is a pitch, a formant size (throat), speed, breath and timbre. Some are alien: a vibrato
  wobble or a ring modulated buzz.
- Every npc's voice comes from its `Seed`: deep, normal, high or tiny.
- A syllable is a consonant's onset (a burst, a hiss, a hum or a glide) into a vowel. Vowels are shaped by
  their formants (mouth resonances). Each syllable falls in pitch, and a question rises.
- Syllables, not letters: "hello there" says he-llo the-re.

## Jingles

[src/sounds/bop/jingles.c](../src/sounds/bop/jingles.c)

Short tunes for moments: `spawn_jingle(world, jingle_level_up)`. Level up, quest taken, quest handed in.

- Zelda's lift, Metroid's mystery: a quick rising arpeggio landing on an open chord, a bell and choir over
  a low root, echoes of the bright notes.
- Each note is a sound with a `SoundDelay`, the mixer starts it on its exact sample.
- `tst jingles` checks they rise and land on a chord, and writes `tst/jingles.wav`.

## Music

[src/musics](../src/musics)

- A music's notes are its ordered children. Each note has a start (`NoteStart`, seconds) and a length.
- `music_play_system` moves the playhead (`MusicTime`) and plays every note it passes, so chords and
  overlapping notes play as they come. It loops after `MusicLength`.
- The main menu plays its synth theme (below). The realm plays generated songs, and they crossfade
  (`crossfade_music`). The imported song (`res/music`, made with `make convert`) is kept: `menu_music_from_file`.

### The menu theme

[src/musics/fun/theme.c](../src/musics/fun/theme.c)

A slow, dark synth piece (a lone planet's title screen), new each boot and each return to the menu, the same at heart.

- **The call**, two bars in every theme (`theme_call`): up a fifth, the octave, falling to a minor sixth that sinks
  back. It's what you remember.
- **From the seed**: the key, a dark mode (aeolian, phrygian, harmonic minor), the tempo, the chords under the call,
  the answer (two bars, left open, then turning home), the arpeggio's pattern, the floor plan.
- **Sections**: intro (wind, a pad, the arpeggio coming in), main (the lead's call and answer), echo (glass plays it
  over a choir), rise (all of it, a pulsing bass, a choir under the lead, bells), outro (thinning out, home).
- Bright notes echo a dotted eighth later, quieter, like a big empty room.
- A music tagged `MenuTheme` is composed by `compose_menu_theme`, others by `compose_song`. `play_menu_music` gives
  a stopped theme a new seed (`menu_theme_seed`), so it's recomposed as it fades in.

### Composing

[src/musics/fun/compose.c](../src/musics/fun/compose.c)

Songs are composed like dungeons are generated:

| Dungeon | Song |
|---|---|
| floor plan | the form: sections in order (intro A A' B A' C outro) |
| room templates | motifs, reused and varied when they come back |
| corridors | chord progressions, a walk through a chord graph, each section ending on a cadence |
| decoration | layers: melody, bass, arpeggio and pad, by the section's role |

Then `perform_song` plays the grid like a player would, into timed notes: strummed chords, grace notes,
swing on some seeds, a few milliseconds of looseness. The same seed always composes the same song.

## Volumes

[src/sounds/set/volume.c](../src/sounds/set/volume.c)

- `master_volume` scales everything, `volume_sfx` the effects and voices, `volume_music` the music.
- Effects sit about 2 to 6 times over the music on small speakers (`tst sound_balance`).
- The menu's song and the generated songs are kept as loud as each other.

## Tests

| Test | Checks |
|---|---|
| `sound_mixer` | panning, turning around, distance, voices finishing, the limiter |
| `sound_balance` | every effect and voice over the realm's song, through a small speaker filter |
| `sound_instruments` | every instrument generates, writes `tst/instruments/*.wav` |
| `voice_sounds` | syllables, voices from seeds, pitch, vowels, consonants, writes `tst/voices.wav` |
| `music_compose` | 20 seeds: form, key, chords, performance, writes `tst/song.wav` |
| `music_theme` | 20 menu themes: the call in each, keys, modes, forms and answers vary, as loud as the realm's song, writes `tst/menu_theme.wav` |
| `music_timing` | notes play on their starts, chords together, looping |

- Tests run `--background`, which closes the audio device: they stay silent even when they turn sounds on.
- To listen to a test, or record what the game plays:

```bash
SDL_AUDIO_DRIVER=disk bash bsh/tst.sh resume_no_punch --sounds
# the mix is written to bin/zoxel_*_dev/sdlaudio.raw (stereo float32, 44100hz)
```
