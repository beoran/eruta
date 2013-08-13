module Zori
  module Component
   attr_accessor :elements
   attr_accessor :page_id

   def initialize(page_id = :default)
     @page_id  = page_id
     @elements = []
   end
  end # module Component
end
