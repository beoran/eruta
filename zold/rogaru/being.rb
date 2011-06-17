module Rogaru
  module Dice
    def d5(mul = 1)
      res = 0
      mul.times { res += rand(6) }
      return 
    end
    
    def d5x2()
      return d5(2)
    end
    
    def d5x4()
      return d5(4)
    end
  end # module Dice     


  class Being
    include Dice
  
    class Stat
      attr_reader :now      
      attr_reader :normal
      attr_reader :base
      attr_reader :max
      
      def initialize(value, max = nil)
        @base     = @normal = @now = value
        @max      = max
        @derived  = [] # Stats influenced by this one.
      end
      
      def derive(other_stat, ratio = 1)
        @derived << other_stat unless @derived.member? other_stat 
      end
      
      def update_derived(act, *value)
        @derived.each do  | ind | 
          # ind.send(act.to_sym, *value) 
        end
      end
           
      def normal_percent(per)
        (self.normal * per / 100).to_i
      end
      
      def buff(by)
        return false if @now > @normal 
        @now += by
        update_derived(:buff, by)
        return true
      end
      
      def debuff(by)
        return false if @now < @normal 
        @now -= by
        update_derived(:debuff, by)
        return true
      end
      
      def normalize()
        delta= @normal - now
        @now = @normal
        update_derived(:buff, delta) 
        # XXX this is an ugly hack and it doesn't work
      end
      
      def equip(value)
        @now    += value
        @normal += value
        @max    += value if @max
        update_derived(:equip, value)
      end
      
      def unequip(value)
        @now    -= value
        @normal -= value
        @max    -= value if @max
        update_derived(:unequip, value)
      end
      
      def grow(value)
        @base   += value
        @now    += value
        @normal += value
        @max    += value if @max
        update_derived(:grow, value)
      end
      
      def shrink(value)
        @base   -= value
        @now    -= value
        @normal -= value
        @max    -= value if @max
        update_derived(:shrink, value)
      end
      
      def to_s
        return "#{self.max}/#{self.now}" if self.max 
        return "#{self.now}"        
      end
      
      def to_i
        return self.now
      end
      
    end # class Stat
    
    # Derived is like a stat, but it is not independent, 
    # but depends on another stat
    class Derived < Stat
      def initialize(value, derive_from = nil, max = nil)        
        super(value, max)  
        @from = derive_from
      end
      
      def max
        if @max 
          res =  ( @from.max ? max + @from.max : max) 
        else
          res =  @max
        end
        return res  
      end
      
      def now
        return @now + @from.now
      end
      
      def normal
        return @normal + @from.now
      end
      
      def base
        return @base + @from.base
      end
      
    end
    
    class Skill 
      def initialize(name, stat)
        @name       = name
        @stat       = stat
        @level      = 0 # my own level
        @experience = 0 # my own experience
      end
      
      def level
        return @stat.now + level
      end
      
      def equip(value)
        @level   += value
      end
      
      def unequip(value)         
        @level  -= value
      end
      
      def level_up()
        @level += 1
      end
      
      def train(exp)
        @experience += exp
        while @experience > (@level * 100 ) 
          level_up()
        end
      end
      
      def to_s
        return "#{self.level}"
      end
      
      def to_i
        return self.level
      end
      
    end
    
    attr_reader :strength
    attr_reader :endurance
    attr_reader :agility
    attr_reader :knowledge
    attr_reader :insight
    attr_reader :dexterity
    attr_reader :charisma
    
    attr_reader :life
    attr_reader :health
    attr_reader :mental
    
    attr_reader :attack         # Current weapon attack power
    attr_reader :focus          # Mental attack power
    attr_reader :defense        # Defense against physical attacks
    attr_reader :protection     # Protection against energy attacks
    attr_reader :evade          # Current evasion against physical attacks
    attr_reader :block          # Current evasion against mental attacks
     
    attr_reader :skills 
    
    DAMAGE_KINDS = [ :piercing, :cutting,    :impact,  
                     :heat,     :cold,       :shock,  
                     :light,    :corruption, :venom,  :arcane ]
    
    AILMENTS     = [ :gone, :unconscious, :asleep, :paralyzed, :stunned, 
                     :lock, :ill,         :poisoned, :pain               ]    
    
    def skill(name, base)
      if base.respond_to? :to_sym
        base = @stats[base.to_sym]
      end     
      skill = Skill.new(name, base)
      @skills[name.to_sym] = skill
    end
      
    #########################################################################
    #
    # Skills (9 attribute system):
    # 9 attributes -> 9 archetypes 
    # Soldier, Worker, Athlete, Artisan, Scientist, Healer, Leader, Artist, Trader
    #
    # Parry and counter are folded into the relative fightig skill.     
    #    
    # STR  Swords
    # STR  Axes
    # STR  Spears
    # STR  Toiling 
    # STR  Climbing
    # END  Martial
    # END  Health
    # END  Armours
    # END  Survival
    # END  Travel
    # AGI  Knives
    # AGI  Evade 
    # AGI  Shields 
    # AGI  Vests 
    # AGI  Acrobatics
    # DEX  Guns
    # DEX  Tailor
    # DEX  Sleight 
    # DEX  Blacksmith
    # DEX  Locks and Traps
    # KNO  Electromancy
    # KNO  Biology 
    # KNO  Engineering 
    # KNO  Medicine
    # KNO  Chemistry
    # INS  Biomancy
    # INS  Photomancy
    # INS  Clothes 
    # INS  Staves
    # INS  Block 
    # WIL  Wands
    # WIL  Pyromancy 
    # WIL  Kenomancy 
    # WIL  Animals 
    # WIL  Courage 
    # PAS  Mental
    # PAS  Music 
    # PAS  Art 
    # PAS  Cooking 
    # PAS  Instruments 
    # CHA  Arcane 
    # CHA  Trade
    # CHA  Speaking 
    # CHA  Leadership 
    # CHA  Teaching 

    def initialize()
      @strength   = Stat.new(10)
      @endurance  = Stat.new(10)
      @agility    = Stat.new(10)
      @knowledge  = Stat.new(10)
      @insight    = Stat.new(10)
      @dexterity  = Stat.new(10)
      @passion    = Stat.new(10)
      @will       = Stat.new(10)
      @charisma   = Stat.new(10)
      
      @stats      = { strength: @strength, endurance: @endurance,
                      agility: @agility, knowledge: @knowledge,
                      insight: @insight, dexterity: @dexterity,
                      charisma: @charisma }  
      
      @attack     = Derived.new(0, @strength)
      @focus      = Derived.new(0, @knowledge)
      @defense    = Derived.new(0, @endurance)
      @protection = Derived.new(0, @insight)
      @evade      = Derived.new(0, @agility)
      @block      = Derived.new(0, @dexterity)
      @life       = Stat.new(3)
      hx          = @endurance.now
      @health     = Stat.new(hx, hx)
      mx          = @insight.now
      @mental     = Stat.new(mx, mx)
      @skills     = {}
      yield self if block_given?
      

      
      skill('Swords'      , @strength )
      skill('Axes'        , @strength )
      skill('Spears'      , @strength )
      skill('Toiling'     , @strength )
      skill('Climbing'    , @strength )
      
      skill('Martial'     , @endurance)
      skill('Health'      , @endurance)
      skill('Armours'     , @endurance)
      skill('Survival'    , @endurance)
      skill('Traveling'   , @endurance)
                 
      skill('Knives'      , @agility  )
      skill('Evade'       , @agility  ) 
      skill('Shields'     , @agility  )
      skill('Vests'       , @agility  ) 
      skill('Acrobatics'  , @agility  ) # Running, jumping, etc.
                  
      skill('Guns'        , @dexterity)
      skill('Tailor'      , @dexterity)
      skill('Metalworks'  , @dexterity)
      skill('Sleight'     , @dexterity)
      skill('Locks&Traps' , @dexterity)
      
      skill('Electromancy', @knowledge) # Electricty Numen
      skill('Biology'     , @knowledge)
      skill('Engineering' , @knowledge)
      skill('Medicine'    , @knowledge)
      skill('Chemistry'   , @knowledge) # Item lore
      
      skill('Photomancy'  , @insight  ) # Light Numen
      skill('Biomancy'    , @insight  ) # Life  Numen
      skill('Clothes'     , @insight  ) # Light armor
      skill('Staves'      , @insight  ) # Wands and rods
      skill('Mental'      , @insight  ) # Determines M.P.      
      
      skill('Block'       , @will ) # Block Numen related attacks  
      skill('Cryomancy'   , @will ) # Water Numen
      skill('Kenomancy'   , @will ) # Void  Numen (Kenos= "void, empty")
      skill('Animals'     , @will )
      skill('Courage'     , @will )       
    
      skill('Rage'        , @passion) # Using innate Numen effects
      skill('Pyromancy'   , @passion) # Fire Numen                      
      skill('Music'       , @passion)
      skill('Cooking'     , @passion)
      skill('Art'         , @passion)
      
      skill('Arcane'      , @charisma )
      skill('Trade'       , @charisma )
      skill('Speaking'    , @charisma )
      skill('Leadership'  , @charisma )      
      skill('Teaching'    , @charisma )
      
      
    end
    
    def strike(other)
      
    end
    
  end # class Being
end # module Rogaru


if __FILE__ == $0
  b = Rogaru::Being.new
  b.strength.equip(3)  
  puts b.strength
  puts b.attack
  b.strength.equip(4)  
  puts b.strength
  puts b.attack
  b.strength.unequip(3)  
  puts b.strength
  puts b.attack
  b.attack.equip(6)  
  puts b.strength
  puts b.attack
  b.strength.unequip(4)  
  puts b.strength
  puts b.attack
  b.attack.unequip(6)  
  puts b.strength
  puts b.attack  
  b.strength.buff(7)  
  puts b.strength
  puts b.attack  
  b.strength.debuff(3)  
  puts b.strength
  puts b.attack
  b.strength.normalize()
  puts b.strength
  puts b.attack
  puts b.skills
  puts b.health
 
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  







end