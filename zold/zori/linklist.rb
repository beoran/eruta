#
# Linklist is a mix in module that can transform any normal data 
# structure into a node of a doubly  linked list, that also works as a list 
# itself.
#
module Zori
  module Linklist
    # Include this in the nodes.
    module Node
      attr_accessor :node_next
      attr_accessor :node_previous
      
      # Appends a node in the list before self.
      def node_prepend(node)
        oldprev                 = self.node_previous
        self.node_previous      = node
        node.node_next          = self
        if oldprev # append head node to appended node, if any head
          oldprev.node_next     = node
          node.next             = oldprev
        end        
      end
      
      # Appends a node to the list behind self
      def node_append(node)
        oldnext                 = self.node_next
        self.node_next          = node
        node.node_previous      = self
        if oldnext # append tail node to appended node, if any tail
          oldnext.node_previous = node
          node.next             = oldnext
        end
      end
      
      # Iterates over the nodes, starting with self.
      def each(&block)
        now = self
        while now
          block.call(now)
          now = now.node_next
        end
      end
      
      # Iterates over the nodes in reverse order, starting with self.
      def each_reverse(&block)
        now = self
        while now
          block.call(now)
          now = now.node_previous
        end
      end

    end

    # A class to hold your linked list nodes.
    class List
      attr_accessor :list_head
      attr_accessor :list_tail
      def initialize()
        @list_head = nil
        @list_tail = nil
      end 

      def each(&block)
        return @list_head.each if @list_head
      end

    end

  end # module Linklist
end  