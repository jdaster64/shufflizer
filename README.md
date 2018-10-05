### Paper Mario: TTYD (U) Shufflizer v1.00 by Jdaster64

### Credits  
* **PistonMiner** for TTYD scripting disassembly, the initial code for the "rel" framework, and the elf2rel, gcipack, and rellink utilities. (GitHub repo for these tools [here](https://github.com/PistonMiner/ttyd-tools).)
* PistonMiner and **Zephiles** for their work on TTYD symbol maps, and the GCI loader code (GitHub repo for the loader [here](https://github.com/Zephiles/TTYD-Randomizers).)

### Overview
The Paper Mario: TTYD **Shufflizer** is a mod that introduces the following random elements based on a seed determined by the save file name:
* Replaces all one-time field item pickups and Charlieton / Pianta / badge shop items with a random permutation of a fixed pool of items.
* Randomizes all item shops such that they contain each normal item once, plus a selection of random recipes and badges.
* (Optionally) shuffles the order of enemy encounters in the Pit of 100 Trials, and balances their stats according to their placement within the Pit.

In addition, the player can pull up a menu to globally adjust enemies' HP, ATK, and DEF power, and toggle on/off Pit floor randomization (hold Z and use the D-Pad to make / adjust menu selections, and hold Z + press L to toggle on/off options).

More detailed information can be found at the following link: **[Shufflizer Doc](https://goo.gl/VhiqZH)**

**The Shufflizer currently only supports the North American NTSC version of TTYD**, and is designed to work only with a **retail disc or unmodified ISO** (md5 of **db9a997a617ee03bbc32336d6945ec02**).

**Combination with any other mods or cheat codes is not officially supported.**

### Installation Guide
* Copy the GCI containing the Shufflizer module to the memory card. (This file contains code required to run Shufflizer, and has no relation to your TTYD save file.)
  * For **Dolphin** users:
    * Use the memcard manager located in **Tools > Memcard Manager (GC)** to import the GCI into the file Dolphin is using for a memory card.
    * Alternatively, select "GCI Folder" for the format of the memory card in Slot A (in **Config dialog > GameCube > Device settings**), and copy the .gci file directly into the folder, located by default at **{Dolphin directory}/GC/USA/Card A**.
  * For **Nintendont** or other console users:
    * Use the appropriate homebrew apps / folders, etc. to copy the file from your SD card to your GC memory card or virtual memory card.
* Use the Gecko loader cheat code in the "relloader-ttyd" directory to make the game load the Shufflizer GCI.
  * For **Dolphin** users:
    * Right-click your TTYD game in the games list and go to **Properties > Gecko Codes**, and add the US Gecko loader code (at **relloader-ttyd/REL_Loader/Gecko/REL_Loader_US_Gecko.txt**).
    * (For older revisions of Dolphin v5, instead go to **Properties > Show Defaults**, and copy the text code into the Gecko codes section of the "G8ME01.ini" file that opens, marked by the text **[Gecko]**; if the section doesn't exist, add it.)
    * Enable cheats in **Config dialog > General**, and enable the loader code in **Properties > Gecko Codes** for TTYD.
  * For **Nintendont** users:
    * Copy the loader GCT (at **relloader-ttyd/REL_Loader/GCT/G8ME01.gct**) onto your SD card, either to **/codes/G8ME01.gct** or into the folder with your TTYD ISO.
    * Enable cheats in Nintendont's settings before booting the game.
  * Alternatively, you can use the Action Replay loader code (e.g., if you have a physical Action Replay disc).