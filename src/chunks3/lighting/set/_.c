
byte sunlight = 255;        // full sunlight
byte zox_debug_lights = 0;
byte darklight = 16;        // min light
byte light_air_decay = 24;

// NOTE: These 'distances' can be very random, since propogation happens in many ways... so theyre more like algorithm limiters... - can remove if the rules are good to not break
byte light_propogation_distance = 12;
byte darklight_propogation_distance = 254;  // just keep going?