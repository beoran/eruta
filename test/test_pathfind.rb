# Tests screen related functions
require 'test_common'
require 'sdl'

END { SDL.quit() }


# Pathfinding module
module PF
  class Node
  end
  
  # Very simplistic priority Queue
  class PriorityQueue
    def initialize
    @list = []
    end
    
    def add(item)
      # Add @list.length so that sort is always using Fixnum comparisons,
      # which should be fast, rather than whatever is comparison on `item'
      # @list << [item.cost, @list.length, item]
      @list << item 
      @list.sort!
      return self
    end
    
    def update
      @list.sort!
    end  
    
    def <<(pritem)
      add(*pritem)
    end
    
    # Returns item on top of priority queue and removes it from the queue
    # The item on top has the lowest priority value as determined by <=>
    def poll
      # @list.shift [2]
      @list.shift
    end
    
    def empty?
      @list.empty?
    end
  end
  
    # A path node
    class Node
      include Comparable
      attr_accessor :visited
      attr_accessor :path
      attr_accessor :cost
      attr_reader   :x
      attr_reader   :y
      
      def initialize(x, y, cost = 10)
        @x, @y    = x, y
        @path     = [ self ]
        @cost     = cost
        @visited  = false
      end
            
      def <=>(other)
        return  self.cost <=> other.cost
      end
      
      def inspect
        return "Node: (#@x, #@y), #@cost"
      end
            
      def add_path(node)
        # XXX... path is constructed in reverse. that kind of sucks.  
        for el in node.path do  
          @path << el 
        end  
      end

    end   
    
    # visited list
    class Visited
      def initialize(w, h)
        @data = Array.new(w) { Array.new(h) }  
        # @list[key]
        # @list = {}
      end
      
      def node2key(node)
        return [node.x, node.y]
      end
      
      def <<(node)
        # key = node2key(node)
        # @list[key] = true
        @data[node.x][node.y] = true
      end
      
      def member?(node)
        return @data[node.x][node.y]      
        p "Member?"
        p node         
        key = node2key(node)
        result = @list[key]
        p @list
        p result
        p "-----"
        return result 
      end
    end
  

  class Finder
    def initialize(map, step = 2)
      @map      = map
      @stepsize = step
    end
    
    # Simple distance estimate
    def estimate_simple(from, to)
      ([ (to.x - from.x).abs, (to.y - from.y).abs ].max) * 10      
    end
    
    
    
    
    # Complex distance estimate
    def estimate_complex(from, to) 
      dx = to.x - from.x
      dy = to.y - from.y
      return  Math.sqrt(dx*dx + dy*dy)
    end
    
    def estimate_square(from, to)
      dx = (to.x - from.x).abs
      dy = (to.y - from.y).abs
      if dx > dy 
        return 10*dy + 10*(dx - dy)
      else
        return 10*dx + 10*(dy - dx)
      end      
    end
    
    alias :estimate :estimate_complex 
    
    # Color difference (cost) estimate
    def color_cost(col1, col2)
      dr = (col1.r - col2.r).abs
      dg = (col1.g - col2.g).abs
      db = (col1.b - col2.b).abs     
      return (dr + dg + db)
    end
    
    def adjacent_node(node, dx, dy)
      mx   = node.x + dx
      my   = node.y + dy
      point= @map.get_point(mx, my)
      
      # Black points or points that are not 100% opaque are not considered
      if point.nil? || point == Sisa::Color::Black || (!point.opaque?) 
        return nil
      end
      
      cost = color_cost(point, @goal_color)      
      # Also not if cost is too high  
      if cost > 100
         return nil
      end
      
      
      
      return Node.new(mx, my, cost)
    end
    
    # Gets all the nodes around the current node
    def nodes_around(node)
      deltas = [[-1, 0], [0, 1], [1, 0], [0, -1]]
      dmul   = @stepsize
      result = []
      for delta in deltas do 
        dx, dy    = *delta
        adjacent  = adjacent_node(node, dx * dmul, dy * dmul)
        result << adjacent if adjacent
      end
      return result
    end
    
    def adjust_to_stepsize(val)
      res = val - (val % @stepsize)
      p val, res      
      return res
    end
    
    def adjust_coords(fromx, fromy, tox, toy)
      tox           = adjust_to_stepsize(tox)
      toy           = adjust_to_stepsize(toy)
      fromx         = adjust_to_stepsize(fromx)
      fromy         = adjust_to_stepsize(fromy)
      return fromx, fromy, tox, toy
    end
    
    def find_path(fromx, fromy, tox, toy)
      tox           = adjust_to_stepsize(tox)
      toy           = adjust_to_stepsize(toy)
      fromx         = adjust_to_stepsize(fromx)
      fromy         = adjust_to_stepsize(fromy)
      @goal_color   = @map.get_point(tox, toy)
      # Get the target's color
      start         = Node.new(fromx, fromy, 0)
      goal          = Node.new(tox, toy)
      open_nodes    = PriorityQueue.new 
      open_nodes   << start
      visited       = Visited.new(@map.w, @map.h)
      while !open_nodes.empty?
        old_node    = open_nodes.poll
        cost_so_far = old_node.cost
        next if visited.member?(old_node) 
        path_so_far = old_node.path.dup
        if (old_node.x == goal.x and old_node.y == goal.y)
          return old_node.path.reverse
        end
        visited            << old_node
        around              = nodes_around(old_node)
        around.each do | new_node |
          next if visited.member?(new_node)
          new_cost          = cost_so_far + new_node.cost
          new_node.cost     = new_cost    + estimate(new_node, goal)
          new_node.add_path(old_node)
          open_nodes << new_node
        end  
      end
      return nil
    end
    
    def find_path_start(fromx, fromy, tox, toy)
      fromx, fromy, tox, toy = adjust_coords(fromx, fromy, tox, toy)
      @goal_color   = @map.get_point(tox, toy)
      # Get the target's color
      @start        = Node.new(fromx, fromy, 0)
      @goal         = Node.new(tox, toy)
      @open_nodes   = PriorityQueue.new 
      @open_nodes  << @start
      @visited      = Visited.new(@map.w, @map.h)
    end
    
    def find_path_continue
      if @open_nodes.empty?
        return :fail, nil
      end   
      @old_node    = @open_nodes.poll
      cost_so_far  = @old_node.cost
      if @visited.member?(@old_node)
        return :busy, @old_node.path
      end     
      path_so_far  = @old_node.path.clone
      if (@old_node.x == @goal.x and @old_node.y == @goal.y)
        return :done, @old_node.path.reverse
      end
      @visited           << @old_node
      around              = nodes_around(@old_node)
      around.each do | new_node |
        if @visited.member?(new_node)
           next
        end
        new_cost          = cost_so_far + new_node.cost
        new_node.cost     = new_cost    + estimate(new_node, @goal)
        new_node.add_path(@old_node)
        @open_nodes << new_node
      end
      return :busy, path_so_far
   end
   



    
  end

end





module Test
  class Pathfind < Test::Common
    INPUT_FILE          = 'test_root_bw4.png'
    STEP_SIZE           = 1
    
    def setup
      Sisa.init_all
      open_screen(1280, 1024, false)
      open_queue
      @image            = Sisa::Surface.load_solid(INPUT_FILE)
      @start            = [ 132 / 2, 6 / 2 ]   #  
      @goal             = [ 369 / 2 - 1, 1689 / 2 - 1] # 
      # @start      =             [  55, 3   ]
      # @goal = [ 132, 541 ]
      @start_color      = @image.get_point(*@start)      
      @goal_color       = @image.get_point(*@goal)
      @finder           = PF::Finder.new(@image, STEP_SIZE)
    end
    
    def test_queue
      q = PF::PriorityQueue.new
      q << 7
      q << 2
      q << 5
      q << 1
      assert q.poll == 1, "PriorityQueue returns lowest sorting object first"  
    end

    def test_walk
      assert @screen
      @finder.find_path_start(*@goal, *@start)
      # @finder.find_path_start(*@start, *@goal)
      path      = nil
      status    = :busy
      old_path  = nil
 
      frames, time, fps = draw_wait do       
        wipe_screen
        @image.blit(@screen, 0, 0)
         # Continue to find if needed
         if status == :busy
          status, path = @finder.find_path_continue
         end
         if status == :done 
          color = Sisa::Color::Lime 
         elsif status == :fail
          color = Sisa::Color::Red
         else
          color = Sisa::Color::Yellow
         end 
         
         if path
          old_path = path
         end
           
         to_draw = path || old_path
         if to_draw
          for node in to_draw do
              @screen.put_rectangle(node.x, node.y, 
              STEP_SIZE * 2, STEP_SIZE * 2 , color)
          end
         end 
       end
       assert_drawn(20,50)
       if path
        result    = 0
        last_node = nil
        for node in path do
          if node && last_node
            ds = (node.x - last_node.x)**2 + (node.y - last_node.y)**2
            d  = Math.sqrt(ds)
            result += d
          end
          last_node = node
        end
        puts "Length of root: #{result}."
       else
        puts "Could not trace root." 
       end 
       
    end
    
    
  
  end
end  
