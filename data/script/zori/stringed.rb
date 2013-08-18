module Zori
  # Stringed is an editable (multiline) string that keeps track of it's cursor 
  # position
  class Stringed
    attr_reader :line
    attr_reader :column
    def initialize(text, eol="\n")
      @eol    = eol
      @lines  = text.split
      tex.split(/(\r\n)|(\r)|(\n)/)
      @cursor = @string.
    end
  end
end
    
    