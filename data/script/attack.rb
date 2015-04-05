

# This class models an attack performed by a thing/being/player/npc/foe
class Attack
  # The underlying thing that is used to model the attack
  attr_reader :my_thing
  # The underlying thing that is attacking
  attr_reader :att_thing
  
  # Creates a new attack for the given attacker's thing.
  def initialize(attacking_thing, w, h, offset_x = 0, offset_y = 0) 
    @att_thing = attacking_thing
    x, y, w, h = @att_thing.rectangle_in_front(w, h)
    x         += offset_x
    y         += offset_y
    z          = @att_thing.z
    name       = "#{attacking_thing.name}_attack_#{self.class.serno}"  
    @my_thing  = Thing.make(name, Thing::Kind::ATTACK, x, y, z, w, h)
    @done      = false
    @timer     = Timer.make(3.0) do | timer |
      @done    = true
      p "Attack done by timer."
      # update the attacks
      Attack.update
      true
    end
  end

  
  def done?
    return @done
  end
   
  def delete
    @my_thing.delete
    @timer.done!
  end
  
  def self.serno
    @serno ||= 0
    @serno += 1
    return @serno
  end
   
  def self.register(attack)
    @attacks ||= []
    @attacks << attack
  end
  
  # Purge attacks that are over through the update.
  def self.update
    @attacks ||= []
    done       = []
    @attacks.each do | attack |
      done << attack if attack.done?
    end
    
    done.each do | attack |
      @attacks.delete(attack)
      attack.delete
    end
  end
  
  # Purge attacs that are over due to sprite event
  def self.on_sprite(thing_id)
    @attacks ||= []
    done       = []
    @attacks.each do | attack |
      done << attack if attack.done?
    end
    
    done.each do | attack |
      @attacks.delete(attack)
      attack.delete
    end

  end
  
  
  def self.make(attacking_thing, w, h, offset_x = 0, offset_y = 0) 
    attack = self.new(attacking_thing, w, h, offset_x, offset_y) 
    self.register(attack)
    p "Made attack #{attack}"
    return attack
  end
  

end
