require 'pathname'
require 'forwardable'

module Sisa_SDL

  class Font
    extend Forwardable # We forward most requests to the font @handle
      
    attr_reader   :filename
    attr_reader   :name    
    attr_reader   :pointsize
    attr_reader   :handle        
    attr_accessor :antialias
    # Whether this font should be displayed antialiased or not. True by default.    
    
    def self.font_dir
      return Pathname.new(File.join( '..', 'share', 'font'))
    end    

    # Look for a font file that contains a font with this or a similarname
    def self.filename_for(name)
      try =  self.font_dir + "#{name}.ttf"
      puts  "Trying to load font file #{try} for font #{name}"
      return try if try.exist?
      try =  self.font_dir + "#{name.upcase}.TTF"
      puts  "Trying to load font file #{try} for font #{name}"      
      return try if try.exist?
      # First try in our own data directory
      
      # If that fails, try to load the font from the ones installed on the system 
      Font.font_files.each do | systemfont | 
        sysfontfile = Pathname.new(systemfont)
        p sysfontfile
        # If we find one sufficiently similar, retrn this one.  
        if sysfontfile.basename.to_s.downcase == "#{name.downcase}.ttf"
          try = sysfontfile
          puts  "Trying to load font file #{try} for font #{name}"
          return try if try.exist?
        end  
      end

      raise "Could not find font in font directory or system font directory!"
      return nil # Can't happen.
    end

    
      
    def initialize(name, pointsize=12)      
      SDL::TTF.init unless SDL::TTF.init? # Initialize ttf unless it was already done.
      @name     = name
      @pointsize= pointsize
      @filename = self.class.filename_for(@name)
      @handle   = SDL::TTF.open(@filename.to_s, @pointsize) 
      @antialias= :blended
      @cache    = [] 
      # A cache of the bitmaps that have been rendered before.
      # Do we need this?
    end
   
    protected 
    
    # Checks if a style flag is set. Protected.
    def is_style_flag(flag)
      (@handle.style & flag) != 0
    end
    
    # Sets a style flag is set. Protected. 
    def set_style_flag(flag, set)
      if set and (not is_style_flag(flag))
        # We only toggle a style to on if it wasn't already active
        @handle.style = @handle.style | flag
      elsif is_style_flag(flag)
        # We only toggle a style off if it wasn't already disabled
        @handle.style = @handle.style & (~flag)
      end
    end
    
    public
    
    # Returns true if this font is bold
    def bold?
      is_style_flag(SDL::TTF::STYLE_BOLD)
    end
    
    # Returns true if this font is italic
    def italic?
      is_style_flag(SDL::TTF::STYLE_ITALIC)
    end
    
    # Returns true if this font is underlined
    def underline?
      is_style_flag(SDL::TTF::STYLE_UNDERLINE)
    end
    
    # Sets this font to be bold or not depending on the value of b 
    def bold=(b)
      set_style_flag(SDL::TTF::STYLE_BOLD, b)
    end 
    
    # Sets this font to be italic or not depending on the value of i
    def italic=(i)
      set_style_flag(SDL::TTF::STYLE_ITALIC, i)
    end 
    
    # Sets this font to be underlined or not depending on the value of u
    def underline=(u)
      set_style_flag(SDL::TTF::STYLE_UNDERLINE, u)
    end
    
    # Maximum height of the font in pixels.    
    def_delegator  :@handle, :height, :height
    
    # Maximum height of the ascenders of the font in pixels.
    def_delegator  :@handle, :ascent, :ascent
        
    # Maximum height of the decenders of the font in pixels.
    def_delegator  :@handle, :descent, :descent
    
    # Recommened line skip for this font.
    def_delegator  :@handle, :line_skip, :line_skip
    
    # Returns the width of a given UTF-8 encoded text 
    def_delegator  :@handle, :text_size, :utf8_size
    def_delegator  :@handle, :text_size, :text_size
    
    
    # Quickly renders the given UTF-8 string in the given color as a palettized 8 bit bitmap.
    # Returns the rendered bitmap.
    def render_solid(text, color)
      return @handle.render_solid_utf8(text, *color)
    end
    
    # Renders the given UTF-8 string in the given color and with the given background color  
    # as a palettized 8 bit bitmap. Returns the rendered bitmap.
    def render_shaded(text, color, background)
      fr, fg, fb = *color
      br, bg, bb = *background
      return @handle.render_shaded_utf8(text, fr, fg, fb, br, bg, bb)
    end
    
    # Renders the given UTF-8 string in the given color on a transparent 
    # background as a 32 bit bitmap that has alpha values. Returns the 
    # rendered bitmap.
    def render_blended(text, color)
      return @handle.render_blended_utf8(text, *color)
    end
    
    # Quickly draws the given UTF-8 string to the target bitmap with the given coordinates
    # in the given color.
    def draw_solid(target, x, y,  text, color)
      @handle.draw_solid_utf8(target, text, x , y,  *color)
    end
    
    # Draws the given UTF-8 string to the target bitmap with the given coordinates
    # in the given color.    
    def draw_shaded(target, x, y, text, color, background)
      fr, fg, fb = *color
      br, bg, bb = *background
      @handle.draw_shaded_utf8(target, text, x, y, fr, fg, fb, br, bg, bb)
    end
    
    # Beautifully (but slowly) draws the given UTF-8 string to the target 
    # bitmap with the given coordinates in the given color.    
    def draw_blended(target, x, y, text, color)
      @handle.draw_blended_utf8(target, text, x, y, *color)
    end
    
    # Returns the sizes of the text when written in this font. 
    # The text MUST be UTF-8 encoded.
    def size(text)
      return @handle.text_size(text.force_encoding('UTF-8'))
    end
    
    alias :size_text :size
    
    # Returns the width of the text when written in this font. 
    # The text MUST be UTF-8 encoded.
    def width(text)
      w, h = @handle.text_size(text)
      return w
    end
    
    alias :wide  :width
    alias :w     :width 
     
    
    # Draws the the given text to the target. The text MUST be UTF-8 encoded.
    def draw(target, x, y, text, fg = nil, bg = nil)
      # Workaround for longstanding SDL_TTF misfeature that rendering returns NULL/nil 
      # bitmaps if the text string given is nil, or empty, or starts with a null character. 
      if !text || text.empty? || text =~ /\A\x0/
        return false 
      end  
      if    @antialias == :blended
        return self.draw_blended(target, x, y, text, fg)
      elsif @antialias == :shaded   
        return self.draw_shaded(target, x, y, text, fg, bg)
      else 
        return self.draw_solid(target, x, y, text, fg)
      end
    end  
    
    # Renders the given text. The text MUST be UTF-8 encoded.
    def render(text, fg = nil, bg = nil)
      if    @antialias == :blended
        return self.render_blended(text, fg)
      elsif @antialias == :shaded
        return self.render_shaded(text, fg, bg)
      else   
        return self.render_solid(text, fg)
      end
    end        
    
    # Cached load of a font
    def self.load(name, pointsize=12)            
      @font_setup               = true
      @loaded_fonts           ||= {}
      cachename                 = "#{name}|#{pointsize}"
      font                      = @loaded_fonts[cachename]
      return font if font # Try to get font from cache 
      font                      = Font.new(name, pointsize)
      @loaded_fonts[cachename]  = font
      return font # Load font, cache it, and return it if not already in cache
    end 
    
    # Empty the font cache
    def self.empty_cache() 
      @loaded_fonts = {}
    end
      
    # Allow marchalling of fonts   
    def self._load(data)
      arr = Marshal.load(data)
      p arr
      return self.load(*arr)
    end
    
    # Allow marchalling of fonts
    def _dump(depth=1)
      return Marshal.dump([@name, @pointsize]) 
    end
  
    
if RUBY_PLATFORM =~  /linux/
    # Load the Fontconfig DLL using ruby/dl. Linux only, for now. 
    module FontConfig
      require 'dl'
      require 'dl/import'
      require 'dl/struct'
      
      # Monke patch DL to get rid of spurious warnings
      module ::DL
        class Function
          alias :old_initialize :initialize
          def initialize(*args)
            @unsigned     = false
            old_initialize(*args)
          end
        end
      end    
                
  
      extend DL::Importer
      dlload "libfontconfig.so"
  
      FcFontSet = struct ["int nfont", "int sfont", "FcPattern** fonts"]
      FNAME     = struct ["char* str"]
  
      extern "FcPattern* FcPatternCreate()"
      extern "void FcPatternDestroy(FcPattern*)"
      extern "void FcDefaultSubstitute(FcPattern*)"
      extern "FcFontSet* FcFontSort(void*, FcPattern*, int, void*, int*)"
      extern "int FcPatternGetString(FcPattern*, char*, int, void*)"
      extern "void FcFontSetDestroy(FcFontSet*)"
    end
    
    # Returns all known system font files. Not portable yet.
    def self.get_font_files
      null    = DL::NULL
      result  = []
      pattern = FontConfig.FcPatternCreate
      FontConfig.FcDefaultSubstitute(pattern)
      fs = FontConfig::FcFontSet.new(FontConfig.FcFontSort(null, pattern, 1, null, null))
      FontConfig.FcPatternDestroy(pattern)
  
      fname = DL.malloc(DL::SIZEOF_VOIDP)
      FontConfig.FcPatternGetString(fs.fonts.ptr, "file", 0, fname)
      ptrsize = DL::SIZEOF_VOIDP
      fs.nfont.times do |i|
        FontConfig.FcPatternGetString((fs.fonts+ptrsize*i).ptr, "file", 0, fname)
        result <<  DL::CPtr.new(fname).ptr.to_s
      end
      FontConfig.FcFontSetDestroy(fs)
      return result
    end
    
else 
 # Non-linux stub 
    def self.get_font_files
      return []
    end
end    
    
    
    # Returns all known font files, cached, unless refresh is called
    def self.font_files(refresh = false)
      @font_files ||= nil
      if @font_files and (not(refresh))
        return @font_files 
      end
      @font_files = get_font_files
      return @font_files 
    end
    
  end # class Font

end # module Sisa


if __FILE__ == $0  
  require '../filename.rb'
  puts Sisa::Font.font_files
  puts Sisa::Font.font_files
  puts Sisa::Font.filename_for('dejavuserif')
end
  