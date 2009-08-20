#!/usr/bin/env ruby
# Program to cut up image into tiles

require 'rubygems'
require 'RMagick' 
# We use Rmagick. ruby-gd din't quite cut it.
TILE_H = 32
TILE_W = 32

# Creates one tile that contains a tile cut from the input image at the given row and column.
def cut_one_tile(image, row, col, tile_w, tile_h)
   src_x  = col * tile_w
   src_y  = row * tile_h 
   tile   = image.crop(src_x, src_y, tile_w, tile_h, true)   
   return tile
end

# Save one tile to the png file with the given prefix, automatically adding an index. 
def save_one_tile(tile, prefix, row, col, colcount)
  index     = col + row * colcount
  filename  = "#{prefix}_#{index}.png"
  puts filename
  file      = File.new(filename, 'wb')
  tile.write(file)
  file.close
end

# Cuts up the image in many tiles.
def cut_image(imagename, tile_w, tile_h)
  prefix      = imagename.gsub(".png", '')
  images      = Magick::Image.read(imagename)
  image       = images[0]
  wide        = image.columns
  high        = image.rows
  colcount    = wide / tile_w
  rowcount    = high / tile_h
  for row in (0...rowcount) 
    for col in (0...colcount)
      tile = cut_one_tile(image, row, col, tile_w, tile_h)
      save_one_tile(tile, prefix, row, col, colcount)
      tile    = nil 
    end
  end
  image       = nil
end  

filename = ARGV[0]
unless filename
  puts "tilecut.rb filename"
  exit 1
end

cut_image(filename, TILE_W, TILE_H)
