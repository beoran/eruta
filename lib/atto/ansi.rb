module Atto
  # ANSI colors
  module Ansi
    extend self
    
    ATTRIBUTES = { 
    :reset     => 0 , :bold     => 1 , :dark      => 2 , :underline => 4 ,
    :blink     => 5 , :negative => 7 , :concealed => 8 , :black     => 30,
    :red       => 31, :green    => 32, :yellow    => 33, :blue      => 34,
    :magenta   => 35, :cyan     => 36, :white     => 37, :on_black  => 40,
    :on_red    => 41, :on_green => 42, :on_yellow => 43, :on_blue   => 44,
    :on_magenta=> 45, :on_cyan  => 46, :on_white  => 47,  }
    
    # Replaces symbols witch refer to ANSI escape codes with those escape codes.
    def color(*args)
      out = []
      for arg in args
        col = ATTRIBUTES[arg] 
        if col  
          out << "\e[#{col}m"
        else
          out << arg
        end
      end
      out << "\e[0m"
      return out
    end
    
    # Replaces ansi codes and returns the joined string
    def color_string(*args)
      return color(*args).map { |e| e.to_s}.join
    end   
    
    # puts output colored with ANSI escape codes
    def puts(*args)
      Kernel.puts(*self.color(*args))
    end
    
    # printf output colored with ANSI escape codes 
    def printf(*args)
      Kernel.printf(*self.color(*args))
    end
  end
end