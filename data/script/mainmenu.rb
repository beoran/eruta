

class MainMenu
  
  attr_accessor :active

  def load
    @main_music   = Store.load_audio_stream('/music/hiver_com64.ogg')
    @main_back    = Store.load_bitmap('/image/background/eruta_mainmenu.png')
    @main_graph   = Graph.make_image(0, 0, @main_back.id)
    # Eruta::Audio.music_id = @main_music.id
    # res = Eruta::Audio.play_music
    # puts "play_music #{res} #{@main_music.id}"
  end

  def initialize()
    @active = true
    load
  end
  
  def done
    # Need some more engine functionality here...
  end
  
  def on_poll(*args)
    return false if(!@active)
    p "args: #{args}"
    return true
  end
end