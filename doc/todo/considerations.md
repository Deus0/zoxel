# Considerations
[Things still under consideration]

[Questions]
- Should we add headers so we can compile modules quicker and easier
- What is the best

[Tests]
- Marching Cubes option on terrain mesh

[Refactors]
- Settings and tuning values as entities, not c globals (2026-11-27): i.e. the sun's defaults (sun_day_length,
  sky_night_tint in src/sky/fun/sun.c) and the render_ globals. The old ecs kept settings in global entities; Unity
  has better helpers (ScriptableObjects, singletons). Needs thought: how a system finds them (a singleton
  component, a link from the realm or app), how they're edited and saved, and what stays a plain const
