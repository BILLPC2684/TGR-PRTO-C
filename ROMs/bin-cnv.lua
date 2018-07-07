if #arg == 0 then
 io.write("Input lua ArrayRom(leave \".lua\" out of filename): ")
 filename = io.read()
 require(filename)
else
 print(arg[1])
 require(arg[1])
 filename = arg[1]
end
--io.open(ifilename,"r"):read()()
file = io.open(filename..".tgr","wb")

function tohex(int)
 if int < 0x10 then
  return string.upper("0"..string.format('%x', int))
 else
  return string.upper(string.format('%x', int))
 end
end

print("-----[[DEBUG-PRINT]]------")
local tmp = ""
tmp = tmp..".______________________________________________________.\n"
tmp = tmp.."|ROM   |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F|\n"
tmp = tmp.."|------|-----------------------------------------------|\n"
tmp = tmp.."|000000|"
local j = 1
for i=1,#ROM do
 if j > 15 then
  tmp = tmp..tohex(ROM[i]).."|\n|"
  if #tohex(i) == 2 then
   tmp = tmp.."0000"..tohex(i).."|"
  elseif #tohex(i) == 3 then
   tmp = tmp.."000"..tohex(i).."|"
  elseif #tohex(i) == 4 then
   tmp = tmp.."00"..tohex(i).."|"
  elseif #tohex(i) == 5 then
   tmp = tmp.."0"..tohex(i).."|"
  else
   tmp = tmp..tohex(i).."|"
  end
  j = 0
 else
  tmp = tmp..tohex(ROM[i]).." "
 end
 j = j + 1
end
if j > 0 then
 for i=j,15 do
  tmp = tmp.."-- "
 end
 tmp = tmp.."--|\n"
end
tmp = tmp.."|______|_______________________________________________|"
print(string.upper(tmp))
print("Length: "..#ROM.."/0x"..tohex(#ROM))

for i=1,#ROM do
 io.write(ROM[i].." ")
 file:write(string.char(ROM[i]))
end
print("\nSaving to: \""..filename..".tgr\"...")
print("----[[END-OF-PROGRAM]]----")
file:close()