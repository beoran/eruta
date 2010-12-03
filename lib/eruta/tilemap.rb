#
# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.
#
#
#
#

require 'rubygame'
require 'rexml/document'
require 'rogaru/image'

module Tilemap

class Tile
	
	def self.set_tile_directory(tiledir)
		@@tiledir = tiledir		
	end

	def self.get_tile_directory()
		return @@tiledir		
	end
	
	attr_accessor :walk	
	attr_accessor :swim	
	attr_accessor :jump
	attr_accessor :ledge
	attr_accessor :swim
	attr_accessor :pain
	attr_accessor :up
	attr_accessor :down
	
	def initialize() 
		@images  = []
		@sources = []
		@types   = []
		@max 	 = 0
		@now 	 = 0
		@index   = nil
		# Below are the tile's effects.
		@walk 	 = true		# The tile can be walked over.
		@jump	 = true		# The tile can be jumped over.
		@ledge   = false    # The tile will cause a jump when walked over.
		@swim	 = false	# The tile is some kind of swimmable liquid.
		@pain	 = false	# Walking over the tile causes pain.
		@up	 	 = false    # The tile is a step-up to the next layer.
		@down	 = false    # The tile is a step-down to the previous layer.
		@direction = nil    # The direction of this tile's effect.
	end
	
	def add(surface, source, type)
		@images.push surface
		@sources.push source
		@types.push type
		@max += 1
		return @max - 1
	end
	
	def get_phase(phase) 
		return @images[phase]
	end	
	
	def get()
		return @images[@now]
	end
	
	def advance()
		@now += 1
		@now = 0 if @now >= @max
	end

	def self.xml_get_yesno_property(xml, attribute, default)
		xml_prop  = xml.attributes[attribute]
		if xml_prop.nil? or xml_prop.empty?
			value = default
		elsif xml_prop == 'no'
			value = false
		elsif xml_prop == 'yes'	
			value = true
		else 
			value = default
		end
		return value
	end

	def self.xml_set_yesno_property(value)
		return 'yes' if value
		return 'no'
	end

	def to_xml() 
		xml_tile   = REXML::Element.new 'tile';
		xml_tile.attributes['walk'] 	= Tilemap::Tile.xml_set_yesno_property(self.walk)
		xml_tile.attributes['swim'] 	= Tilemap::Tile.xml_set_yesno_property(self.swim)
		xml_tile.attributes['jump'] 	= Tilemap::Tile.xml_set_yesno_property(self.jump)
		xml_tile.attributes['ledge'] 	= Tilemap::Tile.xml_set_yesno_property(self.ledge)
		xml_tile.attributes['up'] 		= Tilemap::Tile.xml_set_yesno_property(self.up)
		xml_tile.attributes['down'] 	= Tilemap::Tile.xml_set_yesno_property(self.down)
		xml_tile.attributes['pain'] 	= Tilemap::Tile.xml_set_yesno_property(self.pain)
		xml_images = xml_tile.add_element 'images'
		for i in (0...@max)
			xml_image = xml_images.add_element('image')
			xml_image.add_attribute('index', i.to_s)
			xml_image.add_attribute('type' , @types[i].to_s)
			xml_image.add_attribute('src'  , @sources[i].to_s)
		end
		return xml_tile
	end	

	
	def self.new_from_xml(xml_tile)
		tile_dir = Tilemap::Tile.get_tile_directory
		tile = Tilemap::Tile.new()
		tile.walk  = self.xml_get_yesno_property(xml_tile, 'walk' , true )
		tile.swim  = self.xml_get_yesno_property(xml_tile, 'swim' , false)
		tile.jump  = self.xml_get_yesno_property(xml_tile, 'jump' , true )
		tile.ledge = self.xml_get_yesno_property(xml_tile, 'ledge', false)
		tile.up    = self.xml_get_yesno_property(xml_tile, 'up'   , false)
		tile.down  = self.xml_get_yesno_property(xml_tile, 'down' , false)
		tile.pain  = self.xml_get_yesno_property(xml_tile, 'pain' , false)
		xml_tile.elements.each('images/image') do | xml_image |
			imindex = xml_image.attributes['index']
			imtype  = xml_image.attributes['type']
			imsrc   = xml_image.attributes['src']
			case imtype
			when 'solid' 
				image = Image.load_solid(tile_dir + imsrc)
			when 'colorkey'
				image = Image.load_colorkey(tile_dir + imsrc)	
			when 'alpha'
				image = Image.load_alpha(tile_dir + imsrc)	
			else 	
				image = Image.load_solid(tile_dir + imsrc)
			end	
			tile.add(image, imsrc, imtype)
		end
		
		return tile
	end
	
	def animated?()
		return (@images.size > 1) 
	end
	
end



class Layer
	def initialize(map, wide, high, tilehigh= 40, tilewide = 40 ) 
		@map   = map
		@layer = []
		@layer_save = []
		@high = high
		@wide = wide
		for index in (0...@high)
			@layer[index] = []
			@layer_save[index] = []
			for jdex in (0...@wide)
				@layer[index].push(nil)
				@layer_save[index].push(-1)
			end	
		end
		@tilehigh = tilehigh
		@tilewide = tilewide	
		@realhigh = @tilehigh * @high
		@realwide = @tilewide * @wide
		@x = 0
		@y = 0
		@vx = 0
		@vy = 0
	end
	
	def set_tiles(tilearray) 
		@tiles = tilearray
	end

	def add_tile(tile) 
		return @map.add_tile(tile)
	end

	def set(x, y, tile_id)
		raise "Index #{x} #{y} #{tile_id} not valid." if (x < 0) or (y<0) or ( x >= @wide) or (y >= @high)
		if tile_id.nil? or tile_id < 0 then 
			real_tile_id 	= -1
			real_tile 		= nil
		else
			real_tile 		= @map.get_tile(tile_id)
			real_tile_id 	= tile_id
		end
		@layer[y][x] 		= real_tile 
		@layer_save[y][x]  	= real_tile_id
	end
	
	def get(x, y) 
		gx = x; gy = y
		gy = @high if gy > @high
		gx = @wide if gx > @wide
		gy = 0 if gy < 0 
		gx = 0 if gx < 0
		return @layer_save[gy][gx]
	end

	def draw(screen, x, y)
		txstart     = ( x / @tilewide )
		tystart     = ( y / @tilehigh )
		xtilestop   = (screen.w / @tilewide) + 1
		ytilestop   = (screen.h / @tilehigh) + 1
		txstop      = xtilestop + txstart
		tystop      = ytilestop + tystart
		drawx       = 0
		drawy       = 0
		row         = nil
		aidimage    = nil
		nilcount    = 0
		return if (txstart >= @wide or tystart >= @high) 
		txstart = 0    if (txstart < 0) 
		tystart = 0    if (tystart < 0)
		txstop  = @wide if (txstop > @wide) 
		tystop  = @high if (tystop > @high) 
		drawy   = -y + ( (tystart-1) * @tilehigh )
		for tydex in (tystart...tystop) do 
			drawy   += @tilehigh;
			drawx   = -x + ( (txstart-1) * @tilewide )
			row     = @layer[tydex]
			for txdex in (txstart...txstop) do
				drawx   += @tilewide				
				aidtile  = row[txdex]
				unless aidtile.nil? then
					aidimage    = aidtile.get
					aidimage.blit(screen, [drawx, drawy] )
				end
			end
        end
	end

	def draw_effects(screen, x, y)
		txstart     = ( x / @tilewide )
		tystart     = ( y / @tilehigh )
		xtilestop   = (screen.w / @tilewide) + 1
		ytilestop   = (screen.h / @tilehigh) + 1
		txstop      = xtilestop + txstart
		tystop      = ytilestop + tystart
		drawx       = 0
		drawy       = 0
		row         = nil
		aidimage    = nil
		nilcount    = 0
		return if (txstart >= @wide or tystart >= @high) 
		txstart = 0    if (txstart < 0) 
		tystart = 0    if (tystart < 0)
		txstop  = @wide if (txstop > @wide) 
		tystop  = @high if (tystop > @high) 
		drawy   = -y + ( (tystart-1) * @tilehigh )
		for tydex in (tystart...tystop) do 
			drawy   += @tilehigh;
			drawx   = -x + ( (txstart-1) * @tilewide )
			row     = @layer[tydex]
			for txdex in (txstart...txstop) do
				drawx   += @tilewide				
				aidtile  = row[txdex]
				unless aidtile.nil? then
					screen.draw_box_s([drawx, drawy], [drawx+2, drawy+2], [10,10,10]) unless aidtile.walk
					screen.draw_box_s([drawx+4, drawy], [drawx+6, drawy+2], [0,0,255]) if aidtile.swim
					screen.draw_box_s([drawx, drawy+4], [drawx+2, drawy+6], [255,0,0]) if aidtile.pain
					screen.draw_box_s([drawx+8, drawy], [drawx+10, drawy+2], [0,255,255]) unless aidtile.jump
					screen.draw_box_s([drawx+12, drawy], [drawx+14, drawy+2], [255,255,0]) if aidtile.ledge
					screen.draw_box_s([drawx+16, drawy], [drawx+18, drawy+2], [255,255,255]) if aidtile.up
					screen.draw_box_s([drawx+16, drawy+16], [drawx+18, drawy+18], [255,255,255]) if aidtile.down
				end
			end
        end
	end

	def w
		return @realwide
	end

	def h
		return @realhigh
	end
	
	def tw
		return @wide
	end

	def th
		return @high
	end

	def to_xml() 
		layer = REXML::Element.new 'layer'
		sizehash = {'wide' => @wide.to_s, 
					'high' => @high.to_s,
					'tilewide' => @tilewide.to_s, 
					'tilehigh' => @tilehigh.to_s }
					
		layer.add_attributes(sizehash)
		rows = layer.add_element 'rows'
		for y in (0...@high) do
			row = rows.add_element('row')
			row.add_attribute('y', y.to_s) 
			row.text = @layer_save[y].join(' ');			
		end
		return layer
	end
	
	def self.new_from_xml(xml_layer, map)
		mw    = xml_layer.attributes['wide'].to_i
		mh    = xml_layer.attributes['high'].to_i
		tw    = xml_layer.attributes['tilewide'].to_i
		th 	  = xml_layer.attributes['tilehigh'].to_i
		layer = Tilemap::Layer.new(map, mw, mh, tw, th)
		xml_layer.elements.each('rows/row') do | xml_row |
			y   = xml_row.attributes['y'].to_i
			row = xml_row.text.tr("\n\t\r,;", "     ").strip.split(" ").map { |v| v.to_i  }
			# parse the row, translate all seps to spaces, strip the edges, and split on space
			# and finally transfor into an array of integers.
			stop = row.size
			for x in (0...stop) do
				layer.set(x, y, row[x])
			end
		end
		return layer
	end
		
	def screen_to_tile_x(top_x, screen_x)
		return ((top_x + screen_x) / @tilewide).floor
	end 
	
	def screen_to_tile_y(top_y, screen_y)
		return ((top_y + screen_y) / @tilehigh).floor
	end 

	def tile_to_screen_x(top_x, tile_x)
		return   (tile_x * @tilewide) - top_x
	end 
	
	def tile_to_screen_y(top_y, tile_y)
		return   (tile_y * @tilehigh) - top_y
	end 

	

end

class Map
	def initialize()
		@layer_count = 0
		@layers = []
		@tiles  = [] 
		# These tiles will be shared between the map and all it's layers. 
		# Tiles with negative index will be skipped.  
		@moving_tiles  = [ ] 
		@x 	= 0
		@y 	= 0
		@vx = 0
		@vy = 0
		@maxwide 		= 0
		@maxhigh 		= 0
		@maxtilehigh 	= 0
		@maxtilewide 	= 0
	end	
	
	def update(screen_wide, screen_high)
		@x += vx
		@y += vy
		@x = 0 if @x < 0
		@y = 0 if @y < 0
		limit_x = @maxwide - screen_wide
		limit_y = @maxhigh - screen_high
		@y = limit_y if @y > limit_y
		@x = limit_x if @x > limit_x
	end
	
	def advance
		@moving_tiles.each { | tile | tile.advance }
	end

	def add_tile(tile) 
		@tiles.push tile
		@moving_tiles.push tile if(tile.animated?)
		return @tiles.size - 1
	end
	
	def get_tile(index) 
		return @tiles[index]
	end

	def add_layer(layer)
		layer.set_tiles @tiles
		@layers.push layer
		@layer_count += 1 
		if @maxwide < layer.w then
			@maxwide 		= layer.w 
			@maxtilewide 	= layer.tw
		end
		if @maxhigh < layer.h then
			@maxhigh 		= layer.h 
			@maxtilehigh 	= layer.th
		end
	end
	
	def new_layer(wide, high, tilewide = 40, tilehigh = 40)
		layer = Tilemap::Layer.new(self, wide, high, tilewide, tilehigh)
		self.add_layer(layer)
	end
	
	def draw_parallax_auto(screen)
		self.draw_parallax(screen, @x, @y)
	end
	
	def draw_auto(screen)
		self.draw(screen, @x, @y)
	end
	
	attr_accessor :vx
	attr_accessor :vy
	attr_reader :x
	attr_reader :y

	def tw()
		return @maxtilewide
	end

	def th()
		return @maxtilehigh
	end
	
	def draw_parallax(screen, x, y)
		top = @layers.last
		top_w = top.w
		top_h = top.h
		@layers.each do
			|layer|	
			x_scale = (layer.w - screen.w).to_f  / top_w
			y_scale = (layer.h - screen.h).to_f  / top_h
			x_draw  = (x * x_scale).to_i
			y_draw  = (y * y_scale).to_i
			layer.draw(screen, x_draw, y_draw)
		end
	end
	
	def draw(screen, x, y)
		@layers.each do
			|layer|	
			layer.draw(screen, x, y)
		end
	end


	def draw_with_effects(screen, x, y)
		@layers.each do
			|layer|	
			layer.draw(screen, x, y)
			layer.draw_effects(screen, x, y)
		end
	end
	
	def draw_auto_with_effects(screen)
		draw_with_effects(screen, @x, @y)
	end
	
	def to_xml()
		map   = REXML::Element.new 'map'
		tiles = map.add_element 'tiles'
		for i in (0...@tiles.size)
			tile = @tiles[i].to_xml()
			tile.add_attribute('index', i.to_s)
			tiles.add_element(tile)
		end
		layers = map.add_element 'layers'
		for i in (0...@layers.size)
			layer = @layers[i].to_xml()
			layer.add_attribute('z', i.to_s)
			layers.add_element(layer)
		end
		return map
	end
	
	def self.new_from_xml(xml)
		map = Tilemap::Map.new()
		xml.elements.each('*/tiles/tile') do | xml_tile | 
			tile = Tilemap::Tile.new_from_xml(xml_tile)
			map.add_tile(tile)
		end
		xml.elements.each('*/layers/layer') do | xml_layer |
			layer = Tilemap::Layer.new_from_xml(xml_layer, map)
			map.add_layer(layer)
		end
		return map
	end
	
	def self.load_from(filename)
		doc = REXML::Document.new File.new(filename)
		return self.new_from_xml(doc)
	end
	
	def save_to(filename) 
		xml = self.to_xml 
		# Do conversion first, so the map is not clobbered in case it bombs.
		File.open(filename, 'w') do |file|
                  formatter = REXML::Formatters::Pretty.new( 1, false )
                  formatter.write xml, file
		end
	end
	
	def screen_to_tile_x(layer_index, screen_x)
		@layers[layer_index].screen_to_tile_x(@x, screen_x)
	end 
	
	def screen_to_tile_y(layer_index, screen_y)
		@layers[layer_index].screen_to_tile_y(@y, screen_y)
	end 

	def tile_to_screen_x(layer_index, tile_x)
		@layers[layer_index].tile_to_screen_x(@x, tile_x)
	end 
	
	def tile_to_screen_y(layer_index, tile_y)
		@layers[layer_index].tile_to_screen_y(@y, tile_y)
	end 
	
	def tile_count
		return @tiles.size
	end
	
	def layer_count
		return @layers.size
	end

	def set(x, y , z, tile_id) 
		layer = @layers[z]
		return nil if layer.nil?
		layer.set(x, y, tile_id)
	end
	
	def get(x, y , z) 
		layer = @layers[z]
		return nil if layer.nil?
		layer.get(x, y)
	end
	
end


end	