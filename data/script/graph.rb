
class Graph < Eruta::Graph
  class Node
    attr_accessor :id
    
    def initialize(id) 
      @id = id
      Graph.register(self)
    end
    
    def background_color=(r, g, b, a=255)
      Eruta::Graph.background_color_(@id, r, g, b, a)
    end
    
    def border_color=(r, g, b, a=255)
      Eruta::Graph.border_color_(@id, r, g, b, a)
    end
    
    def color=(r, g, b, a=255)
      Eruta::Graph.color_(@id, r, g, b, a)
    end
    
    def border_thickness=(t)
      Eruta::Graph.border_thickness_(@id, r, g, b, a)
    end
    
    def angle=(a)
      Eruta::Graph.angle_(@id, r, g, b, a)
    end
    
    def size=(w, h)
      Eruta::Graph.size_(@id, w, h)
    end
  end
  
  def self.registry
    @registry ||= {}
  end
  
  def self.register(thing)
    @registry ||= {}
    @registry[thing.id] = thing
  end
   
  
  def self.get_unused_id
    29000.times do | i | 
      return i unless self.registry[i]
    end
    return nil
  end
  
  def self.make_box(x, y, w, h, rx, ry, style_id = -1)
    id  = self.get_unused_id
    nid = Eruta::Graph.make_box(id, x, y, w, h, rw, ry, style_id) 
    return nil if (nid < 0)
    return Node.new(nid)
  end
               
  def self.make_image(x, y, store_id, style_id = -1)
    id  = self.get_unused_id
    nid = Eruta::Graph.make_image(id, x, y, store_id, style_id) 
    return nil if (nid < 0)
    return Node.new(nid)
  end
  
  def self.make_text(x, y, text, style_id = -1)
    id  = self.get_unused_id
    nid = Eruta::Graph.make_image(id, x, y, text, style_id) 
    return nil if (nid < 0)
    return Node.new(nid)
  end
  
end













