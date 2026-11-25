# Rendering

## Notes

- The 3D particle renderer caps each draw batch at 100,000 instances. Color and size uploads use that clamped count, while the position upload uses the full ECS batch count. A batch above the cap can request a position upload larger than its GPU buffer. Use the same clamped count for every attribute upload and the draw. [Renderer](../../src/particles/particles3/sys/render.c) · [Buffer allocation](../../src/particles/particles3/sha/particle.c)
