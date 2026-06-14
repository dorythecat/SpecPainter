# SpecPainter
> A spectrum painter for all kinds of radios

## The project
SpecPainter is, in theory, a simple program. It comes from a desire to create a software capable of putting ANY image into a format that can then be transmitted into the wonders of the radiofrequency spectrum.

SpecPainter will be composed of two modules:
- SpecPainterCore (SPC) will be the main program. It will be a downloadable executable, compatible with as many operating systems as I can get working.
- SpecPainterWeb (SPW) will be a web accessible interface, usable from computers, tablets, and phones, that will run locally in a static webpage (mainly so it can be hosted on GitHub Pages). Due to this, it will have reduced functionality, but will still count with many of the features of the main program, if a bit more limited at times.

## The methodology
SPC will be built on pure C. That's right, not C++, nor any of that fancy nonsense. I want this to be a program that can be as efficient as possible in every architecture. To maintain this methodology, and, to teach myself a few more facets of programming and everything around it, I will also not be using ANY external libraries. That means that image decoding, painting, user interface, mathematical operations, storage structures, memory handling, and every other little nuanced thing we could usually offload to some other libraries, is not going to have that lucky faith here. Instead, everything will be derived from first principles, giving us zero dependencies, and the uttermost efficiency (or maybe not, who can say) out of this little program.

For SPW, I will not go so heavily, and will rely a bit more heavily on standard JavaScript functionality to make my way through the development, though still with a laser focus on efficiency and reliability.
