require 'open3'
  
  
def notify(title, message, status)
  notify = 'notify-send'
  image  = status > 0 ? 'dialog-error' : 'dialog-information'
  message = message.gsub("'","\\'") # escape quotes
  # color green  and red become italic
  message = message.gsub("\e[1;32m", '<i>')
  message = message.gsub("\e[0m", '</i>')
  
  options = "-t 30000 -i #{image} '#{title}' '#{message}'"
  warn "#{title} : #{message}"
  system("#{notify} #{options}") 
end

def run(cmd)
  puts(cmd)
  msg, statobj = Open3.capture2e(cmd)
  status  = statobj.exitstatus
  return msg, status
end



watch('test/.*/(test_.*\.c\Z)') do |md| 
  full    = md[0]  
  base    = md[1].gsub(/\.c\Z/, '')
  msg, stat  = run("cmake .")
  p msg, stat
  msg, stat  = run("make #{base}")
  if (stat > 0)
    notify("Parse Error", msg, stat) 
  else
    msg, stat  = run("ctest -V -R #{base}")
    notify("Test Results", msg, stat)
  end
end


