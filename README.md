# EbookViewerNX

## Description :

Ebook reader for nintendo switch using mupdf library

## Screenshots :

<p align="center">
    <img src="https://user-images.githubusercontent.com/9384676/69355458-1a9d0000-0c82-11ea-9331-4ef38d3f2a1c.jpg">
    <img src="https://user-images.githubusercontent.com/9384676/69355459-1a9d0000-0c82-11ea-85a3-45ec3955e52c.jpg">
    <img src="https://user-images.githubusercontent.com/9384676/69355461-1b359680-0c82-11ea-8195-5ddc72072728.jpg">
    <img src="https://user-images.githubusercontent.com/9384676/69355463-1b359680-0c82-11ea-8ca4-534b7b0a3fbe.jpg">
</p>

## Usage :
- Move the .nro in /switch folder
- Move all ebooks in /switch/ebookViewerNX folder
- Warning : ebook title must not have '='
- Enjoy !

## Control :
```
- Home menu :
- Launch ebook : A
- Next book : Right ( with Right joystick )
- Previous book : Left ( with Right joystick )
- Quit : Start button
- Help : X button
	
- Reading :
- Hide/Show bar : A
- Portrait/Landscape : ZR
- Next page : Right ( with Right joystick )
- Previous page : Left ( with Right joystick )
- Next 10 pages : Up ( with Right joystick )
- Previous 10 pages : Down ( with Right joystick )
- Quit : Start button
- Help : X button
```

## Progress roadmap :
- [x] Read ebook (.pdf, .epub)
- [x] landscape/portrait
- [x] touchscreen support
- [x] info screen
- [x] menu bar
- [ ] Portrait view in home menu
- [ ] .cbr support
- [ ] Sort by last read
- [ ] print current page (10/250)
- [ ] Double page
- [ ] cache to load pages faster
- [ ] vibrate when button pressed
- [ ] Customisable control in config file
- [ ] Add progress bar for visual loading
- [ ] multiple view in home

## Building (For developers) :

```
git clone --recursive https://github.com/SegFault42/ebookViewerNX
cd ebookViewerNX/libs
make -f Makefile.mupdf
cd ../
make
```

## improvement :

- Load 3 pages to switch page faster (prev, curr, next)
- kill all useless process to save battery
