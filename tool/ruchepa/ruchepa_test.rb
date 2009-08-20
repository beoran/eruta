# Unit tests for Ruchepa
require 'ruchepa'

require 'test/unit'

class Ruchepatest < Test::Unit::TestCase
  
  def setup
    @input        = Ruchepa::Input.new("12345")
    @empty_input =  Ruchepa::Input.new("")
  end

  def test_input_1
    taid      = Ruchepa::Input.new('345')
    i2        = Ruchepa::Input.new("12345")
    assert_equal( i2.index, 0, "Indexes are zero after new construction")
    assert_equal("12345"  , @input.to_s,  "Can convert to string")
    assert_equal("" ,  @empty_input.to_s,  "Empty input works")
    assert((@input <=>  i2) == 0,  "Comparison operator works for equality")
    assert(@input == i2,  "Comparable interface works")
    assert(@input == @input,  "Compare to self works")
    h, t      = @input.match_head('12')
    assert_equal('12', h, "Head token is correct")
    assert_equal('345', t.to_s, "Tail to string is correct")
    assert_equal( t, taid, "Tail is correct")
    assert_equal( t.index, 2, "Tail index is correct")
    h, t      = @input.match_head('345')
    assert_nil(h, "Head token is correct for missing match")  
    assert_equal( t, @input, "Tail is correct")
    assert_equal( t.index, 0, "Tail index is correct")
  end
  
  def test_rule
    rabc  = Ruchepa::Rule.make('a', 'b', 'c')
    assert_equal(rabc.to_s, 'Ruchepa::Rule::Sequence: a + b + c', 'to_s works correctly')
    i12345= Ruchepa::Input.new("12345"); 
    i54321= Ruchepa::Input.new("54321"); 
    r123  = Ruchepa::Rule.make('1', '2', '3')    
    ok, result, rest = r123.parse(i12345)
    assert(ok, 'Parse should be sucessful');
    assert_equal(rest, '45', 'Remainder is correct');
    assert_equal(result, [["1"],["2"],["3"]], 'Remainder is correct');  
    ok, result, rest = r123.parse(i54321)
    assert(!ok, 'Parse should fail');
    assert_equal(rest, '54321', 'Remainder is correct');
    assert_equal(result, nil, 'Resimt');  
  end
  
  
  
  def test_rule_choice
    r1               = Ruchepa::Rule.make('foo')              
    r2               = Ruchepa::Rule.make('bar')
    rc               = r1 / r2
    p rc
    
    i                = Ruchepa::Input.new("foo bar baz quux");     
    i2               = Ruchepa::Input.new("bar baz quux");     
    aidarr           = "abcdefghijklmnopqrstuvwxyz".split('')
    ralpha           = Ruchepa::Rule.any('a'..'z')    
    ralrep           = ralpha.repeat

    ok, result, rest = rc.parse(i)
    assert(ok, 'Parse should be sucessful');
    assert_equal(rest.to_s, ' bar baz quux', 'Remainder is correct');
    assert_equal(result, [[["foo"]]], 'Result is correct');  
        
    
    
    # ralpha  = Ruchepa::Rule.any("a", "f")
    ok, result, rest = ralpha.parse(i)
    assert(ok, 'Parse should be sucessful');
    assert_equal(rest.to_s, 'oo bar baz quux', 'Remainder is correct');
    assert_equal(result, [[["f"]]], 'Result is correct');  
    
    
    ok, result, rest = ralrep.parse(i)
    assert(ok, 'Parse should be sucessful');
    assert_equal(rest.to_s, ' bar baz quux', 'Remainder is correct');
    # assert_equal(result, [[["f"],["o"],["o"]]], 'Result is correct');  
        
    ok, result, rest = ralrep.parse(i2)
    assert(ok, 'Parse should be sucessful');
    assert_equal(rest.to_s, ' baz quux', 'Remainder is correct');
    # assert_equal(result, [[["b"],["a"],["r"]]], 'Result is correct');  
    
    
  end
  
  
  
 
  
end




