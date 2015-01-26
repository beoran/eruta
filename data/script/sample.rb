# Models a sample (a sound clip) that has been loaded from disk
# and kept in storage
class Sample < Store
  extend Store::Forward

  # Loads the sound sample.
  def self.load(name, vpath, buffer_count=8, samples=8000)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_sample(nid, vpath)
    end 
  end
  
  # Plays this music as the current music starting from the beginning
  # without transition.
  def play!
    Eruta::Audio.play_sample @id
  end

end
