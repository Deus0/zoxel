# Plan: the core Dirty / Update (done 2026-11-21)

The core `Dirty` (writers add it) and `Update` (the frame after, `dirty_system` in zoxp_remove) were used as an event
system; observers only need `Dirty`. Audited 2026-11-21: ~50 writers, readers in 6 groups.

| Group | Readers | Plan |
|---|---|---|
| settings | 14 observers on Dirty, setting_save_system on Update | done (Dirty); the save stays (one write a frame) |
| stats, items | none (icon_labels was dead) | **A: remove the 13 writes** (done) |
| quests | quest_tracker_updated on Dirty and on Update, zox_event_quest_marker on Dirty | keep: Update catches a quest just given (its tracker linked a frame later) |
| sounds | sound_play, sound_play_ref, sound_debug systems on Update | **B: observers on Dirty** (done) |
| grids | grid_system on Update (needs sizes settled) | keep |
| texts | text_update, texts_position, text_background, text_parent_background, glyph_spawn on Update | **C: design** |

## A. Dead writes (done)

Stats (damage, regen, bonuses, level ups, consumables, plus buttons) and items (activate, consume) added `Dirty`,
read only by `stat_icon_label_system`, commented out: removed, with its file. perf_stream release, before / after:
24.6 / 28.7ms a frame (its usual spread), the most moved ids are particles' (~37k), core Dirty wasn't in the list.

## B. Sounds and quests to observers on Dirty

- sounds: the 3 play / debug systems become observers on `[none] Sound, [none] core.Dirty` (handing a sound to the
  mixer is light, a few a frame). Their writers set the data before the Dirty, so it's ready.
- quests: kept on both: on Dirty a quest just given has no tracker link yet, Update a frame on catches it.
- done: systems + observers 566 + 97 to 563 + 100 (the same count: 3 systems now observers, one of them main
  thread in zoxp_spawn); perf_fly 25.7, 21.9ms (its usual spread). Tests: sound_trigger, music_plays, sound_balance,
  voice_sounds, hit_sound_links, footsteps, water_splash, music_crossfade, quest_dialogue.

## C. Texts, and a non fragmenting Dirty

`Dirty` can only become non fragmenting (no table moves) when no system matches it: `dirty_system` (Dirty to Update)
does. Texts are the case that needs it: their 5 systems run on `Update`, glyph_spawn_system early in the frame
(zoxp_spawn), so `Update` lasting the whole next frame is what lets it see a text dirtied mid frame.

Options:
1. Keep as is: Dirty fragmenting, dirty_system and update_system stay (2 systems), texts unchanged.
2. Texts get their own fragmenting pair (`TextDirty` / `TextUpdate`, `zoxc_dirty(Text)`), the core Dirty becomes
   non fragmenting with only observers, `dirty_system` / `update_system` go: +2 tags for texts, -2 core systems,
   no table moves for every other Dirty (settings, quests, sounds, grids would need the same for grid_system).
3. An observer on the core Dirty adds `Update` for the entities that need systems: removes dirty_system, but Update
   added mid frame would be removed at the frame's end before zoxp_spawn's glyph spawn sees it.

Recommended: 1 until texts show up in a profile's moved most (they don't now); then 2.

## Done: Dirty an event, Refresh / Update for systems

Chosen (user): reuse core components, not a pair per module. In core:

- `Dirty`: non fragmenting, an event: observers react the moment it's added, `dirty_cleanup_event` removes it. No
  table moves, so any entity can be marked (stats, items, quests, sounds, settings): uis and other modules can
  react to it (animations) without a cost when nothing does.
- `Refresh` / `Update`: fragmenting, for systems: writers add `Refresh`, `refresh_system` (zoxp_remove) makes it
  `Update` for the frame after, `update_system` removes it. Texts (their 5 systems, glyph spawning in zoxp_spawn),
  grids and the settings' save (once a frame) use it.
- A writer adds `Dirty` for observers, `Refresh` for systems, both when both react (settings).
- A. the stat and item Dirty writes stay (restored): events for uis and modules, nearly free now.
- B. quests on Dirty only: a quest just given is dirtied again when its tracker links (tracker.c); with Dirty
  removed at each merge that second add fires again, the tracker reads the quest whole (the Update observer gone).

Profile, after (release): perf_stream 21.0ms a frame (before 24.6, 28.7), perf_fly 18.5ms (before 18.5 to 29), the
same moved most (particles); pipeline systems / observers 430 / 111, lint 563 + 100. Tests: settings 5, quests 4,
sounds, texts, tooltips, navigators, editor, game_log, menus, stats, items, respawn (29).
