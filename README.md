![alt text](https://github.com/BILLPC2684/TGR-PRTO-C/raw/master/bin/TGR-logo(8x).png)

# TGR-PRTO-C
TheGameRazer is a custom retro 16-bit style emulator made in C,\
this is still in prototype stages...

# Websites:
Official TGR Github: https://billpc2684.github.io/TGR-PRTO-C/ (WIP)\
BILLPC2684's offical: http://billcraft.byethost33.com/TheGameRazer/

# Is the current build runnable? (will update)
## For Build: v0.0.42a Alpha
 - Runnable Build:  `Yes`      [theres a slight glitch with clock usage...]
 - Stable CPU EMU:  `Yes`      (Not completed)
 - Stable Memory:   `Yes`      (128 MB of RAM | 64 MB of VRAM)
 - Has Video:       `Yes`      (SDL | uses OpenGL)
 - Has Audio:       `No`       (Requesting help)
 - Has Inputs:      `Yes`      (SDL Window Input)
 - Has Network:     `No`       (Requesting help)
 - Saveable data:   `Yes`      (Fully functional)
 - Compiler Status: `Outdated` (Needs to be updated and has some bugs)
## NOTE: if the emulator crashes or glitch out, check your ROM before reporting(if someone else made the ROM then contact them)

# What is the next expected build version to be updated?
## that will be Build v0.0.46x at Dec/xx/2019

# Why does GitHub updates take so long?
 - i myself(BILLPC2684) is mostly the only one working on this project, so it's a slow process
 - life issues and school blocking from getting work done(plus not to mention other projects)
 - i like to push the most stable build that i can before making the next big update
 - also i'm working on making python3 able to be compiled into TGR bytecode(the design is still being worked on)
## PLEASE BE PATIENT... Updates will come ASAP on schedule(hopefully)

# Are ROMs free?
Yes for the most part, now people can charge or setup a donation system for their ROMs
## We have no responsibility if your ROM gets hacked/modded or republished without your permission

# Building the Emulator
## For Linux
 - All required Packages are located in: `linux-requirements.txt`\
   (all needs to be installed from your PackageManager)
 - open a terminal in the current folder and run: `make` (this compiles the emulator for you to run)\
 - to run the emulator, run: `./build/TGR-$(arch) <ROM> <ARGS>`\
   (replace `<ROM>` with the filepath to your ROM | for `<ARGS>` look down)
 - you can leave the `<ROM>` and `<ARGS>` blank and just press `[CTRL]+[O]` once the emulator has started
## For Windows
 - *undefined*\
   *requesting help: windows seems to be unstable to build onto...*
 - (download `.exe` build from github `releases`, *if they exists*)
## For MAC(OSX)
 - *undefined*\
   requesting help: i've never used a MAC before...*

# Emulator Arguments
ARGs are not needed to run a ROM but can be useful for debuging...
 - `-s <ms>`     or `--slow <ms>`:     replace `<ms>` with the delay you want to have per instruction
 - `-d`          or `--debug`:         enabled `debug mode` for telling you whats going on
 - `-pl`         or `--pauseLoad`:     waits for user to press enter after running the BIOS
 - `-wi`         or `--waitInput`:     requires `debug mode` to work, after every instruction it waits for you to continue
 - `-sk <inst>`  or `--skip <inst>`:   replace `<inst>` with the amount of instructions you want to skip
 - `-i`          or `--info`:          prints the current `FramesPerSec`/`InstructionsPerSec` in the terminal every second.
 - `-nu`         or `--noUnicode`:     prints DataDump in ascii instead of unicode
 - `-np`         or `--noPrint`:       disables print instruction to terminal
 - `-di`         or `--devInfo`:       shows bref dev info...
 - `-fr`         or `--forceRender`:   forces every frame to render
 - `-db`         or `--debugBIOS`:     does the same as `--waitInput` but for the BIOS
 - `-sb`         or `--skipBIOS`:      skips the BIOS from running(saves ROM debuging time)
 - `-nd`         or `--noDump`:        skips BIOS/ROM dump from the terminal
 - `-hi`         or `--hudInfo`:       does the same as `--info` but for the HUD
 - `-2x`         or `--render2x`:      renders at 2x the size
 - `-sl`         or `--scanLines`:     renders at 2x with scanlines(50%) effect
 - `-px`         or `--pixelate`:      renders at 2x with pixelate effect
 - `-3x`         or `--render3x`:      renders at 3x the size
 - `-sl3`        or `--scanLines3x`:   renders at 3x with scanlines(25%) effect
 - `-sl35`       or `--scanLines3x5`:  renders at 3x with scanlines(75%) effect
 - `-px3`        or `--pixelate3x`:    renders at 3x with pixelate effect
 - `-si`         or `--showInput`:     shows inputs
 - `-sav <file>` or `--extSAV <file>`: replace `<file>` with the `.sav` file you want to use !!not fully tested!!

# How to make ROMs and compiling assembly programs:
check out out the WorkingInProgress offical github page: https://billpc2684.github.io/TGR-PRTO-C/ \
(this should have everything you need)

# Want to join our  discord server? it's free
## Invite link: https://discord.gg/PWAf8ek
<br>
<br>

# How to make a TGR Assembly File?
To compile programs for TGR, theres a python script is included named `compile_TGRASM.py`.\
It takes 2-3 arguments: the first is the input file and the second is the output file(the 3rd is not needed unless says it so).\
Note: there might be slight probblems with it skiping some instructions that are in the code, we are trying to fix that...\
\
The assembly code can contain any of the following instructions: (alittle broken but up-to-date)
 - labels: (any word with a : after it, just like nasm assembly)
 - `raw`     `<0xOO,0xAB,0xCI,0xII,0xII,0xII>` (O: Instruction[8-bit] | A/B/C: 3 REGS[12-bit] | I: Intermediate[24-bit])
 - `mov`     `<destination(reg)> <source(reg or int)>` (mov 8-bit int)
 - `lmov`    `<destinationA(reg)> <destinationB(reg)> <label name>` (mov 16-bit int)
 - `add`     `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `sub`     `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `mul`     `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `div`     `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `inc`     `<destination(reg)>`
 - `dec`     `<destination(reg)>`
 - `and`     `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `or`      `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `xor`     `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `bsl`     `<destination(reg)>`
 - `bsr`     `<destination(reg)>`
 - `not`     `<destination(reg)>`
 - `split`   `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `combine` `<A(reg)> <B(reg or int)> <destination(reg)>`
 - `jmp`     `<label name>`
 - `led`     `<0xRRGGBB>`
 - `cmpeq`   `<A(reg)> <B(reg or int)>`
 - `cmplt`   `<A(reg)> <B(reg or int)>`
 - `cmpgt`   `<A(reg)> <B(reg or int)>`
 - `rpos`    `<A(reg)> <B(reg)>`
 - `wram`    `<A(reg or address)> <B(reg if A is used for address)>`
 - `rram`    `<A(reg or address)> <B(reg if A is used for address)>`
 - `wvram`   `<A(reg or address)> <B(reg if A is used for address)>`
 - `rvram`   `<A(reg or address)> <B(reg if A is used for address)>`
 - `rsav`    `<A(address)> <B(reg)>`
 - `wsav`    `<A(address)> <B(reg)>`
 - `rrom`    `<A(address)> <B(address)> <C(reg)>`
 - `hlt`     [`<HALT_INFO>`](https://billpc2684.github.io/TGR-PRTO-C/systemSpecs.html) click to see them
 - `disp`    `<A(reg)> <B(reg[optional])> <C(reg[optional])>`
 - `flags`   `<A(reg)>`
 - `dsend`   `<A(reg)> <deviceID> <instruction>`
 - `drecv`   `<A(reg)> <deviceID> <instruction>`
 - `icout`   `<A(reg)> <B(reg)>`
 - `exec`    [`<EXEC_INFO>`](https://billpc2684.github.io/TGR-PRTO-C/systemSpecs.html) click to see them
 - `rbios`   `<A(address)> <B(address)> <C(reg)>`
 - `push`    `<A(reg or int)>`
 - `pop`     `<A(reg or int)>`
 - `call`    `<label name>`
 - `ret`     takes no arguments
 - `swap`    takes no arguments
 - `gclk`    `<A(reg)> <reset?>`
 - `wait`    `<MS Delay>`
 - `nop`     What? you expect me to take a argument? NOPe sorry, not today!

## When making a function should follow the following protocol concept: (optional)
```assembly
function_name:
	pop g ;save the return adress in an unused register (g in this case)

	<some code not using the g register here>

	mov h,<return value> ;the h register is preserved by function calls so return values can work

	push g ;push g to the top of the stack so ret knows where to return to
	ret
call [function_name]
```
