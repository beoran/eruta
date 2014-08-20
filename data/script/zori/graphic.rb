module Zori
  # Helper module for everyhing that uses Graphs
  module Graphic
    attr_reader :graph

    def initialize(params={}, &block)
      init_graph()
    end
    
    
    def init_graph()
      @graph = []
    end
    
    def hide_graph
      @graph.each  { |g| g.visible = false }
    end
    
    def show_graph
      @graph.each  { |g| g.visible = true }
    end
   
    def move_graph(x, y)
      @graph.each  { |g| g.position= [x, y] }
    end
   

    def graph_box(x, y, w, h, rx = 4, ry = 4, style_id = -1)
      gr = Graph.make_box(x, y, w, h, rx, ry, style_id)
      graph_add(gr)
      return gr
    end

    def graph_image(x, y, store_id, style_id = -1)
      gr = Graph.make_image(x, y, store_id, style_id)
      graph_add(gr)
      return gr
    end

    def graph_text(x, y, w, h, text, style_id = -1)
      gr = Graph.make_text(x, y, w, h, text, style_id)
      graph_add(gr)
      return gr
    end
    
    def graph_longtext(x, y, w, h, text, style_id = -1)
      gr = Graph.make_longtext(x, y, w, h, text, style_id)
      graph_add(gr)
      return gr
    end

    def graph_add(gr)
      @graph << gr
    end

    def graph_delete(gr)
      @graph.delete(gr)
    end

  end
end
