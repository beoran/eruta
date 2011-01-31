module Gari
  # Extra functions for the font
  class Font
    # returns the width and the height of the text in that order
    def size_text(text)
      w = self.width_of(text)
      h = self.lineskip
      return w, h
    end
    
    SYM_TO_NUM = { :normal    =>  Gari::Font::NORMAL, 
                   :bold      =>  Gari::Font::BOLD,
                   :italic    =>  Gari::Font::ITALIC,
                   :underline =>  Gari::Font::UNDERLINE,
                   :solid     =>  Gari::Font::SOLID,
                   :blended   =>  Gari::Font::BLENDED,
                   :shaded    =>  Gari::Font::SHADED  
                 }
                 
    def antialias=(how)
      num = SYM_TO_NUM[how] || how.to_i
      self.mode = num
    end 
  end
end