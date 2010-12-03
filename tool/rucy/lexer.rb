
class Lexer
  
  class Token
    attr_reader :name
    attr_reader :regexp
    
    def initialize(regexp, name=nil)
      if name 
        @name      = name.to_sym
      else  
        @name      = regexp.source.to_sym
      end      
        raise "Cannot use empty name for #{regexp}." unless(@name && (not @name.to_s.empty?) ) 
      @regexp   = Regexp.new('\\A' + regexp.source) 
    end
    
    def match(input)
      return @regexp.match(input)
    end 
    
    def to_s
      return "#{@name}"  
    end
    
    def ==(other)
      return @name == @other.name
    end
     
  end
  
  class Word
    attr_reader :token
    attr_reader :text
    attr_reader :start
    attr_reader :stop
    attr_reader :linenr
    attr_reader :colnr
    
    def initialize(token, text, start=0, stop=0, linenr=0, colnr=0)
      @token  = token  
      @text   = text
      @start  = start 
      @stop   = stop
      @linenr = linenr
      @colnr  = colnr
    end
    
    def to_s
      return "#{@token}:#{@text}:#{@linenr}"
    end
    
  end
  
  class Error < ::Exception
    
  end
  
  def initialize(txt = '')
    self.text= (txt)
    @tokens       = {}
    @tokens_order = []
  end
  
  def text=(txt)
    @text         = txt
    @current      = txt
    @position     = 0
    @linenr       = 1
    @colnr        = 0
    @words        = []
    # Words already passed out
    @stop         = txt.size
  end
  
  def add_token(tok)
    @tokens_order           << tok
    # Append tokens in right order.
    @tokens[tok.name]       = tok
    # Hash map of tokens
  end
  
  def get_token(name) 
    return @tokens[name.to_sym]
  end
  
  def token(regexp, name = nil)
    tok  = Token.new(regexp, name)
    add_token(tok)
  end
  
  def advance(by)
    @position = @position + by
    @current  = @text[@position, @text.size]
  end
  
  # Lex the text that was given, and get a word from it
  # If the expected parameter is given, then it should contain a 
  # list of the symbolic names of the acceptable tokens. Otherwise all 
  # tokens that the lexer knows about are checked.
  # 
  # Returns the token that matches expected. Returns nil if the end of the text is reached.
  # Will throw a Lexer::Error if the lexer's text does not mach any know or expected token
  #
  def get(expected = [])
    return nil if @position >= @text.size
    return nil if @current == ""
    return get_expected(expected) if expected.size > 0
    @tokens_order.each do | tok |
      match = tok.match(@current)
      if(match)  
        matchtext = match[0]
        delta =  matchtext.size
        if matchtext.size < 1
           raise Lexer::Error.new("Token #{tok} matches empty text at: #{@linenr} : #{@current[0,32]} ...") 
        end  
        word = Word.new(tok, matchtext, @position, @position + delta, @linenr)
        advance(delta)
        @words.push word
        @linenr += matchtext.scan(/\r\n|\n|\r/).size
        # Count the amount of newlines in the match text and add that to the line nr.
        return word
      end  
    end
    raise Lexer::Error.new("Unknown text at : #{@linenr} : #{@current[0,32]} ...")
  end
  
  # Unlex the last lexed token 
  def unget()
      last = @words.last
      @words.pop
      return last
  end
  
  def each(expected=[])
    token = get(expected)
    while token do
      yield token
      token = get(expected)
    end
    return nil
  end
  
  
  
end

if $0 == __FILE__

=begin 
 Block comment for testing of lexer.
=end 
  
  lexer = Lexer.new()
  # Strings and regexps
  lexer.token( %r{"([^"\\]*(\\.[^"\\]*)*)*"}, :string)
  lexer.token( %r{'([^'\\]*(\\.[^'\\]*)*)*'}, :simple_string)
  # lexer.token( %r{/([^/\\]*(\\.[^/\\]*)*)*/}, :regexp)
  lexer.token( %r{/([^/\\]?(\\.[^/\\]*)?)*/}, :regexp)
  lexer.token( /\%r\{[^\}]*\}/, :regexp_2)
  lexer.token( /\%q\{[^\}]*\}/, :string_2)
  lexer.token( /\%Q\{[^\}]*\}/, :simple_string_2)
  
  
  # Keywords
  lexer.token(/if/)
  lexer.token(/then/)
  lexer.token(/else/)
  lexer.token(/end/)
  lexer.token(/case/)
  lexer.token(/ensure/)
  lexer.token(/module/)
  lexer.token(/elsif/)
  lexer.token(/def/)
  lexer.token(/rescue/)
  lexer.token(/not/)
  lexer.token(/then/)
  lexer.token(/yield/)
  lexer.token(/for/)
  lexer.token(/self/)
  lexer.token(/false/)
  lexer.token(/retry/)
  lexer.token(/return/)
  lexer.token(/true/)
  lexer.token(/if/)
  lexer.token(/defined\?/)
  lexer.token(/super/)
  lexer.token(/undef/)
  lexer.token(/break/)
  lexer.token(/in/)
  lexer.token(/nil/)
  lexer.token(/until/)
  lexer.token(/unless/)
  lexer.token(/or/)
  lexer.token(/next/)
  lexer.token(/when/)
  lexer.token(/redo/)
  lexer.token(/and/)
  lexer.token(/begin/)
  lexer.token(/__LINE__/)
  lexer.token(/class/)
  lexer.token(/__FILE__/)
  lexer.token(/END/)
  lexer.token(/BEGIN/)
  lexer.token(/while/)
  lexer.token(/alias/) 
  
  lexer.token(/\./       , :op_send     ) 
  # The period operator sends a message
  
  lexer.token(/\:\:/       , :namespace   ) 
  # The namespace operator.
  
  # Assignment with operation operators
  lexer.token(/\+=/       , :op_plus_assign     ) 
  lexer.token(/\-=/       , :op_minus_assign    ) 
  lexer.token(/\*\*=/     , :op_power_assign    ) 
  lexer.token(/\*=/       , :op_multiply_assign )
  lexer.token(%r{\/=}     , :op_divide_assign   )
  lexer.token(%r{\%=}     , :op_modulus_assign    )  
  lexer.token(%r{<<=}     , :op_leftshift_assign  )
  lexer.token(%r{>>=}     , :op_rightshift_assign )  
  lexer.token(/\|\|=/     , :op_oror_assign) 
  lexer.token(/\&\&=/     , :op_andand_assign)   
  lexer.token(/\|=/       , :op_or_assign) 
  lexer.token(/\&=/       , :op_and_assign)   
  
  # Math operators
  lexer.token(/\+/       , :op_plus     ) 
  lexer.token(/\-/       , :op_minus    ) 
  lexer.token(/\*\*/     , :op_power    ) 
  lexer.token(/\*/       , :op_multiply )
  lexer.token(%r{\/}     , :op_divide   )
  lexer.token(%r{\%}     , :op_modulus  )  
  lexer.token(%r{<<}     , :op_leftshift )
  lexer.token(%r{>>}     , :op_rightshift)  
      
  
  # Logical operators  
  lexer.token(/\|\|/      , :op_oror     ) 
  lexer.token(/\&\&/      , :op_andand   )   
  lexer.token(/\|/        , :op_or      ) 
  # Pipe character, may be an op or a parameter
  lexer.token(/\&/        , :op_and     )   
  

  
  # Comparison operators
  
  lexer.token(/=>/       , :op_more_or_equal ) 
  lexer.token(/>/        , :op_more ) 
  lexer.token(/<=/       , :op_less_or_equal ) 
  lexer.token(/</        , :op_less          )
  lexer.token(/===/      , :op_long_equal    )
  lexer.token(/==/       , :op_equal         )
  lexer.token(/~=/       , :op_mathc         )
  # Match operator
  # Assignment operator
  lexer.token(/=/        , :op_assign        )
  # Block separators
  lexer.token(/\{/       , :block_open       )
  lexer.token(/\}/       , :block_close      )
  # Parenthesis
  lexer.token(/\(/       , :paren_open       )
  lexer.token(/\)/       , :paren_close      )
  # Brackets
  lexer.token(/\[/       , :bracket_open     )
  lexer.token(/\]/       , :bracket_close    )
  
  
  # Comma separator
  lexer.token(/\,/       , :comma            )
    
  
  lexer.token(/[0-9][0-9_]*/                     , :integer        )
  # An integer number
  lexer.token(/[0-9][0-9_]*(\.[0-9]+)?([eE]-?[0-9])?/, :float      ) 
  # A floating point number, with possible exponent and mantissa
  # A symbol starts with a : and a letter
  lexer.token(/\:[A-Za-z][A-Za-z0-9_]+/       , :symbol             ) 
  
  # Constants
  lexer.token(/[A-Z][A-Za-z0-9_]+/             , :constant           ) 
  # A global variable, can contain any characters
  lexer.token(/\$[^ ]+/                        , :global_variable    ) 
  # Instance variable starts with an @
  lexer.token(/\@[a-z][A-Za-z0-9_]+/           , :instance_variable  ) 
  # Class variable starts with an @@
  lexer.token(/\@@[a-z][A-Za-z0-9_]+/          , :class_variable     ) 
  # Normal variable, can also be a method call to self, a parameter, or local variable
  # May end with ? or !
  lexer.token(/[a-z][A-Za-z0-9_]+[\?\!]?/      , :variable_message   ) 
  
  
  # Comments
  lexer.token(/\#[^\r\n]*/                 , :comment_line) 
  lexer.token(/=begin[.\r\n]*=end/         , :comment_block) 
  

  lexer.token(/[ \t]+/                    , :whitespace) 
  lexer.token(/(\r\n|\r|\n|;)/            , :newline) 
  # New line, may also be generated with a ;
  
  thisfile = File.open(__FILE__, 'r')
  thistext = thisfile.read()
  thisfile.close
  # Now for the test, lexer, lex yourself!
  
  lexer.text = thistext
  
  lexer.each do | token |
    puts token.to_s
  end
  
  
  
end
