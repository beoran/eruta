game
.
 ; 

.
 ; 

.L00 ;  rem ' Musinum

.L01 ;  rem ' Implementation of the music in numbers algorythm of Lars Kindermann 

.L02 ;  rem ' for ATARI 2600

.L03 ;  rem ' This basic file must be pulled through the C preprocessor to compile.

.L04 ;  rem ' Note definitions and length 

.L05 ;  rem ' Use it under the zlib license.

.
 ; 

.
 ; 

.
 ; 

.L06 ;  rem ' The notes below are all for voice 12 

.L07 ;  rem 'Timer ticks per second, 60 for NTSC

.L08 ;  rem ' Music wave form to use.

.L09 ;  rem ' these two below will determine tthe tune that is generated

.L010 ;  rem 'Timer and pointer for music and sound

.L011 ;  dim music_counter = m

.L012 ;  dim music_timer = n

.L013 ;  dim sound_timer = p

.L014 ;  dim music_last = z

.musinum_reset
 ; musinum_reset

.L015 ;  rem ' Volume down and set music channels.

.L016 ;  AUDC0 = 4

	LDA #4
	STA AUDC0
.L017 ;  AUDV0 = 0

	LDA #0
	STA AUDV0
.L018 ;  AUDC0 = 0

	LDA #0
	STA AUDC0
.L019 ;  rem ' Initialize music timer

.L020 ;  music_counter =  101

	LDA #101
	STA music_counter
.L021 ;  music_timer  =  0

	LDA #0
	STA music_timer
.L022 ;  music_last  =  0

	LDA #0
	STA music_last
.L023 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel0
PF_data0
	.byte %00000000, %10000000, %00000000, %00000000
	.byte %00000000, %10000000, %11111000, %00000000
	.byte %00000000, %10000000, %10011111, %00000000
	.byte %00000000, %10000000, %10000011, %00000011
	.byte %00000000, %10000000, %10000000, %00000000
	.byte %00000000, %10000000, %10000000, %00000000
	.byte %00000000, %10000000, %10000000, %00000000
	.byte %00000000, %11111000, %10000000, %00000000
	.byte %00000000, %11111100, %10000000, %00000000
	.byte %00000000, %11111110, %10000000, %00000000
	.byte %00000000, %11111100, %00000000, %00000000
	.byte %00000000, %01111000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
pflabel0
	lda PF_data0,x
	sta playfield,x
	dex
	bpl pflabel0
.101
 ; 101

.L024 ;  if switchreset then goto musinum_reset

 lda #1
 bit SWCHB
	BNE .skipL024
.condpart0
 jmp .musinum_reset

.skipL024
.L025 ;  gosub musinum_music

 jsr .musinum_music

.L026 ;  rem ' Draw screen and reset

.L027 ;  drawscreen

 jsr drawscreen
.L028 ;  goto 101

 jmp .101

.L029 ;  data musinum_notes

	JMP .skipL029
musinum_notes
	.byte     23, 20, 19, 17, 15, 14, 12, 11

.skipL029
.L030 ;  data musinum_notes_penta

	JMP .skipL030
musinum_notes_penta
	.byte     31, 28, 23, 20, 18 

	.byte     15, 13, 11

.skipL030
.musinum_music
 ; musinum_music

.L031 ;  if music_timer  =  0 then goto musinum_music_timer_down_end

	LDA music_timer
	CMP #0
     BNE .skipL031
.condpart1
 jmp .musinum_music_timer_down_end

.skipL031
.L032 ;  rem ' Update music timer and skip note changing if

.L033 ;  music_timer  =  music_timer  -  1

	DEC music_timer
.musinum_music_timer_down_end
 ; musinum_music_timer_down_end

.L034 ;  if music_timer  >  1 then goto musinum_music_timer_end

	LDA #1
	CMP music_timer
     BCS .skipL034
.condpart2
 jmp .musinum_music_timer_end

.skipL034
.L035 ;  rem 'Change background on note change

.L036 ;  rem ' Sum of the bits

.L037 ;  temp1  =  0

	LDA #0
	STA temp1
.L038 ;  if music_counter{0} then temp1  =  temp1  +  1

	LDA music_counter
	LSR
	BCC .skipL038
.condpart3
	INC temp1
.skipL038
.L039 ;  if music_counter{1} then temp1  =  temp1  +  1

	LDA music_counter
	AND #2
	BEQ .skipL039
.condpart4
	INC temp1
.skipL039
.L040 ;  if music_counter{2} then temp1  =  temp1  +  1

	LDA music_counter
	AND #4
	BEQ .skipL040
.condpart5
	INC temp1
.skipL040
.L041 ;  if music_counter{3} then temp1  =  temp1  +  1

	LDA music_counter
	AND #8
	BEQ .skipL041
.condpart6
	INC temp1
.skipL041
.L042 ;  if music_counter{4} then temp1  =  temp1  +  1

	LDA music_counter
	AND #16
	BEQ .skipL042
.condpart7
	INC temp1
.skipL042
.L043 ;  if music_counter{5} then temp1  =  temp1  +  1

	LDA music_counter
	AND #32
	BEQ .skipL043
.condpart8
	INC temp1
.skipL043
.L044 ;  if music_counter{6} then temp1  =  temp1  +  1

	BIT music_counter
	BVC .skipL044
.condpart9
	INC temp1
.skipL044
.L045 ;  if music_counter{7} then temp1  =  temp1  +  1

	BIT music_counter
	BPL .skipL045
.condpart10
	INC temp1
.skipL045
.L046 ;  rem 'set matching note. Sum can never be more than 8

.L047 ;  temp2  =  musinum_notes_penta[temp1]

	LDX temp1
	LDA musinum_notes_penta,x
	STA temp2
.L048 ;  COLUBK  =  0

	LDA #0
	STA COLUBK
.L049 ;  COLUPF  =  temp2  *  16

	LDA temp2
	asl
	asl
	asl
	asl
	STA COLUPF
.L050 ;  music_timer  =  15

	LDA #15
	STA music_timer
.L051 ;  AUDC0  =  4

	LDA #4
	STA AUDC0
.L052 ;  AUDF0  =  temp2

	LDA temp2
	STA AUDF0
.L053 ;  rem ' randomly insert a rest

.L054 ;  temp3  =  rand

 jsr randomize
	STA temp3
.L055 ;  if temp3  <  240 goto musinum_setnote

	LDA temp3
	CMP #240
	bcc .musinum_setnote
.L056 ;  AUDV0  =  0

	LDA #0
	STA AUDV0
.L057 ;  goto musinum_music_setnote_end

 jmp .musinum_music_setnote_end

.musinum_setnote
 ; musinum_setnote

.L058 ;  AUDV0  =  4

	LDA #4
	STA AUDV0
.L059 ;  rem 'On to next note, allowing overflow.  

.musinum_music_setnote_end
 ; musinum_music_setnote_end

.L060 ;  music_counter  =  music_counter  +  2

	LDA music_counter
	CLC
	ADC #2
	STA music_counter
.musinum_music_counter_end
 ; musinum_music_counter_end

.musinum_music_timer_end
 ; musinum_music_timer_end

.L061 ;  return

	RTS
       echo "    ",[(scoretable - *)]d , "bytes of ROM space left")
 
 
 
