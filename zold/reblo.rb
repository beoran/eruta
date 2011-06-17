
require 'bindata'


class Reblo
  
  NAME_MAX   =  
  SIZE_MAX   = 2**64
  
  class Block < BinData::Record
    string    :name, :length => 16, :trim_value => true
    uint64le  :bsiz #,  :value => lambda { elts.size }
    array     :elts, :type => Block, :initial_size => :bsiz,
              :read_until => lambda { index >= (self.bsiz - 1) },
              :onlyif => lambda { self.bsiz > 0 } 
    
    def child(name) 
      block = Block.new
      block.name = name
      self.elts << block
      self.bsiz = self.elts.size
      return block
    end
    
  end

  
  class Block2
    attr_reader :size
    attr_reader :name
    attr_reader :data

    def initialize(name, data = nil)
      @size = data ? data.size : 0
      raise "Siz too big"   if @size > 16
      raise "Name too big" if name.bytesize > 16
      @name = name
    end
    
    def to_binary
      res = String.new.force_encoding(binary)
      res << self.name.force_encoding('binary').ljust(16, "\0")
      res << [self.size]
      if self.data
        res << self.data.to_binary
      end  
    end
    
    def from_binary(bin)
      
      
    end
  end
  
  
end

if __FILE__ == $0
  block = Reblo::Block.new
  block.name = "REBLO V1.0"
#   block.elts.assign([1,2,3])
#   block.bsiz = block.elts.size
  pae        = block.child('PAE')  
  layer1     = pae.child('LAYER')
  layer2     = pae.child('LAYER')
  block.elts[0] =  pae
  timama     = block.child('TIMAMA')
  
  p block
  File.open("try.blo", 'w+') do |f|
    block.write(f)
  end
  iblock = Reblo::Block.new
  File.open("try.blo", 'r+') do |f|
    iblock.read(f)
  end
  p iblock
  p iblock.elts
  p iblock.elts[0].elts
  
end


