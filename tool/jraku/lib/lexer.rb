#
#
# Lexer for the JRaku minimal parser/interpreter/compiler
#
#

require 'strscan'

module Raku
   # A Fail is used for anything that fails.
   module Fail
     def fail!(msg = "")
       @fail_message = msg
       @fail_failed  = true
       return self
     end

     def fail?()
       @fail_failed  ||= false
       return @fail_failed
     end

     def fail_message()
       return @fail_message
     end
   end

   # A token is the result of a single step of the lexer
   class Token
     include Fail
     attr_reader :kind
     attr_reader :text

     def initialize(kind, text = "", line = 0, col = 0)
       @kind   = kind.to_sym
       @text   = text.to_s
       @line   = line
       @col    = col
     end
   end

   class Lexer
      # Initialize the lexer with empty text
      def initialize(text = "")
        text ||= ""
        # start with an empty scan string/
        @scan   = new StringScanner(text)
        @line   = 0
        @col    = 0
      end

      # Append text to the text to be lexed by the lexer.
      def <<(text)
        @scan << text
      end

      # generates a token. shortcut to Token.new
      def token(kind, text = "")
        return Token.new(kind, text, @line, @col)
      end

      # scans self, automatically advancing line and col
      def scan(pattern)
        res = @scan.scan(pattern)
        return res unless res
        # advance col and line if we were able to parse some tokens off.
        aid = res.split(/\r\n|\n|\r/)
        @line += aid.size
        @col  += aid.last.size
        return res
      end

      # gets the next character from self, as a string.
      # Automatically advances line and col
      def getch(pattern)
        ch = @scan.getch
        return ch unless ch
        # advance col and line if we were able to parse some tokens off.
        aid = res.split(/\r\n|\n|\r/)
        @line += 1 if ch == "\n" || ch == "\r"
        @col  += ch.size
        return res
      end

      # Tries to lex an escaped newline, or a newline.
      def lex_nl
        res = scan(/\\[ \t]*(\r\n|\r|\n)+/)
        return token(:esc_nl, res) if res
        res = scan(/(\r\n|\r|\n)+/)
        return token(:nl, res) if res
        return nil
      end


      # Tries to lex whitespace. returns a token on success, nil on failiure.
      def lex_ws
        res = scan(/[ \t]+/)
        return nil unless res
        return token(:ws, res)
      end

      # Tries to lex keywords, brackets, special operators and separators
      def lex_key
        search = {
          '('   => :lbrace,
          ')'   => :lbrace,
          '['   => :lbracket,
          ']'   => :rbracket,
          '{'   => :lcurly,
          '}'   => :rcurly,
          ':'   => :colon,
          ';'   => :semicolon,
          ','   => :comma,
          '.'   => :period,
        }
        for k, v in search do
          res = scan(k)
          return token(v, k) if res
        end
        return null
      end

      # Tries to lex comments
      def lex_comment
        # Single line comment
        res = scan(%r{\#[^\r\n]+(\r\n|\r|\n)})
        return token(:comment, text) unless res
      end
      
      # Tries to lex symbols (start with a letter, and can contain letters, numbers and underscores)
      def lex_symbol
        # Single line comment
        res = scan(%r{\w[\w\d_]+})
        return token(:symbol, text) unless res
      end

      # Tries to lex operators (consist of one or more of &|@^!*%+-=/<>$
      def lex_operator
        # Single line comment
        res = scan(%r{[&|@^!*%+-=/<>$]+})
        return token(:operator, text) unless res
      end
      
      # Tries to lex integer numbers
      def lex_int()
        res = scan(%r{\d[\w\d_]+})
        return token(:integer, text) unless res
      end

      # Tries to lex floating point numbers
      def lex_float()
        res = scan(%r{\d[\d_]+(\.)?[\d_]+([eE]\d[\d_]+(\.)?[\d_]+)?})
        return token(:integer, text) unless res
      end

      #
      # tries to lex strings

      # Gets one token. Returns fail? on failure.
      def lex()
        return token(:eof) if @scan.eos?
        tok = lex_ws
        return tok if tok
        tok = lex_nl
        return tok if tok
        tok = lex_key
        return tok if tok



        return fail!("Unknown text at #{line}: #{col}.")
      end



   end
end



