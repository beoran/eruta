# ultra tiny behaviour driven / specification / test driven development helper
require 'pathname'
require 'timeout'

module Attobdd

  module Attoansi
    ATTRIBUTES = { 
    :reset     => 0 , :bold     => 1 , :dark      => 2 , :underline => 4 ,
    :blink     => 5 , :negative => 7 , :concealed => 8 , :black     => 30,
    :red       => 31, :green    => 32, :yellow    => 33, :blue      => 34,
    :magenta   => 35, :cyan     => 36, :white     => 37, :on_black  => 40,
    :on_red    => 41, :on_green => 42, :on_yellow => 43, :on_blue   => 44,
    :on_magenta=> 45, :on_cyan  => 46, :on_white  => 47,  }
   
    # Replaces symbols witch refer to ANSII escape codes to the escape codes.
    def color(*args)
      out = []
      for arg in args
        col = ATTRIBUTES[arg] 
        if col  
          out << "\e[#{col}m"
        else
          out << arg
        end
        out << "\e[0m"
      end
      out
    end
    
    # puts output colored with ANSI escape codes
    def puts(*args)
      Kernel.puts(*self.color(*args))
    end
    
    # printf output colored with ANSI escape codes 
    def printf(*args)
      Kernel.printf(*self.color(*args))
    end
    
    extend self
    
  end  
    
   
  
# A vendorded nanotest class. Based on the Nanotest gem, but included here
# For ease of use. 
module Nanotest
  extend self
  
#   class Result
#     attr_reader :file
#     attr_reader :line
#     attr_reader :text
#     attr_reader :fail
#     def initialize(text, file, line, kind) 
#     end
#   end

  @@failures, @@dots = [], []

  def assert(msg=nil, file=nil, line=nil, stack=caller, &block)
    res    = nil
    raised = nil
    begin 
      Timeout.timeout(10) do 
        res = block.call
      end  
    rescue 
      raised = $!
      res = nil
    end
      
    if res
      @@dots << '.'
    elsif raised 
      f,l = stack.first.match(/(.*):(\d+)/)[1..2]
      @@failures << "(%s:%0.3d) %s" % [file || f, line || l, msg || "exception raised!\n" + raised.to_s + "\n" + raised.backtrace.join("\n") ]
      @@dots << 'E'
    else 
      f,l = stack.first.match(/(.*):(\d+)/)[1..2]
      @@failures << "(%s:%0.3d) %s" % [file || f, line || l, msg || "assertion failed"]
      @@dots << 'F'
    end
  end

  def self.results #:nodoc:
    @@dots.join + "\n" + @@failures.join("\n")
  end

  at_exit { puts results unless results.strip.empty?; exit @@failures.empty? }
end




module Nanotest
  class << self
    alias :redgreen__orig_results :results
    def results
      ::RedGreen.colour = (@@failures.empty?) ? :green : :red
      redgreen__orig_results.strip + "\n" + RedGreen.visual + "\n\n"
    end
  end
end


include Nanotest
 
 
 
 
 
 
 
end
 


