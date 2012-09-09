# encoding: UTF-8
# ruby prototype for the Beo language

module Beo
  class Token 
    attr_accessor :type
    attr_accessor :value
  end
  
  class Lexer
  
    def initialize
      @text  = '' 
      @now      = Token.new
      @state    = :start
      @next     = nil
      @rules    = []
    end
    
    def rule(state, re, act, newstate = :start, type = nil)
      rule   = { :state    => state   , :re => re, :act => act, 
                 :newstate => newstate, :type => type }
      @rules << rule
      return rule
    end  
    
    # lexes a single character, slow, but done like this to match C easily    
    # returns the state reached , and a token if finished. 
    # use a simplistic state machine
    def lex_char(ch) 
      for rule in @rules do 
        next if rule[:state] != :any && rule[:state] != @state
        next if rule[:re]    !~ ch
        @state = rule[:newstate]
        case rule[:act] 
          when :skip # do nothing
            # return current info 
            return @state, @text, rule[:type]
          when :store # store character
            @text << ch
          when :ok  # done parsing one token, return token and empty buffer, 
          # when you recieve this you mustcall lex_char again with the same token
          # so it gets processed too!
            text  = @text
            @text = ''
            return :ok, text, rule[:type]  
          else 
            warn "Unknown action #{rule[:act]} in rule!"
        end 
        # return current info 
        return @state, @text, rule[:type]
      end  
        
      # if we get here it's an error. 
      return :error, @text, nil
    end
    
    def lex_str(str) 
      result = []
      str << ' ' # add an extra character so the last token also gets parsed
      str.each_char do |ch|
        # keep calling the lexer with the same character as long as we get ok tokens
        state = nil
        begin          
          state, value, type = lex_char(ch)
          printf ">%s< (%d)-> %s (%s) (%s)\n", ch, ch.ord, state, value, type
          return :error, value, type if state == :error          
          if state == :ok
            result << [ type, value ] 
            printf "Token: >%s< %s \n", value, type
          end            
        end while state == :ok        
      end
      return result
    end
  end
end


lex = Beo::Lexer.new
# if there is a line comment start character, it should be a comment
lex.rule( :start        , /#/             , :skip , :commentstart) 
# #{ starts a block comment it should be ended with } 
lex.rule( :commentstart , /\{/            , :skip , :commentblock)
# otherwise it's a line comment
lex.rule( :commentstart , /./             , :skip , :commentline) 

# end of comment line is a newline or carriage return. 
lex.rule( :commentline  , /[\n\r]/        , :store, :commentend , :comment) 
# end of comment block is }
lex.rule( :commentline  , /./             , :store, :commentline, :comment) 

# anything in coment lines is just stored.
lex.rule( :commentblock , /[^\}]/         , :store, :commentblock, :comment) 
# end of comment block
lex.rule( :commentblock , /\}/            , :skip , :commentend  , :comment) 
# end of comment, any character is fine (and will be retried)
lex.rule( :commentend   , /./             , :ok   , :start       , :comment) 

# skip whitespaces
lex.rule( :start        , /[ \t]/         , :skip , :start  , :whitespace  )

# if there is a { it starts a block
lex.rule( :start        , /\{/           , :store , :block_open   )
# if there is a ( it starts a paren list
lex.rule( :start        , /\(/           , :store , :block_open   )
# if there is a [ it starts a block list
lex.rule( :start        , /\[/           , :store , :block_open   )


# if there is a } it closes a block
lex.rule( :start        , /\}/           , :store , :block_close  )
# if there is a ) it closes a pren list
lex.rule( :start        , /\)/           , :store , :block_close  )
# if there is a ] it closes a block list
lex.rule( :start        , /\]/           , :store , :block_close  )

# in all cases of {}[](), any character after it ends it
lex.rule( :block_open   , /.|\n|\r/      , :ok   , :start     , :block_open   )
lex.rule( :block_close  , /./            , :ok   , :start     , :block_close  )



# if there is a newline or cr ; it's the end of a statement 
lex.rule( :start        , /\r/          , :skip , :eol    )
lex.rule( :start        , /\n/          , :skip , :eol    )
lex.rule( :start        , /;/           , :skip , :eol    )
# any character ends an EOL
lex.rule( :eol          , /./           , :ok   , :start  , :eol    )


# if there is a numerical character it should be a number
lex.rule( :start          , /[0-9]/       , :store, :number )
# if there is a quote it should be a string
lex.rule( :start          , /\"/          , :skip , :stringdq )
# if there is a quote it should be a string
lex.rule( :start          , /\'/          , :skip , :stringsq )
# if there is a backtick it should be a string
lex.rule( :start          , /\`/          , :skip , :stringbt )

# if there is a - it should be a negative number or a word
lex.rule( :start          , /\-/          , :store, :number_or_word )

# If there is  0 to 9, the number or word is a number
lex.rule( :number_or_word , /[0-9]/       , :store, :word        )
# if its' a space, end the single - word
lex.rule( :number_or_word , /[ \t]/       , :ok   , :word , :word)
# Else it's certainly a word
lex.rule( :number_or_word , /./           , :store , :word        )


# If there is, in a number, a 0 to 9, or a . or an e or E, or a + or - 
# assume it's all part of the number (even though it may not be so!)
lex.rule( :number         ,/[eE0-9\-\+\.]/, :store, :number          )
# any other character ends the number. 
lex.rule( :number         ,/./            , :ok   , :start , :number )

# Finally if there is any character except - or 0 to 9, it should be a word
lex.rule( :start          , /[^0-9\-]/    , :store, :word   )

# a word goes on until whitespace is found
lex.rule( :word       , /[\n\r\t ]/     , :ok   , :start  , :word)
lex.rule( :word       , /[^\n\r\t ]/    , :store, :word   , :word)

# recognise escapes in double quoted strings but don't process them
lex.rule(:stringdq    , /\\/            , :store, :strdqesc , :stresc)
# recognise escapes in double quoted strings but don't process them
lex.rule(:strdqesc    , /[0-9]/        , :store, :strdqesc , :string)
lex.rule(:strdqesc    , /[^0-9]/        , :store, :stringdq , :string)


# a double qouted string goes on until a double quote is found 
lex.rule( :stringdq   , /"/            , :skip , :stringend  , :string)
# a double quoted string goes on until a double quote is found 
lex.rule( :stringdq   , /[^"]/         , :store, :stringdq  , :string)
# end of string


lex.rule( :stringend  , /.|\n|\r/      , :ok   , :start     , :string )
# a single qouted string goes on until a double quote is found 
lex.rule( :stringsq   , /\'/           , :skip , :stringend  , :string)
# a single quoted string goes on until a double quote is found 
lex.rule( :stringsq   , /[^\']/        , :store, :stringsq  , :string)

# a backtick qouted string goes on until a double quote is found 
lex.rule( :stringbt   , /`/            , :skip , :stringend  , :string)
# a backtick quoted string goes on until a double quote is found 
lex.rule( :stringbt   , /[^`]/         , :store, :stringbt  , :string)


# a 

res = lex.lex_str('a+ +good -foo - apple be2 123 456.78e-9 -12345 hello 
#{ comment } 
{{{}}}(){}[]{{}} €€€€
a = 10 + 20 
"A String
Over several lines with an \n escape \001u and another one."
\'single 
quoted\'
`backtick

string
`
')
p res
# 




