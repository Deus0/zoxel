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
zoxc_float3(SoundPosition);
zoxc_float(SoundDelay);         //! seconds it waits before playing (a jingle's notes)     //! where a 3D sound plays from, sounds without it play in the middle
zox_tag(SoundListener);         //! hears the 3D sounds (the game camera)
// realm sounds (doc/sounds.md): a sound entity of the realm, what a material sounds like when hit
zoxc(SoundEffectData, SoundEffect);
zox_tag(HitSoundLink);          // a block or character to its realm hit sound
zox_tag(HitSoundVoiced);        // a body that grunts when hit (in its voice)
#include "sound_data.c"
#include "sound_data_ref.c"

void zox_components_sounds(ecs* world) {
    zoxd_tag(Sound);
    zoxd_byte(InstrumentType);
    zoxd_state(TriggerSound);
    zoxd_state(GenerateSound);
    zoxd_state(ProcessSound);
    zoxd_double(SoundLength);
    zoxd_float3(SoundPosition);
    zoxd_float(SoundDelay);
    zoxd_tag(SoundListener);
    zoxd(SoundEffectData);
    zoxd_nf_link(HitSoundLink);
    zoxd_tag(HitSoundVoiced);
    zoxd_float(SoundFrequency);
    zoxd_float(SoundVolume);
    zoxd_int(SoundFrequencyIndex);
    zoxd(SoundDataRef);
    zoxd_dest(SoundData);
}
