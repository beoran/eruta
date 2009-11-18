class Ocode
  # Implementations for Linux 
  class Linux
    STDOUT      = 1
    STDIN       = 2
    STDERR      = 3
    SYS_EXIT    = 1
    SYS_FORK    = 2
    SYS_READ    = 3
    SYS_WRITE   = 4
    SYS_OPEN    = 5
    SYS_CLOSE   = 6
    SYS_BRK     = 45 

    def sys_call_raw
      asm(:int, 0x80)
    end
     
    # Generic linux syscall with up to 3 arguments 
    def sys_call_arg(fun, *args)
      map   = [ :ebx, :ecx, :edx ]
      stop  = args.size
      stop  = 3 if stop > 3
      for i in (0...stop) do
        dst = map[i]
        src = args[i] 
        asm(:movl, src, dst)
      end 
      sys_call_raw 
    end
    
    def sys_write(fileno, str_address, str_size) 
      sys_call_arg(SYS_WRITE, fileno, str, str_size)
    end
    
    def sys_exit(retval)
      sys_call_arg(SYS_EXIT, retval)
    end
      
    def sys_brk(delta)
      sys_call_arg(SYS_BRK, delta)
    end
    
    def sys_alloc(size)
      sys_brk(0) 
      asm(:addl, size, :eax)
      asm(:movl, :eax, :ebx)
      sys_brk(:ebx)  
    end
  end
end  