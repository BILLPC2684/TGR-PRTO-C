#!/usr/bin/env python3

import sys,os,re
from array import array

def save_bytecode(bytecode,name):
  with open(name,"wb") as f:
    arr = array('B', bytecode)
    arr.tofile(f)

global enTitle,title
enTitle = False
title = ""
labels = {}
instID = []
linedecode = re.compile(r"\s*(?:(?P<label>\w+):)?(?: *(?P<opcode>\w+)(?: +(?P<arg>.*))?)?")
datadef = re.compile(r"\s*(?P<label>\w+):\{(?P<contents>.*?)\}", re.MULTILINE | re.DOTALL)

def get_all_subclasses(cls):
    all_subclasses = []

    for subclass in cls.__subclasses__():
        all_subclasses.append(subclass)
        all_subclasses.extend(get_all_subclasses(subclass))

    return all_subclasses


class OpcodeError(SystemExit):
  pass

def reg(x):
  try:
    return "abcdefgh".index(x)
  except ValueError:
    return -1

def combinetochar(a,b):
  if a < 0xf and b < 0xf:
    return a | b << 4
  else:
    raise ValueError("values too large to fit in char")

def extend(x,to,extendhar=0x00):
  return list(x[:to]) + [0 for _ in range(to-len(x))]

class instruction:
  def __init__(self):
    pass

  def instruction(self,*args):
    self.instr = extend(args,6)


class nop(instruction):
  def prehandle(self,*args):
    if len(args) > 0:
      raise OpcodeError("expected 0 operands, got {}".format(len(args)))

    self.instruction(0xFF,0x00,0x00,0x00,0x00,0x00)
    return 6


  def handle(self):
    pass

class raw(instruction):
  def prehandle(self,*args):
    inst = []
    if len(args) != 6:
      raise OpcodeError("expected 6 operands, got {}".format(len(args)))
    for j in range(len(args)):
      try:
        inst.append(int(args[j],0))
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(type(args[j])))
    self.instruction(inst[0],inst[1],inst[2],inst[3],inst[4],inst[5])
    return 6


  def handle(self):
    pass

class add(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    
    if reg(args[1]) < 0:
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      self.instruction(0x01,combinetochar(0x00,reg(args[0])),combinetochar(0x0,reg(args[2])),0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x01,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x0,reg(args[2])))
      return 6


  def handle(self):
    pass

class inc(instruction):
  def prehandle(self,*args):
    if len(args) != 1:
      raise OpcodeError("expected 1 operand, got {}".format(len(args)))
    self.alias = add()
    return self.alias.prehandle(args[0],"1",args[0])


  def handle(self):
    self.instr = self.alias.instr

class dec(instruction):
  def prehandle(self,*args):
    if len(args) != 1:
      raise OpcodeError("expected 1 operand, got {}".format(len(args)))
    self.alias = sub()
    return self.alias.prehandle(args[0],"1",args[0])

  def handle(self):
    self.instr = self.alias.instr

class call(instruction):
  def prehandle(self,*args):
    if len(args) != 1:
      raise OpcodeError("expected 1 operand, got {}".format(len(args)))

    if args[0].startswith("[") and args[0].endswith("]"):
      self.label = args[0][1:-1]
      return 6        
    else:
      raise OpcodeError("not a label: {}.".format(args[0]))
    
  def handle(self):
    if self.label in labels:
      self.instruction(0x25,0x00,0x00,(labels[self.label]&0xff0000)>>16,(labels[self.label]&0x00ff00)>>8,labels[self.label]&0x0000ff)
    else:
      raise OpcodeError("could not find label {}.".format(self.label))

class ret(instruction):
  def prehandle(self,*args):
    if len(args) != 0:
      raise OpcodeError("expected 0 operands, got {}".format(len(args)))
    self.instruction(0x26)
    return 6

  def handle(self):
    pass

class sub(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    
    if reg(args[1]) < 0:
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      self.instruction(0x02,combinetochar(0x00,reg(args[0])),combinetochar(0x00,reg(args[2])),0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x02,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
      return 6


  def handle(self):
    pass

class div(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    
    if reg(args[1]) < 0:
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      self.instruction(0x04,combinetochar(0x00,reg(args[0])),combinetochar(0x00,reg(args[2])),0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x04,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
      return 6
  def handle(self):
    pass

class mul(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    
    if reg(args[1]) < 0:
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      self.instruction(0x03,combinetochar(0x00,reg(args[0])),combinetochar(0x0,reg(args[2])),0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x03,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x0,reg(args[2])))
      return 6
  def handle(self):
    pass

class band(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    
    if reg(args[1]) < 0:
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      self.instruction(0x05,combinetochar(0x00,reg(args[0])),combinetochar(0x00,reg(args[2])),0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x05,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
      return 6
  def handle(self):
    pass

class bor(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    
    if reg(args[1]) < 0:
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      self.instruction(0x06,combinetochar(0x00,reg(args[0])),combinetochar(0x00,reg(args[2])),0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x06,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
      return 6
  def handle(self):
    pass

class bxor(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    
    if reg(args[1]) < 0:
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      self.instruction(0x07,combinetochar(0x00,reg(args[0])),combinetochar(0x00,reg(args[2])),0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x07,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
      return 6
  def handle(self):
    pass

class bnot(instruction):
  def prehandle(self,*args):
    if len(args) != 1:
      raise OpcodeError("expected 1 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))
    else:
      try:
        res = int(args[0],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[0]))
      self.instruction(0x08,combinetochar(0x00,reg(args[0])),combinetochar(0x00,0x00))
      return 6
  def handle(self):
    pass

class rrom(instruction):
  def prehandle(self,*args):
    if len(args) not in (2,3):
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[1]) < 0:
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      self.instruction(0x18,combinetochar(0x00,reg(args[0])),0x00,0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    else:
      self.instruction(0x17,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
      return 6
  def handle(self):
    pass


class push(instruction):
  def prehandle(self,*args):
    if len(args) != 1:
      raise OpcodeError("expected 1 operand, got {}".format(len(args)))

    if reg(args[0]) < 0:
      try:
        res = int(args[0],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[0]))
      self.instruction(0x24,0x01,0x00,0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x24,combinetochar(0x00,reg(args[0])))
      return 6

  def handle(self):
    pass

class pop(instruction):
  def prehandle(self,*args):
    if len(args) != 1:
      raise OpcodeError("expected 1 operand, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))
    else:
      self.instruction(0x23,combinetochar(0x00,reg(args[0])))
    return 6


  def handle(self):
    pass

class rbios(instruction):
  def prehandle(self,*args):
    if len(args) == 2:
      if reg(args[0]) < 0:
        raise OpcodeError("invalid operand {}. should be register name".format(args[0]))
      
      try:
        res = int(args[1],0)
      except:
        raise OpcodeError("invalid operand {}. should be register name or int".format(args[0]))
      self.instruction(0x22,combinetochar(0x00,reg(args[0])),0x00,0x00,(res&0xff00)>>8,res&0x00ff)
    elif len(args) == 3:
      if reg(args[0]) < 0:
        raise OpcodeError("invalid operand {}. should be register name".format(args[0]))
      
      if reg(args[1]) < 0:
        raise OpcodeError("invalid operand {}. should be register name".format(args[1]))
      
      if reg(args[2]) < 0:
        raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
      self.instruction(0x21,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
    else:
      raise OpcodeError("expected 2 or 3 operands, got {}".format(len(args)))
    return 6


  def handle(self):
    pass

class hlt(instruction):
  def prehandle(self,*args):
    if len(args) != 0:
      raise OpcodeError("expected 0 operands, got {}".format(len(args)))
    
    self.instruction(0x19,0x00,0x00,0x00,0x00,0x01)
    return 6


  def handle(self):
    pass

class exec(instruction):
  def prehandle(self,*args):
    if len(args) != 2:
      raise OpcodeError("expected 2 operands, got {}".format(len(args)))
    
    try:
      res1 = int(args[0],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[0]))

    try:
      res2 = int(args[1],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[1]))

    if res1 > 0xff:
      raise OpcodeError("invalid operand {}. int should be less than 256".format(args[1]))
    if res2 > 0xffffff:
      raise OpcodeError("invalid operand {}. int should be less than 16777216".format(args[1]))

    self.instruction(0x20,((res1&0x0F)<<4|(res1&0xF0)>>4),0x00,(res2&0xff0000)>>16,(res2&0x00ff00)>>8,res2&0x0000ff)
    return 6


  def handle(self):
    pass

class split(instruction):
  def prehandle(self,*args):
    if len(args) != 4:
      raise OpcodeError("expected 4 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[1]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[1]))
    
    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    try:
      arg3 = int(args[3],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[3]))

    self.instruction(0x09,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x0,reg(args[2])),0x00,0x00,arg3)
    return 6

  def handle(self):
    pass

class combine(instruction):
  def prehandle(self,*args):
    if len(args) != 4:
      raise OpcodeError("expected 4 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[1]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[1]))
    
    if reg(args[2]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
    try:
      arg3 = int(args[3],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[3]))

    self.instruction(0x0A,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x0,reg(args[2])),0x00,0x00,arg3)
    return 6

  def handle(self):
    pass

class swap(instruction):
  def prehandle(self,*args):
    if len(args) != 0:
      raise OpcodeError("expected 0 operands, got {}".format(len(args)))
    else:
      self.instruction(0x27)
      return 6


  def handle(self):
    pass

class mov(instruction):
  def prehandle(self,*args):
    if len(args) != 2:
      raise OpcodeError("expected 2 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

    if reg(args[1]) < 0:

      if args[1].startswith("[") and args[1].endswith("]"):
        self.label = args[1][1:-1]
        self.reg = args[0]
        return 6
      else: 
        try:
          res = int(args[1],0)
        except:
          raise OpcodeError("invalid operand {}. should be register name or int".format(args[1]))
      
      if res > 0xffff:
        raise OpcodeError("invalid operand {}. int should be less than 65536".format(args[1]))
      self.instruction(0x00,combinetochar(0x0,reg(args[0])),0x00,0x00,(res&0xff00)>>8,res&0x00ff)
      return 6
    else:
      self.instruction(0x1f,combinetochar(reg(args[0]),reg(args[1])))
      return 6

  def handle(self):
    if hasattr(self,"label"):
      if self.label in labels:
        if labels[self.label] > 0xFFFF:
          raise OpcodeError("invalid operand {}. int should be less than 65536(0xFFFF)".format(labels[self.label]))
          #raise OpcodeError("invalid operand {}. int should be less than 16777215".format(labels[self.label]))
        self.instruction(0x00,combinetochar(0x0,reg(self.reg)),0x00,0x00,(labels[self.label]&0xff00)>>8,labels[self.label]&0x00ff)
      else:
        raise OpcodeError("could not find label {}.".format(self.label))
class lmov(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))
    if reg(args[1]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[1]))

    if reg(args[2]) < 0:

      if args[2].startswith("[") and args[2].endswith("]"):
        self.label = args[2][1:-1]
        self.regA = args[0]
        self.regB = args[1]
        return 6
      else:
        try:
          res = int(args[2],0)
        except:
          raise OpcodeError("invalid operand {}. should be register name or int".format(args[2]))
      """0xAAAABBBB"""
      if res > 0xFFFFFFFF:
        raise OpcodeError("invalid operand {}. int should be less than 4294967295(0xFFFFFFFF)".format(args[2]))
      self.instruction(0x00,combinetochar(0x0,reg(args[0])),0x00,0x00,(res&0x0000ff00)>> 8,res&0x000000ff)
      self.instruction(0x00,combinetochar(0x0,reg(args[1])),0x00,0x00,(res&0xff000000)>>24,res&0x00ff0000>>16)
      return 12

  def handle(self):
    if hasattr(self,"label"):
      if self.label in labels:
        if labels[self.label] > 0xFFFFFFFF:
          raise OpcodeError("invalid operand {}. int should be less than 4294967295(0xFFFFFFFF)".format(args[2]))
        self.instruction(0x00,combinetochar(0x0,reg(self.regA)),0x00,0x00,(labels[self.label]&0x0000ff00)>>8,labels[self.label]&0x000000ff)
        self.instruction(0x00,combinetochar(0x0,reg(self.regB)),0x00,0x00,(labels[self.label]&0xff000000)>>24,labels[self.label]&0x00ff0000>>16)
      else:
        raise OpcodeError("could not find label {}.".format(self.label))

class rpos(instruction):
  def prehandle(self,*args):
    if len(args) != 2:
      raise OpcodeError("expected 2 operands, got {}".format(len(args)))    

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    

    if reg(args[1]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    

    self.instruction(0x10,combinetochar(reg(args[1]),reg(args[0])),0x00,0x00,0x00,0x00)
    return 6
  def handle(self):
    pass

class wram(instruction):
  def prehandle(self,*args):
    if len(args) < 2 or len(args) > 3:
      raise OpcodeError("expected 2 to 3 operands, got {}".format(len(args)))    
    
    try:
      res0 = int(args[0],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[2]))
    if res0 > 0x1:
      raise OpcodeError("invalid operand {}. int should be less than 1".format(args[1]))
    
    if reg(args[1]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    
    
    if len(args) == 3:
      try:
        res = int(args[2],0)
      except:
        raise OpcodeError("invalid operand {}. should be int".format(args[2]))
      if res1 > 0xfffffff:
        raise OpcodeError("invalid operand {}. int should be less than 268435456(0x10000000)".format(args[2]))
      self.instruction(0x11,combinetochar(reg(args[1]),reg(args[0])),combinetochar(((res&0xf000000)>>24),0x1),(res&0x0ff0000)>>16,(res&0x000ff00)>>8,res&0x000000ff)
      return 6
    else:
      self.instruction(0x11,combinetochar(reg(args[1]),reg(args[0])),0x00,0x00,0x00,0x00)
      return 6
  def handle(self):
    pass

class rram(instruction):
  def prehandle(self,*args):
    if len(args) < 2 or len(args) > 3:
      raise OpcodeError("expected 2 to 3 operands, got {}".format(len(args)))    
    
    try:
      res0 = int(args[0],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[2]))
    if res0 > 0x1:
      raise OpcodeError("invalid operand {}. int should be less than 1".format(args[1]))
    
    if reg(args[1]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    
    
    if len(args) == 3:
      try:
        res = int(args[2],0)
      except:
        raise OpcodeError("invalid operand {}. should be int".format(args[2]))
      if res1 > 0xfffffff:
        raise OpcodeError("invalid operand {}. int should be less than 268435456(0x10000000)".format(args[2]))
      self.instruction(0x12,combinetochar(reg(args[1]),reg(args[0])),combinetochar(((res&0xf000000)>>24),0x1),(res&0x0ff0000)>>16,(res&0x000ff00)>>8,res&0x000000ff)
      return 6
    else:
      self.instruction(0x12,combinetochar(reg(args[1]),reg(args[0])),0x00,0x00,0x00,0x00)
      return 6
  def handle(self):
    pass

class dsend(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))    

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    

    try:
      res1 = int(args[1],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[1]))
    if res1 > 0xf:
      raise OpcodeError("invalid operand {}. int should be less than 16".format(args[1]))

    try:
      res2 = int(args[2],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[2]))
    if res2 > 0xff:
      raise OpcodeError("invalid operand {}. int should be less than 256".format(args[2]))

    
    self.instruction(0x1c,combinetochar(0x00,reg(args[0])),0x00,0x00,combinetochar(res1,0x00),res2)
    return 6


  def handle(self):
    pass

class drecv(instruction):
  def prehandle(self,*args):
    if len(args) != 3:
      raise OpcodeError("expected 3 operands, got {}".format(len(args)))    

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    

    try:
      res1 = int(args[1],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[1]))
    if res1 > 0xf:
      raise OpcodeError("invalid operand {}. int should be less than 16".format(args[1]))

    try:
      res2 = int(args[2],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[2]))
    if res2 > 0xff:
      raise OpcodeError("invalid operand {}. int should be less than 256".format(args[2]))

    
    self.instruction(0x1d,combinetochar(0x00,reg(args[0])),0x00,0x00,combinetochar(res1,0x00),res2)
    return 6


  def handle(self):
    pass

class cmpeq(instruction):
  def prehandle(self,*args):
    if len(args) != 2:
      raise OpcodeError("expected 2 operands, got {}".format(len(args)))    
    
    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    
    
    if reg(args[1]) < 0:
     print("cmpeq IMM ",reg(args[0]),args[1])
     res = 0
     try:
      res = int(args[1],0)
     except:
       raise OpcodeError("invalid operand {}. should be int".format(args[1]))
     if res > 0xffff:
      raise OpcodeError("invalid operand {}. int should be less than 16-bit({})".format(args[1],0x10000))
     self.instruction(0x0d,combinetochar(0x0,reg(args[0])),0x10,0x00,(res&0xff00)>>8,res&0x00ff)
     return 6
    
    print("cmpeq ",reg(args[0]),reg(args[1]))
    self.instruction(0x0d,combinetochar(reg(args[1]),reg(args[0])))
    return 6


  def handle(self):
    pass

class cmplt(instruction):
  def prehandle(self,*args):
    if len(args) != 2:
      raise OpcodeError("cmplt: expected 2 operands, got {}".format(len(args)))    

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    

    if reg(args[1]) < 0:
     print("cmplt IMM ",reg(args[0]),args[1])
     res = 0
     try:
      res = int(args[1],0)
     except:
       raise OpcodeError("invalid operand {}. should be int".format(args[1]))
     if res > 0xffff:
      raise OpcodeError("invalid operand {}. int should be less than 16-bit({})".format(args[1],0x10000))
     self.instruction(0x0e,combinetochar(0x0,reg(args[0])),0x10,0x00,(res&0xff00)>>8,res&0x00ff)
     return 6
    
    print("cmplt ",reg(args[0]),reg(args[1]))
    self.instruction(0x0e,combinetochar(reg(args[1]),reg(args[0])))
    return 6

  def handle(self):
    pass


class cmpgt(instruction):
  def prehandle(self,*args):
    if len(args) != 2:
      raise OpcodeError("cmpgt: expected 2 operands, got {}".format(len(args)))    

    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))    

    if reg(args[1]) < 0:
     print("cmpgt IMM ",reg(args[0]),args[1])
     res = 0
     try:
      res = int(args[1],0)
     except:
       raise OpcodeError("invalid operand {}. should be int".format(args[1]))
     if res > 0xffff:
      raise OpcodeError("invalid operand {}. int should be less than 16-bit({})".format(args[1],0x10000))
     self.instruction(0x0f,combinetochar(0x0,reg(args[0])),0x10,0x00,(res&0xff00)>>8,res&0x00ff)
     return 6
    
    print("cmpgt ",reg(args[0]),reg(args[1]))
    self.instruction(0x0f,combinetochar(reg(args[1]),reg(args[0])))
    return 6


  def handle(self):
    pass

class jmp(instruction):
  def prehandle(self,*args):
    if len(args) != 1:
      raise OpcodeError("expected 1 operands, got {}".format(len(args)))
    
    if args[0].startswith("[") and args[0].endswith("]"):
      self.label = args[0][1:-1]
      return 6
    else:
      raise OpcodeError("not a label: {}.".format(args[0]))
    
  def handle(self):
    if self.label in labels:
      self.instruction(0x0b,0x00,(labels[self.label]&0xff0000)>>24,(labels[self.label]&0xff0000)>>16,(labels[self.label]&0x00ff00)>>8,labels[self.label]&0x0000ff)
    else:
      raise OpcodeError("could not find label {}.".format(self.label))

class wait(instruction):
  def prehandle(self,*args):
    if len(args) != 1:
      raise OpcodeError("expected 1 operands, got {}".format(len(args)))    

    try:
      res0 = int(args[0],0)
    except:
      raise OpcodeError("invalid operand {}. should be int".format(args[0]))
    self.instruction(0x29,0x00,(res0&0xff0000)>>24,(res0&0xff0000)>>16,(res0&0x00ff00)>>8,res0&0x0000ff)
    return 6
    
  def handle(self):
    pass

class disp(instruction):
  def prehandle(self,*args):
    if len(args) > 3 or len(args) < 1:
      raise OpcodeError("expected 1 to 3 operands, got {}".format(len(args)))
    
    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))
    pREG = [reg(args[0]),0,0]
    
    if len(args) == 2:
      if reg(args[1]) < 0:
        raise OpcodeError("invalid operand {}. should be register name".format(args[1]))
      pREG[1] = reg(args[1])
    
    if len(args) == 3:
      if reg(args[2]) < 0:
        raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
      pREG[2] = reg(args[2])
  
    self.instruction(0x1a,combinetochar(pREG[1],pREG[0]),combinetochar(0x0,pREG[2]),0x00,0x00,len(args)-1)
    return 6

  def handle(self):
    pass

class gclk(instruction):
  def prehandle(self,*args):
    self.clear = 0
    if len(args) < 1 or len(args) > 2:
      raise OpcodeError("expected 1 or 2 operands, got {}".format(len(args)))
    elif len(args) < 2:
      self.clear = 0
    else:
      self.clear = int(args[1],2)
    if reg(args[0]) < 0:
      raise OpcodeError("invalid operand {}. should be register name".format(args[0]))
    self.reg = args[0]
    return 6

  def handle(self):
    self.instruction(0x28,combinetochar(self.clear,reg(self.reg)),combinetochar(0x0,0x00))

class led(instruction):
  def prehandle(self,*args):
    if len(args) > 1:
      raise OpcodeError("expected 1 or 2 operands, got {}".format(len(args)))
    elif len(args) == 1:
      if reg(args[0]) > -1:
        raise OpcodeError("invalid operand {}. should be value".format(args[0]))
      self.Color = args[0]
    else:
     self.Color = -1
    return 6

  def handle(self):
    self.instruction(0x0c,combinetochar(self.clear,reg(self.reg)),combinetochar(0x0,0x00))

def include(filestr):
  for i in re.finditer(r"\%include +(.+)",filestr):
    try:
      with open(i.group(1).strip(),"r") as f:
        file = f.read()
      filestr = filestr[:i.start()] + include(file) + filestr[i.end():]

    except FileNotFoundError:
      print("couldn't include {}: file not found".format(repr(i.group(1))))

  return filestr

def main(argv):
  fnamein = argv[1]
  fnameout = argv[2]
  global labels
  instructions = []
  dataobjs = []
  with open(fnamein,"r") as f:
    file = f.read()  
  file = include(file)

  if "%NOCHECKSUM" in file:
    checksum=False
    file = file.replace("%NOCHECKSUM","")
  else:
    checksum=True
  address = (4 if checksum else 0)+6

  if "%TITLE" in file:
    enTitle = True
    file = file.replace("%TITLE","")
  else:
    enTitle = False
  address = (address if not enTitle else 18)

  for m in datadef.finditer(file):
    data = [int(i.strip(),0) for i in m.group("contents").split(",") if i.strip() != ""]
    for i in data:
      if i > 0xff:
        print("data too large. data should be (max) 8 bit comma seperated")
        exit()
    labels[m.group("label")] = address
    address += len(data)
    dataobjs += data
    print(m.group("label"),address)
  file = datadef.sub("",file)

  with open("temp.txt","w") as f:
    f.write(file)
  os.system("gcc -E -x c -P -C --std=c99 temp.txt >temp1.txt")
  with open("temp1.txt","r") as f:
    file = f.read()

  commentrm = re.compile(r"/\*.*\*/",re.MULTILINE | re.DOTALL)
  file = commentrm.sub("",file).replace("__SPACE__",' ').replace("__NEWLINE__",'\n')
  # print(file)
  os.remove("temp.txt")
  os.remove("temp1.txt")

  lines = [i.split(";")[0] for i in file.split("\n")]

  for linenum,i in enumerate(lines):
    print(linenum,i)
    m = linedecode.match(i)
    if m:
      if m.group("label"):
        print("Lable \""+m.group("label")+"\" at: "+str(address))
        labels[m.group("label")] = address
      if m.group("opcode"):
        for j in get_all_subclasses(instruction):
          if j.__name__ == m.group("opcode"):
            instr = j()
            instID.append(i)
            instructions.append(instr)
            if m.group("arg"):
              address += instr.prehandle(*[i.strip() for i in m.group("arg").split(",")])
            else:
              address += instr.prehandle()
            break
        else:
          print("invalid opcode on line {}".format(linenum))
          print(i.strip())
          exit()
    else:
      print("error on line {}".format(linenum))
      print(i.strip())
      exit()
#  j = 0
  for i in instructions:
    #print(instID[j])
    i.handle()
    #j+=1

  if enTitle:
    add = 12
  elif checksum:
    add = 4
  else:
    add = 0

  location = 6 + add + len(dataobjs)
  if location > 0xFFFFFF:
    print("too much data... WIP")
    exit()

  if enTitle == True:
   if len(argv) < 4:
    print("please give an 8 char. ROM name")
    exit()
   argv[3]+=" "*(8-len(argv[3]))
   res = ([0x01,0x54,0x47,0x52]+[ord(argv[3][i]) for i in range(8)] if checksum else []) + [0x0B,0x00,0x00,(location&0x00ff0000)>>16,(location&0x0000ff00)>>8,location&0x000000ff] + dataobjs
  else:
   res = ([0x00,0x54,0x47,0x52] if checksum else []) + [0x0B,0x00,0x00,(location&0x00ff0000)>>16,(location&0x0000ff00)>>8,location&0x000000ff] + dataobjs
   
  for i in instructions:
    res += i.instr

  save_bytecode(res,fnameout)

if __name__ == '__main__':

  if len(sys.argv) < 2:
    print("please give an assembly file to compile")

  elif len(sys.argv) < 3:
    print("please give an output file")

  else:
    main(sys.argv)
##################################################################################################################
