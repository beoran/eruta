#!/usr/local/bin/lua

require 'io'
require 'table'

function table.print(tab)
  for k, v in pairs(tab) do 
    print(k, v)
  end  
end

io.write("Hello!\n")
table.print(io)

Class  = {}
Object = {}


function respond_to(obj, methodname) 
  return type(obj[methodname]) == 'function'
end

function Object.allocate(klass)
  local result = {}
  local meta   = {}
  klass        = klass or Object
  meta.__index = function(table, key) 
    if type(rawget(table, key)) == "function" then
      return table[key]
    else 
      return klass[key]
    end  
  end
  meta.__klass = klass 
  setmetatable(result, meta)
  return result
end

function Object:new(...)
  result = self.allocate(self)  
  if respond_to(result, "initialize") then
    result:initialize(...) 
  end 
  return result
end

function Object:respond_to(methodname)
  return respond_to(self, methodname) 
end

function class(...)
  local parent = ...
  print(parent)
  return Object.allocate(parent)
end

Range = class() 

function Range:initialize(low, high) 
  self["@low"]  = low
  self["@high"] = high
end

function Range:low() 
  return self["@low"]
end

function Range:high() 
  return self["@high"]
end


--[[
function Range.new(low, high)
  range           = Object.allocate(Range)
  range:initialize(low, high)
  return range  
end
]]

r = Range:new(10,20)
print(r, r:low(), r:high(), r:respond_to("low"))

function Object:frobnicate()
  print('Frobnicate!!!')
end

r:frobnicate()

XRange = class(Range)
 
function XRange:low()
  return Range.low(self) + 1 
end

function XRange:high()
  return Range.high(self) - 1 
end


x = XRange:new(10, 20)
print(x, x:low(), x:high())

function altobj()
  local obj = {}
  local meta = {}
  meta.__call = function(...)
    print("You rang m'lord?")
  end
  setmetatable(obj, meta)
  return obj
end

o = altobj()
o()








