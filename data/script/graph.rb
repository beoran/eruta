
class Graph < Eruta::Graph
  class Node
    attr_accessor :id
  
    def initialize(id) 
      @id = id
      Graph.register(self)
    end
  
    def background_color=(col)
      r, g, b, a = *col
      a ||= 255
      Eruta::Graph.background_color_(@id, r, g, b, a)
    end
  
    def border_color=(col)
      r, g, b, a = *col
      a ||= 255
      Eruta::Graph.border_color_(@id, r, g, b, a)
    end
  
    def color=(col)
      r, g, b, a = *col
      a ||= 255
      Eruta::Graph.color_(@id, r, g, b, a)
    end
  
    def border_thickness=(t)
      Eruta::Graph.border_thickness_(@id, t)
    end
  
    def angle=(a)
      Eruta::Graph.angle_(@id, r, g, b, a)
    end
    
    def image_flags=(f)
      Eruta::Graph.image_flags_(@id,f)
    end
    
    def text_flags=(f)
      Eruta::Graph.text_flags_(@id, f)
    end
  
    def size=(size)
      w, h = *size
      Eruta::Graph.size_(@id, w, h)
    end
  
    def visible=(value) 
      Eruta::Graph.visible_(@id, value)
    end
  
    def z
      Eruta::Graph.z(@id)
    end
  
    def z=(newz)
      Eruta::Graph.z_(@id, newz)
    end
  
    def font=(store_id) 
      Eruta::Graph.font_(@id, store_id)
    end
  
    def image=(store_id) 
      Eruta::Graph.image_(@id, store_id)
    end
  
    def background_image=(store_id) 
      Eruta::Graph.background_image_(@id, store_id)
    end
  
    def speed=(v) 
      x, y = *v
      Eruta::Graph.speed_(@id, x, y)
    end
  
    def size=(sz) 
      x, y = *sz
      Eruta::Graph.size_(@id, x, y)
    end
  
    def position=(p) 
      x, y = *p
      Eruta::Graph.position_(@id, x, y)
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

  def self.make_box(x, y, w, h, rx = 4, ry = 4, style_id = -1)
    id  = self.get_unused_id
    nid = Eruta::Graph.make_box(id, x, y, w, h, rx, ry, style_id) 
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
    nid = Eruta::Graph.make_text(id, x, y, text, style_id)
    return nil if (nid < 0)
    return Node.new(nid)
  end

end

