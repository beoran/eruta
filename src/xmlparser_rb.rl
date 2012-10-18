# encoding: UTF-8
# Struct to hold the state of the ragel parser.
class BXMLRagel
  attr_accessor :buffer
  attr_accessor :entity
  attr_accessor :root
  attr_accessor :now
  attr_accessor :attr
  
  # Adds a character to the buffer
  def buffer_add(ch)
    @buffer << ch
  end
  
  # Empties the buffer
  def buffer_empty
    @buffer = ""
  end
  
  # Sets the name of the current tag from the buffer 
  # and empties the buffer
  def set_tag_name
    if @now
      @now.name = @buffer.dup
    end
    buffer_empty
  end
  
  # Sets the value of the current tag from the buffer 
  # and empties the buffer
  def set_tag_value
    if @now
      @now.value = @buffer.dup
    end
    buffer_empty
  end

  
  # Sets the name of the current attributes from the buffer 
  # and empties the buffer
  def set_attr_name
    if @attr
      @attr.name = @buffer.dup
    end
    buffer_empty
  end
  
  # Sets the name of the current attributes from the buffer 
  # and empties the buffer
  def set_attr_value
    if @attr
      @attr.value = @buffer.dup
    end
    buffer_empty
  end
  
  # Begins a new child tag of the current tag and sets it as the current 
  # tag
  def child_new
    tag              = BXML.new(BXML::NODE, @now)
    @now.add_child(tag)
    @now      = tag
  end
  
  # When parsing the curren tag is done,, this sets the current tag 
  # to the parent of the current tag.
  def child_done 
    @now = @now.parent
  end

  # Begins a new attribute of the current tag
  def attr_new
    @attr = BXML.new(BXML::ATTR, parser.now)
    @now.add_attr(@attr)
  end


  
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
  
  def initialize(kind, parent)
    @kind   = kind
    @parent = parent
    @child  = @next = @attr = nil;
  end
  
  # Adds a new child to this node
  def add_child(other)
    other.parent = self
    unless self.child
      return self.child = other     
    end
    aid = self.child
    while aid.next
      aid = aid.next
    end
    aid.child = other
  end
  
  # Adds a new attribute to this node
  def add_attr(other)
    other.parent = self
    unless self.attr
      return self.attr = other     
    end
    aid = self.attr
    while aid.next
      aid = aid.next
    end
    aid.next = other
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
    parser.buffer_add fc.chr 
  }
  
  action newbuffer  { 
    parser.buffer_empty 
  }
  
  action newchild   { 
    parser.child_new
  }
  
  action childdone {
    parser.child_done
  }
  
  action settagname {
    parser.set_tag_name
  }

  action settagvalue {
    parser.set_tag_value
  }
  
  action setattrname {
    parser.set_attr_name
  }
  
  action setattrvalue {
    parser.set_attr_name
  }


  namechar    = ^(space | '/' | '>' | '=') >buffer;
  tagname     = namechar+;
  open        = '<' %newchild;
  close       = '/>'  %childdone;
  notclosed   = '>';
  tagclose    = '</' tagname? '>' @childdone;
  tagstart    = open tagname %settagname; 
  emptytag    = tagstart close;
  fulltag     = tagstart notclosed tagclose;
  tag         = emptytag | fulltag;
  elements    = tag tag*;
  main       := elements @{ result = 1; };
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
    p tag, tag.kind, tag.name, tag.value
  end
  return 0;  
end

ragel_parse("<hello><world/></hello>")




