# ciao

The name of this repo was randomly generated, I just needed a place to throw these things together at. The idea of ciao is just to provide single header libraries that are not too hard to use and don't depend on libc. The idea came from wanting to build some C programs without emscripten and I wanted to use these sort of things. I didn't bother looking up if something existed, I just wrote the APIs that I would like to use.

All libraries are under MIT license and forever will be.

A set of helper single header files with the idea of remaining as agnostic to the platform as I can care about, which is not much to be honest. Also these follow the idea of using functions for defining most of the functionality and less macro magic.

No I don't directly support C++, I'm not adding that extern C thing. Do it yourself if you need it but this project is based just in C and does not take into consideration C++.

- ciao_ca.h    : The crow cahs (caws) and the murder of crows grows. This is a dynamic array.
- ciao_vista.h : Vistas into strings for ease of string handling. This is a string view.
- ciao_strut.h : The model runway struts, this doesn't strike poses but it manages a string buffer. This is a string builder.
