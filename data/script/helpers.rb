
# Loads a script file once only. Uses a global
# variable $scripts_loaded to keep track of all names of loaded scripts.
def script_once(filenname) 
  $scripts_loaded ||= {}
  if $scripts_loaded[filename]
    return 0
  end
  res = script(filename) 
  if res == 0 
    $scripts_loaded[filename] = true 
  end
  return res
end

# loads an ULPCSS sprite layer, fills in the prefix
def sprite_load_ulpcss(name)
  prefix = "image/ulpcss/" 
  return sprite_loadulpcss(prefix + name)
end



