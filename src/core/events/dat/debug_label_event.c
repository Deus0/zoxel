typedef uint (*DebugLabelEvent) (
    ecs *world,
    const entity player,
    char *buffer,
    const uint size,
    uint index
);