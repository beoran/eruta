require 'lexer'

class RubyLexer < Lexer


  def initialize()
    super
    setup_tokens
  end

  def setup_tokens
    # Strings and regexps
    token( %r{"([^"\\]*(\\.[^"\\]*)*)*"}, :string)
    token( %r{'([^'\\]*(\\.[^'\\]*)*)*'}, :simple_string)
    # token( %r{/([^/\\]*(\\.[^/\\]*)*)*/}, :regexp)
    token( %r{/([^/\\]?(\\.[^/\\]*)?)*/}, :regexp)
    token( /\%r\{[^\}]*\}/, :regexp_2)
    token( /\%q\{[^\}]*\}/, :string_2)
    token( /\%Q\{[^\}]*\}/, :simple_string_2)
    
    
    # Keywords
    token(/if/)
    token(/then/)
    token(/else/)
    token(/end/)
    token(/case/)
    token(/ensure/)
    token(/module/)
    token(/elsif/)
    token(/def/)
    token(/rescue/)
    token(/not/)
    token(/then/)
    token(/yield/)
    token(/for/)
    token(/self/)
    token(/false/)
    token(/retry/)
    token(/return/)
    token(/true/)
    token(/if/)
    token(/defined\?/)
    token(/super/)
    token(/undef/)
    token(/break/)
    token(/in/)
    token(/nil/)
    token(/until/)
    token(/unless/)
    token(/or/)
    token(/next/)
    token(/when/)
    token(/redo/)
    token(/and/)
    token(/begin/)
    token(/__LINE__/)
    token(/class/)
    token(/__FILE__/)
    token(/END/)
    token(/BEGIN/)
    token(/while/)
    token(/alias/) 
    
    token(/\./       , :op_send     ) 
    # The period operator sends a message
    
    token(/\:\:/       , :namespace   ) 
    # The namespace operator.
    
    # Assignment with operation operators
    token(/\+=/       , :op_plus_assign     ) 
    token(/\-=/       , :op_minus_assign    ) 
    token(/\*\*=/     , :op_power_assign    ) 
    token(/\*=/       , :op_multiply_assign )
    token(%r{\/=}     , :op_divide_assign   )
    token(%r{\%=}     , :op_modulus_assign    )  
    token(%r{<<=}     , :op_leftshift_assign  )
    token(%r{>>=}     , :op_rightshift_assign )  
    token(/\|\|=/     , :op_oror_assign) 
    token(/\&\&=/     , :op_andand_assign)   
    token(/\|=/       , :op_or_assign) 
    token(/\&=/       , :op_and_assign)   
    
    # Math operators
    token(/\+/       , :op_plus     ) 
    token(/\-/       , :op_minus    ) 
    token(/\*\*/     , :op_power    ) 
    token(/\*/       , :op_multiply )
    token(%r{\/}     , :op_divide   )
    token(%r{\%}     , :op_modulus  )  
    token(%r{<<}     , :op_leftshift )
    token(%r{>>}     , :op_rightshift)  
        
    
    # Logical operators  
    token(/\|\|/      , :op_oror     ) 
    token(/\&\&/      , :op_andand   )   
    token(/\|/        , :op_or      ) 
    # Pipe character, may be an op or a parameter
    token(/\&/        , :op_and     )   
    
  
    
    # Comparison operators
    
    token(/=>/       , :op_more_or_equal ) 
    token(/>/        , :op_more ) 
    token(/<=/       , :op_less_or_equal ) 
    token(/</        , :op_less          )
    token(/===/      , :op_long_equal    )
    token(/==/       , :op_equal         )
    token(/~=/       , :op_mathc         )
    # Match operator
    # Assignment operator
    token(/=/        , :op_assign        )
    # Block separators
    token(/\{/       , :block_open       )
    token(/\}/       , :block_close      )
    # Parenthesis
    token(/\(/       , :paren_open       )
    token(/\)/       , :paren_close      )
    # Brackets
    token(/\[/       , :bracket_open     )
    token(/\]/       , :bracket_close    )
    
    # Comma separator
    token(/\,/       , :comma            )
      
    # Numbers
    # An integer number
    token(/[0-9][0-9_]*/                     , :integer         )
    # A floating point number, with possible exponent and mantissa    
    token(/[0-9][0-9_]*(\.[0-9]+)?([eE]-?[0-9])?/, :float       ) 
    # A symbol starts with a : and a letter
    token(/\:[A-Za-z][A-Za-z0-9_]+/       , :symbol             ) 
    # Constants
    token(/[A-Z][A-Za-z0-9_]+/             , :constant           ) 
    # A global variable, can contain any characters
    token(/\$[^ ]+/                        , :global_variable    ) 
    # Instance variable starts with an @
    token(/\@[a-z][A-Za-z0-9_]+/           , :instance_variable  ) 
    # Class variable starts with an @@
    token(/\@@[a-z][A-Za-z0-9_]+/          , :class_variable     ) 
    # Normal variable, can also be a method call to self, a parameter, or local variable
    # May end with ? or !
    token(/[a-z][A-Za-z0-9_]+[\?\!]?/      , :variable_message   ) 
    
    
    # Comments
    token(/\#[^\r\n]*/                 , :comment_line) 
    token(/=begin[.\r\n]*=end/         , :comment_block) 
    
  
    token(/[ \t]+/                    , :whitespace) 
    token(/(\r\n|\r|\n|;)/            , :newline) 
    # New line, may also be generated with a ;
  end
end


if $0 == __FILE__ then

=begin 
 Block comment for testing of lexer.
=end 
  
  lexer = RubyLexer.new()

  thisfile = File.open(__FILE__, 'r')
  thistext = thisfile.read()
  thisfile.close
  # Now for the test, lexer, lex yourself!
  
  lexer.text = thistext
  
  lexer.each do | token |
    puts token.to_s
  end

end  