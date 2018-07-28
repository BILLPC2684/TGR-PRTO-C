#!/usr/bin/env python3

import os
import compile_TGRASM

for i in os.listdir("assembly"):
	p = os.path.join("assembly",i)
	if p.endswith(".asm"):
		if "bios" in i.lower():
			compile_TGRASM.main(p,os.path.join("bin",i[:-4]+".tgr"))
		else:
			compile_TGRASM.main(p,os.path.join("ROMs",i[:-4]+".tgr"))



