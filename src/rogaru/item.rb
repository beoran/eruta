module Rogaru
  
  # An Item is any object that a Being can have, wear or use. 
  class Item
    # Material is something the item is made of 
    class Material
      attr_reader :name        
      attr_reader :value
      attr_reader :strength
      attr_reader :endurance
      attr_reader :density
      
      def initialize(name, strength, endurance, density)
        @name     = name
        @strength = strength
        @endurance= endurance
        @value    = strength * endurance
        @density  = density
        yield self if block_given?
      end
      
      IRON    = Material.new('Iron'    , 10, 10, 1100)
      STEEL   = Material.new('Steel'   , 20, 20, 1000)
      CERAMIC = Material.new('Ceramic' , 30, 30, 1000)
      TITANIUM= Material.new('Titanium', 40, 40, 1000)
      LASER   = Material.new('Laser'   , 50, 10,  100)
      CRYSTAL = Material.new('Crystal' , 60, 60, 1000)
      
      
    end
    
  
    SIZE_SMALL  = 10
    SIZE_MEDIUM = 20
    SIZE_LARGE  = 30
    SIZES       = [ SIZE_SMALL, SIZE_MEDIUM, SIZE_LARGE ]
    EQUIP_WHERE = [ :good_hand, :off_hand, :head, :body, :arms, :feet ]    
    
    attr_accessor :weight
    attr_accessor :size
    attr_accessor :equip_on # array of places to equip this item, or nil if nowhere
    attr_reader   :material
    attr_reader   :value
    
  
  end
  
end  