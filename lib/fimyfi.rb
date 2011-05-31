# encoding: UTF-8 
# 
# fimyfi.rb Find My Files, a module to help an application find the various
# files it needs.
 
 
module Fimyfi
  CURRENT_DIR       = '.'
  UNIVERSAL_DIRSEP  = '/'
  
  # Returns the name of the application
  def self.application
    @application ||= "fimyfi"
    return @application
  end
  
  # Sets the name of the application that is using fimyfi 
  def self.application=(appname)
    @application = appname
    return @application
  end
    
  def self.preference_dir
    pref_prefix = '.'
    return File.join(self.home, pref_prefix + self.application)
  end
    
  # Converts a "universal" or unix style filename to one the local system can accept
  def self.local_filename(filename)
    File.join(filename.split(UNIVERSAL_DIRSEP))
  end   
  
  # Takes the parts of the name, and returns a name under the preference_dir
  # preference file,  
  def self.preference_file(*name)    
    return File.join(self.preference_dir, *name)
  end
  
  def self.test_caller
    p caller
  end  
  
  def self.expand_path(filename)
    File.expand_path(filename) 
  end
  
  UNIVERSAL_SEPARATOR = '/'
  
  # Splits the file name according to a universal separator (/)
  def self.split(filename)  
    return filename.split(UNIVERSAL_SEPARATOR).map { |f| f.to_sym }  
  end
  
  # Joins and expands the path name.
  def self.join(*filenames)
    to_join         = []
    filenames.each  { |f| to_join += self.split(f.to_s) }     
    to_join.map!    { |f| f.to_s }    
    return self.expand_path(File.join(to_join)) 
  end
  
  # Tries to deterine the current user's home directory in a platform 
  # independent way. If not found, returns CURRENT_DIR in stead
  def self.home()
    home  = ENV['HOME']  || ENV['HOMEPATH']
    return home if home
    return CURRENT_DIR 
  end
  
  def self.main(program = 'eruta')
    @main   ||= self.expand_path(__FILE__)
    @program  = program
  end
  
  def self.maindir
    File.dirname(self.main)
  end
  
  
  # This tries to find the settings file of the program by looking in many 
  # different candidate directories. basically, it's trying all sorts of 
  # plausible directories until it finds one that has the configuration file.
  #
  def self.find_settings_file(program='eruta')
    @program  ||= program    
    tries       = []
    conffile    = "#{@program}.conf"
    # First try based upon this file's location.
    tries << self.join(self.maindir, :share, conffile)
    # Try based upon the main file's location in a different way.
    tries << self.join(self.maindir, '..', '..', :share, conffile)    
    # First try based upon this file's location, alternatively named.
    tries << self.join(self.maindir, :data, conffile)
    # Try based upon the main file's location in a different way, alternative.
    tries << self.join(self.maindir, '..', '..', :data, conffile)    
    # Try in a global location
    tries << self.join(:usr, :share, @program, conffile)
    # Try in a local global location
    tries << self.join(:usr, :local, :share, @program, conffile)
    # Try in a windowsy global location
    tries << self.join('Program Files', @program, conffile)
    # Try in a windowsy global location
    tries << self.join('Program Files(x86)', @program, conffile)
    # Try in a windowsy global location
    tries << self.join('Program Files', @program, :share, conffile)
    # Try in a windowsy global location
    tries << self.join('Program Files(x86)', @program, :share, conffile)
    # Try in a windowsy global location
    tries << self.join('Program Files', @program, :data, conffile)
    # Try in a windowsy global location
    tries << self.join('Program Files(x86)', @program, :data, conffile)
    # Try in a windowsy subdirectory the user's home directory
    tries << self.join(self.home,  @program, :share, conffile)
    # Try in a windowsy subdirectory the user's home directory
    tries << self.join(self.home,  @program, :data, conffile)        
    # Try in a unixy subdirectory the user's home directory    
    tries << self.join(self.home,  ".#{@program}", :share, conffile) 
    # Try in a unixy subdirectory the user's home directory
    tries << self.join(self.home,  ".#{@program}", :data, conffile)     
    # Try in the current working directory 
    tries << self.join(Dir.getwd, :share, conffile) 
    # Try in the current working directory 
    tries << self.join(Dir.getwd, :data, conffile) 
    # Try from the current directory. 
    tries << self.join(Dir.getwd, '..', :share, conffile)
    # Try from the current directory. 
    tries << self.join(Dir.getwd, '..', :data, conffile)
    # Try one up from the current in a different way.
    
         
    # Now try all of these locations in order
    for try in tries do
      puts try
      if File.exist?(try)
        @confname = try
        @confdir  = self.expand_path(File.dirname(try))
        return self.dir
      end
    end
        
    # If we get here, we failed.
    raise "Can't find configuration file, '#{program}.conf'."
    return false
  end
  
  
 
  
  def self.dir(program='eruta')
    find_settings_file(program) unless @confdir
    return @confdir
  end
  
  def self.font_dir
    return join(self.dir, 'font')
  end
  
  def self.image_dir
    return join(self.dir, 'image')
  end
  
  def self.tile_dir
    return join(self.image_dir, 'tile')
  end
  
  def self.background_dir
    return join(self.image_dir, 'background')
  end
  
  def self.sprite_dir
    return join(self.image_dir, 'sprite')
  end
  
  def self.map_dir
    return join(self.dir, 'map')
  end
  
  def self.ui_dir
    return join(self.image_dir, 'ui')
  end
  
  def self.ui_background_dir
    return join(self.ui_dir, 'background')
  end
  
  def self.icon_dir
    return join(self.ui_dir, 'icon')
  end
  
  def self.cursor_dir
    return join(self.ui_dir, 'cursor')
  end
  
  def self.map_dir
    return join(self.dir,  'map')
  end



end # module Fimyfi
