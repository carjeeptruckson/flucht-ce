# Flucht CE - A TI-84 Plus CE Port

**Wall jump as high as you can!**

This is a port of the addictive one-button Thumby game, "Flucht" by Sergi Lazaro, to the TI-84 Plus CE graphing calculator. [1, 2] Experience the classic, challenging gameplay of Flucht, now with colorful graphics on your favorite calculator!

## Gameplay

The core mechanics remain faithful to the original:
*   **Jump:** Press the designated jump key ([Enter] by default) to leap from wall to wall.
*   **Wall Grab:** Your character will automatically grab onto the opposite wall.
*   **Variable Jump Height:** The longer you hold the jump key (up to a certain limit), the higher and further you'll jump. Mastering this is key to reaching new heights!
*   **Ascend:** Keep jumping to climb higher and higher, avoiding hazards.
*   **Hazards:** Watch out for spikes! Touching them means game over.
*   **Challenge:** The game gets progressively harder as you ascend.

## Features

*   Classic "Flucht" gameplay, ported from the Thumby. 
*   Colorful graphics and sprites designed for the TI-84 Plus CE screen.
*   Variable jump height based on key-hold duration.
*   Procedurally generated level elements (hazards, decorative wall elements).
*   High score saving (stored in an AppVar named "FLUCHTHS").
*   Built using the CE C Toolchain.

## Installation

1.  Download the latest release `FLUCHT.8xp`.
2.  Transfer `FLUCHT.8xp` to your TI-84 Plus CE using TI Connect CE software or a compatible linking program.
3.  Run the program from the `[prgm]` menu on your calculator or using arTIfiCE

## Controls

*   **[Enter] (or your assigned jump key):** Jump / Hold for higher jump
*   **[Clear]:** Exit the game

## Building from Source

This project is built using the [CE C Toolchain](https://github.com/CE-Programming/toolchain).
1.  Ensure you have the latest CE C Toolchain installed and configured.
2.  Clone this repository: `git clone [your-repo-url]`
3.  Navigate to the project directory: `cd FluchtCE` (or your project name)
4.  Run `make` to build the project. The output `FLUCHT.8xp` will be in the `bin` directory.

## Credits

*   **Original Thumby Game "Flucht":** Sergi Lazaro ([itch.io page](http://sergilazaro.itch.io/flucht), [GitHub](https://github.com/sergilazaro/flucht-thumby-pygame)) [1]
*   **TI-84 Plus CE Port:** [Your Name/Handle]
