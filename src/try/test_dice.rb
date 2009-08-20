

module Dice
  def self.roll_once(faces = 6, minimum = 1)
    return rand(faces) + minimum
  end

  def self.d4()
    return roll_once(4,1)
  end

  def self.d5()
    return roll_once(6,0)
  end

  def self.d6()
    return roll_once(6,1)
  end

  def self.roll_many(amount = 1, faces = 6, minimum = 1)
    results = []
    amount.times { results << roll_once(faces, minimum) }
    return results
  end   
  
  def self.roll_sum(amount = 1, faces = 6 , minimum = 1 )
    rolls = roll_many(amount, faces, minimum)
    return rolls.inject(0) { |i, s| s += i } # calulate sum of all rolls
  end

  def self.once_outcomes(faces, minimum)
    results = []
    for f in (0..(faces-1)) do
      results << (f + minimum)    
    end
    return results
  end
  
  def self.combine_outcomes(old, new)
    result = []
    for o in old do      
      for n in new do
        aid     =   [] 
        if o.respond_to? :each        
          o.each { |i| aid << i } 
        else
          aid   << o 
        end
        aid     <<  n
        result  <<  aid
      end
    end
    return result
  end  

  def self.outcomes(amount = 1, faces = 6, minimum = 1)
    aid = [] 
    amount.times do 
      aid << once_outcomes(faces, minimum) 
    end
    result = aid.shift
    for a in aid do
        result = combine_outcomes(result, a)
    end
    return result
  end 

  def self.count_results(outcomes)
    counts = {}
    total  = 0  
    for outcome in outcomes 
      result = yield(outcome)
      total += 1
      if counts[result]
        counts[result] += 1
      else
        counts[result] = 1
      end
    end
    return counts.to_a.sort, total 
  end
  
  def self.analyze_results(input, total)
    result = []                  
      for data in input
        result << [ data[0], data[1], data[1] * 100.0 / total  ] 
      end
      return result 
    end
  end

if $0 == __FILE__ # Buit in tests
  p Dice.roll_sum(4, 6, 0)
  outcomes = Dice.outcomes(4, 6, 0)
  res, total =  Dice.count_results(outcomes) do |o|
    if o[2] == 0 and o[3] == 0
      -1000 # critical failiure
    elsif o[2] == 5 and o[3] == 5
       1000 # critical success
    else
     o.inject(0) { |i, s| s += i } # calulate sum of all rolls
    end  
  end 
  
  p total
  analyze =  Dice.analyze_results(res, total)
  for line in analyze do
    puts line.join("\t")
  end
  

end