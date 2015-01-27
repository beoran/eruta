# Models a low level tile map and helps manage loaded tile maps.
class Tilemap < Store

  extend Store::Forward

  # Loads the tile map sample. Supports .tmx files.
  def self.load(name, vpath)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_tilemap(nid, vpath)
    end 
  end

  # Sets a tile map as the active tile map.
  # Does not drop the  previous tile map.
  def self.activate!(tilemap)
    active_map_ tilemap.id
    @active = tilemap
  end

  # returns the current active tilemap
  def self.active
    return @active || nil
  end

  # Equivalent to Tilemap.actvate(self)
  def activate!
    Tilemap.activate!(self)
  end

  # Reloads the current active tile map. Doesn othing if there is no active map
  # Returns the reloaded map. The map ID might have changed.
  def self.reload
    return nil unless self.active 
    path = self.active.vpath
    name = self.active.name
    # Disable map
    active_map_(-1)
    # Drop the current map
    self.active_map.drop!
    # Load the map again and set it as active
    map = self.load(name, vpath)
    self.activate!(map) if map
    return map
  end

end
 
