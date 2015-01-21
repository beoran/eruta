# Models a low level tile map and helps manage loaded tile maps.
class Tilemap

  # registers the tile map
  def self.register(map)
    @registry         ||= {}
    @registry_by_name ||= {}
    @registry[map.id]   = map
    @registry_by_name[map.name.to_sym] = map
  end

  # unregisters a tile map
  def self.unregister(thing)
    @registry         ||= {}
    @registry_by_name ||= {}
    @registry[thing.id] = nil
    @registry_by_name[thing.name.to_sym] = nil
  end

  # Looks up an item in storage by name.
  def self.[](name)
    return @registry_by_name[name.to_sym]
  end

  # Initialize a storage item.
  def initialize(id, name) 
    @id   = id
    @name = name
    self.class.register(self)
  end

  
  
  def self.load(filename, name = nil)
    @filename     = filename
    name        ||= filename
    Store.tilemap = Store.load_tilemap(@filename, name)
    active_map_ Store.tilemap.id
  end


def reload_tilemap
  tilemap_id = Store.tilemap.id
  active_map_(-1) # disable map
  Store.tilemap.drop if Store.tilemap.id
  $tilemap_1  = Store.load_tilemap($tilemap_fn, :map_0001, $tilemap_id)
  active_map_ $tilemap_1.id
end

  


end
 
