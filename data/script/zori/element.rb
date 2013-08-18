module Zori
  module Element
    attr_reader   :can
    attr_reader   :state
    attr_reader   :style
    attr_reader   :bounds
    attr_accessor :z

    def initialize(x, y, w, h)
      init_graph
      @z            = 0
      @style        = Zori::Style.default.dup
      # XXX: should be copy deep of the default style
      @can          = Zori::Capability.new
      @state        = Zori::State.new
      @state.set(:active)
      # Widget is active
      @bounds       = Zori::Rect.new(x, y, w, h)
      # @action       = action
    end

  end
end


