# OCPU is an OCPU vitual machine simulator/executor.

class OCPU
  attr_accessor :r1, :r2, :r3, :r4
  attr_accessor :f1, :f2, :f3, :f4
  
  
  # Layout of the OCPU memory
  # Top of memory  
  # Stack space (grows down) 
  # Unused space
  # Heap space (Grows up)
  # Program instructions
  # OS / Libraries / Reserved area (empty in this simulator) 
  
  attr_reader   :memory
  attr_accessor :ip
  attr_accessor :sp
  attr_accessor :fp
  attr_accessor :high
  attr_accessor :low

  MEMORY_SIZE  = 10000
  
  def initialize
    @memory = Array.new(MEMORY_SIZE, 0)
    @ip     = 0 
    @sp     = MEMORY_SIZE - 1
    @fp     = @sp
    @r1     = 0
    @r2     = 0
    @r3     = 0
    @r4     = 0
    @high   = 0
    @low    = 0
    @f1     = 0.0
    @f2     = 0.0
    @f3     = 0.0
    @f4     = 0.0
  end



end