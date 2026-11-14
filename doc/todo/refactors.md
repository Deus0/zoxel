# Refactors
[AcrossEngine Refactors]

[Tests]
- Move any tests into the lower modules
	- many atm are just thrown in higher ones, we should be testing lower modules first

[Settings]
- We should move them all to use set/_.c folders+files to make it faster to navigate and test stuff

[Consolidate Systems]
- Move core/ sub modules into _
- cleanup all _/T folders into just _/fun/T.c if they dont have many functions

[Consolidate Components]
* if byte event, make into tag, so we can remove after
- GenerateModel
- GenerateTexture

[Frame Based]
* replace with tag events so they're exact and more predictable
- The tests wait a fixed number of frames instead of using the new done signal
- Remove delay event use and just use observers and generic events
- Item icons get textures baked one second after
- Opening a menu one frame later with delay_event(..., 0.01)
- Quest nodes end after 0.05 seconds
- Tests have 100 waits, guesses when things finish
	- can probably just check when initialized is removed for some
	- can use Generating more to check when theyre done too
	- this should speed tests up too
