# uwsh - UnixWare 7 Bourne Shell (sh) Port to Linux

This repository contains a port of the original UnixWare 7 Bourne shell (`sh`) to modern Linux. This work is intended for **private research and historical preservation purposes only**.

## Project Highlights

- **32-bit to 64-bit Stability**: Refactored pointer handling and memory management to work reliably on 64-bit Linux systems.
- **Hybrid Build System**: Supports both GCC (stability) and PCC (historical accuracy).
- **Modern Memory Management**: Replaced the legacy AT&T `sbrk`-based custom allocator with a standard `malloc`-based system.
- **Improved Portability**: Removed SVR4-specific dependencies (e.g., `pfmt`, `gettxt`, privilege management) and replaced them with standard POSIX equivalents.
- **Fixed Hangs & Crashes**: Resolved long-standing issues with `EOF` handling and signal management.

## Hybrid Build System

This port supports a dual-compiler "Hybrid" workflow:

- **Standard Build (GCC)**: High stability, 64-bit validated, and recommended for usage.
  ```sh
  make
  ```
- **Vintage Build (PCC)**: Compiled with the Portable C Compiler to ensure historical K&R code integrity.
  ```sh
  make vintage
  ```
- **Super Paranoid Verification**: Runs the deep stress suite (Recursion, Massive Churn, Job Storms).
  ```sh
  make paranoid
  ```

## Repository Structure

- `include/`: Portability headers and Linux shims.
- `*.c`: Core shell source code.
- `Makefile`: The primary hybrid build file.

## Credits

- **Mario (@wordatet)**: Lead tinkerer, porting architect, and historical preservationist.
- **AI Collaborators (Gemini + Claude)**: Assisted with code refactoring, portability shims, and stabilization.

## Licensing

- **Porting Work**: All compatibility shims (`shim.c`, `include/compat.h`), `Makefile`, and refactoring logic created for this Linux port are released under the **Apache License 2.0** + a notice of the original copyright holder (AT&T/Xinuos).
- **Original Source**: The original Bourne shell source code remains the proprietary property of its respective copyright holders (AT&T/Xinuos).

## Disclaimer

This repository is intended strictly for educational and research use. The author does not claim ownership of the original UNIX source code and will remove this repository upon request by the copyright holder.
