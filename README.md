# EbookViewerNX

## Description :

Ebook reader for nintendo switch using mupdf library

## Usage :
- Move the .nro in /switch folder
- Move all ebooks in /switch/ebookViewerNX folder
- Warning : ebook title must not have '='
- Enjoy !

## Progress roadmap :
- [x] Read ebook (.pdf, .epub)
- [x] landscape/portrait
- [x] touchscreen support
- [x] info screen
- [x] menu bar
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
cd ebookViewerNX
make
```

## improvement :

- Load 3 pages to switch page faster (prev, curr, next)
- kill all useless process to save battery
