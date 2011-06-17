

module Fairy

class Frame 
	def initialize(image)
		@image 		= image
		@rect		= image.make_rect
	end	
	
	
end

class Animation
	def initialize
		@frames 	= []
		@next   	= []
		@current_id = 0
	end
	
	def set_next(idprev, idnext) 
		@next[idprev] = idnext
	end	

	def add(frame)
		@frames.push
		newid = @frames.size - 1
		return newid
	end

	def get()
		return @frames[@current_id]
	end
	
	def update
		if(@current.nil?)
			@current_id = 0
		else
			@current_id = @next[@current_id]
		end
	end
	
	def draw(surface)
		self.get.draw(surface)
	end

end

end



class Tilemap::Entity
	def initialize(x, y, image)
		@x = x
		@y = y
		@image = image	
		@screen_x = x
		@screen_y = y

	end
	
	def draw(screen)
		@image.blit(screen, [@screen_x, @screen_y])
	end
		
	
end
