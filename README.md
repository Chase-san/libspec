libSPEC
=======
Library for Specialized Pokemon Editing in C

Supports the saves for GB (RG/RBY/GS), GBA (RSE/FRLG), DS (DPPt/HGSS), and DSi Enhanced (BW/B2W2).

For a total of 21 games spanning 16 years.

**THIS LIBRARY IS UNFINISHED, EXPECT MASSIVE BREAKING CHANGES**

libSPEC now has [documentation](http://chase-san.github.io/libspec/)!

Building
-------

To build the library, you can simply type `make` and both the static and dynamic libraries will be generated in the lib directory.

Build Dependencies
-------

* GNU Make
* CLANG/LLVM or GCC 4.7+
* Little Endian compile target

The source uses standard C11 syntax, so a non GCC/CLANG compiler should work with a different Makefile.