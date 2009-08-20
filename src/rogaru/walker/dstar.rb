module Rogaru
  # Allows an Actor to perform pathfinding.
  class Walker


    class Dstar
    # Ruby code inspired by C++ D star light algorithm implementation 
    # of James Neufeld (neufeld@cs.ualberta.ca)

      class State 
        attr_accessor :x
        attr_accessor :y
        attr_accessor :k1
        attr_accessor :k2
        def inspect
          return "State: (#@x, #@y)"
        end

        
        def ==(s2)
          return ((@x == s2.x) && (@y == s2.y))
        end
    
        def != (s2)
          return ((@x != s2.x) || (@y != s2.y))
        end
        
        def >(s2)
          if (@k1 > s2.k1) 
            return true
          elsif (@k1 < s2.k1) 
            return false
          end
          return k2 > s2.k2
        end
  
        def <=(s2) 
          if (@k1 <= s2.k1) 
            return true
          elsif (@k1 > s2.k1) 
            return false
          end
          return k2 <= s2.k2
        end
  
        def < (s2) 
          if (@k1 < s2.k1) 
            return true
          elsif (@k1 > s2.k1) 
            return false
          end
          return k2 < s2.k2      
        end
      end
  
  
      class Ipoint2
        attr_accessor :x
        attr_accessor :y
      end
  
      class Cellinfo
        attr_accessor :g
        attr_accessor :rhs
        attr_accessor :cost 
      end


      attr_reader :path
      attr_reader :c1
      attr_reader :k_m
      attr_reader :s_start
      attr_reader :s_goal 
      attr_reader :s_last
      attr_reader :max_steps  
      attr_reader :open_list
      attr_reader :cell_hash
      attr_reader :open_hash

            
      # Constructor sets constants.
      #
      def initialize()  
        @max_steps = 80000;  # node expansions before we give up
        @c1        = 1;      # cost of an unseen cell
        @cell_hash = {}
        @open_hash = {}
        @path      = []
        @open_list = Pqueue.new
        @s_start   = Dstar::State.new
        @s_goal    = Dstar::State.new
        @s_last    = Dstar::State.new
      end
  
    
      # Returns the key hash code for the state u, this is used to compare
      # a state that have been updated
      # 
      def key_hashcode(state) 
        return (state.k1 + 1193*state.k2).to_f
      end
    

      # Returns true if state is on the open list or not by checking if
      # it is in the hash table.
      # 
      def valid?(state)  
        return @open_hash.member?(state)
      end          

      # Returns the path created by replan()
      # 
      def get_path() 
        return @path
      end

      # returns true if the cell is occupied (non-traversable), false
      # otherwise. Non-traversable cells are marked with a cost < 0.
      # 
      def occupied?(state) 
        cell = @cell_hash[state]
        return false unless cell
        return (cell.cost < 0)
      end

      # Init dstar with start and goal coordinates, rest is as per
      # [S. Koenig, 2002]
      # 
      def init(sx, sy, gx, gy)
        @cell_hash.clear()
        @path.clear()
        @open_hash.clear()
        @open_list.pop() while(!@open_list.empty?)
        @k_m = 0;
        @s_start.x = sx
        @s_start.y = sy
        @s_goal.x  = gx
        @s_goal.y  = gy
        make_new_cell(@s_goal,0)
        make_new_cell(@s_start)
#         tmp        = Cellinfo.new 
#         tmp.g      = tmp.rhs      =  0
#         tmp.cost   = @c1
#         @cell_hash[@s_goal] = tmp
#         tmp        = Cellinfo.new        
#         tmp.g      = tmp.rhs = heuristic(@s_start, @s_goal)
#         tmp.cost   = @c1
#         @cellHash[@s_start] = @tmp;
#         @s_start   = calculate_key(@s_start);
#         @s_last    = @s_start
      end

      # Checks if a cell is in the hash table, if not it adds it in.
      # returns the cell found or created
      # 
      def make_new_cell(state, heurval = nil) 
        cell              = @cell_hash[state]
        return cell       if cell   
        tmp               = Cellinfo.new
        heurval         ||= heuristic(state, @s_goal)
        tmp.g             = tmp.rhs = heurval 
        tmp.cost          = @c1
        @cell_hash[state] = tmp
        return tmp
      end
  
      # Returns the G value for a state 
      # 
      def get_g(state) 
        return 0 if state == @s_goal
        return @cell_hash[state].g if @cell_hash.member? state
        return heuristic(state, @s_goal)
      end
   

      # Returns the rhs value for a state 
      #
      def get_rhs(state) 
        return 0 if state == @s_goal
        return @cell_hash[state].rhs if @cell_hash.member? state
        return heuristic(state, @s_goal)
      end

      # Sets the G value for a state      
      def set_g(state, g)
        cell    = make_new_cell(state)
        cell.g  = g
      end

      # Sets the rhs value for state u
      # 
      def set_rhs(state, rhs) 
        cell      = make_new_cell(state)
        cell.rhs  = rhs
      end
      
      M_SQRT2     = ( 2**0.5    ) 
      INFINITY    = ( 1.0 / 0.0 )
    
      # Returns the 8-way distance between state a and state b.
      # 
      def eight_way_distance(a, b)
        temp = 0
        min = (a.x - b.x).abs;
        max = (a.y - b.y).abs;
        min, max = max , min if (min > max)
        # swap min and max if needed  
        return ((M_SQRT2-1.0)*min + max);
      end

      # As per [S. Koenig, 2002] except for 2 main modifications:
      # 1. We stop planning after a number of steps, 'maxsteps' we do this
      #    because this algorithm can plan forever if the start is
      #    surrounded by obstacles. 
      # 2. We lazily remove states from the open list so we never have to
      #    iterate through it.
      # 
      def compute_shortest_path()  
        return 1 if @open_list.empty?
        s         = []
        k         = 0
        @s_start  = calculate_key(@s_start)
        while !@open_list.empty?   
           @s_start  = calculate_key(@s_start)
           test1     = @open_list.top < @s_start
           test2     = get_rhs(@s_start) != get_g(@s_start)
           break unless test1 || test2            
           state     = nil
   
          #  lazy remove
          loop do 
            return 1 if @open_list.empty?            
            state    = open_list.pop();
            continue if (!valid?(state)) 
            return 2 if (!(state < s_start) && (!test2)) 
            break
          end
    
          @open_hash[state]     
          @open_hash.delete(state) 
          k_old = state
          if (k_old < calculate_key(state)) # state is out of date
            insert(u);
          elsif (get_g(state) > getRHS(state)) # needs update (got better)
            set_g(state,get_rhs(state))
            s = get_pred(state)
            s.each { |i| update_vertex(i)} 
          else  # g <= rhs, state has got worse
            set_g(state,INFINITY);
            s = get_pred(state)
            s.each { |i| update_vertex(i)}
            update_vertex(state);
          end
        end  
        return 0
      end


      # Returns true if x and y are within 10E-5, false otherwise
      def close_by(x, y) 
        return true if x == INFINITY && y == INFINITY  
        return (x-y).abs < 0.00001
      end

      # As per [S. Koenig, 2002]
      # 
      def update_vertex(state)
        s = []
        if (state != @s_goal) 
          succ = get_succ(state)
          tmp = INFINITY
          tmp2 = 0.0
          succ.each do |i|   
            tmp2 = get_g(i) + cost(state, i)
            tmp  = tmp2 if (tmp2 < tmp)
          end
          set_rhs(state,tmp) unless close_by(get_rhs(state), tmp)
        end

        insert(state) unless close_by(get_g(state), get_rhs(state))
      end

      # Inserts state into openList and openHash.
      # 
      def insert(state) 
          state   = calculate_key(state)
          cur     = @open_hash[state]
          csum    = key_hashcode(state)
          @open_hash[state] = csum 
          @open_list.push(state, 0)
      end   

      # Removes state from open_hash. The state is removed from the
      # open_list lazilily (in replan) to save computation.
      # 
      def remove(state)  
        open_hash.delete(state)
      end
    
      # Euclidean cost between state a and state b.
      # 
      def true_distance(a, b)
        dx = a.x-b.x
        dy = a.y-b.y
        return Math.sqrt(dx*dx + dy*dy)
      end
    
      # Pretty self explanitory, the heristic we use is the 8-way distance
      # scaled by a constant C1 (should be set to <= min cost). 
      def heuristic(a, b) 
        return eight_way_distance(a, b)* @c1
      end
    
      def calculate_key(state)   
        val = [ get_rhs(state), get_g(state)].min  
        state.k1  = val + heuristic(state, @s_start) + k_m
        state.k2  = val
        return state
      end
    
      # double Dstar::cost(state a, state b)
      # --------------------------
      # Returns the cost of moving from state a to state b. This could be
      # either the cost of moving off state a or onto state b, we went with
      # the former. This is also the 8-way cost.
      # 
      def cost(a, b)
        dx    = (a.x-b.x).abs
        dy    = (a.y-b.y).abs
        scale = 1
        scale = M_SQRT2 if (dx + dy) > 1
        return scale * @c1 if (@cell_hash[a].nil?)
        return scale * @cell_hash[a].cost
      end
    
      # As per [S. Koenig, 2002]
      # 
      def update_cell(x, y, val) 
        state    = State.new
        state.x  = x;
        state.y  = y;
        return false if ((state == @s_start) || (state == @s_goal)) 
        make_new_cell(state) 
        @cell_hash[state].cost = val
        update_vertex(state)
      end
    
    
      SUCC_AID_8 = [ [-1,-1], [-1,0], [-1,1], [0,-1], [0,1], [1,-1], [1,0], [1,1] ]

      SUCC_AID = [ [-1, 0], [0, -1], [0, 1], [1, 0]]
    
      # Returns a list of successor states for state u, since this is an
      # 8-way graph this list contains all of a cells neighbours. Unless
      # the cell is occupied in which case it has no successors. 
      # 
      def get_succ(statein)
        s = []
        state     = statein.dup
        state.k1  = -1
        state.k2  = -1
        return s if (occupied?(state))
        for dx, dy in SUCC_AID do
          newstate    = state.dup
          newstate.x += dx
          newstate.y += dy
          s.unshift(newstate)
        end
        return s
      end
    
      # Returns a list of all the predecessor states for state u. Since
      # this is for an 8-way connected graph the list contails all the
      # neighbours for state u. Occupied neighbours are not added to the
      # list.
      # 
      def get_pred(statein)
        s = []
        state     = statein.dup
        state.k1  = -1;
        state.k2  = -1;
        for dx, dy in SUCC_AID do
          newstate    = state.dup
          newstate.x += dx
          newstate.y += dy
          s.unshift(newstate) unless occupied?(newstate)
        end
        return s
      end
    
      # Update the position of the actor, this does not force a replan.
      # 
      def update_start(x, y)
        @s_start.x = x
        @s_start.y = y
        @k_m      += heuristic(@s_last, @s_start)
        @s_start   = calculate_key(@s_start)
        @s_last    = @s_start
      end
    
      # This is somewhat of a hack, to change the position of the goal we
      # first save all of the non-empty on the map, clear the map, move the
      # goal, and re-add all of non-empty cells. Since most of these cells
      # are not between the start and goal this does not seem to hurt
      # performance too much. Also it frees a good deal of memory we
      # likely no longer use.
      # 
      def update_goal(gx, gy)
        to_add = []    
        i      = nil
        kk     = nil
        for k, i in @cell_hash do
            unless close_by(i, @c1)
              tp        = {}
              tp[:x]    = k.x
              tp[:y]    = k.y
              tp[:cost] = i.cost           
              to_add << tp
            end
        end
        init(@s_start.x, @s_start.y, gx, gy) 
        # reinitialize, refactored mercilessly 
        for kk in to_add do 
          update_cell( kk[:x], kk[:y], kk[:cost])    
        end
      end
    
      # Updates the costs for all cells and computes the shortest path to
      # goal. Returns true if a path is found, false otherwise. The path is
      # computed by doing a greedy search over the cost+g values in each
      # cells. In order to get around the problem of the robot taking a
      # path that is near a 45 degree angle to goal we break ties based on
      # the metric euclidean(state, goal) + euclidean(state,start). 
      #
       
      def replan() 
        @path.clear
        res = compute_shortest_path()
    #    printf("res: %d ols: %d ohs: %d tk: [%f %f] sk: [%f %f] sgr: (%f,%f)\n",res,@open_list.size(),@open_hash.size(),@open_list.top().k1,@open_list.top().k2, @s_start.k1, @s_start.k2,get_rhs(@s_start),get_g(@s_start));
        return false if (res < 0) 
        cur = @s_start; 
        return false if (get_g(s_start) == INFINITY)
    
        while(cur != @s_goal) 
          @path << cur;
          successors = get_succ(cur)
          return false if successors.empty? # no path to goal 
          cmin = INFINITY;
          tmin = 0.0;
          smin = nil
          for succ in successors do   
            # continue if (occupied?(succ))
            val  = cost(cur, succ);
            val2 = true_distance(succ, @s_goal) + true_distance(@s_start,succ); 
            #  (Euclidean) cost to goal + cost to pred
            val += get_g(succ)
            if (close_by(val,cmin)) 
              if (tmin > val2) 
                tmin = val2
                cmin = val
                smin = succ
              end
            elsif (val < cmin) 
              tmin = val2
              cmin = val
              smin = succ
            end
          end  
          cur = smin
          raise "Cur should not be nil" if cur.nil?   
        end
        @path  << @s_goal
        return true
      end

      # Adds one layer from a tile map to the world simulation
      def add_map_layer(layer, layer_index)
        layer.each_tile do |tile, x, y|
          if tile && (!tile.walk)
            update_cell(x, y, -1) # solid tile    
          end           
        end        
      end

      # Adds a tile map to the world simulation
      def add_map(map, oklayer = 1)
        map.each_layer do |layer_index, layer| 
          add_map_layer(layer, layer_index) if layer_index == oklayer          
        end        
      end

      def self.calculate(map, sx, sy, gx, gy)
          dstar = Rogaru::Walker::Dstar.new
          dstar.init(sx, sy, gx, gy)
          dstar.add_map(map, 1)
          res   = dstar.replan
          return res, dstar.get_path  
      end


      
    end # class Dstar

  end # walker
end # module 
