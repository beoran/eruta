# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 

module Heels
  class App
    def location      
    end
  end
  
 
  
class Shoes
    RELEASE_NAME = "Sexy Suzy"
    RELEASE_ID   = 1
    REVISION     = 1
    
    class NotImplementedError < StandardError
      
    end
    
    class VideoError < StandardError
      
    end
    
    def Heels.escape(string)
      string.gsub(/&/n, '&amp;').gsub(/\\\"/n, '&quot;').gsub(/>/n, '&gt;').gsub(/</n, '&lt;');
    end
    
    def Heels.app()       
    end
    
    def Heels.p(*args)       
    end
    
    def Heels.debug(*args)     
    end

    class Flow      
    end
    
    class Stack      
    end

    class Mask
    end
    
    class Shape
      def draw(x, y)        
      end
      
      def move(dx, dy)        
      end
      
      def remove()        
      end
      
      def style(*args)        
      end
      
      def hide()        
      end
      
      def show()        
      end
      
      def toggle()        
      end
      
      def click()        
      end
      
      def hover()        
      end
      
      def leave()        
      end
      
    end
    

    class Image
      def path        
      end
      
      def path=(newpath)        
      end
      
      def draw(x, y)        
      end
      
      def size()        
      end
      
      def move(dx, dy)        
      end
      
      def remove()        
      end
      
      def style(*args)        
      end
      
      def hide()        
      end
      
      def show()        
      end
      
      def toggle()        
      end
      
      def click(*args)        
      end
      
      def hover(*args)        
      end
      
      def leave(*args)        
      end
      
    end
    
    class Video
      def draw(x, y)        
      end
      
      def move(dx, dy)        
      end
      
      def remove()        
      end
      
      def style(*args)        
      end
      
      def hide()        
      end
      
      def show()        
      end
      
      def toggle()        
      end
      
      def click(*args)        
      end
      
      def hover(*args)        
      end
      
      def leave(*args)        
      end
      
      def playing?()        
      end
      
      def play()        
      end
      
      def clear()        
      end
      
      def previous()        
      end
      
      def next()        
      end
      
      def pause()        
      end
      
      def stop()        
      end
      
      def length()        
      end
      
      def position()        
      end
      
      def position=(p)        
      end
      
      def time        
      end
      
      def time=(t)        
      end
      
    end
  
    
    class Pattern
      def move(dx, dy)        
      end
      
      def remove()        
      end
      
      def to_pattern()        
      end
      
      def style(*args)        
      end
      
      def hide()        
      end
      
      def show()        
      end
      
      def toggle()        
      end
    end

  class Background
    def draw(x, y)      
    end
  end
    
  class Border
    def draw(x, y)      
    end
  end
  
  class TextBlock
    def contents      
    end
    
    def parent      
    end
    
    def draw(x, y)      
    end
  
    def cursor=(newcursor)      
    end
    
    def cursor      
    end
    
    def move(dx, dy)
    end
      
    def remove()      
    end
    
    def to_s()      
    end
    
    def replace(*args)      
    end
    
    def style(*args)        
    end
      
    def hide()        
    end
      
    def show()        
    end
      
    def toggle()        
    end
      
    def click(*args)        
    end
      
    def hover(*args)        
    end
      
    def leave(*args)        
    end

    class Para < TextBlock     
    end
    
    class Banner < TextBlock          
    end
    
    class Title < TextBlock           
    end

    class Subtitle < TextBlock           
    end

    class Tagline < TextBlock           
    end
    
    class Caption < TextBlock           
    end
    
    class Inscription < TextBlock           
    end
    
    class Text
      def contents      
      end
    
      def parent      
      end
      
      def style        
      end      
      
      def click()        
      end
      
      def hover()        
      end
      
      def leave()        
      end

    end
    
    class Code < Text      
    end
    
    class Del < Text      
    end
    
    class Em < Text      
    end
    
    class Ins < Text      
    end
       
    class Span < Text      
    end
    
    class Em < Text      
    end
    
    class Ins < Text      
    end

    class Span < Text      
    end

    class Strong < Text      
    end

    class Sub < Text      
    end
    
    class Sup < Text      
    end
    
    class Link < Text      
      def click(*args)        
      end

      def hover(*args)        
      end

      def leave(*args)        
      end      
    end
    
    class LinkHover < Text      
    end

    class Native     
      def style(*args)        
      end
      
      def hide()        
      end

      def show()        
      end
      
      def move(dx, dy)        
      end
      
      def remove()        
      end
      
    end

  class Button < Native
    def draw(x, y)      
    end
    
    def click(*args)      
    end    
  end  
  
  class EditLine < Native
    def draw(x, y)      
    end
    def text()      
    end
    def text=(t)      
    end
    def change(*args)
    end
  end  
  
  class EditBox < Native
    def draw(x, y)      
    end
    def text()      
    end
    def text=(t)      
    end
    def change(*args)
    end
  end
    
  class ListBox < Native
    def draw(x, y)      
    end
    def text()      
    end
    def choose(choice)      
    end
    def change(*args)
    end
    def items=(newitems)      
    end
    def items()      
    end
  end
  
  class Progress < Native
    def draw(x, y)      
    end
  end

  class Animation
    def draw(x,y)      
    end
    def remove      
    end
    def start      
    end
    def stop      
    end
    def toggle      
    end
    
  end

  class Color
    
    def Color.rgb      
    end
    
    def Color.gray      
    end
    
    def Color.parse      
    end
    
    def inspect      
    end
    
    def to_s      
    end
    
    def to_pattern      
    end
    
  end
  
  class LinkUrl    
  end 
   
  def alert(t)    
  end
  
  def ask(t)    
  end
  
  def confirm(t)    
  end
  
  def ask_color(t)    
  end
  
  def ask_open_file()    
  end
  
  def ask_save_file()    
  end
  
  end
  
end
