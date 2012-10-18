# encoding: UTF-8
# Struct to hold the state of the ragel parser.
class BXMLRagel
  attr_accessor :buffer
  attr_accessor :entity
  attr_accessor :root
  attr_accessor :now
  # last added attribute
  attr_accessor :attr 
end

class BXML
  NODE    = 1
  TEXT    = 2
  ATTR    = 3
  
  attr_accessor :kind
  attr_accessor :name
  attr_accessor :value
  
  attr_accessor :parent
  attr_accessor :child
  attr_accessor :attr
  attr_accessor :next
  
  def initialize(kind, parent, name = nil)
    @kind   = kind
    @parent = parent
    @child  = @next = @attr = nil;
    @name   = name
  end
  
  # adds a new child node to this node
  def add_child(newchild)
    newchild.parent = self
    # no child yet
    unless self.child
      self.child = newchild
      return newchild
    end
    # Find last child and append though next pointers
    aid = self.child
    while (aid.next) 
      aid = aid.next
    end
    aid.next = newchild
    return newchild
  end
  
  # adds a new attribute node to this node
  def add_attr(newattr)
    newattr.parent = self
    # no child yet
    unless self.attr
      self.attr = newattr
      return newattr
    end
    # Find last attr and append though next pointers
    aid = self.attr
    while (aid.next) 
      aid = aid.next
    end
    aid.next = newattr
    return newattr
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
  action buffer     { 
    newchar = fc.chr
    parser.buffer << newchar
    # puts "Buffering #{newchar}: #{parser.buffer}." 
  }
  action newbuffer  { 
    parser.buffer = ""
  }
  action newchild   { 
    tag              = BXML.new(BXML::NODE, parser.now)
    parser.now.add_child(tag)
    parser.now       = tag
  }
  
  action newattr   { 
    tag              = BXML.new(BXML::ATTR, parser.now)
    parser.now.add_attr(tag)
    parser.attr      = tag
  }
  
  action setattrname {
    p "Setting attr name!"
    parser.attr.name = parser.buffer.dup
    parser.buffer = ""
  }
  
  action setattrvalue {
    p "Setting attr value!"
    parser.attr.value = parser.buffer.dup
    parser.buffer = ""
  }
  
  action entitybuffer {
    newchar = fc.chr
    parser.entity << newchar
    puts "Buffering entity #{newchar}: #{parser.entity}." 
  }
  
  action entitydone {
    ents = { 'quot' => '"', "apos" => "'", 
             "amp" => "&" , "lt" => "<", "gt" => ">" }
    newchar = nil
    if(parser.entity[0] == '#') 
      if(parser.entity[1] == 'x') 
        newchar = parser.entity[2,16].to_i(16).chr
      else
        newchar = parser.entity[1,16].to_i(10).chr
      end
    else
      newchar = ents[parser.entity]
    end
    if newchar
      parser.buffer << newchar 
    end
    parser.entity = ""
  }
  
  action childdone {
    parser.now = tag.parent
  }
  
  action settagname {
    p "Setting tag name!"
    parser.now.name = parser.buffer.dup;
    parser.buffer = ""
  }
  
  tagnamechar = ^(space | '/' | '>' | '=') >buffer;
  tagname     = (tagnamechar+);
  
  attrname    = (tagnamechar+) >newattr %setattrname;
  entitychar  = /[0-9A-Za-z#]/ > entitybuffer;
  entity      = '&' entitychar+ ';' %entitydone;
  sqchar      = ^("'" | "&") >buffer;
  sqvalue     = "'" (entity | sqchar)* "'";
  dqchar      = ^('"' | '&') >buffer;
  dqvalue     = '"' (entity | dqchar)* '"';
  attrvalue   = (sqvalue | dqvalue) @setattrvalue;
  attribute   = space+  attrname space* '=' space* attrvalue;
  open        = '<'  @newchild;
  close       = '/>' @childdone;
  noclose     = '>';
  attributes  = attribute attribute*;
  tagopen     = (open tagname) %settagname;
  emptytag    = tagopen attributes? close;
  closetag    = '</' tagname? '>' @childdone;
  
  fulltag     = tagopen attributes? noclose tagcontents closetag;
  tag         = emptytag | fulltag;
  tagcontents = tag tag*;
  elements    = tag tag*;
  main       := elements @{ result = 1; };
  
# attribute = ^(space | ‘/’ | ‘>’ | ‘=’)+ >buffer %attributeName space* ‘=’ space*
# ((‘\” ^’\”* >buffer %attribute ‘\”) | (‘”‘ ^’”‘* >buffer %attribute ‘”‘));
# element = ‘<' space* ^(space | '/' | '>‘)+ >buffer %elementStart (space+ attribute)*
# :>> (space* (‘/’ %elementEndSingle)? space* ‘>’ @element);
# elementBody := space* <: ((^'<'+ >buffer %text) <: space*)?
# element? :>> (‘<' space* '/' ^'>‘+ ‘>’ @elementEnd);
# main := space* element space*; git add 
  
  
}%%


%%write data;

def ragel_parse(text)
  parser        = BXMLRagel.new
  parser.buffer = ""
  parser.entity = ""
  parser.root   = BXML.new(BXML::NODE, nil, "#root")
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
    p tag.kind, tag.name, tag.value
  end
  return 0;  
end

s1 = %{<hello/><world/>}
s2 = %{<base><hello/><world I_AM="an attribute" and_THIS_IS='another &quot; one'/><has_children><child1/></has_children></base>}
ragel_parse(s1)
ragel_parse(s2)



