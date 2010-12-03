

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
		previd = newid - 1
		previd = 0 if previd < 0
		set_next(previd, newid);
		set_next(newid , 0);	
		# Default to a cyclical animation.
		return newid
	end

	def get()
		return @frames[@current_id]
	end
	
	def update
		if(@current_id.nil?)
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
