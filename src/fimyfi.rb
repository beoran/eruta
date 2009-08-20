# encoding: UTF-8 
# 
# fimyfi:rb Find My Files, a module to help an application find various
# resources, preferences and configuration files.
 
 
module Fimyfi
  CURRENT_DIR       = '.'
  UNIVERSAL_DIRSEP  = '/'
  
  # Returns the name of the application
  def self.application
    @application ||= "fimefi"
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
  
  
  # Tries to deterine the current user's home directory in a platform 
  # independent way. If not found, returns CURRENT_DIR in stead
  def self.home()
    home  = ENV['HOME']  || ENV['HOMEPATH']
    return home if home
    return CURRENT_DIR 
  end
  



end # module Fimyfi

#
#
#
#
if __FILE__ == $0
  Fimyfi.test_caller
  p __FILE__
  p File.expand_path(__FILE__)
end