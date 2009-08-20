
# This module is obsolete, look in Slacks (or otherwise in Gilir)
module UI

module Text

def self.split_multi(font, text, maxwide) 
	result 		= []
	line   		= ""
	split  		= /[ \n\t\r]/
	list   		= text.split(split)
	linewide    = 0;
	spacewide, spacehigh	= *font.size_text(' ')
	linestart   = true
	wordwide    = 0;
	lastwordwide= 0;
	stop 		= list.size
	list.each do | word |
		wordwide, wordhigh	= *font.size_text(word);
		linewide, linehigh	= *font.size_text(line);
		if wordwide >= maxwide then
		# The word is too wide to ever fit on one line. Put it in the line anyway;
			line += " " unless linestart
			line += word;	
			result.push(line);
			line = ""								
		elsif (wordwide + spacewide + linewide) > maxwide then
		# The word doesn't fit on this line. Move on to the beginning of the next line.
			result.push(line);
			line 		= word;				
			linestart 	= false;
		else 
			line += " " unless linestart
			line += word;	
			linestart = false;
		end
	end
	result.push(line);
	# Do not forget to add the last line.
	return result;
end

def self.split_multi_paragraph(font, text, maxwide) 
	result = []
	paragraphs = text.split(/\n/);
	# Split in paragraphs by newline characters.
	paragraphs.each do | paragraph |
		lines 		= self.split_multi(font, paragraph, maxwide);
		result		+= lines
	end
	return result;
end


class PreparedText
	
def initialize(text, maxwide, font, color ) 
	@font		= font
	@lines 		= UI::Text::split_multi_paragraph(font, text, maxwide);	
	@line_count	= @lines.size
	@wide		= maxwide
	@line_high	= font.line_skip
	@high		= @line_count * @line_high
	@color 		= color
	@prerender  = nil
end
	
def render()
	surface = Sisa::Surface.new( @wide, @high, 32)
	drawx 			= 0;
	drawy			= 0;
	colorkey 		= [0 , 0, 0]
	surface.fill colorkey
	# Draw the text line by line. 	
	for index in (0...@line_count) do
		line	= @lines[index];
		lineimage = nil
		begin
			lineimage = @font.render  line, true, @color # , colorkey
			# Get the image for one line.
		rescue
			# If we don't get a line image, it's probably just an empty line, so we can skip it safely.
		end	
		# If we have an image, draw it! :)
		unless lineimage.nil? 
			lineimage.blit( surface, [drawx, drawy] )
			# Draw the line image to the total text buffer
			lineimage = nil
		end	
	drawy += @line_high;
	end
	# surface.set_colorkey(colorkey)
	return surface.to_display_alpha();
	# make the rendered surface transparent
end
	
def draw(coords, screen)
	@prerender = self.render if @prerender.nil?
	@prerender.blit(coords, screen)	
end


end # Class PreparedText	

def self.prepare_text(text, maxwide, font, color)
	prepared = UI::Text::PreparedText.new(text, maxwide, font, color)
	return prepared 
end 

def self.prepare_and_draw_text(coords, screen, text, maxwide, font, color )
	prepared = self.prepare_text(text, maxwide, font, color)
	prepared.draw(coords, screen)
	return prepared 
end

end # Module Text

end # Module Slacks
