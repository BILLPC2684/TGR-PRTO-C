# TGR-PRTO-C
TheGameRazer is a custom retro 16-bit style emulator made in C,\
this is still in prototype stages...

# Is the current build runnable? (will update)
## For Build: v0.0.19
Runnable Build: `Yes`\
Stable CPU EMU: `Yes`    (not completed)\
Has Video:      `Mostly` (SDL | uses OpenGL)\
Has Audio:      `No`\
Has Inputs:     `No`     (comming up)\
Has Network:    `No`
## NOTE: if the emulator crashes or glitch out, check your ROM before reporting(if someone else made the ROM then contact them)

# Are ROMs free?
Yes for the most part, now ppl can charge or setup a donation system for their ROMs
## We have no responsibility if your ROM gets hacked/modded or republished without your permission

# What is required/suggested for building?
## Packages required for ALL 
Lua5.2+: http://www.lua.org/download.html     (look in `Binaries` for windows)\
SDL2:    https://wiki.libsdl.org/Installation
## Linux
Packages required: GCC (install from your PackageManager)
## Windows
*undefined*
## MAC(OSX)
*undefined*

# Building the Emulator
## Linux
open a terminal in the current folder and run: `./build.sh <ROM>` (optional: autorun `<ROM>` after building)\
to run the emulator, run: `./TGR-C <ROM>` (replace `<ROM>` with the filepath to your ROM)
## Windows
*undefined* (download `.exe` build from github `releases`)
## MAC(OSX)
*undefined*

# How to make ROMs
the file `TGR-info.txt` has all the information you should need including the InstuctionSet, look at the wiki for more info...

# Want to join are discord server? it's free
##Invite link: https://discord.gg/PWAf8ek



# compiling assembly programs:

To compile programs for TGR, a python script is included named compile_TGRASM.py.
It takes 2 arguments: the first is the input file and the second is the output file.

The assembly code can contain any of the following instructions:


- labels: (any word with a : after it, just like nasm assembly)
- jmp <labelname> 
- mov <destination(reg)> <source(reg or int)>
- disp <a(reg)>
- cmpeq <a(reg)> <b(reg)> (skips next instruction if false)
- cmpgt <a(reg)> <b(reg)> (skips next instruction if false)
- cmplt <a(reg)> <b(reg)> (skips next instruction if false)
- push <a(reg or int)>
- pop <a(reg)>
- swap (swaps the top of the stack with the item below)
- call <labelname>
- ret

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