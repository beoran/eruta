  rem ' Musinum
  rem ' Implementation of the mlusic in numbers algorythm for ATARI 2600
  rem ' This basic file must be pulled through the C preprocessor to compile.
  rem ' Note definitions and length 
  #define music_volume  14
  #define music_off  	0
  #define music_voice 	12
  rem ' The notes below are all for voice 12 
  #define note_e3 	31
  #define note_f3 	29
  #define note_g3 	26
  #define note_a3 	23
  #define note_b3 	20
  #define note_c4	19
  #define note_d4	17
  #define note_e4	15
  #define note_f4	14
  #define note_g4	12
  #define note_a4	11  
  #define note_none	0 
  #define note_quarter 	15
  #define note_half	30
  #define note_full	60
  #define note_fullrest 	58
  #define note_halfrest 	28
  #define note_quarterrest	13
  #define note_ndot		2  
  rem 'Timer ticks per second, 60 for NTSC
  #define ticks_per_second 	60
  #define music_instrument 	12
  #define music_notes		69  
  rem ' Music wave form to use.
  #define music_instrument	12
  #define music_notes		69
  #define musinum_skip           1
  rem 'Timer and pointer for music and sound
  dim music_counter=m
  dim music_timer=n
  dim sound_timer=p
  
musinum_reset 
  rem ' Volume down and set music channels.
  AUDC0=music_instrument
  AUDV0=0
  AUDC0=0
  
  rem ' Initialize music timer
  music_counter= 0
  music_timer  = 0
  
musinum_start
  if switchreset then goto musinum_reset  
  gosub musinum_music
  rem ' Draw screen and reset
  playfield:
  XX.............................X
  XX............................XX
  XX............................XX
  XXX.........................XXXX 
  XXX..........................XXX
  XXX.........................XXXX
  XXXX......................XXXXXX
  XXXX.....................XXXXXXX
  XXXXXXX.....XXXXX.......XXXXXXXX
  XXXXXXXXXXXXXXXXXXXX...XXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  drawscreen
  goto musinum_start

  data musinum_notes
    note_a3, note_b3, note_c4, note_d4, note_e4, note_f4, note_g4, note_a4
end


  
musinum_music
  if music_timer < 1  then goto musinum_music_timer_end
  rem ' Update music timer and skip note changing if
  music_timer 	= music_timer - 1
  if music_timer > 1  then goto musinum_music_timer_end
  rem 'Change background on note change
  COLUBK	= rand 
  
  rem ' Sum of the bits
  temp1		= 0
  if music_counter{0} then temp1 = temp1 + 1
  if music_counter{1} then temp1 = temp1 + 1
  if music_counter{2} then temp1 = temp1 + 1
  if music_counter{3} then temp1 = temp1 + 1
  if music_counter{4} then temp1 = temp1 + 1
  if music_counter{5} then temp1 = temp1 + 1
  if music_counter{6} then temp1 = temp1 + 1
  if music_counter{7} then temp1 = temp1 + 1
  rem 'set matching note. Sum can never be more than 8
  music_timer = note_full
  AUDV0       = music_volume
  AUDF0	      = musinum_notes[temp1]
  AUDC0	      = music_instrument	
  rem 'On to next note, allowing overflow.
  if music_counter < 254 then goto musinum_music_counter_inc
  music_counter = 0
  goto musinum_music_counter_end
musinum_music_counter_inc  
  music_counter = music_counter + musinum_skip
musinum_music_counter_end

musinum_music_timer_end
  return


  
  
  
  
  

  
  
  
  

  