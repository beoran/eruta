#
#
# Lexer for the Raku minimal parser/interpreter/compiler
#
#

require 'strscan'

module Raku
   # Lexer for Raku: FIXME this lexer is way too slow, and will probably 
   # need to be rewritten using StringIO, or such.
   class Lexer
    attr_reader :line
    attr_reader :col

    # Initialize the lexer with empty text
    def initialize(text="")
      set_text(text)
    end
    
    # Sets the text to be lexed
    def set_text(text)
      text ||= ""
      # start with an empty scan string/
      @text   = text
      @scan   = StringScanner.new(@text)
      @line   = 1
      @col    = 1
    end
    
    
    # Generates a token. shortcut to Token.new
    def token(kind, value = "")
      return Token.new(kind, value, @line, @col)
    end
    
    # Generates a token. shortcut to Token.new, but only if text is not nil
    # returns nil if text or kind is nil
    def try_token(kind, value = nil)
      return nil if (!kind || !value) 
      return Token.new(kind, value, @line, @col)
    end
    
    
    # scans self, automatically advancing line and col
    def scan(pattern)
      res = @scan.scan(pattern)
      return res unless res
      # advance col and line if we were able to parse some tokens off.
      # XXX: this counts the newline characters, but is buggy
       
      aid     = res.split(/\r\n|\n|\r/, -1)
      aidsize = aid.size
      if aidsize > 1 # more than 1 means we had at least one newline 
        @line += aid.size - 1
        # add the amount of lines skipped 
        @col   = aid.last.size
        # column is advanced 
      else 
        @col  += res.size
      end   
      return res
    end
    
    
    # Tries to lex an escaped newline, or a newline.
    def lex_nl
      res = scan(/\\[ \t]*(\r\n|\r|\n)+/)
      return token(:esc_nl, res) if res
      res = scan(/(\r\n|\r|\n)+/)
      return try_token(:nl, res)
    end
    
    
    # Tries to lex whitespace. returns a token on success, nil on failiure.
    def lex_ws
      res = scan(/[ \t]+/)        
      return try_token(:ws, res)
    end
    
    def lex_kw(name)
      res = scan(%r{#{name}(?![^ \t\n\r])})
      return try_token(name.to_sym, res) if res
    end
    
    
    # Tries to lex keywords, brackets, special operators and separators
    def lex_key
      search = {
        '('   => :lparen,
        ')'   => :rparen,
        '['   => :lbracket,
        ']'   => :rbracket,
        '{'   => :lbrace,
        '}'   => :rbrace,
        ';'   => :semicolon,
      }
      res  = scan(%r{[\(\)\[\]\{\}\:\;\,\.]})
      type = search[res]
      return try_token(type, res) if res
      res = lex_kw(:do)
      return res if res
      res = lex_kw(:end)
      return res if res
      return nil
    end
    
    # Tries to lex comments
    def lex_comment
      # Multi line comment
      res = scan(%r{\#\(([\r\n]|.)*?\)\#})
      return try_token(:comment, res) if res 
      
      # C style multi line comment
      res = scan(%r{\/\*([\r\n]|.)*?\*\/})
      return try_token(:comment, res) if res 
      
      # C style single line comment
      res = scan(%r{\/\/[^\r\n]+(\r\n|\r|\n)})
      return try_token(:comment, res) if res 
      
      # Shell like single line comment 
      res = scan(%r{\#[^\r\n]+(\r\n|\r|\n)})
      return try_token(:comment, res)
    end
    
    # Tries to lex symbols (start with a letter, and can contain letters, numbers and underscores)
    def lex_symbol
      # Symbols
      res = scan(%r{\w[\w\d_]*})
      return try_token(:symbol, res)
    end
    
    # Tries to lex operators (consist of one or more of &|@^!*%+-=/<>$,.:
    def lex_operator
      # Operators
      res = scan(%r{[\&\|\@\^\!\*\%\+\-\=\/\<\>\$\,\.\:]+})
      return try_token(:operator, res)
    end
    
    # Tries to lex integer numbers
    def lex_int()
      res = scan(%r{(\-)?\d[\w\d_]*})
      return try_token(:integer, res)
    end
    
    # Tries to lex floating point numbers
    def lex_float()
      res = scan(%r{(\-)?\d[\d_]*\.[\d_]+([eE]\d[\d_]*(\.[\d_]+)?)?})
      return try_token(:float, res)
    end
    
    #
    # tries to lex strings
    def lex_string()
      res = scan(%r{"([^"\\]|[\r\n]|\\.)*"})
      try = try_token(:string, res)
      return try if try
      res = scan(%r{'([^'\\]|[\r\n]|\\.)*'})
      try = try_token(:string, res)
      return try if try
      res = scan(%r{`([^`\\]|[\r\n]|\\.)*`})
      try = try_token(:string, res)
      return try if try
    end
    
    LEX_ORDER = [ 
      :lex_comment,
      :lex_key,
      :lex_float,
      :lex_int,
      :lex_symbol,
      :lex_string,
      :lex_operator,
      :lex_nl, 
      :lex_ws,
    ]
    
    def lex_fail(msg)
      return Token.new(:fail, "Lex error text at #{@line}: #{@col}: #{@scan.peek(30)}...: #{msg}")
    end
    
    def lex_try
      return token(:eof) if @scan.eos?
      ch = @scan.peek(1)
      case ch
        when '#'
          return lex_comment || lex_fail("Expected comment")
        when '/'
          return lex_comment || lex_operator || lex_fail("Expected comment or operator")
        when ' ', "\t"
          return lex_ws || lex_fail("Expected whitespace")
        when "\r", "\n", '\\'
          return lex_nl || lex_fail("Expected escaped newline or newline")
        when '0', '1', '2', '3', '4', '5', '6', '7' , '8', '9'
          return lex_float || lex_int || lex_fail("Expected number")
        when '-'
          return lex_float || lex_int || lex_operator || 
          lex_fail("Expected number or operator")
        when '"', "'", '`'
          return lex_string || lex_fail("Expected string.")
        when '(',  ')',  '[', ']', '{', '}', ';'
          return lex_key || lex_fail("Expected key symbol.")
        else
          return lex_key || lex_symbol || lex_operator ||
          lex_fail("Expected keyword, symbol or operator, got >#{ch}<")
      end
    end
    
    def lex
      res = lex_try
      return res if res
      return Token.new(:fail, "Lex error text at #{@line}: #{@col}: #{@scan.peek(30)}...")
    end
      
      
    # Gets one token. Returns fail? on failure.
    def lex_rest()
      return token(:eof) if @scan.eos?
      # Try to lex the various possiblel exemes in the list. 
      for action in LEX_ORDER do 
        tok = self.send(action)
        return tok if tok 
        # If we find something, return it.
      end
      # If we got here, it all failed. 
      return Token.new(:fail, "Lex error text at #{@line}: #{@col}: #{@scan.peek(30)}...")
    end
    
    # Gets tokens until the kind is not in the skip list 
    def lex_skip(*skiplist)
      tok = lex() 
      while tok && !tok.eof? && !tok.fail? &&
            skiplist.member?(tok.kind)
        tok = lex()
        return tok if tok.kind == :eof
        return tok if tok.kind == :fail
      end
      return tok
    end
    
    # Gets all tokens, ignoring those to be skipped in skiplist, 
    # and returns them as an array
    def lex_all(*skiplist)
      res = []
      tok = self.lex 
      while tok && tok.kind != :eof && tok.kind != :fail 
        res << tok unless skiplist.member?(tok.kind)
        tok = self.lex
      end
      res << tok unless skiplist.member?(tok.kind)
      return res
    end
    
    # Creates a lexer, lexes everything, and returns the results as an array
    def self.lex_all(prog, *skiplist)
      lexer = self.new(prog)
      return lexer.lex_all(*skiplist)
    end
      

  end
end



