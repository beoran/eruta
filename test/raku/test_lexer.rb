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

def assert_lex_skip(str, repeats = 1, skiplist = [:ws], &block) 
  lexer = Raku::Lexer.new(str)
  res   = nil
  repeats.times { res = lexer.lex_skip(*skiplist) }
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
prog1             = "map { 1 2 3 }"

assert_lex(prog1) { |res| res.value == "map" }
assert_lex(prog1, 2) { |res| res.value == " " }
assert_lex(prog1, 3) { |res| res.value == "{" }
assert_lex(prog1, 4) { |res| res.value == " " }
assert_lex(prog1, 5) { |res| res.value == "1" }
assert_lex(prog1, 6) { |res| res.value == " " }
assert_lex(prog1, 7) { |res| res.value == "2" }
assert_lex(prog1, 8) { |res| res.value == " " }
assert_lex(prog1, 9) { |res| res.value == "3" }
assert_lex(prog1, 10) { |res| res.value == " " }
assert_lex(prog1, 11) { |res| res.value == "}" }
assert_lex(prog1, 12) { |res| res.nil? }

assert_lex_skip(prog1, 1) { |res| res.value == "map" }
assert_lex_skip(prog1, 2) { |res| res.value == "{" }
assert_lex_skip(prog1, 2) { |res| res.kind == :lcurly }
assert_lex_skip(prog1, 3) { |res| res.value == "1" }
assert_lex_skip(prog1, 3) { |res| res.kind == :integer }
assert_lex_skip(prog1, 4) { |res| res.value == "2" }
assert_lex_skip(prog1, 5) { |res| res.value == "3" }
assert_lex_skip(prog1, 6) { |res| res.value == "}" }
assert_lex_skip(prog1, 6) { |res| res.kind == :rcurly }
assert_lex_skip(prog1, 7) { |res| res.nil? }

lexer  = Raku::Lexer.new(prog1)
res    = lexer.lex_all(:ws)
assert { res[0].value == "map" } 
assert { res[5].value == "}" }
assert { res[6].kind == :eof }

res    = Raku::Lexer.lex_all(prog1, :ws)
assert { res[0].value == "map" } 
assert { res[5].value == "}" }
assert { res[6].kind == :eof }

prog2 = %Q{
  map 2 2 2 2.0 {
    layer 1 {
      1 2
      2 1
    }

    layer 2 {
      2 1
      1 2
    }
  }
}

res = Raku::Lexer.lex_all(prog2, :ws)
assert { res } 
assert { res.size > 0 }
res.each { |v| p v.kind } 






