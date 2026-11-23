# Stats

A character's numbers: its level, health, energy and mana, what regenerates them, its damage and defence, its
movement speed, and the attributes a player raises with points.

- Module: [src/stats](../../src/stats) (its ui: [src/stats/ui](../../src/stats/ui))
- The realm's stats: [gam/zoxel/sys/stats.c](../../gam/zoxel/sys/stats.c)
- A character's: `fill_character_base_stats` ([src/stats/sys/character.c](../../src/stats/sys/character.c))

## The idea

Every stat is an entity. The realm makes one of each kind, as a prefab, and each character's stat is an instance of
the realm's (so it shares its name, icon and colour), parented to the character (`zox_set_parent_non_fragment`).

```text
realm
├─ soul (StatLevel)              ── character's soul: its level, ExperienceValue / ExperienceMax
├─ health (StatState, StatHealth)── character's health: StatValue of StatValueMax
├─ health regen (StatRegen)      ── adds to its StatLink (the character's health) a second
├─ physical damage (StatBase, StatDamage)
├─ movement speed (StatBase, StatSpeed) ◄── StatScaleLink: agility
├─ agility (StatAttribute, StatAgility)
└─ stat point (StatCurrency)     ── given at each level up (StatLevelReward)
```

Find a character's stat by its tag: `zox_get_child_by_id(world, character, zox_id(StatHealth))`. That walks its
children, so do it when something changes, not every frame for every npc.

## Kinds

Each kind is a prefab ([pre/_.c](../../src/stats/pre/_.c)), all from `prefab_stat` (`Stat`, `StatValue`).

| Kind | Tag | Holds | Made by | Examples |
|---|---|---|---|---|
| Level | `StatLevel` | its level, `ExperienceValue`, `ExperienceMax` | `spawn_stat_level` | soul |
| State | `StatState` | `StatValue` of `StatValueMax` | `spawn_stat_state` | health, energy, mana |
| Regen | `StatRegen` | a second's regen, its `StatLink` the state it fills | `spawn_stat_regen` | health regen |
| Base | `StatBase` | a value systems read | `spawn_stat_base` | physical damage, defence, movement speed |
| Attribute | `StatAttribute` | points, raising the stats linked to it | `spawn_stat_attribute` | strength, agility, intelligence |
| Currency | `StatCurrency` | points to spend, `StatLevelReward` a level | `spawn_stat_currency` | stat point, skill point |

Tags name the stats systems look for: `StatHealth`, `StatSoul`, `StatDamage`, `StatDefence` (its damage stat's
`StatDefenceLink`), `StatSpeed`, `StatAgility`, `StatIntelligence`, `StatPoint`, `SkillPoint`.

## The realm's stats

`stats_realm_spawn_system` (gam/zoxel/sys/stats.c) makes them with `spawn_realm_stat(world, realm, prefab, name,
texture, colour)`. The texture is the icon's file name in res/textures/stats (`movement_speed` is
movement_speed.bmp); a missing one logs "Stat Texture not found". There are icons there for stats not made yet
(attack speed, magic damage, willpower, perception).

| Stat | Kind | Raised by (attribute) |
|---|---|---|
| soul | level | |
| health, energy, mana | state | strength, agility, intelligence |
| health, energy, mana regen | regen | vitality, dexterity, wisdom |
| physical damage, physical defence | base | strength, endurance |
| movement speed | base, 1 a player's | agility |
| stat point, skill point | currency, 1 each a level | |
| strength, vitality, endurance, intelligence, wisdom, agility, dexterity, charm, luck | attribute | |

## Attributes raise base stats

`set_realm_stat_scale(world, stat, attribute, amount)` links a realm stat to the attribute raising it
(`StatScaleLink`) and how much each point adds (`StatScaleAmount`). The amounts are at the top of
gam/zoxel/sys/stats.c (`attribute_health_scale`...).

- A player spends a stat point on an attribute (the stats window's plus button), the attribute gets `DataDirty` and
  `attribute_bonus_event` runs `apply_attribute_bonus` ([fun/bonus.c](../../src/stats/fun/bonus.c)): every stat of
  the player's scaled by it gains the difference (a state its max and value, others their value). `StatBonus` keeps
  what's been added, so only the change is applied.
- An npc's attributes are its level (`npc_attribute_at`, `npc_attribute_level_increase`): it gets agility and
  intelligence, and its movement speed is worked out from its agility once, as it's made (`npc_speed_at`): a base
  (`npc_speed_base`) plus its agility times the realm speed's `StatScaleAmount`, at most `npc_speed_max`. A low level
  slime moves at 0.58 of a player, a villager (level 16) at 1.03.

## A character's stats

`fill_character_base_stats(world, character, realm, soul, health, is_player)` gives it every realm stat it doesn't
have (`get_realm_base_stats` lists them: the soul, the states, regens, bases, and agility and intelligence):

- npcs: their values from their soul (`npc_stat_start`): states full at `*_base + soul * *_level_increase`
  ([set/_.c](../../src/stats/set/_.c)), regens `base_regen`, speed and attributes as above, other bases the realm's
  value.
- a player: its energy and mana start at their base (they fill), its movement speed the realm's 1, its attributes
  and currencies at 0 (it raises them).
- Character kinds (gam/zoxel/sys/characters.c) are prefabs, filled once at realm generation; their instances copy
  the stats. `character_stats_system` fills any an npc is missing. It checks what the character has, not what's
  spawned in the same deferred batch, so call it once per character.

## Over time

| System | Does |
|---|---|
| `stat_regen_system` | a regen adds its value times `regen_rate` a second to its state, a quarter (`combat_regen_mul`) in combat; dirties it when its whole number changes |
| `pre_death_system`, `death_system` | health at 0: `PreDeath`, its regens off, then `Dead`, `DeathDirty`, `DisableMovement` |
| `experience_system` | a killer gains experience from its victim's soul |
| `level_up_system` | experience past its max: a level, the max times `experience_max_multiplier`, currencies rewarded |

## Who reads them

- combat: damage adds `StatDamage`, defence takes off `StatDefence`
- ai: an npc's movement speed scales its attack and flee speeds and a slime's pounce (it squashes longer and leaps
  slower when slow); its intelligence how long it watches its threat before each attack (`npc_observe_time`,
  ai/sys/behaviour_system.c). `character_speed` and `character_intelligence` ([fun/speed.c](../../src/stats/fun/speed.c))
  read them, on a change, not every frame.
- ui: statbars (health over heads, the player's), the stats window, its icons and tooltips (src/stats/ui)

## Tests

`npc_stats` (every npc has each realm stat once), `npc_speed` (a slime slow and slow witted, a villager quicker),
`player_respawn`, `statbars_hide`, `id_window`, `window_toggle`.
