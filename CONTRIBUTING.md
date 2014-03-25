Contributing
============

To contribute to libSPEC there are a few requirements, keep in mind it is a C library.

Code Style
----------

* All code must be written in C11 (or lower).
* Pragmas must be compatible with CLANG and GCC. MSVC compatibility is a bonus, but is not required.
* Naming Convention: libSPEC uses the underscore naming convention.
 * Your functions should have the following syntax `noun_verb_adjective()`, so `pkm_crypt_party()` would crypt the party pkm data.
 * Your types should be suffixed with \_t, such as `type_t`.
* The only acceptable warnings are unused structs or top level data, but only with intention to use them later.
* All files are to use spaces for indentation, spaces are unacceptable and will be formatted away.
* All code must used the attached bracket style, meaning brackets are attached to the function, enum, struct or statement that defines them.