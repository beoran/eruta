require 'test_helper.rb'

require 'raku'

assert { Raku::Lexer } 

def assert_lex(str, repeats = 1, &block) 
  lexer = Raku::Lexer.new(str)
  res   = nil
  repeats.times { res = lexer.lex }
  # assert { res } 
  assert { ! (res.fail?) }
  if res.fail?
    p res.fail_message
  end
  if(res && block)
    block.call(res) if block.arity == 1
    block.call(res, lexer) if block.arity == 2
  end   
end

STR = %Q{map}
lexer = Raku::Lexer.new(STR)
assert { lexer }
res   = lexer.lex
assert { res } 

assert_lex("map") { |res| res.is_a? Raku::Token } 
assert_lex("map") { |res| res.kind  == :symbol }
assert_lex("map") { |res| res.value == "map" }
assert_lex("map") { |res, lex| lex.col == 4 }
assert_lex("map") { |res, lex| lex.line == 1 }

assert_lex("map { 1 2 3 }") { |res| res.value == "map" }
assert_lex("map { 1 2 3 }", 2) { |res| res.value == " " }
assert_lex("map { 1 2 3 }", 3) { |res| res.value == "{" }
assert_lex("map { 1 2 3 }", 4) { |res| res.value == " " }
assert_lex("map { 1 2 3 }", 5) { |res| res.value == "1" }
assert_lex("map { 1 2 3 }", 6) { |res| res.value == " " }
assert_lex("map { 1 2 3 }", 7) { |res| res.value == "2" }
assert_lex("map { 1 2 3 }", 8) { |res| res.value == " " }
assert_lex("map { 1 2 3 }", 9) { |res| res.value == "3" }
assert_lex("map { 1 2 3 }", 10) { |res| res.value == " " }
assert_lex("map { 1 2 3 }", 11) { |res| res.value == "}" }