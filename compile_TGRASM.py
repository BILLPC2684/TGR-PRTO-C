#!/usr/bin/env python3

import sys,os

def reg(x):
 try:
  return "abcdefgh".index(x)
 except ValueError:
  return -1

def combinetochar(a,b):
 if b < 0xf and a < 0xf:
  return b | a << 4
 else:
  raise ValueError("values too large to fit in char")

def include(FN):
 global asm
 try:
  with open(FN,"r") as file:
   asm+=file.readlines()
 except IOError:
  print("ImportError: File \""+FN+"\" could not be found...")
  exit(-1)

def replace(st,rp):
 loc = st.find(rp)
 if loc == -1: return st[:loc]+st[loc+len(rp):]
 else: return st

ascii = "................................ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~.................................¢£¤¥¦§¨©ª«¬®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ.............................."
def dumpData(name, data, use_non, start, end):
 bytes = []
 print("._______._______________________________________________.________________.\n|{}".format(name),end='')
 if   len(name) < 2:  print("      ",end='')
 elif len(name) < 3:  print("     ",end='')
 elif len(name) < 4:  print("    ",end='')
 elif len(name) < 5:  print("   ",end='')
 elif len(name) < 6:  print("  ",end='')
 elif len(name) < 7:  print(" ",end='')
 print("|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F|0123456789ABCDEF|\n|-------|-----------------------------------------------|----------------|\n|0000000|",end='')
 j,l,m = 1,0,""
 for i in range(start,end):
  if i >= len(data): break
  if j > 15:
   bytes.append(data[i])
   if data[i] < 0x10: print("0",end='');
   print("{}|".format(hex(data[i])[2:]),end='')
   for k in range(16):
    if use_non: print(ascii[bytes[i-16+k]],end='')
    else:       print(ascii[bytes[i-16+k]],end='')
   l=0
   print("|\n|",end='')
   if   i+1 < 0x1:        print("0000000{}|".format(hex(i+1)[2:]),end='')
   elif i+1 < 0x10:       print("000000{}|".format(hex(i+1)[2:]),end='')
   elif i+1 < 0x100:      print("00000{}|".format(hex(i+1)[2:]),end='')
   elif i+1 < 0x1000:     print("0000{}|".format(hex(i+1)[2:]),end='')
   elif i+1 < 0x10000:    print("000{}|".format(hex(i+1)[2:]),end='')
   elif i+1 < 0x100000:   print("00{}|".format(hex(i+1)[2:]),end='')
   elif i+1 < 0x1000000:  print("0{}|".format(hex(i+1)[2:]),end='')
   elif i+1 < 0x10000000: print("{}|".format(hex(i+1)[2:]),end='')
   j = 0
  else:
   if data[i] < 0x10: print("0",end='')
   print(hex(data[i])[2:],end=' ')
   bytes.append(data[i])
  j+=1
 if j > 0:
  for i in range(j,16):
   print("-- ",end='')
   bytes.append(0x00)
  print("--|",end='')
  for i in range(0,j-1):
   if use_non: print(ascii[bytes[j-16+i]],end='')
   else:       print(ascii[bytes[j-16+i]],end='')
  for i in range(j,16):
   print(" ",end='')
  print(" |")
 print("|_______|_______________________________________________|________________|\n\\Size: {}/{} Bytes(".format(hex(len(data)),len(data)),end='')
 if len(data) < 1024: print("{} KB)".format(len(data)/1024))
 else: print("{} MB) of {}/{} bytes".format(len(data)/1024/1024,hex(len(data)),len(data)))

#################################

if len(sys.argv) < 2:
 print("please give an assembly file to compile")
elif len(sys.argv) < 3:
 print("please give an output file")
else:
 asm = []
 include(sys.argv[1])
 FNout = sys.argv[2]
 out=[]
 labels = []
 address = 0
 preaddr = 0
 instructions = ["raw", "mov", "lmov", "add", "inc", "sub", "dec", "mul", "div", "and", "or", "xor", "bsl", "bsr", "not", "split", "combine", "jmp", "led", "cmpeq", "cmplt", "cmpgt", "rpos", "wram", "rram", "wvram", "rvram", "rsav", "wsav", "rrom", "hlt", "disp", "flags", "dsend", "drecv", "icout", "exec", "rbios", "push", "pop", "call", "ret", "swap", "gclk", "wait", "nop"]
 
 ncs = True
 title = False

 print("first scan checks for %<flags> and data")
 for i in range(len(asm)):
  asm[i] = asm[i].replace("\t", " ")
  line = asm[i].split("\n")[0].split(";")[0].split(" ")
  for j in range(len(line)):
   try: line.remove('')
   except ValueError: p=0
  print("[line: "+str(i)+"\t| "+str(preaddr)+"\t]"+asm[i],end='')
  if len(line) < 1: continue
  if   line[0] == "%NOCHECKSUM":
   ncs = False
   asm[i] = "";
  elif line[0] == "%TITLE":
   title = True
   asm[i] = "";
  elif line[0] == "%include":
   include(line[1])
   asm[i] = ""
  elif ":" in line[0]:
   if "{" in line[0]:
    line = line[0].split(":")
    labels.append([line[0],preaddr]); j=i
    while j<len(asm):
     if i==j: pline = (asm[j].split("\n")[0].split(";")[0]+" ")[len(line[0])+2]
     else: pline = asm[j].split("\n")[0].split(";")[0]
     for k in pline.split(","):
      if "0x" in k and not "\n" in k: preaddr+= 1
      if "}" in k: j=len(asm); break
      if k == asm[len(asm[i])]: print("DataError: Data from line "+str(i)+" was expecting \"}\" before reaching the end of the program...."); exit(-1)
     j=j+1
   else:
    labels.append([line[0].split(":")[0],preaddr])
  else:
   if line[0] in instructions:
    if line[0] == "lmov": preaddr+=6
    preaddr+=6
  
 if ncs == True:
  out = [0x00,0x54,0x47,0x52]
  address = 4
  preaddr = 4
 if title == True:
  if len(argv) < 4:
   print("please give an 8 char. ROM name")
   exit()
  sys.argv[3]+=" "*(8-len(sys.argv[3]))
  out+=[ord(sys.argv[3][i]) for i in range(8)]
  address+= 8
  preaddr+= 8

 print("----------------------------------------------------\nsecond scan reads and compiles instructions")
 p=0
 for i in range(len(asm)):
  line = asm[i].split("\n")[0].split(";")[0].split(" ")
  for j in range(len(line)):
   try: line.remove('')
   except ValueError: p=p
  if len(line) < 1: print("[line: "+str(i)+"\t| "+str(address)+"\t] *LINE FLAGED AS UN-USABLE BY FIRST SCAN*"); continue
  elif ":" in line[0]:
   if "{" in line[0]:
    line = line[0].split(":"); j=i
    while j<len(asm):
     if i==j: pline = (asm[j].split("\n")[0].split(";")[0]+" ")[len(line[0])+2]
     else: pline = asm[j].split("\n")[0].split(";")[0]
     print("[line: "+str(j)+"\t| "+str(address)+"\t]"+asm[j],end='')
     for k in pline.split(","):
      if "0x" in k and not "\n" in k: out.append(int(k,16)); address+= 1
      if "}" in k: p=j; j=len(asm); break
      if k == asm[len(asm[i])]: print("DataError: Data from line "+str(i)+" was expecting \"}\" before reaching the end of the program...."); exit(-1)
     j=j+1
  else:
   if i>p: print("[line: "+str(i)+"\t| "+str(address)+"\t]"+asm[i],end='')
  if line[0] in instructions:
   if len(line) < 2: args = []
   else: args = line[1].split(",")
   print("found instruction: "+line[0]+" at ["+str(i)+"|"+str(address)+"]")
   
   if line[0] == "raw":
    if len(args) != 6:
     print("ERROR at line "+str(i)+": the instruction \"RAW\" requres 6 arguments, got "+str(len(args)))
     exit(-1)
    j=0
    try:
     while j<7:
      print(j,args[j],args[j][:2] == "0x")
      out+=int(args[j],16)&0xFF
      j=j+1
    except TypeError:
     print("ERROR at line "+str(i)+"[0]: Value at index["+str(j)+"] for \"RAW\" is invalid (got \""+args[j]+"\")"); exit(-1)
   
   if line[0] == "mov":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if "[" in args[1]:
     if "]" in args[1]:
      k = False
      for j in labels:
       if args[1][1:-1] == j[0]: out+=[0x00,combinetochar(reg(args[0]),0),0x00,0x00,(j[1]&0x000FF00)>> 8,j[1]&0x00000FF]; k = True; break
      if k == False: print("ERROR at line "+str(i)+"[1]: Label \""+args[1][1:-1]+"\" was not found or is invalid\n",labels); print("\""+str(args[1][1:-1])+"\"\n\""+str(j[0])+"\""); exit(-1)
     else: print("ERROR at line "+str(i)+"[1]: Expected \"]\" near label"); exit(-1)
    else:
     print(reg(args[0]),args[0],"|",reg(args[1]),args[1])
     if reg(args[1]) < 0:
      out+=[0x00,combinetochar(reg(args[0]),0),0x00,0x00,(int(args[1],16)&0x000FF00)>> 8,int(args[1],16)&0x00000FF]
     else:
      out+=[0x21,combinetochar(reg(args[0]),reg(args[1])),0x00,0x00,0x00,0x00]
#    else:
#     print("ERROR at line "+str(i)+": was expecting REG or Value for IMM but is invalid (got \""+args[1]+"\")"); exit(-1)
   
   if line[0] == "lmov":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
    if "[" in args[2]:
     if "]" in args[2]:
      k = False
      for j in labels:
       if args[2][1:-1] == j[0]:
        out+=[0x00,combinetochar(reg(args[0]),0),0x00,0x00,(j[1]&0x000FF00)>> 8, j[1]&0x00000FF]
        out+=[0x00,combinetochar(reg(args[1]),0),0x00,0x00,(j[1]&0x00F0000)>>24,(j[1]&0x0FF0000)>>16]
        k = True; break
      if k == False: print("ERROR at line "+str(i)+"[2]: Label \""+args[1][1:-1]+"\" was not found or is invalid\n",labels); print("\""+str(args[1][1:-1])+"\"\n\""+str(j[0])+"\""); exit(-1)
     else: print("ERROR at line "+str(i)+"[2]: Expected \"]\" near label"); exit(-1)
    else:
     out+=[0x00,combinetochar(reg(args[0]),0),0x00,0x00,(int(args[2],16)&0x000FF00)>> 8, int(args[2],16)&0x00000FF]
     out+=[0x00,combinetochar(reg(args[1]),0),0x00,0x00,(int(args[2],16)&0xF000000)>>24,(int(args[2],16)&0x0FF0000)>>16]
    address+=6 # takes up 2 instuctions(12 bytes)
   
   if line[0] == "add":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x01,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
    else:
     out+=[0x01,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,0x00]
   
   if line[0] == "inc":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    out+=[0x01,combinetochar(reg(args[0]),0),combinetochar(reg(args[0]),0),0x00,0x00,0x01]
   
   if line[0] == "sub":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x02,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
    else:
     out+=[0x02,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,0x00]
   
   if line[0] == "dec":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    out+=[0x02,combinetochar(reg(args[0]),0),combinetochar(reg(args[0]),0),0x00,0x00,0x01]
   
   if line[0] == "mul":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x03,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
    else:
     out+=[0x03,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,0x00]
   
   if line[0] == "div":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x04,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
    else:
     out+=[0x04,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,0x00]
   
   if line[0] == "and" or line[0] == "band":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x05,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
    else:
     out+=[0x05,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,0x00]
   
   if line[0] == "or" or line[0] == "bor":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x06,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
    else:
     out+=[0x06,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,0x00]
   
   if line[0] == "xor" or line[0] == "bxor":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x07,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
    else:
     out+=[0x07,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,0x00]

   if line[0] == "bsl":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[2]) > 0: print("ERROR at line "+str(i)+"[2]: Value for IMM is invalid (got \""+args[2]+"\")"); exit(-1)
    out+=[0x08,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
   
   if line[0] == "bsr":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[2]) > 0: print("ERROR at line "+str(i)+"[2]: Value for IMM is invalid (got \""+args[2]+"\")"); exit(-1)
    out+=[0x09,combinetochar(reg(args[0]),0),combinetochar(reg(args[2]),0),0x00,(int(args[1],16)&0x0000FF00)>>8,int(args[1],16)&0x000000FF]
   
   if line[0] == "not" or line[0] == "bnot":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    out+=[0x0A,combinetochar(reg(args[0]),0),combinetochar(reg(args[0]),0),0x00,0x00,0x01]
   
   if line[0] == "split":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
    if reg(args[2]) < 0: print("ERROR at line "+str(i)+"[2]: REG value for B is invalid (got \""+args[2]+"\")"); exit(-1)
    if reg(args[3]) > 0: print("ERROR at line "+str(i)+"[3]: Value for IMM is invalid (got \""+args[3]+"\")"); exit(-1)
    out+=[0x0B,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,int(args[3],16)]
    
   if line[0] == "combine":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
    if reg(args[2]) < 0: print("ERROR at line "+str(i)+"[2]: REG value for B is invalid (got \""+args[2]+"\")"); exit(-1)
    if reg(args[3]) > 0: print("ERROR at line "+str(i)+"[3]: Value for IMM is invalid (got \""+args[3]+"\")"); exit(-1)
    out+=[0x0C,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0),0x00,0x00,int(args[3],16)]
    
   if line[0] == "jmp":
    if "[" in args[0]:
     if "]" in args[0]:
      k = False
      for j in labels:
       if args[0][1:-1] == j[0]: out+=[0x0D,0x00,combinetochar(1,(j[1]&0xF000000)>>24),(j[1]&0x0FF0000)>>16,(j[1]&0x000FF00)>> 8,j[1]&0x00000FF]; k = True; print(str(j)+" | "+hex(j[1])); print(args[0][1:-1]); break
      if k == False: print("ERROR at line "+str(i)+"[0]: Label \""+args[0][1:-1]+"\" was not found or is invalid\n",labels); exit(-1)
     else: print("ERROR at line "+str(i)+"[0]: Expected \"]\" near label"); exit(-1)
    else:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A[Address] is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B[Address] is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x0D,combinetochar(reg(args[0]),reg(args[1])),0x00,0x00,0x00,0x00]
   
   if line[0] == "led":
    out+=[0x0E,combinetochar(reg(args[0]),0),0x00,0x00,(reg(args[1])   &0x000FF00)>>8,reg(args[1])   &0x00000FF]
    
   if line[0] == "cmpeq":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x0F,combinetochar(reg(args[0]),0),0x10,0x00,(reg(args[1])   &0x000FF00)>>8,reg(args[1])   &0x00000FF]
    else:
     out+=[0x0F,combinetochar(reg(args[0]),reg(args[1])),0x00,0x00,0x00,0x00]

   if line[0] == "cmplt":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x10,combinetochar(reg(args[0]),0),0x10,0x00,(int(args[1],16)&0x000FF00)>>8,int(args[1],16)&0x00000FF]
    else:
     out+=[0x10,combinetochar(reg(args[0]),reg(args[1])),0x00,0x00,0x00,0x00]
   
   if line[0] == "cmpgt":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0:
     out+=[0x11,combinetochar(reg(args[0]),0),0x10,0x00,(int(args[1],16)&0x000FF00)>>8,int(args[1],16)&0x00000FF]
    else:
     out+=[0x11,combinetochar(reg(args[0]),reg(args[1])),0x00,0x00,0x00,0x00]
   
   if line[0] == "rpos":
    if reg(args[0]) < 0:
     out+=[0x12,0x00,combinetochar(0x1,(args[0]&0xF000000)>>24),(args[0]&0x0FF0000)>>16,(args[0]&0x000FF00>>8),args[1]&0x00000FF]
    else:
     if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x12,combinetochar(reg(args[0]),reg(args[1])),0x00,0x00,0x00,0x00]
   
   if line[0] == "wram":
    if len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x13,combinetochar(reg(args[0]),1),combinetochar(0,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
    elif len(args) == 1:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     out+=[0x13,combinetochar(reg(args[0]),0),0x00,0x00,0x00,0x00]
    else:
     print("ERROR at line "+str(i)+": the instruction \"WRAM\" requres 1 or 2 arguments, got "+str(len(args)))
   
   if line[0] == "rram":
    if len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x14,combinetochar(reg(args[0]),1),combinetochar(0,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00>>8),args[1]&0x00000FF]
    elif len(args) == 1:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     out+=[0x14,combinetochar(reg(args[0]),0),0x00,0x00,0x00,0x00]
    else:
     print("ERROR at line "+str(i)+": the instruction \"RRAM\" requres 1 or 2 arguments, got "+str(len(args)))
   
   if line[0] == "wvram":
    if len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x13,combinetochar(reg(args[0]),1),combinetochar(1,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
    elif len(args) == 1:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     out+=[0x13,combinetochar(reg(args[0]),0),0x10,0x00,0x00,0x00]
    else:
     print("ERROR at line "+str(i)+": the instruction \"WVRAM\" requres 1 or 2 arguments, got "+str(len(args)))
   
   if line[0] == "rvram":
    if len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x14,combinetochar(reg(args[0]),1),combinetochar(1,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
    elif len(args) == 1:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     out+=[0x14,combinetochar(reg(args[0]),0),0x10,0x00,0x00,0x00]
    else:
     print("ERROR at line "+str(i)+": the instruction \"RVRAM\" requres 1 or 2 arguments, got "+str(len(args)))
   
   if line[0] == "rsav":
    if len(args) == 3:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     if reg(args[2]) < 0: print("ERROR at line "+str(i)+"[2]: REG value for C is invalid (got \""+args[2]+"\")"); exit(-1)
     out+=[0x15,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0x0),0x00,0x00,0x00]
    elif len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x16,combinetochar(reg(args[0]),1),combinetochar(1,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
    else:
     print("ERROR at line "+str(i)+": the instruction \"RSAV\" requres 2 or 3 arguments, got "+str(len(args)))
   
   if line[0] == "wsav":
    if len(args) == 3:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     if reg(args[2]) < 0: print("ERROR at line "+str(i)+"[2]: REG value for C is invalid (got \""+args[2]+"\")"); exit(-1)
     out+=[0x17,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0x0),0x00,0x00,0x00]
    elif len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x18,combinetochar(reg(args[0]),1),combinetochar(1,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
    else:
     print("ERROR at line "+str(i)+": the instruction \"RSAV\" requres 2 or 3 arguments, got "+str(len(args)))
   
   if line[0] == "rrom":
    if len(args) == 3:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     if reg(args[2]) < 0: print("ERROR at line "+str(i)+"[2]: REG value for C is invalid (got \""+args[2]+"\")"); exit(-1)
     out+=[0x19,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0x0),0x00,0x00,0x00]
    elif len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""+args[1]+"\")"); exit(-1)
     args[1] = int(args[1],16)
     out+=[0x1A,combinetochar(reg(args[0]),0),combinetochar(0,(args[1]&0xF000000)>>24),args[1]&(0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
    else:
     print("ERROR at line "+str(i)+": the instruction \"RROM\" requres 2 or 3 arguments, got "+str(len(args)))
   
   if line[0] == "hlt":
    if len(args) > 0:
     if reg(args[0]) >-1: print("ERROR at line "+str(i)+"[0]: Value for IMM is invalid (got \""+args[0]+"\")"); exit(-1)
     out+=[0x1B,0x00,0x00,0x00,0x00,combinetochar(0x0,reg(args[0]))]
    else:
     out+=[0x1B,0x00,0x00,0x00,0x00,0x01]
   
   if line[0] == "disp":
    if len(args) == 1:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     out+=[0x1C,combinetochar(reg(args[0]),0),0x00,0x00,0x00,0x00]
    elif len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x1C,combinetochar(reg(args[0]),reg(args[1])),0x00,0x00,0x00,0x00]
    elif len(args) == 3:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     if reg(args[2]) < 0: print("ERROR at line "+str(i)+"[2]: REG value for C is invalid (got \""+args[2]+"\")"); exit(-1)
     out+=[0x1C,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0x0),0x00,0x00,0x02]
    else:
     print("ERROR at line "+str(i)+": the instruction \"DISP\" requres 1 to 3 arguments, got "+str(len(args)))
   
   if line[0] == "flags":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""+args[0]+"\")"); exit(-1)
    out+=[0x1D,0x00,0x00,0x00,0x00,combinetochar(0x0,reg(args[0]))]
   
   if line[0] == "dsend":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""  +args[1]+"\")"); exit(-1)
    if reg(args[2]) >-1: print("ERROR at line "+str(i)+"[2]: Value for IMM is invalid (got \""  +args[2]+"\")"); exit(-1)
    if args[1] == "GPU":     tmp = 0x00
    if args[1] == "INPUT":   tmp = 0x01
    if args[1] == "SOUND":   tmp = 0x02
    if args[1] == "NETWORK": tmp = 0x03
    else:                    tmp = combinetochar(0x0,int(args[1],16))
    out+=[0x1E,combinetochar(reg(args[0]),0),0x00,0x00,tmp,int(args[2],16)%0x100]
   
   if line[0] == "drecv":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""  +args[1]+"\")"); exit(-1)
    if reg(args[2]) >-1: print("ERROR at line "+str(i)+"[2]: Value for IMM is invalid (got \""  +args[2]+"\")"); exit(-1)
    if args[1] == "GPU":     tmp = 0x00
    if args[1] == "INPUT":   tmp = 0x01
    if args[1] == "SOUND":   tmp = 0x02
    if args[1] == "NETWORK": tmp = 0x03
    else:                    tmp = combinetochar(0x0,int(args[1],16))
    out+=[0x1F,combinetochar(reg(args[0]),0),0x00,0x00,tmp,int(args[2],16)%0x100]
   
   if line[0] == "icout":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
    out+=[0x20,0x00,0x00,0x00,0x00,combinetochar(0x0,reg(args[0]))]
   
   if line[0] == "exec":
    if reg(args[0]) >-1: print("ERROR at line "+str(i)+"[0]: Execute Index for A is invalid (got \""+args[0]+"\")"); exit(-1)
    if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""      +args[1]+"\")"); exit(-1)
    if args[0] == "BIOS": tmp = 0x0
    if args[0] == "ROM":  tmp = 0x1
    if args[0] == "RAM":  tmp = 0x2
    else:                 tmp = int(args[0],16)
    args[1] = int(args[1],16)
    out+=[0x22,combinetochar(tmp,0),combinetochar(0,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
   
   if line[0] == "rbios":
    if len(args) == 3:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) < 0: print("ERROR at line "+str(i)+"[1]: REG value for B is invalid (got \""+args[1]+"\")"); exit(-1)
     if reg(args[2]) < 0: print("ERROR at line "+str(i)+"[2]: REG value for C is invalid (got \""+args[2]+"\")"); exit(-1)
     out+=[0x23,combinetochar(reg(args[0]),reg(args[1])),combinetochar(reg(args[2]),0x0),0x00,0x00,0x00]
    elif len(args) == 2:
     if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
     if reg(args[1]) >-1: print("ERROR at line "+str(i)+"[1]: Value for IMM is invalid (got \""+args[1]+"\")"); exit(-1)
     out+=[0x24,combinetochar(reg(args[0]),0),combinetochar(0,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
    else:
     print("ERROR at line "+str(i)+": the instruction \"RBIOS\" requres 2 or 3 arguments, got "+str(len(args)))
   
   if line[0] == "push":
    if reg(args[0]) >-1: out+=[0x26,combinetochar(reg(args[0]),0x00),0x00,0x00,0x00,0x00]
    else:                out+=[0x26,0x01,combinetochar(0,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
   
   if line[0] == "pop":
    if reg(args[0]) < 0: print("ERROR at line "+str(i)+"[0]: REG value for A is invalid (got \""+args[0]+"\")"); exit(-1)
    out+=[0x25,combinetochar(reg(args[0]),0x0),0x00,0x00,0x00,0x00]
   
   if line[0] == "call":
    if len(args) == 1:
     if "[" in args[0]:
      if "]" in args[0]:
       k = False
       for j in labels:
        if args[0][1:-1] == j[0]: out+=[0x27,0x00,combinetochar(0,(j[1]&0xF000000)>>24),(j[1]&0x0FF0000)>>16,(j[1]&0x000FF00)>>8,j[1]&0x00000FF]; k = True; print(j); print(args[0][1:-1]); break
       if k == False: print("ERROR at line "+str(i)+"[0]: Label \""+args[0][1:-1]+"\" was not found or is invalid\n",labels); exit(-1)
      else: print("ERROR at line "+str(i)+"[0]: Expected \"]\" near label"); exit(-1)
     else:
      print("ERROR at line "+str(i)+"[0]: Expected [label] (got \""+args[2]+"\")"); exit(-1)
    elif len(args) == 2:
     out+=[0x27,combinetochar(reg(args[0]),reg(args[1])),0x10,0x00,0x00,0x00]
    else:
      print("ERROR at line "+str(i)+": the instruction \"CALL\" requres 1 or 2 arguments, got "+str(len(args)))
   
   if line[0] == "ret":
    out+=[0x28,0x00,0x00,0x00,0x00,0x00]
   
   if line[0] == "swap":
    out+=[0x29,0x00,0x00,0x00,0x00,0x00]
   
   if line[0] == "gclk":
    if reg(args[0]) >-1:     out+=[0x2A,combinetochar(reg(args[0]),0x0),0x10,0x00,0x00,0x00]
    elif args[0] == "reset": out+=[0x2A,0x01,0x10,0x00,0x00,0x00]
   
   if line[0] == "wait":
    out+=[0x2B,0x00,combinetochar(0x0,(args[1]&0xF000000)>>24),(args[1]&0x0FF0000)>>16,(args[1]&0x000FF00)>>8,args[1]&0x00000FF]
    
   
   if line[0] == "nop":
    out+=[0xff,0x00,0x00,0x00,0x00,0x00]
    print("What? you want me to do something? NOPe sorry, not today!")
   address+=6
  for o in range(len(out)):
   if out[o] < 0: print(o,[out[o] for o in range(o-6,o)]); #exit(-1)
  print("[",end='')
  for i in out[-6:]:
   if i < 0x10: print("0x0"+hex(i)[2:],end=", ")
   else:        print("0x" +hex(i)[2:],end=", ")
  print("]")
  for i in range(len(out)):
   if out[i] < 0 or out[i] > 0xFF: input()
  #
 #
 print("----------------------------------------------------\ntest2: complete\nlabels:"+str(labels)+"\nout:"+str(out))
 dumpData("out", out, False, 0, len(out))
 for i in range(len(out)):
  if out[i] < 0 or out[i] > 0xFF: print(i,[out[i] for i in range(i-6,i+7)]); exit(-1)
with open(FNout,"wb") as ofile:
 ofile.write(bytes(out))
 ofile.flush()
 ofile.close()
##################################################################################################################
