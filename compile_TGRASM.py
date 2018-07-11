#!/usr/bin/env python3

import sys,os,re
from array import array

def save_bytecode(bytecode,name):
	with open(name,"wb") as f:
		arr = array('B', bytecode)
		arr.tofile(f)

labels = {}
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


class add(instruction):
	def prehandle(self,*args):
		if len(args) != 3:
			raise OpcodeError("expected 3 operands, got {}".format(len(args)))

		if reg(args[0]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(i))

		if reg(args[2]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(i))
		
		if reg(args[1]) < 0:
			try:
				res = int(args[1],0)
			except:
				raise OpcodeError("invalid operand {}. should be register name or int".format(i))
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
		return self.alias.prehandle(args[0],1,args[0])


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
			raise OpcodeError("invalid operand {}. should be register name".format(i))

		if reg(args[2]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(i))
		
		if reg(args[1]) < 0:
			try:
				res = int(args[1],0)
			except:
				raise OpcodeError("invalid operand {}. should be register name or int".format(i))
			self.instruction(0x02,combinetochar(0x00,reg(args[0])),combinetochar(0x00,reg(args[2])),0x00,(res&0xff00)>>8,res&0x00ff)
			return 6
		else:
			self.instruction(0x02,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
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
			self.instruction(0x23,0x00,0x00,0x00,(res&0xff00)>>8,res&0x00ff)
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
			self.instruction(0x22,combinetochar(0x00,reg(args[0])))
		return 6


	def handle(self):
		pass

class rbios(instruction):
	def prehandle(self,*args):
		if len(args) != 3:
			raise OpcodeError("expected 3 operands, got {}".format(len(args)))

		if reg(args[0]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(args[0]))

		if reg(args[1]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(args[1]))

		if reg(args[2]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(args[2]))
		else:
			self.instruction(0x21,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x00,reg(args[2])))
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
			raise OpcodeError("invalid operand {}. should be int".format(i))

		try:
			res2 = int(args[1],0)
		except:
			raise OpcodeError("invalid operand {}. should be int".format(i))

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
		if len(args) != 3:
			raise OpcodeError("expected 3 operands, got {}".format(len(args)))

		if reg(args[0]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(i))

		if reg(args[1]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(i))
		
		if reg(args[2]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(i))
		else:
			self.instruction(0x09,combinetochar(reg(args[1]),reg(args[0])),combinetochar(0x0,reg(args[2])),0x00,0x00,0x01)
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
				if labels[self.label] > 0xffff:
					raise OpcodeError("invalid operand {}. int should be less than 65536".format(args[1]))
				self.instruction(0x00,combinetochar(0x0,reg(self.reg)),0x00,0x00,(labels[self.label]&0xff00)>>8,labels[self.label]&0x00ff)
			else:
				raise OpcodeError("could not find label {}.".format(self.label))

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
		if res1 > 0xff:
			raise OpcodeError("invalid operand {}. int should be less than 256".format(args[2]))

		
		self.instruction(0x1c,combinetochar(0x00,reg(args[0])),0x00,0x00,combinetochar(0x00,res1),res2)
		return 6


	def handle(self):
		pass

class cmpeq(instruction):
	def prehandle(self,*args):
		if len(args) != 2:
			raise OpcodeError("cmpeq: expected 2 operands, got {}".format(len(args)))		

		if reg(args[0]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(args[0]))		

		if reg(args[1]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(args[0]))		

		
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
			raise OpcodeError("invalid operand {}. should be register name".format(args[0]))		

		
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
			raise OpcodeError("invalid operand {}. should be register name".format(args[0]))		

		
		self.instruction(0x0f,combinetochar(reg(args[1]),reg(args[0])))
		return 6


	def handle(self):
		pass

class jmp(instruction):
	def prehandle(self,*args):
		if len(args) != 1:
			raise OpcodeError("expected 3 operands, got {}".format(len(args)))		

		if args[0].startswith("[") and args[0].endswith("]"):
			self.label = args[0][1:-1]
			return 6				
		else:
			raise OpcodeError("not a label: {}.".format(args[0]))		
		
	def handle(self):
		if self.label in labels:
			self.instruction(0x0c,0x00,0x00,(labels[self.label]&0xff0000)>>16,(labels[self.label]&0x00ff00)>>8,labels[self.label]&0x0000ff)
		else:
			raise OpcodeError("could not find label {}.".format(self.label))

class disp(instruction):
	def prehandle(self,*args):
		if len(args) != 1:
			raise OpcodeError("expected 1 operand, got {}".format(len(args)))		
		if reg(args[0]) < 0:
			raise OpcodeError("invalid operand {}. should be register name".format(args[0]))
	
		self.instruction(0x1a,combinetochar(0x0,reg(args[0])))
		return 6


	def handle(self):
		pass


def include(filestr):
	for i in re.finditer(r"\#include +(.+)",filestr):
		try:
			with open(i.group(1).strip(),"r") as f:
				file = f.read()
			filestr = filestr[:i.start()] + include(file) + filestr[i.end():]

		except FileNotFoundError:
			print("couldn't include {}: file not found".format(repr(i.group(1))))

	return filestr

def main(fnamein,fnameout):

	global labels
	instructions = []
	dataobjs = []
	with open(fnamein,"r") as f:
		file = f.read()	
	file = include(file)

	if "#NOCHECKSUM" in file:
		checksum=False
		file = file.replace("#NOCHECKSUM","")
	else:
		checksum=True
	adress = (4 if checksum else 0)+6

	for m in datadef.finditer(file):
		data = [int(i.strip(),0) for i in m.group("contents").split(",") if i.strip() != ""]
		for i in data:
			if i > 0xff:
				print("data too large. data should be (max) 8 bit comma seperated")
				exit()
		labels[m.group("label")] = adress
		adress += len(data)
		dataobjs += data

	file = datadef.sub("",file)

	lines = [i.split(";")[0] for i in file.split("\n")]

	for linenum,i in enumerate(lines):
		m = linedecode.match(i)
		if m:
			if m.group("label"):
				labels[m.group("label")] = adress
			if m.group("opcode"):
				for j in get_all_subclasses(instruction):
					if j.__name__ == m.group("opcode"):
						instr = j()
						instructions.append(instr)
						if m.group("arg"):
							adress += instr.prehandle(*[i.strip() for i in m.group("arg").split(",")])
						else:
							adress += instr.prehandle()
						break
				else:
					print("invalid opcode on line {}".format(linenum))
					print(i.strip())
					exit()
		else:
			print("error on line {}".format(linenum))
			print(i.strip())
			exit()
	for i in instructions:
		i.handle()


	if checksum:
		add = 4
	else:
		add = 0

	location = 6 + add + len(dataobjs)
	if location > 0xffff:
		print("too much data... WIP")
		exit()


	res = ([0x00,0x54,0x47,0x52] if checksum else []) + [0x0C,0x00,0x00,0x00,(location&0xff00)>>8,location&0x00ff] + dataobjs
	for i in instructions:
		res += i.instr

	save_bytecode(res,fnameout)

if __name__ == '__main__':

	if len(sys.argv) < 2:
		print("please give an assembly file to compile")

	elif len(sys.argv) < 3:
		print("please give an output file")

	else:
		main(sys.argv[1],sys.argv[2])