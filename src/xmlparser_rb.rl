# encoding: UTF-8
# Struct to hold the state of the ragel parser.
class BXMLRagel
  attr_accessor :buffer
  attr_accessor :root
  attr_accessor :now
end

class BXML
  NODE    = 1
  TEXT    = 2
  ATTR    = 3
  
  attr_accessor :kind
  attr_accessor :name
  
  attr_accessor :parent
  attr_accessor :child
  attr_accessor :attr
  attr_accessor :next
  
  def initialize(kind, parent)
    @kind   = kind
    @parent = parent
    @child  = @next = @attr = nil;
  end
  
  def walk(&block)
    block.call(self)
    if @attr
      @attr.walk(&block)
    end
    if @next
      @next.walk(&block)
    end
    if @child
      @child.walk(&block)
    end
  end
  
end






%%{
  machine xmlparser;
  action buffer     { parser.buffer << fc.chr; p "buf: #{parser.buffer}" }
  action newbuffer  { parser.buffer = "" ; p "buf: #{parser.buffer}" }
  action newchild   { 
    tag              = BXML.new(BXML::NODE, parser.now)
    parser.now.child = tag
    parser.now       = tag
  }
  
  action childdone {
    parser.now= tag.parent
  }
  
  action setname {
      parser.now.name = parser.buffer;
  }
  
  namechar = ^(space | '/' | '>' | '=') >buffer;
  name     = namechar+;
  open     = '<' @newchild;
  close    = '/>'  @setname @newbuffer @childdone;
  element  = open name close;
  elements = element element*;
  main     := elements @{ result = 1; };
}%%


%%write data;

def ragel_parse(text)
  parser        = BXMLRagel.new
  parser.buffer = ""
  parser.root   = BXML.new(BXML::NODE, nil)
  parser.now    = parser.root
  cs      = -1 
  res     = -1
  data    = text.each_codepoint.to_a;
  p       = 0;
  pe      = text.size;
  %%write init;
  %%write exec;
  
  printf("Result: %d\n%s%s\n", res, parser.buffer, parser.root.to_s);
  parser.root.walk do |tag|
    p tag, tag.kind, tag.name
  end
  return 0;  
end

ragel_parse("<hello/><world/>")




