module Rogaru
  class World
    class Collision
      attr_accessor :first
      attr_accessor :last

      def initialize(first, last)
        @first = first
        @last = last
      end
    end
  end # class Space
end # module Rogaru
