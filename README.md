# uwsh - UnixWare 7 Bourne Shell (sh) Port to Linux

This repository contains a port of the original UnixWare 7 Bourne shell (`sh`) to modern Linux. This work is intended for **private research and historical preservation purposes only**.

## Project Highlights

- **32-bit to 64-bit Stability**: Refactored pointer handling and memory management to work reliably on 64-bit Linux systems (compiled in 32-bit mode for compatibility).
- **Modern Memory Management**: Replaced the legacy AT&T `sbrk`-based custom allocator with a standard `malloc`-based system.
- **Improved Portability**: Removed SVR4-specific dependencies (e.g., `pfmt`, `gettxt`, privilege management) and replaced them with standard POSIX equivalents.
- **Fixed Hangs & Crashes**: Resolved long-standing issues with `EOF` handling and signal management that were present in early versions of the port.

## Building

To build the shell, you will need `gcc` and `make`. The current build targets for 64-bit Linux.

```sh
make
```

The resulting executable will be named `sh`.

## Repository Structure

- `include/`: Portability headers and Linux shims.
- `*.c`: Core shell source code.
- `Makefile`: The primary build file.

## Credits

- **Mario (@wordatet)**: Lead tinkerer, porting architect, and historical preservationist.
- **AI Collaborators (Gemini + Claude)**: Assisted with code refactoring, portability shims, and stabilization.

## Licensing

- **Porting Work**: All compatibility shims (`shim.c`, `include/compat.h`), `Makefile.linux`, and refactoring logic created for this Linux port are released under the **Apache License 2.0** + a notice of the original copyright holder (AT&T/Xinuos).
- **Original Source**: The original Bourne shell source code remains the proprietary property of its respective copyright holders (AT&T/Xinuos). This code is included here for historical research and preservation purposes.

## Disclaimer

This repository is intended strictly for educational and research use. The author does not claim ownership of the original UNIX source code and will remove this repository upon request by the copyright holder.
