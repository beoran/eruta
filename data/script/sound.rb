# Models a Sound, that is a sample or audio clip that has been loaded from disk
# and kept in storage. 
class Sound < Store
  extend Store::Forward

  # Loads the sound sample. Supports wav files.
  # Can load 8/16 bits little endian .wav files and and  .flac files.
  def self.load(name, vpath)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_sample(nid, vpath)
    end 
  end
  
  # Plays this sample immediately 
  def play!
    Eruta::Audio.play_sample @id
  end


end
