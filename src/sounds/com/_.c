zox_tag(Sound);
zoxc_byte(InstrumentType);
zoxc_byte(GenerateSound);        //! A state event for generating sounds
// remember: renamed PlaySound to TriggerSound temporarily, cause of windows.h conflict
zoxc_byte(TriggerSound);         //! A state event for playing sounds
zoxc_byte(ProcessSound);
zoxc_float(SoundFrequency);     //! The frequency of the generated sound
zoxc_float(SoundVolume);     //! The frequency of the generated sound
zoxc_int(SoundFrequencyIndex);     //! The frequency of the generated sound
zoxc_double(SoundLength);       //! The length of a sound
#include "sound_data.c"
#include "sound_data_ref.c"

void define_components_sounds(ecs_world_t *world) {
    zoxd_tag(Sound);;
    zoxd_byte(InstrumentType);;
    zoxd_byte(TriggerSound);;
    zoxd_byte(GenerateSound);;
    zoxd_byte(ProcessSound);;
    zoxd_double(SoundLength);;
    zoxd_float(SoundFrequency);;
    zoxd_float(SoundVolume);;
    zoxd_int(SoundFrequencyIndex);;
    // zoxd_arrayd(SoundData);
    zox_define_component_w_dest(SoundData);;
    zoxd(SoundDataRef);;
}