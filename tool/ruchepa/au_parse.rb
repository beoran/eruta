require 'treetop'
require 'augrammar'








if __FILE__ == $0
# tests
parser = AuParser.new
data   = DATA.read
p '----'
p data
p '+++++'
res    = parser.parse(data)
p res
p res.methods - Object.methods
p parser.methods - Object.methods
if res 
  if res.elements 
    for e in res.elements do 
      p e
    end
  end  
else 
   puts parser.failure_index
   puts parser.failure_line 
   puts parser.failure_column 
   puts parser.failure_reason
end




end


__END__

a+b foo  bar\nbaz quux +++++ 


