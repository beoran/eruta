# #!/usr/local/bin/ruby1.9

require 'joystick'

# make sure a device was specified on the command-line
unless ARGV.size > 0
  $stderr.puts 'Missing device name.'
  exit -1
end

# open the joystick device
Joystick::Device.open(ARGV[0]) { |joy|
  # print out joystick information
  puts "Joystick: #{joy.name}", 
       "Axes / Buttons : #{joy.axes} / #{joy.buttons}",
       ''

  # loop forever
  loop {
    if joy.pending? 
      ev = joy.ev

      case ev.type
      when Joystick::Event::INIT
        puts 'init'
      when Joystick::Event::BUTTON
        puts "button: #{ev.num}, #{ev.val}"
      when Joystick::Event::AXIS
        puts "axis: #{ev.num}, #{ev.val}"
      end
    else
      # do something else
    end  
  }
}
