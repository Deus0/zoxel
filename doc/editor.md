## Editor ##

# Editor
- Line Instancing for tools
- Blue Canvas Edge lines for Editor Mode
- Model Inspector for Blocks, Characters, Items

# Tools #
- terminal log text list
- chunk debugger - show lods of chunks etc
- character debugger - show number per tunk2D of npcs

# Position Debugger
- Debug Component for Position Curve (float3 curve)
- Position Y chart - showing fluctuations - use to stabilize physics
- Position atm shakes when grounded
- If unstuck disabled, it will fall through ground when loading

# Nodes
- Nodegraph UI - show a node graph full of nodes and connections
- simple print blueprint function that logs a blueprint nodes

- Terminal UI
- test key for keyboard navigation mode

- TextureData UI in Inspector Label
- Scrollbar horizontal
- bar at bottom of texture to resize individual component
- Component Element for a list (MeshIndicies, etc)
	- using folded button group
- Resizable Windows
	- add corner button to bottom right
	- debug when release + hold system for the dragging
- Editor Style function that sets:
	- window_color
	- font_size
	- element_font_color + outline
	- header_font_size
	+ header_font_color + outline
- dock hierarchy/window on left of window
	- left drag to edge and release
	- add a button on header to restore from docked
- tag filter dropdown on header of hierarchy
	- Block, Item, Stat, Etc
	- Just add a list of tags for now to it
- Slider UI - visually show points to with little up arrows
Engine [Builds]:
- windows build in zelder, make a bsh/windows.sh
- make builder:
	- build android
	- push to itch io
- move import modules into a new module - which will only be included when building zengine workflow (with another define)
- prefabs should spawn children too
Module [Nodes]:
- create generic variables for our nodes
- our head blueprint needs a l_eye_size r_eye_size
- set these variables on process
- for eye size, its a paint sphere, we can hook a input node up to the fill node
