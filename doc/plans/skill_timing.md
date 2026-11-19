# Skill timing (plan)

Skills and item uses timed like an mmo's (cast time, cooldown, a global cooldown). Instant ones (punch, place, eat) are
animation based: the press starts a swing, the effect lands at the swing's impact. Timing must be exact for combat.
Started: the punch is instant (step 2). Open questions at the end.

The user's answers (2026-11-19):
- any cast (a warmup) shows a castbar
- the punch only damages on its hit: its windup is just the swing's animation, not a cast

## Now

- An action's timers (`_/timing/timers`): `ActivateBegin` (the press) starts a warmup (`WarmupAt`, `WarmupTime`), its
  end triggers `Activate` and a cooldown (`CooldownAt`, `CooldownTime`). The skill or item system acts on `Activate`
  (melee_system hits, item_activate_system places, item_consume_system eats).
- Punch has a 0.25s warmup: a short cast. The swing (bones) starts at the warmup (melee_swing_system,
  item_swing_system poll `WarmupAt`) and lasts warmup + cooldown, its hit halfway, lined up to the timer.
- Imprecise: each step waits for the next system or frame (the warmup ends, then activate, then melee in the next
  frame's spawn phase): the hit lands up to a frame or two (16-33 ms) after the fist does, more at low frame rates.
- The swing starters poll each frame to notice a warmup started: reactions written as systems.

## The plan

1. **Times, as in an mmo**, on the skill or item: a cast time (`WarmupTime`, 0 for instant ones), its cooldown
   (`CooldownTime`), and a global cooldown on the user shared by its actions (later, see the questions).
2. **The swing has its own length** (an attack speed, like a weapon's speed), not warmup + cooldown: `SwingSpeed` from
   the skill or item (later its user's stats), the cooldown separate. A swing can be shorter than the cooldown.
3. **An instant action starts its swing on the press**, its cooldown then too. No warmup to wait for.
4. **The effect lands at the swing's impact mark** (`SwingMark`, bones/sys/swing_mark.c), observers react:
   - punch: the hit (raycast, damage, its sound) at the hit mark
   - place: the block set at the place mark
   - eat: the stat at the first bite, munches and crumbs at each (actions/eve/eat.c already)
   The hit and the animation can't drift apart: they're one event.
5. **Exact time:** a mark's time is `SwingStart + at * SwingSpeed`, computed, not counted in frames. Its observer runs
   the frame after, but anything timed (combat log, a dot's start, hit windows, a server's check) uses the mark's
   time. Frame rate only changes how soon it's seen.
6. **Cast time skills** (spells, later): the cast fills the warmup (a cast bar), then a release swing plays, the
   projectile at its release mark. The same pattern.
7. **No polling:** the press is an event; the swing starters (melee_swing_system, item_swing_system) go, the press
   starts the swing. Per frame work stays in systems (the timers counting, swing_mark_system finding marks), reactions
   in observers.

## Where things live

- Animation in bones (skeleton swings: `SwingStart`, `SwingSpeed`, `SwingKind`, `SwingTarget`, the marks, the poses in
  bones/ik/sys/punch.c, the head's turn) or animations (generic timed clips; a mark list on a clip could move there
  when tweens need events too, see the AnimationEndEvent todo). Gameplay only starts a swing
  (`skeleton_swing_begin`) and reacts to its marks: animations are its crutches, it doesn't time itself off them.
- Gameplay in its module: skills (melee hit), items (place; consumables as an `items/consumables` submodule: the
  consume, the bite observer; it reads the food from the skeleton's `SwingAction` now, so it needs no actions module).
- Timers (`_/timing/timers`) stay generic: cast, cooldown, a global cooldown.

## Steps

1. A swing's own length on skills and items (`SwingSpeed` on them), cooldown separate; tests: punch_wind_up's checks
   move to the press.
2. Done: punch instant, its warmup 0, its swing from the press (melee_system pays and starts it; still a system on
   `Activate`, an observer of the press later), the hit an observer of the hit mark (melee_hit, skills/eve/melee_hit.c);
   the skeleton has `SwingAction` (what's swung) for observers; tst punch_wind_up: no hit before the mark, one hit
   ~10 ms after it. The swing's length is the cooldown until step 1.
3. Place and eat the same (item_activate_system, item_consume_system to observers of their marks).
4. `items/consumables` submodule (the bite observer moves from actions/eve/eat.c).
5. Global cooldown on the user (if wanted now), the actionbar shows it (skill_overlay_system).
6. A castbar for casts (shoot and aura have a 0.5s warmup and none now): the user's cast, its skill's icon and name,
   filling over its warmup.

## Questions

- A global cooldown now, or with spells?
- Where does a swing's speed come from: the skill or item, a user stat (an attack speed, agility), or the walk state
  (todo: "Base Speed of swing off walk speed")?
- Can a swing be cancelled (moving, switching action, a stun) before its impact, and does the cooldown still run?
- Npcs: the same path (their attacks go through melee_system now), so their hits land at their swing's impact too.
