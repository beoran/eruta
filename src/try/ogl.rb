
begin
  require 'rubygems'
rescue Exception
end

require 'sdl'
require 'opengl'

module OGL
  include Gl
  
  # class that holds OGL textures
  class Texture
    attr_accessor :name   # OpenGL texture name (texture id) 
    attr_accessor :format # The color format of the texture 
    attr_accessor :w; # width of the original surface 
    attr_accessor :h; # height of the original surface 
    attr_accessor :wtex;
    attr_accessor :htex; # The actual size of the OpenGL texture 
                         # (it might differ, power of two etc.) 
    def free
      glDeleteTextures(1, @name)
    end
    
  end

  # Placeholder exception class
  class Error < Exception
  end  
  
  # Class to emulate SDL::PixelFormat, since it won't allow us to construct that.
  # Sigh... :p
  class Format
   attr_accessor :Rmask 
   attr_accessor :Gmask
   attr_accessor :Bmask 
   attr_accessor :Amask
   attr_reader   :bpp # bits per pixel
   attr_reader   :bytes_per_pixel # bytes per pixel
   
   def initialize()
    @Rmask = @Gmask = @Bmask  = @Amask = 0
    @bpp   = @bytes_per_pixel = 0
   end 
    
   def bpp=(value)
    @bpp              = value.to_i 
    @bytes_per_pixel  = value.to_i / 8
   end
 
   def bytes_per_pixel=(value)
    @bytes_per_pixel  = value.to_i  
    @bpp              = value.to_i * 8
   end
  end

  def max(a, b) 
     return (a) < (b) ? (b) : (a)  
  end
  
  def min(a, b) 
    return (a) > (b) ? (b) : (a)  
  end  
  
  UNKNOWN = 0
  DEBUG   = true 
  
  
  def dprintf(*args)
    return unless DEBUG
    printf(*args)
    puts("")
  end
  
  def compare_format(a, b)
    dprintf("%.8X, %.8X, %.8X, %8X vs.", a.Rmask, a.Gmask, a.Bmask, a.Amask);
    dprintf("%.8X, %.8X, %.8X, %8X", b.Rmask, b.Gmask, b.Bmask, b.Amask);
    if(a.bytes_per_pixel != b.bytes_per_pixel)
      dprintf("Nope.");
      return false;
    end
   
    if(a.bytes_per_pixel == 3)
        if(a.Rmask == b.Rmask &&
        a.Gmask == b.Gmask &&
        a.Bmask == b.Bmask ) then
          dprintf("Yep.");
          return true;
        end
    else    
        if (a.Rmask == b.Rmask &&
        a.Gmask == b.Gmask &&
        a.Bmask == b.Bmask && a.Amask == b.Amask) then
          dprintf("Yep.");
          return true;
        end          
    end
   dprintf("Nope.");
   return false;
  end
  
  def big_endian?
    return [1].pack("I") == [1].pack("N")
  end
  
  
# Returns an SDL_PixelFormat from GL_RGBA or GL_RGBA for the current machine 
  def get_pixel_format(glFormat) 
    format = Format.new
    if glFormat == GL_RGBA then
            format.bytes_per_pixel = 4;
            if big_endian? 
              format.Rmask = 0xff000000;
              format.Gmask = 0x00ff0000;
              format.Bmask = 0x0000ff00;
              format.Amask = 0x000000ff;
            else  
              format.Rmask = 0x000000ff; 
              format.Gmask = 0x0000ff00;
              format.Bmask = 0x00ff0000;
              format.Amask = 0xff000000;
            end
    else
      format.bytes_per_pixel = 3;
            if big_endian? 
              format.Rmask = 0xff0000;
              format.Gmask = 0x00ff00;
              format.Bmask = 0x0000ff;
              format.Amask = 0;
            else  
              format.Rmask = 0x0000ff; 
              format.Gmask = 0x00ff00;
              format.Bmask = 0xff0000;
              format.Amask = 0;
            end
          end    
    return format
  end

# Initializes SDL with OpenGL with the given width, height, color depth and flags 
  def init(w, h, bpp, flags)
    SDL.init(SDL::INIT_VIDEO)
=begin
    for each in SDL::GL.constants do 
      const = SDL::GL.const_get(each)
      puts "#{each} #{const}"
      attr  = SDL::getGLAttr(SDL::GL::RED_SIZE)
    end
=end
    SDL::setGLAttr(SDL::GL::DOUBLEBUFFER, 1)
    Gl.disable_error_checking
    @screen = SDL.setVideoMode(w, h, bpp, SDL::OPENGL | flags)
    return false unless @screen
=begin     
@buf = glSelectBuffer(512)
    res  = glGetIntegerv( GL_VIEWPORT);
    puts "viewport"
    puts res
    puts @buf.unpack('IIII')    
=end
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT );
    glDisable( GL_DEPTH_TEST );  
    glDisable( GL_LIGHTING ); 
    glDisable( GL_DITHER );
    glLoadIdentity();



# glGetIntegerv( GL_VIEWPORT, iViewport );  
#   
# // Save a copy of the projection matrix so that we can restore it  
# // when it's time to do 3D rendering again.  
# glMatrixMode( GL_PROJECTION );  
# glPushMatrix();  
# glLoadIdentity();  
#   
# // Set up the orthographic projection  
# glOrtho( iViewport[0], iViewport[0]+iViewport[2],  
# iViewport[1]+iViewport[3], iViewport[1], -1, 1 );  
# glMatrixMode( GL_MODELVIEW );  
# glPushMatrix();  
# glLoadIdentity();  
#   
# // Make sure depth testing and lighting are disabled for 2D rendering until  
# // we are finished rendering in 2D  
#   

    return true;
  end  

#  Flips the OpenGL buffers, updates the screen 
  def flip()
    SDL::GLSwapBuffers();
  end  

  def clear()
    glClear(GL_COLOR_BUFFER_BIT)
  end

  # draw textures using triangle strips. 
  # Still has a bug, but it's not faster than quads anyway (tested on 1 machine).
  def blit_triangles(hcw, hch, l, t, r, b)
    glBegin(GL_TRIANGLE_STRIP); #triangle strips, speedier?
        glTexCoord2f(l,t);    glVertex2f(-hcw,-hch);
        glTexCoord2f(t,t);    glVertex2f(hcw,-hch);
        glTexCoord2f(r,b);    glVertex2f(-hcw,hch);
        glTexCoord2f(l,b);    glVertex2f(hcw,hch);
    glEnd();
  end  

  # Draw texture using quads
  def blit_quads(hcw, hch, l, t, r, b)
    glBegin( GL_QUADS );  
    # Top-left 
    glTexCoord2f(l, t);
    glVertex2f(-hcw, -hch);
    # Top-right 
    glTexCoord2f(r, t);
    glVertex2f(hcw, -hch);
    # Bottom-right 
    glTexCoord2f(r, b);
    glVertex2f(hcw, hch);
    # Bottom-left 
    glTexCoord2f(l, b);
    glVertex2f(-hcw, hch);
    glEnd();
  end


  def fastblit(texture, x, y)
    l    = r   = t = b = 0.0;
    ch   = cw  = 0;
    hch  = hcw = 0.0;
  
    glBindTexture( GL_TEXTURE_2D, texture.name );
    glPushMatrix();
  
    cw = texture.w;
    ch = texture.h;  
    l = t = 0;
    r = b = 1;    
    hcw = (cw / 2.0);
    hch = (ch / 2.0);
    glTranslatef(x + hcw, y + hch, 0); 
    blit_quads(hcw, hch, l, t, r, b);
    # glLoadIdentity(); 
    glPopMatrix();  
  end
      
  def blit(texture, x, y, rotation = 0, clipping = nil, vZoom = 1, hZoom = 1)
    l    = r   = t = b = 0.0;
    ch   = cw  = 0;
    hch  = hcw = 0.0;
  
    glBindTexture( GL_TEXTURE_2D, texture.name );
    glPushMatrix();
  
    if(clipping) then
      l = clipping.x.to_f / texture.wTex;
      t = clipping.y.to_f / texture.hTex;
  
      r = (clipping.x + clipping.w).to_f / texture.wTex;
      b = (clipping.y + clipping.h).to_f / texture.hTex;
  
      cw = clipping.w;
      ch = clipping.h;
    else
      cw = texture.w;
      ch = texture.h;  
      l = t = 0;
      r = b = 1;
    end
  
    hcw = (cw / 2.0) * hZoom;
    hch = (ch / 2.0) * vZoom;
    glTranslatef(x + hcw, y + hch, 0);
    glRotatef(rotation, 0, 0, 1.0);
    blit_quads(hcw, hch, l, t, r, b)
    glPopMatrix();  
  end

# (internal) Returns the smallest power-of-two value in which 'val' fits, with a max size of 'max'
  def get_power_two(val, max)
    i, count, high = 0;
    count = 0;
    for i in 0..32 
      result = 1 >> i
      break if val <= result 
    end    
    return min(val, max)
  end 

  # Determines maximum size of texture
  def max_texture_size
    return [glGetIntegerv(GL_MAX_TEXTURE_SIZE)].pack('I').unpack('I').first
  end

# (internal) Checks if a surface with the given format needs to be resized or 
# converted before turning it into an OpenGL surface. Returns true if the 
# surface is "ok", ie. doesn't have to be resized. Suggests what a good texture 
# size is by also retrning the new size if the original size didn't work.
 
   def get_tex_size(size, format)
    ret = true;  
    maxsize = max_texture_size()

    if(size.x > maxsize || size.y > maxsize)
      size.x = min(size.x, maxsize);
      size.y = min(size.y, maxsize);
      ret = false;
    end
  
    if(!probe_texture(size.x, size.y, format))
      size.x = get_power_two(size.x, maxsize);
      size.y = get_power_two(size.y, maxsize);  
      size.x = size.y = 0 unless probe_texture(size.x, size.y, format)
      ret = 0;
    end
    return ret, size;
  end

# (internal) Probes the OpenGL driver with a specified texture size and format. 
# Returns true if the size and format is ok. 
  def probe_texture(w, h, format)  
    bpp = 0;
    width = 0;
    if format == GL_RGB || format == GL_BGR 
      bpp = 3;
    else
      bpp = 4;
    end

    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, nil);  
    width = glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH);
    # puts @buf.unpack('II')

    unless width and width > 0
      dprintf("Texture probing failed for %i x %i", w, h);
    end
    dprintf("Width is %d", width)
    return width > 0;
  end


# (internal) Determines the corresponding OpenGL texture color format from an 
# SDL_Surface. Returns OGL_UNKNOWN if it has an unsupported color format. 
  def get_format(surface)
    if surface.format.Amask != 0
      rgba = get_pixel_format(GL_RGBA)
      dprintf("Test RGBA")
      if compare_format(rgba, surface.format)
        dprintf("It's RGBA")
        return GL_RGBA;
      end
    else
      if surface.format.bytes_per_pixel == 3
        dprintf("Test RGB")
        rgb = get_pixel_format(GL_RGB)
        if compare_format(rgb, surface.format)
          dprintf("It's RGB")
          return GL_RGB
        end
      end
    end
    dprintf("OGL_UNKNOWN")
    return UNKNOWN
  end

# (internal) Converts an SDL_Surface with a compatible color format to an OGL_Texture
  def from_surface_convert(surface, format, origsize)
    texture   = nil;
    tmp_surf  = nil;
    return texture unless surface
    texture   = Texture.new
    return texture unless texture
    texture.w       = origsize.x;
    texture.h       = origsize.y;
    texture.htex    = surface.w;
    texture.wtex    = surface.h;
    texture.format  = format;
    textures        = glGenTextures(1); 
    return nil unless textures && textures.size > 0
    texture.name    = textures.first 
    puts texture.name
    glBindTexture(GL_TEXTURE_2D, texture.name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    # glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, 1);
    surface.lock if surface.must_lock?
    arr = surface.pixels.unpack('C*')
    raise "Can't happen" if arr.size != surface.w * surface.h * surface.format.bytes_per_pixel
    # texture.format
    glTexImage2D(GL_TEXTURE_2D, 0, surface.format.bytes_per_pixel, 
                 surface.w, surface.h, 0, GL_RGBA, 
                 GL_UNSIGNED_BYTE, arr)
    surface.unlock if surface.must_lock?
    return texture;
  end

# (internal) Makes an RGBA-copy of a given SDL_Surface with the given new size
  def copy_surface(from, size)
    ret = nil
    fmt = nil;
    fmt = get_pixel_format(GL_RGBA);
    ret = SDL::Surface.new(SDL::SRCALPHA | SDL::SWSURFACE, size.x, size.y, 32, 
                           fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask);
    SDL::Surface.blit(from, 0,0,0,0, ret, 0,0) if ret
    return ret;
  end

# Creates an OGL_Texture from an SDL_Surface and frees the SDL_Surface*/
  def from_surface_free(surface)
    ret     = from_surface(surface);
    surface = nil if ret
    return ret
  end

# Creates an OGL_Texture from an SDL_Surface 
  def from_surface(surface)
    format    = nil;
    size      = SDL::Rect.new(0,0,0,0)
    origsize  = SDL::Rect.new(0,0,0,0)
    reblit    = error = false;
    ret       = nil;
    unless surface
      raise Error.new("ogl: Surface passed to OGL::from_surface is nil.");
    end

    format      = get_format(surface)
    origsize.x  = size.x = surface.w;
    origsize.y  = size.y = surface.h;
    if format == UNKNOWN
    # Unsupported color format for direct conversion, see if the size is ok 
      dprintf("Unsupported colorspace");
      ok, size = get_tex_size(size, GL_RGBA);
      reblit   = true;
    else
    # Supported color format, see if the size is ok 
      dprintf("Supported colorspace");
      ok, size  = get_tex_size(size, format);
      reblit    = !ok
    end
    # If a OGL_GetTexSize didn't fail to find a texture size 
    if (size.x == 0 && size.y == 0)
      raise Error.new("ogl: Couldn't find a suitable texture size for the given surface.");
    end
  
    unless reblit
      # The original surface size and color format was ok for direct conversion 
      ret = from_surface_convert(surface, format, origsize)
    else
      # The original surface size and/or color format wasn't ok for direct conversion.
      # Do an RGBA-copy of it with the suggested size and then convert it.
      tmp = copy_surface(surface, size);
    unless tmp
      raise Error.new("oglblit: Couldn't copy an unsupported surface to an RGBA surface.");
    end
      ret = from_surface_convert(tmp, GL_RGBA, origsize)
    unless ret  
      raise Error.new("SDL_oglblit: Couldn't perform a surface to texture conversion, even though the given surface seems compatible.")
    end
      tmp = nil
    end  
    return ret
  end
  extend OGL


end # module OGL

if __FILE__ == $0 # Built-in testing
    require 'time' # It's gonna take time, a whole lot of precious time... :)
    
    OGL.init(640, 480, 32, 0 )
    # SDL::HWSURFACE | SDL::FULLSCREEN
    sur1    = SDL::Surface.load('../../share/image/tile/tile_grass32.png')
    sur2    = SDL::Surface.load('../../share/image/tile/tile_ugly_cloud_al32.png')
    tex1    = OGL.from_surface(sur1)
    tex2    = OGL.from_surface(sur2)
    p tex1
    p tex2

    done    = false
    levels  = [tex2, tex1, tex1, tex2, tex2]
    start   = Time.now()
    frames  = 0
  
    until done do
      event = SDL::Event2.poll
        if !event.nil?
          done = true if event.is_a?(SDL::Event2::Quit)
          if event.is_a?(SDL::Event2::KeyDown) # This means it's some kind of key press
            # if event.sym == SDL::Key::RIGHT
            # if event.sym == SDL::Key::LEFT
            done = true if event.sym == SDL::Key::ESCAPE
          end
        end
      (0..0).step(1) do |z|
        (0..640).step(32) do |x|
          (0..480).step(32) do |y|
            OGL.fastblit(levels[z], x, y)
            # OGL.blit(levels[z], x, y, 0)
          end
        end
      end
      OGL.flip()
      frames += 1
    end
    stop  = Time.now()
    delta = stop - start
    fps   = frames.to_f / delta.to_f
    puts "#{frames} frames in #{delta} s: #{fps} fps"
    SDL.quit()
end
