typedef struct {
    entity e;       // element toggled
    entity player;  // player toggling
    byte value;     // new value of toggle
} ToggleEventData;

zoxc_function(ToggleEvent, void, ecs*, const ToggleEventData*);