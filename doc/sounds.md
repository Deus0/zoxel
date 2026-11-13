# Sounds

What things sound like when they're hit. Each block or character kind links a sound entity of its realm.
For the mixer, effects, voices and music, see [Audio](audio.md).

## Hit sounds

- A hit sound is an entity (prefab `realm_sound`) with `SoundEffectData`: an instrument, a range of notes,
  lengths and volumes. It is never played itself. A hit reads it and spawns a sound.
- A block or character kind links its sound with `HitSoundLink`. The entity is the sound (see [Entities, Not Ids](coding_standards.md#entities-not-ids)).
- [src/sounds/bop/hits.c](../src/sounds/bop/hits.c)

## Materials

Named effects in [src/sounds/fun/effects.c](../src/sounds/fun/effects.c), instruments in
[src/sounds/bop/impacts.c](../src/sounds/bop/impacts.c).

| Material | Instrument | Sounds like | Used by |
|---|---|---|---|
| `hit_sound_flesh` | impact | a body thump, and a grunt | characters |
| `hit_sound_slime` | squelch | a wet bubble rising in pitch | slimes |
| `hit_sound_stone` | knock | a dry click and inharmonic rings | stone, bricks, obsidian, dungeon walls |
| `hit_sound_dirt` | thud | a low body falling in pitch, dull noise | dirt, grassy dirt, roads, debris |
| `hit_sound_sand` | crunch | grainy noise | sand |
| `hit_sound_wood` | tock | a hollow ring at 1 and 2.76 times its note | wood |
| `hit_sound_leaves` | rustle | noise that swells and fades | leaves, grass, flowers |

## Linking

- `link_hit_sound(world, e, parent, &hit_sound_stone)`: spawns the sound entity under `parent` (the realm or
  biome spawning the kind), links `e` to it, and returns it.
- Each one is nudged by its parent's `Seed`: up to 2 semitones, 0.9 to 1.1 times as long. So one biome's stone
  rings a little differently from another's.
- Called where the realm makes its kinds: [biome_blocks.c](../gam/zoxel/sys/biome_blocks.c),
  [blocks.c](../gam/zoxel/sys/blocks.c), [dungeons realm_blocks.c](../src/dungeons/fun/realm_blocks.c),
  and [characters.c](../gam/zoxel/sys/characters.c).
- Npcs link their prefab's sound when they spawn (`setup_character3`).
- Sounds are parented to the realm or biome, never to a character prefab. A prefab's children are copied
  into every npc spawned from it.

## How a hit plays

- A block hit (`block_damage`): `spawn_hit_sound_at`, the block kind's sound at the block, then a chime if it breaks.
- A character hit (skill damage): `spawn_character_hit_sound`, the body's sound from the character.
- Without a link, the old effect plays (`sound_effect_block_damage`, `sound_effect_hit`).

## Grunts

- A voiced sound (`HitSoundVoiced`) grunts on 3 of 4 hits. Flesh adds the tag, and a character with no link
  (a player) counts as voiced.
- A grunt is the character's dialogue voice (`voice_from_seed` of its `Seed`, so it sounds like it talks):
  pitched lower (0.85), slower (1.6), breathier, a falling "ugh", "eh" or "ah".
- It goes through a `tanh` soft distortion, like a strained throat.
- `generate_voice_grunt` in [src/sounds/bop/voice.c](../src/sounds/bop/voice.c)

## Adding a material

1. Write its sound function in `impacts.c`.
2. Add an instrument id in `src/sounds/dat/instrument_types.c` and a row in the table in `src/sounds/bop/instruments.c`.
3. Add a `hit_sound_x` effect in `effects.c`, and add it to the `hit_sounds` list.
4. Call `link_hit_sound` where the kind spawns.
5. Run `hit_sounds`, `sound_balance` and `hit_sound_links`.

## Tests

- `hit_sounds`: each material:
  - generates and dies away
  - differs from every other in brightness or pitch
  - stone and sand are brighter than dirt
  - nudges stay within 2 semitones
  - grunts stay in range
  - writes `tst/instruments/grunt_N.wav`
- `sound_balance`: every material is at least 2x the music under it, on small speakers.
- `sound_instruments`: writes `tst/instruments/knock.wav`, `thud.wav` and so on, to listen to.
- `hit_sound_links` (in game): every realm block and npc links a sound, slimes squelch, stone knocks, dirt thuds.

## Footsteps

- A character tagged `Footsteps` (players for now) plays the block underfoot's hit sound each stride, at 35% volume
  and 60% length (`spawn_footstep_sound`, `characters/jump/sys/footsteps.c`).
- `FootstepDistance` adds up the ground walked while grounded, a stride is 1.6 times its half height.
- Main thread, in the spawn phase: flecs can't create entities (sounds) on worker threads.

## Next

- Npcs near the player could make footsteps too (tag them by distance, many would cost)
