# The registry module can be extended into a class to enable
# by name and by id registtry storage and lookup
module Registry

  # Sets up the registry
  def prepare_registry
    @registry_by_id              ||= {}
    @registry_by_name            ||= {}
  end

  # Gets the id and name for the object for use with the registry
  def get_registry_id_and_name(obj, id = nil, name = nil)
    id      = obj.id          if obj.respond_to? :id
    name    = obj.name        if obj.respond_to? :name
    name  ||= id.to_s.to_sym
    return id, name
  end
  
  # Registers the object with the given numeric ID and name
  # The obj should respond to :name and :id
  def register(obj, id = nil, name = nil)
    prepare_registry
    id, name                       = get_registry_id_and_name(obj, id, name)
    @registry_by_id[id]            = obj
    @registry_by_name[name.to_sym] = obj
  end


  # Unregisters the object
  def unregister(obj, id = nil, name = nil)
    prepare_registry
    id, name                        = get_registry_id_and_name(obj, id, name)
    @registry_by_id[id]             = nil
    @registry_by_name[name.to_sym]  = nil
  end

  # Looks up an item in the registry by name or by id.
  # If thing responds to :to_sym it's looked up as a name,
  # otherwise it's looked up by numerical
  def [](name_or_id)
    if name_or_id.respond_to? :to_sym
      return @registry_by_name[name_or_id.to_sym]
    else
      return @registry_by_id[name_or_id.to_sym]
    end
  end

  # Looks up an ID that hasn't been used yet in the per-ID registry
  # pass in the valdd range for the ID
  def get_unused_registry_id(range = (1000..30000))
    prepare_registry
    low  = range.begin
    stop = range.end - range.begin
    stop.times do | i | 
      j = i + low
      return j unless @registry_by_id[j]
    end
    return nil
  end

end


