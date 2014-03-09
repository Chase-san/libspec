libspec
=======
Library for Specialized Pokemon Editing in C

Supports the saves for GB (RG/RBY/GS), GBA (RSE/FRLG), DS (DPPt/HGSS), and DSi Enhanced (BW/B2W2).

For a total of 21 games spanning 16 years.

<span style="color:red">__THIS LIBRARY IS UNFINISHED, EXPECT MASSIVE BREAKING CHANGES__</span>

Building
-------

To build the spec.exe, the testing exe, use

```
make
```

To build libspec_d.a, use

```
make lib
```

To build libspec.a, use

```
make rlib
```

To build with GCC rather then CLANG, define GCC on the make line.
```
make GCC=1
```

Build Dependencies
-------

* GNU Make
* CLANG/LLVM or GCC 4.7+

The source uses standard C11 syntax, so a non GCC/CLANG compiler should work with a different Makefile.