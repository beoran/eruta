  rem ' Musinum
  rem ' Implementation of the music in numbers algorythm of Lars Kindermann 
  rem ' for ATARI 2600
  rem ' This basic file must be pulled through the C preprocessor to compile.
  rem ' Note definitions and length 
  rem ' Use it under the zlib license.
  #define music_volume  4
  #define music_off  	0
  #define music_voice 	4
  #define music_voice_2	12
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
  
  
  #define note_c3s 31
  #define note_d3s 28
  #define note_f3s 23
  #define note_g3s 20
  #define note_a3s 18
  #define note_c4s 15
  #define note_d4s 13
  #define note_f4s 11
  #define note_rest	255
  #define note_none	0 
  #define note_short 10
  #define note_long 15
  

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
  #define music_instrument 4
  #define music_notes		6
  rem ' these two below will determine tthe tune that is generated
  #define musinum_skip  2
  #define musinum_start 101
  #define musinum_silence 240
  rem 'Timer and pointer for music and sound
  dim music_counter=m
  dim music_pointer=o
  dim music_timer=n
  dim sound_timer=p
  dim music_last=z
  
musinum_reset 
  rem ' Volume down and set music channels.
  AUDC0=music_instrument
  AUDV0=0
  AUDC0=0
  COLUBK = 0
  COLUPF = 255
  rem ' Initialize music timer
  music_counter= 0
  music_pointer= 0
  music_timer  = 1
  music_last   = 0
  
  playfield:
  ...............X................
  ...............XXXXXX...........  
  ...............XX..XXXXX........
  ...............XX.....XXXX......
  ...............XX...............
  ...............XX...............
  ...............XX...............
  ...........XXXXXX...............
  ..........XXXXXXX...............
  .........XXXXXXXX...............
  ..........XXXXXX................
  ...........XXXX.................
  ................................
end


musinum_start
  if switchreset then goto musinum_reset  
  rem ' gosub musinum_music
  rem ' Draw screen and reset
  gosub music_play
  drawscreen
  goto musinum_start
  
  
  
  #define c4  30
  #define d4   27  
  #define e4   24
  #define f4   23
  #define g4   20
  #define a4   18
  #define b4   16
  #define c5   15
  #define rn   note_rest
  #define n0   120
  #define n1   60
  #define n2   30
  #define n3   20
  #define n4   15
  #define n6   10
  #define n8    8
  
  #define music_size_1 48
  
  data music_notes_1
  c5, n2, a4, n2, f4, n2, d4, n2
  c5, n2, a4, n2, f4, n2, d4, n2  
  c4, n2, f4, n2, g4, n2, d4, n2
  c4, n2, f4, n2, g4, n2, d4, n2
  a4, n0, f4, n0, d4, n0, rn, n0
  f4, n0, a4, n0, c5, n0, rn, n0
  
end

  #define music_size_2 64
  data music_notes_2
end
  
  

music_play
  rem ' Update music timer and change note if needed
  rem ' If we get here, the timer is not 0 yet. 
  rem ' Go on to the next note, and leave it at that   
  if music_timer > 0 then goto music_no_note_change
  gosub music_change_note
music_no_note_change
  music_timer 	 = music_timer - 1
  rem COLUPF         = rand
  return
  
music_change_note
  temp1 = music_notes_1[music_pointer]
  AUDF0 = temp1
  AUDF1 = temp1
  COLUPF= temp1
  AUDC0 = music_voice
  AUDC1 = music_voice_2
  if temp1 = note_rest then AUDV0 = 0 else AUDV0 = music_volume
  if temp1 = note_rest then AUDV1 = 0 else AUDV1 = music_volume
  COLUBK=AUDV0
  music_pointer = music_pointer + 1  
  music_timer = music_notes_1[music_pointer]
  music_pointer = music_pointer + 1
  if music_pointer >= music_size_1 then music_pointer = 0
  return
  
  
  
  
  
  
  
  

  data musinum_notes
    note_a3, note_b3, note_c4, note_d4, note_e4, note_f4, note_g4, note_a4
end

  data musinum_notes_penta
    note_c3s, note_d3s, note_f3s, note_g3s, note_a3s,
    note_c4s, note_d4s, note_f4s
end
  
musinum_music
  if music_timer = 0  then goto musinum_music_timer_down_end
  rem ' Update music timer and skip note changing if
  music_timer 	= music_timer - 1
musinum_music_timer_down_end  
  if music_timer > 1  then goto musinum_music_timer_end
  rem 'Change background on note change

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
  temp2 = musinum_notes_penta[temp1] 
  COLUBK = 0
  COLUPF = temp2 * 16
  music_timer = note_long
  AUDC0 = music_instrument
  AUDF0 = temp2
  rem ' randomly insert a rest
  temp3 = rand
  if temp3 < musinum_silence goto musinum_setnote
  AUDV0 = 0
  goto musinum_music_setnote_end
musinum_setnote
  AUDV0 = music_volume
  rem 'On to next note, allowing overflow.  
musinum_music_setnote_end  
  music_counter = music_counter + musinum_skip
musinum_music_counter_end

musinum_music_timer_end
  return


  
  
  
  
  

  
  
  
  

  