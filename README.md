# ciao

The name of this repo was randomly generated, I just needed a place to throw these things together at. The idea of ciao is just to provide single header libraries that are not too hard to use and don't depend on libc. The idea came from wanting to build some C programs without emscripten and I wanted to use these sort of things. I didn't bother looking up if something existed, I just wrote the APIs that I would like to use.

All libraries are under MIT license and forever will be.

A set of helper single header files with the idea of remaining as agnostic to the platform as I can care about, which is not much to be honest. Also these follow the idea of using functions for defining most of the functionality and less macro magic.

- jmnuf_ca.h : Dynamic arrays straight from a crow's beak, caw!
- jmnuf_vista.h : String view usage for managing strings for ease of operation on strings
