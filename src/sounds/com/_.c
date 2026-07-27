zox_tag(Sound);
zoxc_byte(InstrumentType);
zoxc_state(GenerateSound);        //! A state event for generating sounds
// NOTE: renamed PlaySound to TriggerSound temporarily, cause of windows.h conflict
zoxc_state(TriggerSound);         //! A state event for playing sounds
zoxc_state(ProcessSound);
zoxc_float(SoundFrequency);     //! The frequency of the generated sound
zoxc_float(SoundVolume);     //! The frequency of the generated sound
zoxc_int(SoundFrequencyIndex);     //! The frequency of the generated sound
zoxc_double(SoundLength);       //! The length of a sound

#include "sound_data.c"
#include "sound_data_ref.c"

void define_components_sounds(ecs* world) {
    zoxd_tag(Sound);
    zoxd_byte(InstrumentType);
    zoxd_state(TriggerSound);
    zoxd_state(GenerateSound);
    zoxd_state(ProcessSound);
    zoxd_double(SoundLength);
    zoxd_float(SoundFrequency);
    zoxd_float(SoundVolume);
    zoxd_int(SoundFrequencyIndex);
    zoxd_dest_old(SoundData);
    zoxd(SoundDataRef);
}
