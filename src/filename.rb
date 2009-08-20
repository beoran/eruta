# encoding: UTF-8
#
# filename.rb : An Object-Oriented Ruby File Name Class
#
# Copyright (c) Bjorn De Meyer 2007-2009. Released under the zlib license.
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
# 
# 1. The origin of this software must not be misrepresented; you must not
#     claim that you wrote the original software. If you use this software
#     in a product, an acknowledgment in the product documentation would be
#     appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#     misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
#
# Filename represents a the name of a file or directory in a portable way 
# It is a replacement of the standard Filename library which has some 
# portability issues. The key features are as follows:
#  
# 1) This Filename class uses UNIX style filenames with forward slashed, 
# on all systems in it's external API, but translates them 
# internally, or when the file is used to OS-specific file names.
# 2) Internally, it does not use a string representation for the path, but 
# an array of directories, which makes it more quick and simple to process 
# file names. 


class Filename 
  if File::ALT_SEPARATOR
    OUT_SEPARATOR = File::ALT_SEPARATOR
  else
    OUT_SEPARATOR = File::SEPARATOR
  end    
  
  IN_SEPARATOR        = '/'
  DIRECTORY_NOW       = '.'
  DIRECTORY_UP        = '..'
  
  protected
  # c:/windows/system -> "c:/", [windows, system]   
  # //foo/bar/baz -> "//foo", [bar, baz]
  # /bar/baz -> "/", [bar, baz]
  # bar/baz  -> 
  # Parses a universal name. Private
  def self.parse_universal_name(name)
    match = name.match(%r{\A(//[^/]+/|[^:]+:/|/|)(.*)\Z})
    # The regexp parses universal names that either have:
    # a //server/ style root
    # a windows:/ style root
    # unix root (just /)
    # no root
    raise "universal file name not valid" unless match
    root = match[1]
    rest = match[2]  
    if rest.empty? 
      return root, []
    else
      return root, rest.split(%r{#{IN_SEPARATOR}+})      
    end    
  end
  
  # Parses a system-dependent file name 
  # Currently just replaces the system-dependent separator
  # by the universal one, and calls parse_universal_name
  def self.parse_local_name(name)
    uniname = name.tr(OUT_SEPARATOR, IN_SEPARATOR)
    return self.parse_universal_name(uniname)
  end
  
  # Checks if the other object is ducktype compatible with this object
  def compatible?(other)
    return (other.respond_to? :root and other.respond_to? :parts)
  end
  
  
  public
  
  # Returns a new name Filename form a local file name
  def self.new_local(name)
     root, parts = self.parse_local_name(name) 
     return self.new(root, *parts)   
  end
  
  # The root part of the path
  def root
    return @root.dup
  end
  
  #  An array with the parts of the path
  def parts
    return @parts.dup
  end
  
  #
  # You can initialize Filename with a +universal_name+ string,
  # that is a string with a file name that uses unix file separator characters,
  # but that may include non-unix roots, or server designations.
  # Filename will make sure that the file name is translated to a valid name on 
  # the current platform. Double slashes in the middle of the file name will be 
  # reduced to a single one, and trailing slashes will be ignored.
  #
  # Examples of valid universal names:
  # /foo/bar//baz/
  # baz/quux
  # c:/foo/bar
  # //server/sub/dir
  # 
  # You can also pass in a filename or another object that has root and parts
  # methods. The root should return a string, the parts should return an array 
  # of the parts of the path. 
  # 
  # You can also construct a name by giving in the root in universal_name, and 
  # the parts of that file name as the optional arguments
  #  
  def initialize(universal_name, *other_parts)
    # Ducktype copy constructor
    if compatible?(universal_name)
      @root   =  universal_name.root
      @parts  =  universal_name.parts
    else
      @root, @parts = self.class.parse_universal_name(universal_name)    
    end
    @parts += other_parts
    @path   = platform_name
    # Precalculate the platform-dependent name because we'll need it a lot.
    @name   = universal_name
    # Also precalculate the universal name.
    self.taint if other_parts.tainted?
    self.taint if universal_name.tainted?
  end
  
  def freeze()  super; @root.freeze ; @parts.freeze ; self end
  def taint()   super; @root.taint  ; @parts.taint  ; self end
  def untaint() super; @root.untaint; @parts.untaint; self end

  #
  # Compare this Filename with +other+.  The comparison ties to be smart,
  # however it might be fooled by '..' or '.' references
  # 
  def ==(other)
    return false unless compatible?(other)
    return other.root == @root && other.parts == @parts
  end
  alias === ==
  alias eql? ==

  # Provides for comparing Filenames, case-sensitively.
  def <=>(other)
    return false unless compatible?(other)
    return other.parts <=> @parts
  end

  def hash # :nodoc:
    @parts.hash
  end

  # Determines the platform-dependent path for this universal Filename, 
  # adjusted to the current platform's file name  conventions.
  def platform_name
    return @root + @parts.join(OUT_SEPARATOR)
  end
  
  # Determines the path as a universal name String. 
  # Useless doubled slashes and trailing slashes are removed, so this might not 
  # be the same as what you passed into the constructor!  
  def universal_name
    return @root + @parts.join(IN_SEPARATOR)
  end
  
  # Return the path as a String, adjusted to the current platform's file name 
  # conventions
  def to_s
    return @path
  end
    
  # to_path is implemented so Filename objects are usable with File.open, etc.
  alias_method :to_path, :to_s
  
  # to_str is implemented so Filename objects are usable with File.open, etc.
  alias_method :to_str, :to_s
  
  # path is implemented as yet another useful alias.
  alias_method :path, :to_s
  
  # Return a Filename for which the universal name is substituted by String#sub.
  def sub(pattern, *rest, &block)
    self.class.new(self.universal_name.sub(pattern, *rest, &block))
  end
  
  # Returns a cleaned Filename in which useless dots and double dots 
  # have been removed without accessing the filesystem. This function is 
  # not guaranteed to remove all useless directories.
  #
  # The argument +consider_symlink+ is accepted only for compatibility with 
  # Filename.  If you care about symlinks, you should acces the file system.
  #
  def cleanpath(consider_symlink=false)
    if consider_symlink
    warn "Filename#cleanpath does not consider symlinks. Use realpath in stead."
    end
    newparts = @parts.dup
    newparts2 = []
    aid       = newparts.pop
    skip      = 0
    while (aid)          
      # We'll have to skip one of parts of the directory, since we have to move one up,
      # unless if this is the firts part of the directory,
      # or unless if the next level of the directory also moves up or at the 
      # same level
      if aid == DIRECTORY_UP 
        skip+= 1
      # Skip any '.' parts, unless it is the first part of the directory name.  
      elsif aid == DIRECTORY_NOW and newparts.size > 1
        # Do nothing.
      else # Add to the parts, but remember to skip one if we have to.
        if skip < 1
          newparts2.unshift(aid)        
        else
          skip -= 1  
        end
      end      
      aid       = newparts.pop
    end 
    
    # If and while there are still directories to go up, add them, 
    # unless if we have a root (to prevent going higher than the root).
    if @root.empty?
      while skip > 0 
        newparts2.unshift(DIRECTORY_UP)        
        skip -= 1  
      end
    end
    
    return self.class.new(@root, *newparts2)    
  end

  # #parent returns the parent directory.
  #
  def parent
    newparts = @parts.dup
    newparts.pop
    return self.class.new(@root, *newparts)        
  end

  # #mountpoint? returns +true+ if <tt>self</tt> points to a mountpoint.
  def mountpoint?
    begin
      stat1 = self.lstat
      stat2 = self.parent.lstat
      stat1.dev == stat2.dev && stat1.ino == stat2.ino ||
      stat1.dev != stat2.dev
    rescue Errno::ENOENT
      false
    end
  end

  # #root?  returns +true+ if this filename represents a root directory.  
  # It doesn't access the filesystem.  So it may return +false+ for some
  # Filenames which points to roots such as <tt>/usr/..</tt>.
  #
  def root?
    return (!@root.empty?) && (@parts.empty)
  end

  # Predicate method for testing whether a path is absolute.
  # It returns +true+ if the Filename has a non-empty root
  def absolute?
    !relative?
  end

  # The opposite of #absolute?
  def relative?
    return (!@root.empty?)
  end

  #
  # Iterates over each component of the path.
  #
  #   Filename.new("c:/usr/bin/ruby").each_filename {|filename| ... }
  #     # yields "usr", "bin", and "ruby".
  #
  def each_filename # :yield: filename    
    @parts.each { |filename| yield filename }
    nil
  end

  # Iterates over and yields a new Filename object
  # for each element in the given path in descending order.
  #
  #  Filename.new('/path/to/some/file.rb').descend {|v| p v}
  #     #<Filename:/>
  #     #<Filename:/path>
  #     #<Filename:/path/to>
  #     #<Filename:/path/to/some>
  #     #<Filename:/path/to/some/file.rb>
  #
  #  Filename.new('path/to/some/file.rb').descend {|v| p v}
  #     #<Filename:path>
  #     #<Filename:path/to>
  #     #<Filename:path/to/some>
  #     #<Filename:path/to/some/file.rb>
  #
  # It doesn't access the filesystem.
  def descend
    vs = []
    ascend {|v| vs << v }
    vs.reverse_each {|v| yield v }
    nil
  end

  # Iterates over and yields a new Filename object
  # for each element in the given path in ascending order.
  #
  #  Filename.new('/path/to/some/file.rb').ascend {|v| p v}
  #     #<Filename:/path/to/some/file.rb>
  #     #<Filename:/path/to/some>
  #     #<Filename:/path/to>
  #     #<Filename:/path>
  #     #<Filename:/>
  #
  #  Filename.new('path/to/some/file.rb').ascend {|v| p v}
  #     #<Filename:path/to/some/file.rb>
  #     #<Filename:path/to/some>
  #     #<Filename:path/to>
  #     #<Filename:path>
  #
  # It doesn't access the filesystem.
  #
  def ascend
    path = @path
    yield self
    parts =  @parts.dup
    parts.pop
    while parts.size > 1            
      yield self.class.new(@root, parts)
      parts.pop
    end
  end

  #
  # Filename#+ appends a Filename fragment to this one to produce a new Filename
  # object.
  #
  #   p1 = Filename.new("/usr")      # Filename:/usr
  #   p2 = p1 + "bin/ruby"           # Filename:/usr/bin/ruby
  #   p3 = p1 + "/etc/passwd"        # Filename:/etc/passwd
  #
  # This method doesn't access the file system. The root of the new Filename
  # will be that of +self+. 
  #
  def +(other)
    other     = Filename.new(other) unless compatible?(other)
    newparts  = @parts + other.parts    
    return Filename.new(@root, newparts)
  end
  
  #
  # Filename#join joins universal file names.
  #
  # <tt>path0.join(path1, ..., pathN)</tt> is the same as
  # <tt>path0 + path1 + ... + pathN</tt>.
  #
  def join(*args)
    result = self.dup
    for arg in args do
      arg    = Filename.new(arg) unless compatible?(arg)
      result = result + arg
    end
    return result
  end
    
end

class Filename    # * IO *
  #
  # #each_line iterates over the line in the file.  It yields a String object
  # for each line.
  #
  # This method has existed since 1.8.1.
  #
  def each_line(*args, &block) # :yield: line
    IO.foreach(@path, *args, &block)
  end

  # See <tt>IO.read</tt>.  Returns all the bytes from the file, or the first +N+
  # if specified.
  def read(*args) IO.read(@path, *args) end

  # See <tt>IO.readlines</tt>.  Returns all the lines from the file.
  def readlines(*args) IO.readlines(@path, *args) end

  # See <tt>IO.sysopen</tt>.
  def sysopen(*args) IO.sysopen(@path, *args) end
end


class Filename    # * File *

  # See <tt>File.atime</tt>.  Returns last access time.
  def atime() File.atime(@path) end

  # See <tt>File.ctime</tt>.  Returns last (directory entry, not file) change time.
  def ctime() File.ctime(@path) end

  # See <tt>File.mtime</tt>.  Returns last modification time.
  def mtime() File.mtime(@path) end

  # See <tt>File.chmod</tt>.  Changes permissions.
  def chmod(mode) File.chmod(mode, @path) end

  # See <tt>File.lchmod</tt>.
  def lchmod(mode) File.lchmod(mode, @path) end

  # See <tt>File.chown</tt>.  Change owner and group of file.
  def chown(owner, group) File.chown(owner, group, @path) end

  # See <tt>File.lchown</tt>.
  def lchown(owner, group) File.lchown(owner, group, @path) end

  # See <tt>File.fnmatch</tt>.  Return +true+ if the receiver matches the given
  # pattern.
  def fnmatch(pattern, *args) File.fnmatch(pattern, @path, *args) end

  # See <tt>File.fnmatch?</tt> (same as #fnmatch).
  def fnmatch?(pattern, *args) File.fnmatch?(pattern, @path, *args) end

  # See <tt>File.ftype</tt>.  Returns "type" of file ("file", "directory",
  # etc).
  def ftype() File.ftype(@path) end

  # See <tt>File.link</tt>.  Creates a hard link.
  def make_link(old) File.link(old, @path) end

  # See <tt>File.open</tt>.  Opens the file for reading or writing.
  def open(*args, &block) # :yield: file
    File.open(@path, *args, &block)
  end

  # See <tt>File.readlink</tt>.  Read symbolic link.
  def readlink() self.class.new_local(File.readlink(@path)) end

  # See <tt>File.rename</tt>.  Rename the file.
  def rename(to) File.rename(@path, to) end

  # See <tt>File.stat</tt>.  Returns a <tt>File::Stat</tt> object.
  def stat() File.stat(@path) end

  # See <tt>File.lstat</tt>.
  def lstat() File.lstat(@path) end

  # See <tt>File.symlink</tt>.  Creates a symbolic link.
  def make_symlink(old) File.symlink(old, @path) end

  # See <tt>File.truncate</tt>.  Truncate the file to +length+ bytes.
  def truncate(length) File.truncate(@path, length) end

  # See <tt>File.utime</tt>.  Update the access and modification times.
  def utime(atime, mtime) File.utime(atime, mtime, @path) end

  # See <tt>File.basename</tt>.  Returns the last component of the path.
  def basename(*args) self.class.new_local(File.basename(@path, *args)) end

  # See <tt>File.dirname</tt>.  Returns all but the last component of the path.
  def dirname() self.class.new_local(File.dirname(@path)) end

  # See <tt>File.extname</tt>.  Returns the file's extension.
  def extname() File.extname(@path) end

  # See <tt>File.expand_path</tt>.
  def expand_path(*args) self.class.new_local(File.expand_path(@path, *args)) end

  # See <tt>File.split</tt>.  Returns the #dirname and the #basename in an
  # Array.
  def split() File.split(@path).map {|f| self.class.new_local(f) } end  
  
end


class Filename    # * FileTest *

  # See <tt>FileTest.blockdev?</tt>.
  def blockdev?() FileTest.blockdev?(@path) end

  # See <tt>FileTest.chardev?</tt>.
  def chardev?() FileTest.chardev?(@path) end

  # See <tt>FileTest.executable?</tt>.
  def executable?() FileTest.executable?(@path) end

  # See <tt>FileTest.executable_real?</tt>.
  def executable_real?() FileTest.executable_real?(@path) end

  # See <tt>FileTest.exist?</tt>.
  def exist?() FileTest.exist?(@path) end

  # See <tt>FileTest.grpowned?</tt>.
  def grpowned?() FileTest.grpowned?(@path) end

  # See <tt>FileTest.directory?</tt>.
  def directory?() FileTest.directory?(@path) end

  # See <tt>FileTest.file?</tt>.
  def file?() FileTest.file?(@path) end

  # See <tt>FileTest.pipe?</tt>.
  def pipe?() FileTest.pipe?(@path) end

  # See <tt>FileTest.socket?</tt>.
  def socket?() FileTest.socket?(@path) end

  # See <tt>FileTest.owned?</tt>.
  def owned?() FileTest.owned?(@path) end

  # See <tt>FileTest.readable?</tt>.
  def readable?() FileTest.readable?(@path) end

  # See <tt>FileTest.world_readable?</tt>.
  def world_readable?() FileTest.world_readable?(@path) end

  # See <tt>FileTest.readable_real?</tt>.
  def readable_real?() FileTest.readable_real?(@path) end

  # See <tt>FileTest.setuid?</tt>.
  def setuid?() FileTest.setuid?(@path) end

  # See <tt>FileTest.setgid?</tt>.
  def setgid?() FileTest.setgid?(@path) end

  # See <tt>FileTest.size</tt>.
  def size() FileTest.size(@path) end

  # See <tt>FileTest.size?</tt>.
  def size?() FileTest.size?(@path) end

  # See <tt>FileTest.sticky?</tt>.
  def sticky?() FileTest.sticky?(@path) end

  # See <tt>FileTest.symlink?</tt>.
  def symlink?() FileTest.symlink?(@path) end

  # See <tt>FileTest.writable?</tt>.
  def writable?() FileTest.writable?(@path) end

  # See <tt>FileTest.world_writable?</tt>.
  def world_writable?() FileTest.world_writable?(@path) end

  # See <tt>FileTest.writable_real?</tt>.
  def writable_real?() FileTest.writable_real?(@path) end

  # See <tt>FileTest.zero?</tt>.
  def zero?() FileTest.zero?(@path) end
end


class Filename    # * Dir *
  # See <tt>Dir.glob</tt>.  Returns or yields Filename objects.
  def Filename.glob(*args) # :yield: p
    if block_given?
      Dir.glob(*args) {|f| yield self.new_local(f) }
    else
      Dir.glob(*args).map {|f| self.new_local(f) }
    end
  end

  # See <tt>Dir.getwd</tt>.  Returns the current working directory as a Filename.
  def Filename.getwd() self.new_local(Dir.getwd) end
  class << self; alias pwd getwd end

  
  # Return the entries (files and subdirectories) in the directory, each as a
  # Filename object.
  def entries() Dir.entries(@path).map {|f| self.class.new_local(f) } end

  # Iterates over the entries (files and subdirectories) in the directory.  It
  # yields a Filename object for each entry.
  def each_entry(&block) # :yield: p
    Dir.foreach(@path) {|f| yield self.class.new_local(f) }
  end

  # See <tt>Dir.mkdir</tt>.  Create the referenced directory.
  def mkdir(*args) Dir.mkdir(@path, *args) end

  # See <tt>Dir.rmdir</tt>.  Remove the referenced directory.
  def rmdir() Dir.rmdir(@path) end

  # See <tt>Dir.open</tt>.
  def opendir(&block) # :yield: dir
    Dir.open(@path, &block)
  end
end


class Filename    # * Find *
  #
  # Filename#find is an iterator to traverse a directory tree in a depth first
  # manner.  It yields a Filename for each file under "this" directory.
  #
  # Since it is implemented by <tt>find.rb</tt>, <tt>Find.prune</tt> can be used
  # to control the traverse.
  #
  # If +self+ is <tt>.</tt>, yielded Filenames begin with a filename in the
  # current directory, not <tt>./</tt>.
  #
  def find(&block) # :yield: p
    require 'find'
    if @path == '.'
      Find.find(@path) {|f| yield self.class.new_local(f.sub(%r{\A\./}, '')) }
    else
      Find.find(@path) {|f| yield self.class.new_local(f) }
    end
  end
end


class Filename    # * FileUtils *
  # See <tt>FileUtils.mkpath</tt>.  Creates a full path, including any
  # intermediate directories that don't yet exist.
  def mkpath
    require 'fileutils'
    FileUtils.mkpath(@path)
    nil
  end

  # See <tt>FileUtils.rm_r</tt>.  Deletes a directory and all beneath it.
  def rmtree
    # The name "rmtree" is borrowed from File::Path of Perl.
    # File::Path provides "mkpath" and "rmtree".
    require 'fileutils'
    FileUtils.rm_r(@path)
    nil
  end
end


class Filename    # * mixed *
  # Removes a file or directory, using <tt>File.unlink</tt> or
  # <tt>Dir.unlink</tt> as necessary.
  def unlink()
    begin
      Dir.unlink @path
    rescue Errno::ENOTDIR
      File.unlink @path
    end
  end
  alias delete unlink  
  
  # Tries to deterine the home directory in a platform independent way
  # Defaults to the given +default+, which is DIRECTORY_NOW if nothing 
  # better could be found.
  def self.home(default = DIRECTORY_NOW)
    home  = ENV['HOME']  || ENV['HOMEPATH']    
          ## Unix, OS X    Win95
    if home then return self.new_local(home) end
    return self.new_local(default)
  end
  
  # Tries to deterine the directory for temporary files in a platform 
  # independent way. Defaults to the given +default+, which is '/tmp' 
  # if omitted? DIRECTORY_NOW if nothing better could be found.
  def self.temporary(default = '/tmp')
    tmp  = ENV['TMP'] || ENV['TEMP']  || ENV['TMPDIR'] || ENV['TEMPDIR']  
    ## Unix, OS X
    if tmp then return self.new_local(tmp) end
    return self.new_local(default)
  end
  
  #
  # Directory in which the non-writable data of applications is usually 
  # installed on this platform
  #
  def self.share_dir(default = '/usr/share')
    dir = ENV['PROGRAM_FILES'] || default
    # Windows, and unixy default    
    return self.new_local(dir)
  end
  
  #
  # Directory in which the non-writable data of applications is sometimes 
  # installed on this platform
  #
  def self.share_local_dir(default = '/usr/share/local')
    dir = ENV['PROGRAM_FILES'] || default
    # Windows, and unixy default    
    return self.new_local(dir)
  end
  
  # Makes a filename from an environment setting, or returns nil if no such 
  # environment setting was found
  def self.from_env(name)
    f = ENV[name]
    return self.new_local(f) if f
    return nil
  end
  
end


# Some built in manual tests
if __FILE__ == $0
  puts Dir.getwd
  f = Filename.new(__FILE__, '..', '..')
  g = Filename.new(__FILE__, '..', '..')
  d = Filename.new('../share/../share')
  p f == f
  p g == f
  p f.to_s
  p f.exist?
  ff = f.cleanpath
  puts "root"
  p ff.root
  puts "parts"
  p ff.parts
  p ff.to_s
  p ff.exist?
  p d.to_s
  e = d.cleanpath
  p e.to_s
  p e.exist?
  p e.cleanpath.to_s
  p Filename.home.to_s
  p Filename.new(__FILE__).dirname.to_s
  puts Filename.new(__FILE__).dirname.parent.parent.join('share', 'eruta.conf')
  
  
  
end  
  

