# EbookViewerNX

## Description :

Ebook reader for nintendo switch using mupdf library

## Screenshots :

<p align="center">
    <img src="https://user-images.githubusercontent.com/9384676/69355458-1a9d0000-0c82-11ea-9331-4ef38d3f2a1c.jpg">
    <img src="https://user-images.githubusercontent.com/9384676/69355459-1a9d0000-0c82-11ea-85a3-45ec3955e52c.jpg">
    <img src="https://user-images.githubusercontent.com/9384676/69355460-1a9d0000-0c82-11ea-85ec-f312354da43f.jpg">
    <img src="https://user-images.githubusercontent.com/9384676/69355461-1b359680-0c82-11ea-8195-5ddc72072728.jpg">
    <img src="https://user-images.githubusercontent.com/9384676/69355462-1b359680-0c82-11ea-88b0-be7896ab13d7.jpg">
    <img src="https://user-images.githubusercontent.com/9384676/69355463-1b359680-0c82-11ea-8ca4-534b7b0a3fbe.jpg">
</p>

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
cd ebookViewerNX
make
```

## improvement :

- Load 3 pages to switch page faster (prev, curr, next)
- kill all useless process to save battery
