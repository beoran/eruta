
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





