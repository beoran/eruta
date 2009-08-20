require 'filename.rb'
require 'singleton'

class Settings
  include Singleton
  
  def find_settings_file
    tries = []    
    # First try based upon this file's location.
    tries << @main.dirname.parent.join("share/#{@program}.conf")
    # Try based upon the main file's location in a different way.
    tries << @main.dirname.parent.parent.join("share/#{@program}.conf")
    # Try in a global location
    tries << Filename.share_dir.join("share/#{@program}.conf") 
    # Try in a local global location
    tries << Filename.share_local_dir().join("share/#{@program}.conf")     
    # Try in a windowzy subdirectory the user's home directory
    tries << Filename.home.join("#{@program}/share/#{@program}.conf")
    # Try in a unixy subdirectory the user's home directory
    tries << Filename.home.join('./#{@program}/#{@program}.conf') 
    # Try in the current working directory 
    tries << Filename.getwd.join('share/#{@program}.conf') 
    # Try from the current directory. 
    tries << Filename.getwd.join('#{@program}.conf') 
    # Try one up from the current in a different way. 
    
    # Now tru all of these locations in order
    for try in tries do
      
      puts try
      if try.exist?
        @confname = try
        @confdir  = try.dirname
        return true
      end
    end
        
    # If we get here, we failed.
    raise "Can't find configuration file, 'eruta.conf'."
    return false
  end
  
  
  def initialize(program = 'eruta')
    main      = __FILE__
    @main     = Filename.new(main).expand_path()
    @program  = program
    find_settings_file    
  end
  
  def dir
    return @confdir
  end
  
  def font_dir
    return @confdir + 'font'
  end
  
  def image_dir
    return @confdir + 'image'
  end
  
  def ui_dir
    return self.image_dir + 'ui'
  end
  
  def ui_background_dir
    return self.ui_dir + 'background'
  end
  
  def icon_dir
    return self.ui_dir + 'icon'
  end
  
  def cursor_dir
    return self.ui_dir + 'cursor'
  end
  
  def map_dir
    return @confdir + 'map'
  end
  
  def self.dir
    return self.instance.dir
  end
  
  def self.font_dir
    return self.instance.font_dir
  end
  
  def self.image_dir
    return self.instance.image_dir
  end
  
  def self.map_dir
    return self.instance.map_dir
  end
  
  def self.ui_dir
    return self.instance.ui_dir
  end
  
  def self.icon_dir
    return self.instance.icon_dir
  end
  
  def self.cursor_dir
    return self.instance.cursor_dir
  end
  
  def self.ui_background_dir
    return self.instance.cursor_dir
  end
  
  
end
