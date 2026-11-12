# Future
- Bigger ideas for later

## Alien Languages
- npcs speak their own languages, not english, for the alien world feeling (like hyperlight drifter)
	- the dialogue box is a translator, what they say is translated, not spoken
	- major cities speak a common tongue, the wilds their own languages
- each language its own sounds: a set of phonemes, syllable shapes and rhythm
	- voices already babble syllables (sounds/bop/voice.c, dialogues/fun/voice.c)
	- a language could pick which consonants and vowels it uses, and how syllables are built (cv, cvc, ccv)
	- the babble would follow the language, not the english letters
- the translator could be imperfect: untranslated words shown in glyphs, learning a language translates more

## Multi Chunk Models
- if models grow past what a frame can mesh (a depth 5 mesh is ~5ms): models split into chunks, like Teardown
	- cap a model chunk at depth 4 (16^3), bigger models are several chunks
	- each chunk meshes on its own, so the mesh budget splits a model across frames
	- an edit only rebuilds its chunk, how chunked voxel engines keep rebuilds small
	- bones / skeleton meshes per chunk, lods per chunk
