module Zori    
  # A dialog that can run comands
  class Console < Dialog
    attr_reader :history
    
    def do_command(command, &block)
      reply           = block.call(command, binding)
      @output.heading = "#{reply.inspect}" rescue "Exception!"
      rescue SyntaxError, StandardError
      @output.heading = "Exception: #{$!}" rescue "Exception!"
    end

    
    def handle_enter(&block)
      command         = @input.value
      @history       << command 
      @histindex     += 1
      @input.text     = ''
      do_command(command, &block)
    end
    
        
    def handle_up
      @histindex     -= 1
      @histindex      = 0 if @histindex < 0
      command         = @history[@histindex] || ''
      @input.text     = command
    end
    
        
    def handle_down
      @histindex     += 1
      @histindex      = @history.size - 1 if @histindex >= @history.size
      command         = @history[@histindex] || ''
      @input.text     = command
    end


    
    def make_run_action(&block)
      dialog = self
      result = lambda do |input, why|
        case why
          when :up 
          handle_up
          when :down 
          handle_down
          else
          handle_enter(&block)
        end
      end
      result
    end
    
=begin
(&block)
["1+1", "1+1", "1+1", "2+2", "3+3", "3+3", "10+10", "10+10", "111111111111111", "p history", "p history", "p history", "\"hello world\"", "+10+++", "p history", "p history", "p history", "+10+++", "p history", "+10+++", "p history", "p history", "p history", "p history", "p history", "p history", "p history", "p history", "p history", "p history"]
Console execute called: p history: ["1+1", "1+1", "1+1", "2+2", "3+3", "3+3", "10+10", "10+10", "111111111111111", "p history", "p history", "p history", "\"hello world\"", "+10+++", "p history", "p history", "p history", "+10+++", "p history", "+10+++", "p history", "p history", "p history", "p history", "p history", "p history", "p history", "p history", "p history", "p history"]

=end
    
    def initialize(heading = 'Console', &block)
      super() {}        
      @history      = []
      @histindex    = 0
      @label        = Zori::Label.new(heading)
      @maxchar      = 200
      @input        = Zori::Input.new('#', &make_run_action(&block))
      @output       = Zori::Label.new('results')
      self        << @label
      self        << @input
      self        << @output
      @label.x      = 10
      @label.y      = 10
      @input.x      = 10
      @input.y      = 50
      @output.x     = 10
      @output.y     = 100
      @output.h     = 100
      @output.w     = 300
      # self.layout_all
      # self.fit_child_high_simple
      # self.fit_child_wide_simple
      @input.focus!
      # Focus on the input text
      # to_center
    end
    
  end
end