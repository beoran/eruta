require 'open3'
  
  
def notify(title, message, status)
  notify = 'notify-send'
  image  = status > 0 ? 'dialog-error' : 'dialog-information'
  nmsg   = message.gsub("'","\\'") # escape quotes
  # color green  and red become italic
  nmsg   = nmsg.gsub(/\e\[1;[0-9]+m/, '<i>')
  nmsg   = nmsg.gsub("\e[0m", '</i>')
  
  options = "-t 30000 -i #{image} '#{title}' '#{nmsg}'"
  warn "#{title} : #{message}"
  system("#{notify} #{options}") 
end

def run(cmd)
  puts(cmd)
  msg, statobj = Open3.capture2e(cmd)
  status  = statobj.exitstatus
  return msg, status
end


def make_and_run_test(base) 
  msg, stat  = run("cmake .")
  puts msg
  msg, stat  = run("make #{base}")
  if (stat > 0)
    notify("Parse Error", msg, stat) 
  else
    puts msg
    msg, stat  = run("ctest -V -R #{base}")
    notify("Test Results", msg, stat)
  end
end

watch('test/.*/(test_.*\.c\Z)') do |md| 
  full    = md[0]  
  base    = md[1].gsub(/\.c\Z/, '')
  make_and_run_test(base)
end

watch('include/(.*)/(.*\.h\Z)') do |md|
  full    = md[0]
  dir     = md[1]
  base    = md[2].gsub(/\.h\Z/, '')
  testname= "test_" + base.gsub("#{dir}_",'')
  testc   = File.join('test', dir, testname + ".c")
  if File.exist?(testc)
    make_and_run_test(testname)
  end
end  

watch('src/(.*)/(.*\.c\Z)') do |md|
  full    = md[0]
  dir     = md[1]
  base    = md[2].gsub(/\.c\Z/, '')
  testname= "test_" + base.gsub("#{dir}_",'')
  testc   = File.join('test', dir, testname + ".c")
  if File.exist?(testc)
    make_and_run_test(testname)
  else 
    warn("No test for #{full}.")
  end
end
 
def build_eruta
  msg, stat  = run("cmake .")
  puts msg
  msg, stat  = run("make eruta")
  if (stat > 0)
    notify("Parse Error", msg, stat) 
  else
    puts msg
    notify("Eruta Build OK", msg, stat)
  end
end

watch('src/eruta/(.*\.c\Z)') do
  build_eruta
end

watch('include/eruta/(.*\.h\Z)') do
  build_eruta
end

