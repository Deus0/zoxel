### bugs ###

- chunk seems are there again between lods
  - its something to do with chunk neighbor depths not showing straight away
  - debugging in Sides System with asolid checks
  - Moving back and forth seems to get it to be proper, so its an order of operations issue maybe

- controller hotkeys broken for steamdeck
  - r3 is start
  - rt and lt not working - since theyre analogue
- fix memory growth when returning to main menu and returning
  - probably things like tunks not dying on terrain death
- sometimes raycasting not working on chunk
  - itll just shoot past first block - scale issue?
  - only happens around first chunk I think