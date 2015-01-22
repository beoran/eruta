# module that keeps track of the state of the game
# Just so no global variables are needed...
module State
  class << self
    def prepare_registry
      @registry ||= {}
    end

    def get(name)
      prepare_registry
      return @registry[name.to_sym]
    end

    def put(name, value)
      prepare_registry
      return @registry[name.to_sym] = value
    end


    def [](name)
      prepare_registry
      return get(name)
    end

    def []=(name, value)
      prepare_registry
      return put(name, value)
    end

    def method_missing(name, *args)
      names = name.to_s
      if names[-1] == '='
        self.put(names[0, names.size - 1], *args)
      else
        self.get(names)
      end
    end

  end
end





