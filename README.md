# CHIP-8

Has your friend ever sent you a `.ch8` file? Well, this is the perfect application
to see that file's contents. Using SDL2 and an emulation of a Chip-8 VM opcodes you
can see what peculiar message your friend decided to encode into that file.

![Outlaw Demo](res/outlaw.png)

From `rom/outlaw.ch8`

![IBM Demo](res/ibm.png)

From `rom/chip8-test-suite.ch8`

## To compile and use
- Download the code.
- Paste SDL2 inside a `lib` folder in the `chip-8` folder.
- Compile the program using `make` in the `chip-8` folder from the command line.
- Use `./chip8.exe [rom_name]` in the command line to run the emulator.

### Compiling notes
- SDL2 binaries may need to be on your machine's path.
- Options such as directories in the [Makefile](Makefile) may also need to be changed.
- You may need to install a C compiler if not on Linux. I suggest gcc.
Use a package manager like [homebrew](https://docs.brew.sh/Installation) or [scoop](https://scoop.sh/) to install gcc.

## Emulator specs
- 4 bytes of Memory
- 16 input keys corresponding to hexadecimal (0-F)

````
                                Keyboard Diagram

     Chip8                                             QWERTY Corresponding
    1 2 3 C                                                   1 2 3 4
    4 5 6 D                                                   Q W E R
    7 8 9 E                                                   A S D F
    A 0 B F                                                   Z X C V
````