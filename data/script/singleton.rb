# Own implementation of the singleton pattern
module Singleton
  # Returns the instance of the singleton
  def instance(*args)
    @instance ||= self.new(*args)
    return @instance 
  end
end
  
  
end # modume Singleton
