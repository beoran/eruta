# require 'nanotest'
# require 'redgreen'

require 'pathname'
require 'timeout'
require 'term/ansicolor'

module RedGreen
  extend self

  attr_accessor :colour

  def visual(s=nil)
    s ||= ("="*78)
    Term::ANSIColor.send(colour || :green) { s }
  end
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
      Timeout.timeout(5) do 
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


def test_file(fname)
  return File.join('test', fname)
end


$: << '../lib'
# $: << '../ext/gari'
# require 'gari'


