![alt text](https://github.com/BILLPC2684/TGR-PRTO-C/raw/master/bin/TGR-logo(8x).png)

# TGR-PRTO-C
TheGameRazer is a custom retro 16-bit style emulator made in C,\
this is still in prototype stages...

# Websites:
Official TGR Github: https://billpc2684.github.io/TGR-PRTO-C/ (WIP)\
BILLPC2684's offical: http://billcraft.byethost33.com/TheGameRazer/

# Is the current build runnable? (will update)
## For Build: v0.0.34 Alpha
 - Runnable Build: `Yes`
 - Stable CPU EMU: `Yes`     (Not completed)
 - Stable Memory:  `Yes`     (128 MB of RAM | 64 MB of VRAM)
 - Has Video:      `Yes`     (SDL | uses OpenGL)
 - Has Audio:      `No`      (Requesting help)
 - Has Inputs:     `Yes`     (SDL Window Input)
 - Has Network:    `No`      (Requesting help)
 - Saveable data:  `Yes`     (Fully functional)
## NOTE: if the emulator crashes or glitch out, check your ROM before reporting(if someone else made the ROM then contact them)

# Are ROMs free?
Yes for the most part, now ppl can charge or setup a donation system for their ROMs
## We have no responsibility if your ROM gets hacked/modded or republished without your permission

# What is required for building?
## Linux
Required Packages are located in: `linux-requirements.txt`\
(all needs to be installed from your PackageManager)
## Windows
*undefined*\
*requesting help: windows seems to be unstable to build onto...*
## MAC(OSX)
*undefined*\
*requesting help: i've never used a MAC before...*

# Building the Emulator
## Linux
open a terminal in the current folder and run: `make` (this compiles the emulator for you to run)\
to run the emulator, run: `./build/TGR-$(arch) <ROM> <ARGS>`\
(replace `<ROM>` with the filepath to your ROM | for <ARGS> look down)
## Windows
*undefined* (download `.exe` build from github `releases`, *if they exists*)
## MAC(OSX)
*undefined*

# Emulator Arguments
ARGs are not needed to run a ROM but can be useful for debuging...
 - `-d`         or `--debug`:       enabled `debug mode` for telling you whats going on
 - `-i`         or `--showInfo`:    prints the current `FramesPerSec`/`InstructionsPerSec` in the terminal every second.
 - `-pl`        or `--pauseLoad`:   waits for user to press enter after running the BIOS
 - `-s <ms>`    or `--slow <ms>`:   replace <ms> with the delay you want to have per instruction
 - `-wi`        or `--waitInput`:   requires `debug mode` to work, after every instruction it waits for you to continue
 - `-sk <inst>` or `--skip <inst>`: replace <inst> with the amount of instructions you want to skip
 - `-i`         or `--showInfo`:    shows basic info
 - `-nu`        or `--noUnicode`:   prints DataDump in ascii instead of unicode
 - `-np`        or `--noPrint`:     disables print instruction to terminal
 - `-di`        or `--devInfo`:     shows bref dev info...
 - `-fr`        or `--forceRender`: forces every frame to render
 - `-db`        or `--debugBIOS`:   does the same as `--waitInput` but for the BIOS
 - `-sb`        or `--skipBIOS`:    skips the BIOS from running(saves ROM debuging time)

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
The assembly code can contain any of the following instructions:

- labels: (any word with a : after it, just like nasm assembly)
- mov   <destination(reg)> <source(reg or int)> (load/mov 8-bit int)
- jmp   [<label name>]
- disp  <a(reg)>
- add   <a(reg)> <B(reg or int)> <c(reg)>
- sub   <a(reg)> <B(reg or int)> <c(reg)>
- mul   <a(reg)> <B(reg or int)> <c(reg)>
- div   <a(reg)> <B(reg or int)> <c(reg)>
- inc   <a(reg)>
- dec   <a(reg)>
- cmpeq <a(reg)> <b(reg or int)> (skips next instruction if false)
- cmpgt <a(reg)> <b(reg or int)> (skips next instruction if false)
- cmplt <a(reg)> <b(reg or int)> (skips next instruction if false)
- push  <a(reg or int)> (pushes data to top of the stack)
- pop   <a(reg)> (pulls data from top of the stack)
- swap  (swaps the top of the stack with the item below)
- call  [<label name>] (calls label as function)
- ret   (returns from function)
- nop   (litteraly does nothing)
- dsend <source(reg)> <Device(4-bit int)> <Inst(8-bit int)>
- drecv <source(reg)> <Device(4-bit int)> <Inst(8-bit int)>
- wait  (debug mode only)
- gclk  <a(reg)> <reset(optinional)> (CPU clock outputs to a(reg) and reset is 2-bit)
- raw   <0xOO,0xAB,0xCI,0xII,0xII,0xII> (O: Instruction[8-bit] | A/B/C: 3 REGS[12-bit] | I: Intermediate[24-bit])
- hlt   (stop the program!)

# USE CALLFN IN STEAD OF CALL!

In general, do not use the call instruction. Use the supplied macro in std.asm (included with %include assembly/std.asm) called CALLFN(). It will take care of saving and restoring all registers so they won't bestroyed by the called function.

When making a function one should follow the following protocol:

```assembly
functionname:
	pop g ;save the return adress in an unused register (g in this case)

	<some code not using the g register here>

	mov h,<return value> ;the h register is preserved by function calls so return values can work

	push g ;push g to the top of the stack so ret knows where to return to
	ret
```

a typical function call routine:
```assembly
CALLFN(square,5)
mov a,h ;move out of h just to be sure, h is often used for return values so you never know what will overwrite it

p a
hlt

mult:
	pop g

	pop a
	pop b

	mov c,0
	mov d,0

	multloop0:
		cmpeq d,b
		jmp [multend0]
		add c,a,c
		dec b
		jmp [multloop0]
multend0:
	push g

	mov h,c
	ret
```

