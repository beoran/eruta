
class Font
  extend Store::Forward

  def self.load_ttf(name, vpath)
    return Store.load_ttf_font(forward_name(name), vpath)
  end

end
  
  










