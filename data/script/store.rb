# some more OO wrappers for the low level store functionality

# How to organise the game state, loaded data, etc in a neat way?
# Use a central repository or per type/class repositories???
# -> use per type registries for ease of use.
# In a central repository it's neccessary to do something like
# Repo[:font_tuffy]
# withe with per-type repos I can say
# Font[:tuffy] with is more compact and clear.
# Store items will have to be maintained both in
# Store by ID and in the relevant subclass  

script 'registry.rb'

class Store
  attr_reader :id
  attr_reader :name

  extend Registry
  
  # Initialize a storage item.
  def initialize(id, name) 
    @id   = id
    @name = name
  end

  # Load helper
  def self.load_something(name, vpath, klass = nil, &block)
    # Need to drop first or get_unusedid will not cotrrectly reuse the dropped id
    Store[name].drop! if Store[name]
    id    = Eruta::Store.get_unused_id(1000)
    return nil if id < 0
    res = block.call(id)
    return nil unless res
    klass ||= self
    loaded = klass.new(id, name)
    Store.register(loaded, id, name)
    p Store.registry_by_name.keys
    return loaded
  end


  

   
  
  # Loads a tile map
  def self.load_tilemap(name, vpath)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_tilemap(nid, vpath)
    end 
  end
  
  # type numbers 
  NONE          = 0
  BITMAP        = 1 
  FONT          = 2 
  AUDIO_STREAM  = 3 
  SAMPLE        = 4
  CONFIG        = 5
  OTHER         = 8
  TILEMAP       = 9

  
  # The type number of the stored object 
  def kind
    return Eruta::Store.kind(@id)
  end
  
  # Drops this object from storage
  def drop!
    res = Eruta::Store.drop(@id)
    Store.unregister(self)
    return res
  end
  

  # extend this module to makea class forward it's load, drop! and lookup calls 
  module Forward
    def forward_name(name)
      return self.to_s.downcase + '_' + name.to_s
    end
    
    def [](name)
      p "forward", Store.registry_by_name.keys
      return Store[forward_name(name)]
    end

    def drop!(name)
      obj = self[name]
      obj.drop! if obj
    end
  end
  
end


