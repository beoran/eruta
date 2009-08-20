; feel free to modify the score graphics - just keep each digit 8 high
; and keep the conditional compilation stuff intact
 ifconst ROM2k
   ORG $F7AC
 else
   ifconst bankswitch
     if bankswitch == 8
       ORG $2F94-bscode_length
       RORG $FF94-bscode_length
     endif
     if bankswitch == 16
       ORG $4F94-bscode_length
       RORG $FF94-bscode_length
     endif
     if bankswitch == 32
       ORG $8F94-bscode_length
       RORG $FF94-bscode_length
     endif
   else
     ORG $FF9C
   endif
 endif


scoretable
       .byte %00000000
       .byte %00000000
       .byte %00000000
       .byte %00000000
       .byte %00000000
       .byte %00000000
       .byte %00000000
       .byte %00000000

       .byte %00111100
       .byte %01111111
       .byte %00111100
       .byte %00000000
       .byte %00000000
       .byte %00000000
       .byte %00000000
       .byte %00000000

       .byte %00111100
       .byte %01111111
       .byte %00111100
       .byte %00000000
       .byte %00111100
       .byte %01111111
       .byte %00111100
       .byte %00000000

       .byte %00010000
       .byte %00111000
       .byte %00010000
       .byte %01010100
       .byte %11111110
       .byte %01010100
       .byte %00010000
       .byte %00111000
       
       .byte %00010000
       .byte %00111000
       .byte %01010100
       .byte %11010110
       .byte %10111010
       .byte %11010110
       .byte %10010010       
       .byte %11111110
       
       .byte %01111110
       .byte %01011010
       .byte %10100101
       .byte %11011011
       .byte %11111111
       .byte %01111110
       .byte %00011000
       .byte %00000000
       
       .byte %10010000
       .byte %01100000
       .byte %01110000
       .byte %10111000
       .byte %00011100
       .byte %00001110
       .byte %00000111
       .byte %00000011

       .byte %11111110
       .byte %10000010
       .byte %10000010
       .byte %10010010
       .byte %10111010
       .byte %10101010
       .byte %10000010
       .byte %11111110
       
       .byte %11111110
       .byte %10010010
       .byte %10001010
       .byte %11111110
       .byte %10100010
       .byte %10010010
       .byte %10001010
       .byte %11111110

       .byte %00111100
       .byte %01000110
       .byte %00000110
       .byte %00111110
       .byte %01100110
       .byte %01100110
       .byte %01100110
       .byte %00111100

 ifconst ROM2k
   ORG $F7FC
 else
   ifconst bankswitch
     if bankswitch == 8
       ORG $2FF4-bscode_length
       RORG $FFF4-bscode_length
     endif
     if bankswitch == 16
       ORG $4FF4-bscode_length
       RORG $FFF4-bscode_length
     endif
     if bankswitch == 32
       ORG $8FF4-bscode_length
       RORG $FFF4-bscode_length
     endif
   else
     ORG $FFFC
   endif
 endif
