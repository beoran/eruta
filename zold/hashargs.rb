# Hashargs is a mix in module to help processing hash arguments.
module Hashargs
  def hash_with_defaults(hash, defaults) 
    defaults.each do |k, v|
      val  = hash[k] || v
      yield k, val
    end
  end
  
  # call this in your constructor, with defaults
  # only keys in defaults will be allowed as args
  def hash_to_ivar(hash, defaults = {}) 
    hash_with_defaults(hash, defaults) do |k, val|
      ivar = "@#{k}".to_sym
      instance_variable_set(ivar, val)
    end
  end
  
  # xxx: todo call this to generate arguments for a function
  
  def initialize(*args)
    super(*args)
  end
end