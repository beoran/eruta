module Rogaru

  # A being is a numerical model for an actor 
  class Being
    # A stat is a numerical value that describes some of the abilities 
    # of a being
    class Stat
      attr_reader :now      # Current value of the stat, with equipments and effects
      attr_reader :normal   # Value the stat has normally, with equipment 
      attr_reader :max      # Maximal value of the stat or nil if no maximum.
      attr_reader :natural  # Value the stat has without equipment
      def initialize(value, max = nil)
        @natural = @normal = @now = value 
        @max     = max
      end

      def buff(by)
        return false if @now > @normal
        @now = @normal + by
      end

      def debuff(by)
        return false if @now < @normal
        @now = @normal - by
      end

    end
    
    # Damage kinds: 
    # pierce, cut  , crush, corrosion      (physical)
    # heat  , shock,  cold, light          (energy) 
    # arcane (cannot be resisted)
    #
    # Or perhaps simplify to:  
    # pierce, impact, corrosion
    # heat, shock, cold,
    # light/arcane    
    #
    # Effect kinds: 
    # vs bio: toxin/fatal/nervous , vs mech: jam/destroy/circuit 
    # weaken (lower stats), disable (cause status ailment)
    #
    # Healing kinds: 
    # heal , repair, arcane healing
    # 
    #
    # Statuses 
    # poisoned, silence/techlock, stunned/held, paralyzed, wounded/KO, gone
    # slow, defenseless, weak
    # quick, strong, phys defended, energy defended 
    
    # AP ATT CON CHA DEF EVA EDF EEV HP INT LP MANA MEN MIN MP NUM NP PP POW 
    # PER PDF PEV STA STR SPE SOC TOU TP QUI VIG WIS WILL
    #    
    # Body : Explosive / resistance  (speed)
    # Mind : intensive / resistance
    # Speed, nimbleness, quickness
    # Social/charm/charisma
    # Artistic, perception, dexterity, skillfullness
    # 
    # Derived attributes:
    # Attack, physical defense, energy defense, life
    # mp/np/ap/tp/mana/numen/energy, hp/health,physical,pp(physical power)/bp(body )
    # 
    # Strength : STR : swords, daggers, martial arts, pushing
    # Endurance: END : resistance, lp, carrying, swimming
    # Agility  : AGI : evade, parry, block, run
    # Knowledge: KNO : energy techniques, hacking, 
    # Insight  : INS : perception, energy resistance, healing through energy techniques
    # Dexterity: DEX : guns, music, graphical arts
    # Charm    : CHA : trade, charm, conversation
    #
    # 
    # Life     : LIFE : How alive and unhurt you are. A lowered Life means being is wounded. ( max based on all stats ) 
    # Health   : H.P. : Health power. Physical energy, determines how many hits you can 
    #                   resist before you're knocked out and/or wounded. (based on endurance)   
    # Mental   : M.P. : Mental power: how mentally fit and energetic Energy available for techniques and skills.
    #                   chance to fall asleep if mp = 0 (based on insight)
    #
    # Attack      : ATTK : Quality of equippped weapon   
    # P. Defense  : PDEF : Physical defense of armour
    # E. Defense  : EDEF : Energy defense of armour 
    
    #
    # Skills: 
    # Health    Determines health power
    # Mental    Determines mental power
      
    # Evade Energy
    # Evade Physical
    # Parry Attack   ( Any melee attack, ranged also but at lower chance. ) 
    # Counterattack  ( Melee attack only )
    # Fists   (0h)
    # Knives  (1h)
    # Wands   (1h/2h)
    # Swords  (1h/2h)
    # Axes    (1h/2h)    
    # Polearms(2h)
    
    # Guns    (1h)
    # Rifles  (2h)
    # Rockets (2h)
    # Shield Block (1h)
    # Heavy Armour \  -> def + / evade - / MP - / Energy Arts down  
    # Medium Armour > skills increase benefits and decrease problems
    # Light Armour /  -> def - / evade + / MP + / Energy Arts up
    
    # Fire Arts
    # Ice Arts    
    # Bolt Arts    
    # Light Arts
    # Life Arts
    # Dark Arts
    # Arcane Arts
    
    # Biology     ( vs bio )
    # Engineering ( vs mechs / repairing )
    # Computers   ( stat change vs mechs / non-battle use  )     
    # Medical     ( non-arts healing using medication / bandage, etc. )    
    # Chemistry   ( item creation / use of damaging items )
    
    # Smithing    ( craft weapon and heavy armors )
    
    # Tailor      ( craft light armors )    
    # Painting    ( non battle, item creation )  
    # Locks       (  )
    # Traps           
    # Stealing    (  )
    
    # Music       ( stat change vs bio   / non-battle use  )    
    # Trade       ( non battle, trading of items )    
    # Dancing     ( stat change vs bio / not battle use )    
    # Conversation( non battle, persuasion)     
    
 
    class Attribute < Stat 
    end
    
  
  end # class Being
    
    
end # Module Rogaru  


if __FILE__ == $0
  s = Rogaru::Being::Stat.new(20)
  s.buff(10)
  s.buff(10)
  p s.now
  s.debuff(10)
  s.debuff(10)
  p s.now
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
end