










module C
end

class C::Type
  def initialize(c)
    @c = c
  end
  
  def to_c
    return @c
  end
  
  def *(other)
  end
end

class C::Variable
  def initialize(c)
    @c = c
  end
  
  def to_c
    return @c
  end
  
end


class C::Expression
  def initialize(args=[]) 
  end
end


module C
    
  
end




class Test
  
  def declare(ctext, var)
    return ctext + var.to_c + ";\n" if var.respond_to? :to_c
    return ctext + var.to_s + ";\n"
  end
  
  def initialize()
    @c = ""
  end
  
  def int(var)
    @c += declare("int ", var)
  end

  def char_ptr_ptr(var)
    @c += declare("char ** ", var)
  end
  
  def to_c
    return @c
  end

  def main(argc, argv)
    int argc
    char_ptr_ptr argv
  end
  
end

t = Test.new
t.main('argc', 'argv')
puts t.to_c


