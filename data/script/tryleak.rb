def on_update
  puts "hello"
end

index = 0
memuser = "*"

while index < 1000000
  on_update
  begin
     @foo = 10 
  rescue Exception => ex
    p ex
  end 
  memuser = memuser + memuser
  if (memuser.length > 1000) 
    memuser = ""
    GC.start
  end
  index += 1
end
  


