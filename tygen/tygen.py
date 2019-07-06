#!/usr/bin/python3
import sys,os,time

vars = [] #this holds the info/RAM address for vars.
#[name,size,address] example: ["par3",16,0x00000B7]

funcs = [] #this holds the functions
#[name,address] example: ["a",0x0076]

if len(sys.argv) < 2:
 print("Please provide a input[.ty] and output[.tgr] address...")
else:
 with open(sys.argv[1],"r") as file:
  program = file.read()
 #
 print(program)
 
 
 
 
 #8-bit var
 #ReadRAM vars[i][2],rA
 #
 #
 
 
 
 
 
 #
 #print("out: "+str(out).."\nvars: "+str(vars)+"\nfunctions: "+str(funcs))
#
