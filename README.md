# stfs
Modern C++23 library designed to parse Xbox 360 Secure Transaction File System (STFS) packages (CON, PIRS, LIVE).

## Features
- Header & signature parsing
- Metadata parsing (V1 and V2), including license entries and volume descriptors
- File listing parser
- Block offset resolution and hash-table-aware block chain
- File extraction ( Optional SHA-1 chain-of-trust verification )
