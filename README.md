# SK++


# TO-DO

- [X] Make webviews resize when window resizes
- [X] Make SK++ work on MacOS
- [ ] Make SK++ work on iOS
- [ ] Make SK++ work on Linux
- [ ] Make SK++ work on Android

- Make windows transparent + accept mouse and keyboard input
	- [X] Windows
	- [ ] MacOS

- Implement window move area (mousedown+drag) (via CSS class or attribute, or alike)
	- [ ] Windows
	- [ ] MacOS
	
- Implement window resize at corners when window.transparent=true
	- [ ] Windows
	- [ ] MacOS

- Implement window min/max size when resizing
	- [X] Windows
	- [ ] MacOS

- Implement window style types (with titlebar, without titlebar, dialog without parent, dialog with parent, etc..)
	- [ ] Windows
	- [ ] MacOS
	
- Implement window event callbacks, both in hard backend SK_Window and to soft backend
We should be able to easily define and handle window events such as WM_PAINT, WM_SIZE and others in the hard backend.
In the soft backend we should merely just be notified when these events are fired so that we can do stuff in the soft backend and/or views accordingly.

	- [ ] Windows
	- [ ] MacOS

- Implement onWindowMoveBegin, onWindowMoveEnd, onWindowMoving, onWindowResizeBegin, onWindowResizeEnd, onWindowResizing
SK:Muon needs to know when certain events happen.
	- [ ] Windows
	- [ ] MacOS