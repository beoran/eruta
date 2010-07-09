 processor 6502
 include "vcs.h"
 include "macro.h"
 include "2600basic.h"
 include "2600basic_variable_redefs.h"
 ifconst bankswitch
  if bankswitch == 8
     ORG $1000
     RORG $D000
  endif
  if bankswitch == 16
     ORG $1000
     RORG $9000
  endif
  if bankswitch == 32
     ORG $1000
     RORG $1000
  endif
 else
   ORG $F000
 endif
game
.
 ; 

.
 ; 

.L00 ;  rem ' Ature Adventure

.L01 ;  rem ' Copyright beoran@rubyforge.org 2009, 2010.

.L02 ;  rem ' May be distributed under the Zlib License. No warranty, use at your own risk.

.L03 ;  rem ' You will need to modify Batari Basic from sources to compile this program

.L04 ;  rem ' You'll also need to pull it through a C preprocessor like cpp -P

.L05 ;  rem ' Because I'm using much more constants than Batari Basic supports normally.

.L06 ;  rem ' And I use the C preprocessor as a workaround to Batari Basic's

.L07 ;  rem ' bugs with regards to constant definition

.L08 ;  rem ' It's much easier to understand what's going on if I use constants galore.

.L09 ;  rem ' Note: I only use integer math, for speed.

.L010 ;  rem ' To Do: Fix some weirdness in the color handling. 

.L011 ;  rem ' To Do: Fix sound effects and music. 

.L012 ;  rem ' To Do: Make the game bigger. Perhaps add a few more areas?

.L013 ;  rem ' We use playfield colors, 32K and smart branching.

.L014 ;  set kernel_options pfcolors

.L015 ;  set romsize 32k

.L016 ;  set smartbranching on

.L017 ;  rem ' Cycle debugging

.L018 ;  rem ' set debug cycles

.L019 ;  rem ' set debug cyclescore

.L020 ;  rem ' dim mincycles=z

.
 ; 

.L021 ;  rem ' mincycles=255

.
 ; 

.L022 ;  rem 'Constant definitions

.
 ; 

.L023 ;  rem 'const pfres=12

.L024 ;  rem 'const pfrowheight=8

.
 ; 

.L025 ;  rem 'Commonly used NTSC Colors

.L026 ;  const black = 0

.L027 ;  const gray = 6

.L028 ;  const white = 14

.L029 ;  const yellow = 30

.
 ; 

.L030 ;  const brown = 32

.L031 ;  const orange = 32

.L032 ;  const blue = 144

.L033 ;  const skyblue = 174

.L034 ;  const green = 192

.L035 ;  const darkgreen = 191

.L036 ;  const riverblue = 170

.L037 ;  rem ' XXX: there is a bug in the ompiler (or my patches to it) 

.L038 ;  rem ' some of my consants like the one below are translated in stead 

.L039 ;  rem ' like a dim statement: eg  const turfy=218 -> dim   turfy=218

.L040 ;  rem ' a workaround seem to be to reorder the constants a bit

.L041 ;  rem ' or to remove a few of them, but this only bugs out other constants :p

.L042 ;  rem ' so I replaced some constants by their values, and use the cpp 

.L043 ;  rem ' preprocessor in stead.    

.
 ; 

.L044 ;  const seablue  =  144

.L045 ;  const red  =  64

.L046 ;  const pink  =  78

.L047 ;  const skin  =  254

.L048 ;  const sand  =  230

.L049 ;  const turfy  =  218

.L050 ;  rem ' const ochre=16

.
 ; 

.
 ; 

.L051 ;  rem 'Playfield dimensions

.L052 ;  const field_left = 17

.L053 ;  const field_left_enter = 18

.L054 ;  const field_right = $88

.L055 ;  const field_right_enter = $87

.L056 ;  const field_top = 10

.L057 ;  const field_top_enter = 11

.L058 ;  const field_bottom = 90

.L059 ;  const field_bottom_enter = 89

.L060 ;  const field_hcenter = 61

.L061 ;  const field_vcenter = 44

.
 ; 

.L062 ;  rem ' Note definitions and length const music_voice=12

.L063 ;  rem const note_c4=30

.L064 ;  rem const note_d4=27

.L065 ;  rem const note_e4=24

.L066 ;  rem const note_f4=23

.L067 ;  rem const note_g4=20

.L068 ;  rem const note_a4=18

.L069 ;  rem const note_b4=16

.L070 ;  rem const note_c5=15

.L071 ;  rem const note_none=0

.L072 ;  rem const note_full=60

.L073 ;  rem const note_half=30

.L074 ;  rem const note_quarter=15

.L075 ;  rem const note_fullrest=58

.L076 ;  rem const note_halfrest=28

.L077 ;  rem const note_quarterest=13

.L078 ;  rem const note_ndot=2

.
 ; 

.
 ; 

.L079 ;  rem ' An object will be positioned at 255 to hide it

.L080 ;  const nowhere = 255

.
 ; 

.L081 ;  rem 'Timer ticks per second, 60 for NTSC

.L082 ;  const ticks_per_second = 60

.
 ; 

.L083 ;  rem 'Score, split up in individual digits

.L084 ;  dim sc0 = score

.L085 ;  dim sc1 = score + 1

.L086 ;  dim sc2 = score + 2

.
 ; 

.L087 ;  rem 'Initial values

.L088 ;  rem 'Normal start

.L089 ;  const room_start = 49

.L090 ;  rem '49

.L091 ;  rem 'On the east side of the river

.L092 ;  rem 'const room_start=4

.
 ; 

.L093 ;  const hero_start_x = 75

.L094 ;  const hero_start_y = 75

.L095 ;  const item_start_x = 60

.L096 ;  const item_start_y = 40

.L097 ;  const item_start_hp = 1

.L098 ;  const hero_base_hp = 11

.L099 ;  const hero_high = 8

.L0100 ;  const hero_half_high = 4

.L0101 ;  const hero_feet_high = 4

.L0102 ;  const hero_wide = 8

.L0103 ;  const hero_half_wide = 4

.L0104 ;  rem 'Hero's position, speed and room

.L0105 ;  dim hero_x = player0x

.L0106 ;  dim hero_y = player0y

.
 ; 

.L0107 ;  dim hero_oldx = v

.L0108 ;  dim hero_oldy = w

.L0109 ;  rem 'Flags:

.L0110 ;  dim hero_flags = g

.L0111 ;  rem 'hero_flags{0} -> facing north

.L0112 ;  rem 'hero_flags{1} -> facing east

.L0113 ;  rem 'hero_flags{2} -> facing south

.L0114 ;  rem 'hero_flags{3} -> facing west

.L0115 ;  rem 'hero_flags{4} -> set when attacking is allowed, blocked when not

.L0116 ;  dim hero_items = q

.L0117 ;  rem 'hero_items{0} -> Key Leaf 1

.L0118 ;  rem 'hero_items{1} -> Key Leaf 2

.L0119 ;  rem 'hero_items{2} -> Key Leaf 3

.L0120 ;  rem 'hero_items{3} -> Numen Shield

.L0121 ;  rem 'hero_items{4} -> Numen Armor 

.L0122 ;  rem 'hero_items{5} -> Numen Sword

.L0123 ;  rem 'hero_items{6} -> Heal Book

.L0124 ;  rem 'hero_items{7} -> Strike Book

.L0125 ;  dim hero_room = r

.L0126 ;  dim hero_hp = statusbarlength

.L0127 ;  dim hero_mp = o

.L0128 ;  dim hero_next = k

.L0129 ;  dim hero_level = l

.L0130 ;  rem 'Quest flags contain important achievments of Signe.

.L0131 ;  dim quest_flags = h

.L0132 ;  rem 'quest_flags{0} -> Defeated Ikaza

.L0133 ;  rem 'quest_flags{1} -> Flipped Switch 1

.L0134 ;  rem 'quest_flags{2} -> Flipped Switch 2

.L0135 ;  rem 'quest_flags{3} -> Flipped Switch 3

.L0136 ;  rem 'quest_flags{4} -> Flipped Switch 4

.L0137 ;  rem 'quest_flags{5} -> Flipped Switch 5

.L0138 ;  rem 'quest_flags{6} -> Flipped Switch 6

.L0139 ;  rem 'quest_flags{7} -> Flipped Switch 7

.
 ; 

.L0140 ;  rem 'Other flags: Reserved for future extension

.L0141 ;  rem 'quest_flags{1} -> Defeated Armor Boss

.L0142 ;  rem 'quest_flags{2} -> Defeated Sword Boss 

.L0143 ;  rem 'quest_flags{3} -> Defeated 

.L0144 ;  rem 'quest_flags{0} -> Defeated Leaf Boss 1

.L0145 ;  rem 'quest_flags{1} -> Defeated Leaf Boss 2

.L0146 ;  rem 'quest_flags{2} -> Defeated Leaf Boss 3

.L0147 ;  rem 'status bar color in Z, which si otherwise used for debugging

.L0148 ;  rem ' Can't sue changes top platfield row color somehow. :p

.L0149 ;  rem ' dim statusbarcolor=z

.
 ; 

.L0150 ;  const strike_cost = 1

.L0151 ;  const last_monster = 32

.
 ; 

.L0152 ;  const hero_speed = 1

.L0153 ;  const hero_speed_negative = # - 1

.
 ; 

.L0154 ;  rem item/monster position speed and kind

.L0155 ;  dim item_x = player1x

.L0156 ;  dim item_y = player1y

.L0157 ;  dim item_kind = s

.L0158 ;  dim item_hp = i

.L0159 ;  dim item_oldx = t

.L0160 ;  dim item_oldy = u

.L0161 ;  rem 'Active item flags: 

.L0162 ;  dim item_flags = f

.L0163 ;  rem 'item_flags{0} -> Monster missile flies north

.L0164 ;  rem 'item_flags{1} -> Monster missile flies east

.L0165 ;  rem 'item_flags{2} -> Monster missile flies south

.L0166 ;  rem 'item_flags{3} -> Monster missile flies west

.
 ; 

.
 ; 

.L0167 ;  rem 'Game timer. Controls animation. Keeps on ticking, by overflow.

.L0168 ;  dim game_timer = e

.L0169 ;  const timer_loop = 128

.L0170 ;  const timer_second = 60

.
 ; 

.
 ; 

.L0171 ;  rem 'Item and monster kinds. 

.L0172 ;  rem 'Monster kind is divided as follows: the 6 bits record the looks 

.L0173 ;  rem 'and the 'index' for data tables of the monster or item

.L0174 ;  rem 'The high 2 bits are used for 2 flag variables: 

.L0175 ;  rem 'If item_kind{7} is set it's an immobile item, otherwise, a monster

.L0176 ;  rem '6 is yet to be determined

.L0177 ;  const item_kind_mask = %00111111

.L0178 ;  const is_item_mask = %10000000

.L0179 ;  const item_none = %10000000

.L0180 ;  const monster_slime = 1

.L0181 ;  const monster_crab = 2

.L0182 ;  const monster_bat = 3

.L0183 ;  const monster_scorpio = 4

.L0184 ;  rem 'The famous rabid rabbit ^_^

.L0185 ;  const monster_rabid = 5

.L0186 ;  const monster_spider = 6

.L0187 ;  const monster_snake = 7

.L0188 ;  const monster_fish = 8

.L0189 ;  const monster_lion = 9

.L0190 ;  const monster_wolf = 10

.L0191 ;  const monster_grunt = 11

.L0192 ;  const monster_archer = 12

.L0193 ;  const monster_knight = 13

.L0194 ;  const monster_cannon = 14

.L0195 ;  const monster_zombie = 15

.L0196 ;  const monster_skeleton = 16

.L0197 ;  const monster_ghost = 17

.L0198 ;  const monster_mage = 18

.L0199 ;  const monster_flower = 19

.L0200 ;  const monster_treant = 20

.L0201 ;  const monster_mushroom = 21

.L0202 ;  const monster_book = 22

.L0203 ;  const monster_sword = 23

.L0204 ;  rem 'Monster curse blocks the way in rooms. It's invulnerable,

.L0205 ;  rem 'but can be removed by game events.

.L0206 ;  const monster_curse = 24

.L0207 ;  const monster_first_boss = 25

.L0208 ;  const monster_leaf1_boss = 25

.L0209 ;  const monster_leaf2_boss = 26

.L0210 ;  const monster_leaf3_boss = 27

.L0211 ;  const monster_armor_boss = 28

.L0212 ;  rem 'Boss that guards the Numen Sword

.L0213 ;  const monster_sword_boss = 29

.L0214 ;  rem 'Boss that guards the Strike book

.L0215 ;  const monster_strike_boss = 30

.L0216 ;  const monster_ikaza = 31

.
 ; 

.L0217 ;  const item_inventory_mask = 160

.L0218 ;  const item_leaf1 = 160

.L0219 ;  const item_leaf2 = 161

.L0220 ;  const item_leaf3 = 162

.L0221 ;  const item_armor = 163

.L0222 ;  const item_shield = 164

.L0223 ;  const item_sword = 165

.L0224 ;  const item_bookheal = 166

.L0225 ;  const item_bookstrike = 167

.L0226 ;  const item_healhp = 168

.L0227 ;  const item_healmp = 169

.L0228 ;  const item_healallhp = 170

.L0229 ;  const item_healallmp = 171

.L0230 ;  const item_first_switch = 172

.L0231 ;  const item_switch_on = 7

.L0232 ;  const item_switch1 = 172

.L0233 ;  const item_switch2 = 173

.L0234 ;  const item_switch3 = 174

.L0235 ;  const item_switch4 = 175

.L0236 ;  const item_switch5 = 176

.L0237 ;  const item_switch6 = 177

.L0238 ;  const item_switch7 = 178

.L0239 ;  const item_last_switch = 178

.L0240 ;  const item_first_switch_on = 179

.L0241 ;  const item_switch1_on = 179

.L0242 ;  const item_switch2_on = 180

.L0243 ;  const item_switch3_on = 181

.L0244 ;  const item_switch4_on = 182

.L0245 ;  const item_switch5_on = 183

.L0246 ;  const item_switch6_on = 184

.L0247 ;  const item_switch7_on = 185

.L0248 ;  const item_last_switch_on = 182

.L0249 ;  const item_victory = 191

.L0250 ;  const monster_normal = 0

.L0251 ;  rem 'Bit 0 in monster flags

.L0252 ;  const monster_shoots = 1

.L0253 ;  rem 'Bit 1 in monster flags

.L0254 ;  const monster_randomwalk = 2

.L0255 ;  rem 'Bit 2 in monster flags

.L0256 ;  const monster_aggressive = 4

.L0257 ;  rem 'Bit 3 in monster flags

.L0258 ;  const monster_static = 8

.L0259 ;  rem 'Bit 4 in monster flags

.L0260 ;  const monster_alwaysdrop = 16

.L0261 ;  rem 'Bit 5 in monster flags

.L0262 ;  const monster_nowalls = 32

.L0263 ;  rem 'Bit 6 in monster flags

.L0264 ;  const monster_huge = 64

.L0265 ;  rem 'Bit 7 in monster flags. Will triple if huge is also set

.L0266 ;  rem 'Currently, double nor triple doesn't work well with the AI

.L0267 ;  const monster_double = 128

.L0268 ;  const monster_boss_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0269 ;  const monster_boss2_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0270 ;  const monster_boss3_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0271 ;  const monster_swordboss_info = monster_alwaysdrop  +  monster_aggressive  +  monster_nowalls

.L0272 ;  const monster_cannon_info = monster_shoots  +  monster_static  +  monster_nowalls

.
 ; 

.L0273 ;  rem ' Music wave form to use.

.L0274 ;  const music_instrument = 12

.L0275 ;  const music_notes = 69

.
 ; 

.
 ; 

.
 ; 

.L0276 ;  rem 'Timer and pointer for music and sound

.L0277 ;  dim music_pointer = m

.L0278 ;  dim music_timer = n

.L0279 ;  dim music_which = d

.L0280 ;  dim sound_timer = p

.L0281 ;  rem 'Unused variables: a b c

.
 ; 

.L0282 ;  rem 'Set back to 1 when not debugging.

.L0283 ;  const hero_start_level = 20

.L0284 ;  rem 'set back to 0 when not debugging

.L0285 ;  const hero_start_items = 

.
 ; 

.L0286 ;  rem ' Colors to use for the item display 

.L0287 ;  data item_colors

	JMP .skipL0287
item_colors
	.byte   black, green, red, blue, black, green, red, blue 

	.byte   black, green, red, blue, black, green, red, blue 

	.byte   black, green, red, blue, black, green, red, blue 

	.byte   black, black, red, red, black, orange, red, blue 

	.byte   black, green, red, blue, black, green, red, blue 

	.byte   red , green, red, green, black, green, red, blue 

	.byte   white, yellow, orange, black, green, red, blue, white 

	.byte   yellow, orange, blue, black, green, red, blue, blue 

.skipL0287
.
 ; 

.L0288 ;  rem 'Damage modifier for the MONSTER/item. 0 means no damage

.L0289 ;  rem 'There is no monster nr 0, that's for item_none.

.L0290 ;  rem 'The curse deals massive damage to be sure it works.

.L0291 ;  data item_damage

	JMP .skipL0291
item_damage
	.byte    0, 1, 1, 1, 1, 2, 2, 2

	.byte    2, 4, 4, 4, 4, 6, 6, 6

	.byte   10, 16, 16, 1, 16, 4, 16, 16

	.byte  128, 20, 22, 24, 48, 48, 48, 64

	.byte    0, 0, 0, 0, 0, 0, 0, 0

	.byte    0, 0, 0, 0, 0, 0, 0, 0

	.byte    0, 0, 0, 0, 0, 0, 0, 0

	.byte    0, 0, 0, 0, 0, 0, 0, 0

.skipL0291
.
 ; 

.L0292 ;  rem ' 

.L0293 ;  rem ' Monster information, bitflags (monster 0 doesn't exist, but is in this list!)

.L0294 ;  data monster_info

	JMP .skipL0294
monster_info
	.byte   monster_normal, monster_normal, monster_nowalls+monster_shoots, monster_nowalls

	.byte   monster_normal, monster_nowalls, monster_normal, monster_aggressive

	.byte   monster_shoots + monster_nowalls, monster_normal, monster_normal, monster_normal

	.byte   monster_shoots, monster_aggressive, monster_cannon_info, monster_randomwalk

	.byte   monster_normal, monster_nowalls, monster_nowalls + monster_shoots, monster_cannon_info

	.byte   monster_normal, monster_normal, monster_cannon_info, monster_static + monster_nowalls

	.byte   monster_static + monster_huge + monster_nowalls, monster_boss_info, monster_boss_info, monster_boss3_info

	.byte   monster_boss_info, monster_swordboss_info, monster_boss_info, monster_aggressive + monster_shoots

.skipL0294
.
 ; 

.L0295 ;  rem ' Items the monster drops, or for items, what item flag to set

.L0296 ;  data monster_drops

	JMP .skipL0296
monster_drops
	.byte   item_healhp, item_healhp, item_healhp, item_healmp 

	.byte   item_healhp, item_healhp, item_healhp, item_healhp 

	.byte   item_healhp, item_healhp, item_healhp, item_healhp 

	.byte   item_healhp, item_healallhp, item_healhp, monster_skeleton 

	.byte   monster_ghost, item_switch5, monster_book, item_healmp 

	.byte   item_healallmp, item_healhp, item_bookheal, item_sword 

	.byte   item_healhp, item_leaf1, item_leaf2 , item_leaf3 

	.byte   item_armor, monster_sword, item_bookstrike, item_healallhp 

.skipL0296
.
 ; 

.
 ; 

.L0297 ;  rem VOLUME DOWN AND SET CHANNELS

.L0298 ;  AUDV0 = 0

	LDA #0
	STA AUDV0
.L0299 ;  AUDV1 = 0

	LDA #0
	STA AUDV1
.L0300 ;  AUDC0 = music_instrument

	LDA #music_instrument
	STA AUDC0
.
 ; 

.L0301 ;  rem INITIALIZE POINTERS AND TIMERS

.
 ; 

.L0302 ;  rem 'lives control

.L0303 ;  lifecolor = black

	LDA black
	STA lifecolor
.L0304 ;  rem 'Lives are used for the MP

.L0305 ;  lives = 32

	LDA #32
	STA lives
.L0306 ;  lives:

	LDA #<lives__L0306
	STA lifepointer
	LDA lifepointer+1
	AND #$E0
	ORA #(>lives__L0306)&($1F)
	STA lifepointer+1
.
 ; 

.L0307 ;  rem 'Go to intro screen, but not now while we are debugging.

.L0308 ;  gosub intro_screen bank5

 sta temp7
 lda #>(ret_point1-1)
 pha
 lda #<(ret_point1-1)
 pha
 lda #>(.intro_screen-1)
 pha
 lda #<(.intro_screen-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #5
 jmp BS_jsr
ret_point1
.L0309 ;  goto main_loop_start

 jmp .main_loop_start

.
 ; 

.main_loop_start
 ; main_loop_start

.L0310 ;  rem ' Set up initial values.

.L0311 ;  gosub music_restart bank5

 sta temp7
 lda #>(ret_point2-1)
 pha
 lda #<(ret_point2-1)
 pha
 lda #>(.music_restart-1)
 pha
 lda #<(.music_restart-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #5
 jmp BS_jsr
ret_point2
.L0312 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L0313 ;  lifecolor = yellow

	LDA yellow
	STA lifecolor
.L0314 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L0315 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L0316 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L0317 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L0318 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L0319 ;  quest_flags = 0

	LDA #0
	STA quest_flags
.L0320 ;  hero_level = hero_start_level

	LDA #hero_start_level
	STA hero_level
.L0321 ;  hero_next =  ( hero_level  /  2 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	CLC
	ADC #1
	STA hero_next
.L0322 ;  hero_hp = hero_level * 2 + hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0323 ;  rem 'MP is between 1 and 6

.L0324 ;  hero_mp = hero_level / 16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0325 ;  gosub set_mp_display

 jsr .set_mp_display

.L0326 ;  hero_flags  =  0

	LDA #0
	STA hero_flags
.L0327 ;  item_flags  =  0

	LDA #0
	STA item_flags
.L0328 ;  game_timer  =  0

	LDA #0
	STA game_timer
.L0329 ;  item_kind = item_none

	LDA #item_none
	STA item_kind
.L0330 ;  hero_items  =  hero_start_items

	LDA #hero_start_items
	STA hero_items
.L0331 ;  rem 'Player starts facing south, able to attack

.L0332 ;  hero_flags{2}  =  1

	LDA hero_flags
	ORA #4
	STA hero_flags
.L0333 ;  hero_flags{4}  =  1

	LDA hero_flags
	ORA #16
	STA hero_flags
.L0334 ;  rem player0x=75

.L0335 ;  rem player0y=75

.L0336 ;  item_x = item_start_x

	LDA #item_start_x
	STA item_x
.L0337 ;  item_oldx = item_start_x

	LDA #item_start_x
	STA item_oldx
.L0338 ;  item_y = item_start_y

	LDA #item_start_y
	STA item_y
.L0339 ;  item_oldy = item_start_x

	LDA #item_start_x
	STA item_oldy
.L0340 ;  missile1x = nowhere

	LDA #nowhere
	STA missile1x
.L0341 ;  missile1y = nowhere

	LDA #nowhere
	STA missile1y
.L0342 ;  scorecolor = 64

	LDA #64
	STA scorecolor
.L0343 ;  item_hp = item_start_hp

	LDA #item_start_hp
	STA item_hp
.L0344 ;  gosub show_inventory

 jsr .show_inventory

.L0345 ;  gosub hero_draw_s

 jsr .hero_draw_s

.L0346 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point3-1)
 pha
 lda #<(ret_point3-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point3
.L0347 ;  drawscreen

 sta temp7
 lda #>(ret_point4-1)
 pha
 lda #<(ret_point4-1)
 pha
 lda #>(drawscreen-1)
 pha
 lda #<(drawscreen-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point4
.main_loop
 ; main_loop

.L0348 ;  COLUP0  =  skin

	LDA #skin
	STA COLUP0
.L0349 ;  temp1  =  item_kind  &  item_kind_mask

	LDA item_kind
	AND #item_kind_mask
	STA temp1
.L0350 ;  COLUP1  =  item_colors[temp1]

	LDX temp1
	LDA item_colors,x
	STA COLUP1
.L0351 ;  rem 'Special effects for the item (double or triple size, double or triple instances)

.L0352 ;  if item_kind{7} goto special_effects_end

	BIT item_kind
 if ((* - .special_effects_end) < 127) && ((* - .special_effects_end) > -128)
	bmi .special_effects_end
 else
	bpl .0skipspecial_effects_end
	jmp .special_effects_end
.0skipspecial_effects_end
 endif
.L0353 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0354 ;  if temp2{6} then NUSIZ1  =  $07

	BIT temp2
	BVC .skipL0354
.condpart0
	LDA #$07
	STA NUSIZ1
.skipL0354
.L0355 ;  if temp2{7} then NUSIZ1  =  $02

	BIT temp2
	BPL .skipL0355
.condpart1
	LDA #$02
	STA NUSIZ1
.skipL0355
.L0356 ;  if temp2{7}  &&  temp2{7} then NUSIZ1  = $06

	BIT temp2
	BPL .skipL0356
.condpart2
	BIT temp2
	BPL .skip2then
.condpart3
	LDA #$06
	STA NUSIZ1
.skip2then
.skipL0356
.special_effects_end
 ; special_effects_end

.L0357 ;  PF0 = $00

	LDA #$00
	STA PF0
.
 ; 

.
 ; 

.L0358 ;  rem 'Collisions with the walls. For now, we use push-back collision 

.L0359 ;  rem 'for the monsters, and preventive collision detection for the player. 

.L0360 ;  rem 'This is inconsistent, but push-back is easier to manage with regards to 

.L0361 ;  rem 'and matters less for AI, so I'll leave it at that for now.

.L0362 ;  if !collision(player0,playfield) then goto herofield_collide_end

	BIT CXP0FB
	BMI .skipL0362
.condpart4
 jmp .herofield_collide_end

.skipL0362
.L0363 ;  rem 'Could do something here ??? Some overlap is OK, IMO

.herofield_collide_end
 ; herofield_collide_end

.
 ; 

.L0364 ;  rem 'Collisions with the walls

.L0365 ;  rem 'For the item

.L0366 ;  if !collision(player1,playfield) then goto itemfield_collide_end

	BIT CXP1FB
	BMI .skipL0366
.condpart5
 jmp .itemfield_collide_end

.skipL0366
.L0367 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0368 ;  rem 'Some monsters ignore walls

.L0369 ;  if temp2{5} then goto itemfield_collide_end

	LDA temp2
	AND #32
	BEQ .skipL0369
.condpart6
 jmp .itemfield_collide_end

.skipL0369
.
 ; 

.L0370 ;  if item_y  =  item_oldy  &&  item_x  =  item_oldx then goto itemfield_collide_still

	LDA item_y
	CMP item_oldy
     BNE .skipL0370
.condpart7
	LDA item_x
	CMP item_oldx
     BNE .skip7then
.condpart8
 jmp .itemfield_collide_still

.skip7then
.skipL0370
.L0371 ;  item_y  =  item_oldy

	LDA item_oldy
	STA item_y
.L0372 ;  item_x  =  item_oldx

	LDA item_oldx
	STA item_x
.L0373 ;  goto itemfield_collide_end

 jmp .itemfield_collide_end

.itemfield_collide_still
 ; itemfield_collide_still

.L0374 ;  rem if item_y < field_vcenter then item_y = item_oldy - 1 else item_y = item_oldy + 1

.L0375 ;  rem if item_x < field_hcenter then item_x = item_oldx - 1 else item_x = hero_oldx + 1

.itemfield_collide_end
 ; itemfield_collide_end

.
 ; 

.
 ; 

.L0376 ;  rem 'Remember curent position which should be OK here, 

.L0377 ;  rem 'and with that I mean not colliding with the walls. 

.L0378 ;  hero_oldx  =  hero_x

	LDA hero_x
	STA hero_oldx
.L0379 ;  hero_oldy  =  hero_y

	LDA hero_y
	STA hero_oldy
.L0380 ;  item_oldx  =  item_x

	LDA item_x
	STA item_oldx
.L0381 ;  item_oldy  =  item_y

	LDA item_y
	STA item_oldy
.
 ; 

.
 ; 

.L0382 ;  rem 'Teleport to Sygne's home on reset button

.L0383 ;  if !switchreset then goto reset_end

 lda #1
 bit SWCHB
	BEQ .skipL0383
.condpart9
 jmp .reset_end

.skipL0383
.L0384 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L0385 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L0386 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L0387 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L0388 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L0389 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point5-1)
 pha
 lda #<(ret_point5-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point5
.reset_end
 ; reset_end

.
 ; 

.L0390 ;  rem 'Update game timer, and let it overflow back to 0 .

.L0391 ;  game_timer  =  game_timer  +  1

	INC game_timer
.L0392 ;  rem 'Update sound timer, and set to silence if sound is done

.L0393 ;  if sound_timer  <  1 then goto sound_end

	LDA sound_timer
	CMP #1
     BCS .skipL0393
.condpart10
 jmp .sound_end

.skipL0393
.L0394 ;  sound_timer  =  sound_timer  -  1

	DEC sound_timer
.L0395 ;  if sound_timer  >  1 then goto sound_end

	LDA #1
	CMP sound_timer
     BCS .skipL0395
.condpart11
 jmp .sound_end

.skipL0395
.L0396 ;  AUDV1  =  0

	LDA #0
	STA AUDV1
.L0397 ;  AUDC1  =  0

	LDA #0
	STA AUDC1
.L0398 ;  AUDF1  =  0

	LDA #0
	STA AUDF1
.L0399 ;  sound_timer  =  0

	LDA #0
	STA sound_timer
.sound_end
 ; sound_end

.
 ; 

.L0400 ;  rem 'Collision between hero and monster/item or the monster's missile

.L0401 ;  if !collision(player0,missile1) then goto no_missile_collide

	BIT CXM1P
	BMI .skipL0401
.condpart12
 jmp .no_missile_collide

.skipL0401
.L0402 ;  rem 'Ennemy missile hit player, remove it

.L0403 ;  missile1y  =  nowhere

	LDA #nowhere
	STA missile1y
.L0404 ;  missile1x  =  nowhere

	LDA #nowhere
	STA missile1x
.L0405 ;  item_flags =  0

	LDA #0
	STA item_flags
.L0406 ;  rem ' If the player doesn't have a shield, jump to monster collide

.L0407 ;  rem 'Otherwise ignore the projectile and remove it  

.L0408 ;  if !hero_items{3} then goto monster_collide

	LDA hero_items
	AND #8
	BNE .skipL0408
.condpart13
 jmp .monster_collide

.skipL0408
.no_missile_collide
 ; no_missile_collide

.L0409 ;  if !collision(player0,player1) then goto item_collide_end

	BIT CXPPMM
	BMI .skipL0409
.condpart14
 jmp .item_collide_end

.skipL0409
.L0410 ;  rem 'Distinguish between item or monster collide

.L0411 ;  if item_kind{7} then goto item_collide

	BIT item_kind
	BPL .skipL0411
.condpart15
 jmp .item_collide

.skipL0411
.L0412 ;  rem 'Collision with monster or monster missile

.monster_collide
 ; monster_collide

.L0413 ;  COLUP0  =  red

	LDA red
	STA COLUP0
.L0414 ;  rem 'Make a hit sound

.L0415 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0416 ;  AUDC1 = 6

	LDA #6
	STA AUDC1
.L0417 ;  AUDF1 = 10

	LDA #10
	STA AUDF1
.L0418 ;  AUDV1 = 14

	LDA #14
	STA AUDV1
.
 ; 

.L0419 ;  rem 'Push back the hero, but only if the playfield is free behind her.

.L0420 ;  rem 'Also don't push back if too close to the edge of the screen.

.
 ; 

.
 ; 

.L0421 ;  if !hero_flags{0} then goto hero_hit_north_end

	LDA hero_flags
	LSR
	BCS .skipL0421
.condpart16
 jmp .hero_hit_north_end

.skipL0421
.L0422 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0423 ;  temp2  =   ( hero_y  +  8  -  1 )   /  8

; complex statement detected
	LDA hero_y
	CLC
	ADC #8
	SEC
	SBC #1
	lsr
	lsr
	lsr
	STA temp2
.L0424 ;  if pfread ( temp1 ,  temp2 )  goto hero_hit_north_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point6-1)
 pha
 lda #<(ret_point6-1)
 pha
 lda #>(pfread-1)
 pha
 lda #<(pfread-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point6
 if ((* - .hero_hit_north_end) < 127) && ((* - .hero_hit_north_end) > -128)
	BEQ .hero_hit_north_end
 else
	bne .1skiphero_hit_north_end
	jmp .hero_hit_north_end
.1skiphero_hit_north_end
 endif
.L0425 ;  hero_y  =  hero_y  +  8

	LDA hero_y
	CLC
	ADC #8
	STA hero_y
.L0426 ;  rem 'Still inside the screen?  

.L0427 ;  if hero_y  <  field_bottom then goto hero_hit_north_end

	LDA hero_y
	CMP #field_bottom
     BCS .skipL0427
.condpart17
 jmp .hero_hit_north_end

.skipL0427
.L0428 ;  rem 'if we get here, we'd be pushed out of the screen. Prevent this.

.L0429 ;  hero_y  =  field_bottom_enter

	LDA #field_bottom_enter
	STA hero_y
.hero_hit_north_end
 ; hero_hit_north_end

.
 ; 

.L0430 ;  if !hero_flags{1} then goto hero_hit_east_end

	LDA hero_flags
	AND #2
	BNE .skipL0430
.condpart18
 jmp .hero_hit_east_end

.skipL0430
.L0431 ;  temp1  =   ( hero_x  -  8  -  17 )   /  4

; complex statement detected
	LDA hero_x
	SEC
	SBC #8
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0432 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

; complex statement detected
	LDA hero_y
	SEC
	SBC #hero_half_high
	SEC
	SBC #1
	lsr
	lsr
	lsr
	STA temp2
.L0433 ;  if pfread ( temp1 ,  temp2 )  goto hero_hit_east_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point7-1)
 pha
 lda #<(ret_point7-1)
 pha
 lda #>(pfread-1)
 pha
 lda #<(pfread-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point7
 if ((* - .hero_hit_east_end) < 127) && ((* - .hero_hit_east_end) > -128)
	BEQ .hero_hit_east_end
 else
	bne .2skiphero_hit_east_end
	jmp .hero_hit_east_end
.2skiphero_hit_east_end
 endif
.L0434 ;  hero_x  =  hero_x  -  8

	LDA hero_x
	SEC
	SBC #8
	STA hero_x
.L0435 ;  rem 'Still inside the screen?  

.L0436 ;  if field_left  <  hero_x then goto hero_hit_east_end

	LDA #field_left
	CMP hero_x
     BCS .skipL0436
.condpart19
 jmp .hero_hit_east_end

.skipL0436
.L0437 ;  rem 'if we get here, we'd be pushed out of the screen. Prevent this.

.L0438 ;  hero_x  =  field_left_enter

	LDA #field_left_enter
	STA hero_x
.hero_hit_east_end
 ; hero_hit_east_end

.
 ; 

.L0439 ;  if !hero_flags{2} then goto hero_hit_south_end

	LDA hero_flags
	AND #4
	BNE .skipL0439
.condpart20
 jmp .hero_hit_south_end

.skipL0439
.L0440 ;  if pfread ( temp1 ,  temp2 )  then goto hero_hit_south_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point8-1)
 pha
 lda #<(ret_point8-1)
 pha
 lda #>(pfread-1)
 pha
 lda #<(pfread-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point8
	BNE .skipL0440
.condpart21
 jmp .hero_hit_south_end

.skipL0440
.L0441 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0442 ;  temp2  =   ( hero_y  -  hero_high  -  8  -  1 )   /  8

; complex statement detected
	LDA hero_y
	SEC
	SBC #hero_high
	SEC
	SBC #8
	SEC
	SBC #1
	lsr
	lsr
	lsr
	STA temp2
.L0443 ;  hero_y  =  hero_y  -  8

	LDA hero_y
	SEC
	SBC #8
	STA hero_y
.L0444 ;  if hero_y  >  field_top then goto hero_hit_south_end

	LDA #field_top
	CMP hero_y
     BCS .skipL0444
.condpart22
 jmp .hero_hit_south_end

.skipL0444
.L0445 ;  rem 'if we get here, we'd be pushed out of the screen. Prevent this.

.L0446 ;  hero_y  =  field_top_enter

	LDA #field_top_enter
	STA hero_y
.hero_hit_south_end
 ; hero_hit_south_end

.
 ; 

.L0447 ;  if !hero_flags{3} then goto hero_hit_west_end

	LDA hero_flags
	AND #8
	BNE .skipL0447
.condpart23
 jmp .hero_hit_west_end

.skipL0447
.L0448 ;  temp1  =   ( hero_x  +  hero_wide  +  8  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_wide
	CLC
	ADC #8
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0449 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

; complex statement detected
	LDA hero_y
	SEC
	SBC #hero_half_high
	SEC
	SBC #1
	lsr
	lsr
	lsr
	STA temp2
.L0450 ;  if pfread ( temp1 ,  temp2 )  then goto hero_hit_west_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point9-1)
 pha
 lda #<(ret_point9-1)
 pha
 lda #>(pfread-1)
 pha
 lda #<(pfread-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point9
	BNE .skipL0450
.condpart24
 jmp .hero_hit_west_end

.skipL0450
.L0451 ;  hero_x  =  hero_x  +  8

	LDA hero_x
	CLC
	ADC #8
	STA hero_x
.L0452 ;  rem 'Still inside the screen?  

.L0453 ;  if hero_x  <  field_right then goto hero_hit_west_end

	LDA hero_x
	CMP #field_right
     BCS .skipL0453
.condpart25
 jmp .hero_hit_west_end

.skipL0453
.L0454 ;  rem 'if we get here, we'd be pushed out of the screen. Prevent this.

.L0455 ;  hero_x  =  field_right_enter

	LDA #field_right_enter
	STA hero_x
.hero_hit_west_end
 ; hero_hit_west_end

.
 ; 

.L0456 ;  temp1  =  item_kind  &  item_kind_mask

	LDA item_kind
	AND #item_kind_mask
	STA temp1
.L0457 ;  temp1  =  item_damage[temp1]

	LDX temp1
	LDA item_damage,x
	STA temp1
.L0458 ;  rem 'Halve damage if hero has the armor 

.L0459 ;  if hero_items{4} then temp1  =  temp1  /  2

	LDA hero_items
	AND #16
	BEQ .skipL0459
.condpart26
	LDA temp1
	lsr
	STA temp1
.skipL0459
.
 ; 

.L0460 ;  rem 'Game over if damage is higher than health

.L0461 ;  if temp1  <  hero_hp then goto do_damage

	LDA temp1
	CMP hero_hp
     BCS .skipL0461
.condpart27
 jmp .do_damage

.skipL0461
.L0462 ;  rem 'But escape by the skin of the teeth if Signe has the healing book and 

.L0463 ;  rem 'Numen to use it.

.L0464 ;  if hero_items{6}  &&  hero_mp  >  0 then goto use_heal_spell

	BIT hero_items
	BVC .skipL0464
.condpart28
	LDA #0
	CMP hero_mp
     BCS .skip28then
.condpart29
 jmp .use_heal_spell

.skip28then
.skipL0464
.L0465 ;  hero_hp  =  0

	LDA #0
	STA hero_hp
.L0466 ;  goto game_over bank5

 sta temp7
 lda #>(.game_over-1)
 pha
 lda #<(.game_over-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #5
 jmp BS_jsr
.use_heal_spell
 ; use_heal_spell

.L0467 ;  COLUP0  =  green

	LDA green
	STA COLUP0
.L0468 ;  hero_mp  =  hero_mp  -  1

	DEC hero_mp
.L0469 ;  gosub set_mp_display

 jsr .set_mp_display

.L0470 ;  hero_hp  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0471 ;  goto item_collide_end

 jmp .item_collide_end

.do_damage
 ; do_damage

.L0472 ;  hero_hp  =  hero_hp  -  temp1

	LDA hero_hp
	SEC
	SBC temp1
	STA hero_hp
.L0473 ;  goto item_collide_end

 jmp .item_collide_end

.item_collide
 ; item_collide

.L0474 ;  rem 'If colliding with an item, pick it up if it is not a switch.

.L0475 ;  if item_kind  <  item_switch1 then goto item_do_collide

	LDA item_kind
	CMP #item_switch1
     BCS .skipL0475
.condpart30
 jmp .item_do_collide

.skipL0475
.L0476 ;  if item_kind  >  item_switch7_on then goto item_do_collide

	LDA #item_switch7_on
	CMP item_kind
     BCS .skipL0476
.condpart31
 jmp .item_do_collide

.skipL0476
.L0477 ;  goto item_collide_end

 jmp .item_collide_end

.item_do_collide
 ; item_do_collide

.L0478 ;  gosub item_pickup

 jsr .item_pickup

.L0479 ;  gosub show_inventory

 jsr .show_inventory

.item_collide_end
 ; item_collide_end

.
 ; 

.L0480 ;  rem 'Collision between monster missile and field. 

.L0481 ;  rem 'Or between sword and monster missile.  Remove missile.

.L0482 ;  rem 'Remove misile if monster is gone.

.L0483 ;  if item_kind  >  last_monster goto missile_remove

	LDA #last_monster
	CMP item_kind
 if ((* - .missile_remove) < 127) && ((* - .missile_remove) > -128)
	bcc .missile_remove
 else
	bcs .3skipmissile_remove
	jmp .missile_remove
.3skipmissile_remove
 endif
.L0484 ;  if collision(missile0,missile1) then goto missile_remove

	BIT CXPPMM
	BVC .skipL0484
.condpart32
 jmp .missile_remove

.skipL0484
.L0485 ;  if missile1x  >  field_right then goto missile_remove

	LDA #field_right
	CMP missile1x
     BCS .skipL0485
.condpart33
 jmp .missile_remove

.skipL0485
.L0486 ;  if missile1x  <  field_left then goto missile_remove

	LDA missile1x
	CMP #field_left
     BCS .skipL0486
.condpart34
 jmp .missile_remove

.skipL0486
.L0487 ;  if missile1y  <  field_top then goto missile_remove

	LDA missile1y
	CMP #field_top
     BCS .skipL0487
.condpart35
 jmp .missile_remove

.skipL0487
.L0488 ;  if missile1y  >  field_bottom then goto missile_remove

	LDA #field_bottom
	CMP missile1y
     BCS .skipL0488
.condpart36
 jmp .missile_remove

.skipL0488
.L0489 ;  if !collision(missile1,playfield) then goto missile_remove_end

	BIT CXM1FB
	BMI .skipL0489
.condpart37
 jmp .missile_remove_end

.skipL0489
.L0490 ;  rem 'Don't remove the misile even when coliding with the playfield if the

.L0491 ;  rem 'monster ignores the walls.

.L0492 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0493 ;  rem 'temp2 = temp2 & monster_nowalls

.L0494 ;  if temp2{5} then goto missile_remove_end

	LDA temp2
	AND #32
	BEQ .skipL0494
.condpart38
 jmp .missile_remove_end

.skipL0494
.missile_remove
 ; missile_remove

.L0495 ;  missile1y  =  nowhere

	LDA #nowhere
	STA missile1y
.L0496 ;  missile1x  =  nowhere

	LDA #nowhere
	STA missile1x
.L0497 ;  item_flags =  0

	LDA #0
	STA item_flags
.missile_remove_end
 ; missile_remove_end

.
 ; 

.L0498 ;  rem 'Collision between monster and hero's attack

.L0499 ;  if !collision(player1,missile0) then goto slash_collide_end

	BIT CXM0P
	BMI .skipL0499
.condpart39
 jmp .slash_collide_end

.skipL0499
.L0500 ;  rem 'Distinguish between item or monster collide

.L0501 ;  if item_kind{7} then goto slash_item_collide

	BIT item_kind
	BPL .skipL0501
.condpart40
 jmp .slash_item_collide

.skipL0501
.
 ; 

.L0502 ;  rem 'The curse monster cannot be harmed normally. 

.L0503 ;  rem 'It can be harmed if Signe has the three leaves, or the Numen Sword. 

.L0504 ;  rem 'Do the special handling here.

.L0505 ;  if item_kind  <>  monster_curse then goto monster_no_curse

	LDA item_kind
	CMP #monster_curse
     BEQ .skipL0505
.condpart41
 jmp .monster_no_curse

.skipL0505
.L0506 ;  rem 'Signe has the sword? The curse can be damaged.

.L0507 ;  if hero_items{5} then goto monster_no_curse

	LDA hero_items
	AND #32
	BEQ .skipL0507
.condpart42
 jmp .monster_no_curse

.skipL0507
.L0508 ;  rem 'Signe has the three leaves? The curse can be damaged. 

.L0509 ;  if hero_items{0}  &&  hero_items{1}  &&  hero_items{2} then goto monster_no_curse

	LDA hero_items
	LSR
	BCC .skipL0509
.condpart43
	LDA hero_items
	AND #2
	BEQ .skip43then
.condpart44
	LDA hero_items
	AND #4
	BEQ .skip44then
.condpart45
 jmp .monster_no_curse

.skip44then
.skip43then
.skipL0509
.L0510 ;  rem 'If we get here, Signe's unable to do any damage.

.L0511 ;  rem 'Make a deflected sound

.L0512 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0513 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0514 ;  AUDV1 = 14

	LDA #14
	STA AUDV1
.L0515 ;  AUDF1 = 1

	LDA #1
	STA AUDF1
.L0516 ;  goto slash_collide_end

 jmp .slash_collide_end

.monster_no_curse
 ; monster_no_curse

.L0517 ;  rem 'Make a hit sound once

.L0518 ;  sound_timer = 1

	LDA #1
	STA sound_timer
.L0519 ;  AUDC1 = 8

	LDA #8
	STA AUDC1
.L0520 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0521 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0522 ;  hero_flags{4}  =  0

	LDA hero_flags
	AND #239
	STA hero_flags
.L0523 ;  rem COLUP1 = red

.L0524 ;  rem 'Push back monster

.L0525 ;  if hero_flags{0} then item_y  =  item_y  -  4

	LDA hero_flags
	LSR
	BCC .skipL0525
.condpart46
	LDA item_y
	SEC
	SBC #4
	STA item_y
.skipL0525
.L0526 ;  if hero_flags{1} then item_x  =  item_x  +  4

	LDA hero_flags
	AND #2
	BEQ .skipL0526
.condpart47
	LDA item_x
	CLC
	ADC #4
	STA item_x
.skipL0526
.L0527 ;  if hero_flags{2} then item_y  =  item_y  +  4

	LDA hero_flags
	AND #4
	BEQ .skipL0527
.condpart48
	LDA item_y
	CLC
	ADC #4
	STA item_y
.skipL0527
.L0528 ;  if hero_flags{3} then item_x  =  item_x  -  4

	LDA hero_flags
	AND #8
	BEQ .skipL0528
.condpart49
	LDA item_x
	SEC
	SBC #4
	STA item_x
.skipL0528
.L0529 ;  rem 'Damage is level / 2 + 1

.L0530 ;  temp1  =   ( hero_level  /  2 ) 

; complex statement detected
	LDA hero_level
	lsr
	STA temp1
.L0531 ;  temp1  =  temp1  +  1

	INC temp1
.L0532 ;  rem 'Double damage with the Numen Sword

.L0533 ;  if hero_items{5} then temp1  =  temp1  *  2

	LDA hero_items
	AND #32
	BEQ .skipL0533
.condpart50
	LDA temp1
	asl
	STA temp1
.skipL0533
.L0534 ;  rem 'Prevent damage overflow

.L0535 ;  if temp1  <  0 then temp1  =  0  -  temp1

	LDA temp1
	CMP #0
     BCS .skipL0535
.condpart51
	LDA #0
	SEC
	SBC temp1
	STA temp1
.skipL0535
.L0536 ;  if temp1  <  item_hp then goto item_survived

	LDA temp1
	CMP item_hp
     BCS .skipL0536
.condpart52
 jmp .item_survived

.skipL0536
.L0537 ;  rem 'Item / monster killed or destroyed

.L0538 ;  if item_kind  =  monster_ikaza then quest_flags{0}  =  1

	LDA item_kind
	CMP #monster_ikaza
     BNE .skipL0538
.condpart53
	LDA quest_flags
	ORA #1
	STA quest_flags
.skipL0538
.L0539 ;  rem 'Make sure Ikaza cannot come back

.L0540 ;  rem 'Give experience and level up if needed

.L0541 ;  rem 'Experience received is danger level of the monster 

.L0542 ;  temp2  =  item_damage[item_kind]

	LDX item_kind
	LDA item_damage,x
	STA temp2
.L0543 ;  rem 'No experience for killing the curse, as it's a sitting duck.

.L0544 ;  if item_kind  =  monster_curse then goto give_experience_end

	LDA item_kind
	CMP #monster_curse
     BNE .skipL0544
.condpart54
 jmp .give_experience_end

.skipL0544
.L0545 ;  if temp2  <  hero_next then goto give_experience

	LDA temp2
	CMP hero_next
     BCS .skipL0545
.condpart55
 jmp .give_experience

.skipL0545
.L0546 ;  rem 'Level up, but not more than 99

.L0547 ;  if hero_level  >  98 then goto give_experience_end

	LDA #98
	CMP hero_level
     BCS .skipL0547
.condpart56
 jmp .give_experience_end

.skipL0547
.L0548 ;  rem 'Level up sound

.L0549 ;  COLUP0 = blue

	LDA blue
	STA COLUP0
.L0550 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0551 ;  AUDF1 = 24

	LDA #24
	STA AUDF1
.L0552 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0553 ;  sound_timer = 8

	LDA #8
	STA sound_timer
.L0554 ;  hero_level  =  hero_level  +  1

	INC hero_level
.L0555 ;  hero_hp  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0556 ;  rem 'Actually, this is buggy for levels above 80

.L0557 ;  hero_mp  =  hero_level / 16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0558 ;  gosub set_mp_display

 jsr .set_mp_display

.L0559 ;  hero_next  =   ( hero_level  /  2 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	CLC
	ADC #1
	STA hero_next
.L0560 ;  COLUP0  =  turfy

	LDA #turfy
	STA COLUP0
.L0561 ;  goto give_experience_end

 jmp .give_experience_end

.give_experience
 ; give_experience

.L0562 ;  hero_next  =  hero_next  -  temp2

	LDA hero_next
	SEC
	SBC temp2
	STA hero_next
.give_experience_end
 ; give_experience_end

.L0563 ;  rem 'Drop an item, perhaps

.L0564 ;  temp6  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp6
.L0565 ;  rem 'Bit 4: monster always drops item 

.L0566 ;  if temp6{4} then goto drop_item

	LDA temp6
	AND #16
	BEQ .skipL0566
.condpart57
 jmp .drop_item

.skipL0566
.L0567 ;  if rand  >  128 then goto drop_item

	LDA #128
	CMP rand
     BCS .skipL0567
.condpart58
 jmp .drop_item

.skipL0567
.L0568 ;  rem 'Drop occasional, not always, depeding on monster flags

.drop_no_item
 ; drop_no_item

.L0569 ;  item_kind  =  item_none

	LDA #item_none
	STA item_kind
.L0570 ;  item_hp  =  0

	LDA #0
	STA item_hp
.L0571 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L0572 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
.L0573 ;  goto drop_item_done

 jmp .drop_item_done

.drop_item
 ; drop_item

.L0574 ;  temp2  =  monster_drops[item_kind]

	LDX item_kind
	LDA monster_drops,x
	STA temp2
.L0575 ;  item_kind  =  temp2

	LDA temp2
	STA item_kind
.L0576 ;  gosub item_setup_kind bank4

 sta temp7
 lda #>(ret_point10-1)
 pha
 lda #<(ret_point10-1)
 pha
 lda #>(.item_setup_kind-1)
 pha
 lda #<(.item_setup_kind-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #4
 jmp BS_jsr
ret_point10
.drop_item_done
 ; drop_item_done

.L0577 ;  goto slash_collide_end

 jmp .slash_collide_end

.item_survived
 ; item_survived

.L0578 ;  item_hp  =  item_hp  -  temp1

	LDA item_hp
	SEC
	SBC temp1
	STA item_hp
.L0579 ;  rem 'Push item around

.L0580 ;  if hero_flags{0} then item_y  =  item_y  -  4

	LDA hero_flags
	LSR
	BCC .skipL0580
.condpart59
	LDA item_y
	SEC
	SBC #4
	STA item_y
.skipL0580
.L0581 ;  if hero_flags{1} then item_x  =  item_x  +  4

	LDA hero_flags
	AND #2
	BEQ .skipL0581
.condpart60
	LDA item_x
	CLC
	ADC #4
	STA item_x
.skipL0581
.L0582 ;  if hero_flags{2} then item_y  =  item_y  +  4

	LDA hero_flags
	AND #4
	BEQ .skipL0582
.condpart61
	LDA item_y
	CLC
	ADC #4
	STA item_y
.skipL0582
.L0583 ;  if hero_flags{3} then item_x  =  item_x  -  4

	LDA hero_flags
	AND #8
	BEQ .skipL0583
.condpart62
	LDA item_x
	SEC
	SBC #4
	STA item_x
.skipL0583
.L0584 ;  goto slash_collide_end

 jmp .slash_collide_end

.slash_item_collide
 ; slash_item_collide

.L0585 ;  rem 'Make a hit sound once

.L0586 ;  sound_timer = 1

	LDA #1
	STA sound_timer
.L0587 ;  AUDC1 = 8

	LDA #8
	STA AUDC1
.L0588 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0589 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0590 ;  if item_kind  <  item_switch1 then goto slash_item_pickup

	LDA item_kind
	CMP #item_switch1
     BCS .skipL0590
.condpart63
 jmp .slash_item_pickup

.skipL0590
.L0591 ;  if item_kind  >  item_switch7_on then goto slash_item_pickup

	LDA #item_switch7_on
	CMP item_kind
     BCS .skipL0591
.condpart64
 jmp .slash_item_pickup

.skipL0591
.L0592 ;  rem 'If colliding with an item that's not a switch, pick it up.

.L0593 ;  rem 'Otherwise it's a switch that's hit.  

.L0594 ;  goto hit_switch

 jmp .hit_switch

.slash_item_pickup
 ; slash_item_pickup

.L0595 ;  gosub item_pickup

 jsr .item_pickup

.L0596 ;  gosub show_inventory

 jsr .show_inventory

.L0597 ;  goto slash_collide_end

 jmp .slash_collide_end

.hit_switch
 ; hit_switch

.L0598 ;  rem 'Do nothing with the already triggered switches. 

.L0599 ;  rem '(Turn them off again, perhaps?)

.L0600 ;  if item_kind  >  item_switch7 then goto slash_collide_end

	LDA #item_switch7
	CMP item_kind
     BCS .skipL0600
.condpart65
 jmp .slash_collide_end

.skipL0600
.L0601 ;  rem 'Handle hitting of the switch: set the quest flag and change the display.

.L0602 ;  if item_kind  =  item_switch1 then quest_flags{1}  =  1

	LDA item_kind
	CMP #item_switch1
     BNE .skipL0602
.condpart66
	LDA quest_flags
	ORA #2
	STA quest_flags
.skipL0602
.L0603 ;  if item_kind  =  item_switch2 then quest_flags{2}  =  1

	LDA item_kind
	CMP #item_switch2
     BNE .skipL0603
.condpart67
	LDA quest_flags
	ORA #4
	STA quest_flags
.skipL0603
.L0604 ;  if item_kind  =  item_switch3 then quest_flags{3}  =  1

	LDA item_kind
	CMP #item_switch3
     BNE .skipL0604
.condpart68
	LDA quest_flags
	ORA #8
	STA quest_flags
.skipL0604
.L0605 ;  if item_kind  =  item_switch4 then quest_flags{4}  =  1

	LDA item_kind
	CMP #item_switch4
     BNE .skipL0605
.condpart69
	LDA quest_flags
	ORA #16
	STA quest_flags
.skipL0605
.L0606 ;  if item_kind  =  item_switch5 then quest_flags{5}  =  1

	LDA item_kind
	CMP #item_switch5
     BNE .skipL0606
.condpart70
	LDA quest_flags
	ORA #32
	STA quest_flags
.skipL0606
.L0607 ;  if item_kind  =  item_switch6 then quest_flags{6}  =  1

	LDA item_kind
	CMP #item_switch6
     BNE .skipL0607
.condpart71
	LDA quest_flags
	ORA #64
	STA quest_flags
.skipL0607
.L0608 ;  if item_kind  =  item_switch7 then quest_flags{7}  =  1

	LDA item_kind
	CMP #item_switch7
     BNE .skipL0608
.condpart72
	LDA quest_flags
	ORA #128
	STA quest_flags
.skipL0608
.L0609 ;  gosub item_setup_kind bank4

 sta temp7
 lda #>(ret_point11-1)
 pha
 lda #<(ret_point11-1)
 pha
 lda #>(.item_setup_kind-1)
 pha
 lda #<(.item_setup_kind-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #4
 jmp BS_jsr
ret_point11
.slash_collide_end
 ; slash_collide_end

.
 ; 

.
 ; 

.
 ; 

.L0610 ;  rem 'Player action though button

.L0611 ;  if !joy0fire then goto hero_action_done

 lda #$80
 bit INPT4
	BEQ .skipL0611
.condpart73
 jmp .hero_action_done

.skipL0611
.L0612 ;  rem 'Skip all this if action is blocked.

.L0613 ;  if !hero_flags{4} then goto hero_action_blocked

	LDA hero_flags
	AND #16
	BNE .skipL0613
.condpart74
 jmp .hero_action_blocked

.skipL0613
.L0614 ;  if !hero_items{7} then goto hero_act_nostrike

	BIT hero_items
	BMI .skipL0614
.condpart75
 jmp .hero_act_nostrike

.skipL0614
.L0615 ;  if hero_mp  <  strike_cost then goto hero_act_nostrike

	LDA hero_mp
	CMP #strike_cost
     BCS .skipL0615
.condpart76
 jmp .hero_act_nostrike

.skipL0615
.L0616 ;  rem ' The b switch controls whether the book of striking is in use or not.

.L0617 ;  if switchleftb then goto hero_act_nostrike

 lda #$40
 bit SWCHB
	BNE .skipL0617
.condpart77
 jmp .hero_act_nostrike

.skipL0617
.L0618 ;  if item_kind  <  last_monster then goto hero_act_strike

	LDA item_kind
	CMP #last_monster
     BCS .skipL0618
.condpart78
 jmp .hero_act_strike

.skipL0618
.L0619 ;  if item_kind  >=  item_switch1  &&  item_kind  <=  item_switch7 then goto hero_act_strike

	LDA item_kind
	CMP #item_switch1
     BCC .skipL0619
.condpart79
	LDA #item_switch7
	CMP item_kind
     BCC .skip79then
.condpart80
 jmp .hero_act_strike

.skip79then
.skipL0619
.hero_act_nostrike
 ; hero_act_nostrike

.L0620 ;  temp1  =  hero_flags  &  %00001111

	LDA hero_flags
	AND #%00001111
	STA temp1
.L0621 ;  rem 'Use this to jump to the right attack action.

.L0622 ;  rem '          0     1=north 2=east 3=ne   4=south  5=n+s(emu onlu)  6=se 7=nse 

.L0623 ;  rem ' 8 = west

.L0624 ;  rem ' 9 10 11 12 13 14 15(all sides, emu only)    

.L0625 ;  on temp1 goto had hero_act_n hero_act_e hero_act_n hero_act_s hero_act_s hero_act_n hero_act_n hero_act_w hero_act_n hero_act_w hero_act_s had had had

	LDX temp1
	LDA .L0625jumptablehi,x
	PHA
	LDA .L0625jumptablelo,x
	PHA
	RTS
.L0625jumptablehi
	.byte >(.had-1)
	.byte >(.hero_act_n-1)
	.byte >(.hero_act_e-1)
	.byte >(.hero_act_n-1)
	.byte >(.hero_act_s-1)
	.byte >(.hero_act_s-1)
	.byte >(.hero_act_n-1)
	.byte >(.hero_act_n-1)
	.byte >(.hero_act_w-1)
	.byte >(.hero_act_n-1)
	.byte >(.hero_act_w-1)
	.byte >(.hero_act_s-1)
	.byte >(.had-1)
	.byte >(.had-1)
	.byte >(.had-1)
.L0625jumptablelo
	.byte <(.had-1)
	.byte <(.hero_act_n-1)
	.byte <(.hero_act_e-1)
	.byte <(.hero_act_n-1)
	.byte <(.hero_act_s-1)
	.byte <(.hero_act_s-1)
	.byte <(.hero_act_n-1)
	.byte <(.hero_act_n-1)
	.byte <(.hero_act_w-1)
	.byte <(.hero_act_n-1)
	.byte <(.hero_act_w-1)
	.byte <(.hero_act_s-1)
	.byte <(.had-1)
	.byte <(.had-1)
	.byte <(.had-1)
.hero_act_n
 ; hero_act_n

.L0626 ;  missile0x = hero_x  +  4

	LDA hero_x
	CLC
	ADC #4
	STA missile0x
.L0627 ;  missile0y = hero_y  -  14

	LDA hero_y
	SEC
	SBC #14
	STA missile0y
.L0628 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0629 ;  NUSIZ0 = $00

	LDA #$00
	STA NUSIZ0
.L0630 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_e
 ; hero_act_e

.L0631 ;  missile0x = hero_x  +  9

	LDA hero_x
	CLC
	ADC #9
	STA missile0x
.L0632 ;  missile0y = hero_y  -  4

	LDA hero_y
	SEC
	SBC #4
	STA missile0y
.L0633 ;  missile0height = 0

	LDA #0
	STA missile0height
.L0634 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0635 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_s
 ; hero_act_s

.L0636 ;  missile0x = hero_x  +  4

	LDA hero_x
	CLC
	ADC #4
	STA missile0x
.L0637 ;  missile0y = hero_y  +  8

	LDA hero_y
	CLC
	ADC #8
	STA missile0y
.L0638 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0639 ;  NUSIZ0 = $00

	LDA #$00
	STA NUSIZ0
.L0640 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_w
 ; hero_act_w

.L0641 ;  missile0x = hero_x  -  7

	LDA hero_x
	SEC
	SBC #7
	STA missile0x
.L0642 ;  missile0y = hero_y  -  4

	LDA hero_y
	SEC
	SBC #4
	STA missile0y
.L0643 ;  missile0height = 0

	LDA #0
	STA missile0height
.L0644 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0645 ;  goto hero_action_end

 jmp .hero_action_end

.L0646 ;  rem NUSIZ0=$00

.hero_act_strike
 ; hero_act_strike

.L0647 ;  hero_mp  =  hero_mp  -  strike_cost

	LDA hero_mp
	SEC
	SBC #strike_cost
	STA hero_mp
.L0648 ;  gosub set_mp_display

 jsr .set_mp_display

.L0649 ;  rem 'Strike will hit the ennemy wherever it is. A guaranteed hit I'd say ^_^

.L0650 ;  missile0x = item_x

	LDA item_x
	STA missile0x
.L0651 ;  missile0y = item_y

	LDA item_y
	STA missile0y
.L0652 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0653 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0654 ;  COLUP0 = rand

 sta temp7
 lda #>(ret_point12-1)
 pha
 lda #<(ret_point12-1)
 pha
 lda #>(randomize-1)
 pha
 lda #<(randomize-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point12
	STA COLUP0
.L0655 ;  COLUP1 = red

	LDA red
	STA COLUP1
.L0656 ;  goto hero_action_end

 jmp .hero_action_end

.had
 ; had

.hero_action_done
 ; hero_action_done

.L0657 ;  hero_flags{4}  =  1

	LDA hero_flags
	ORA #16
	STA hero_flags
.L0658 ;  rem 'Allow attacks again

.hero_action_blocked
 ; hero_action_blocked

.L0659 ;  rem 'If the action is blocked or released, hide the missile

.L0660 ;  missile0x  =  nowhere

	LDA #nowhere
	STA missile0x
.L0661 ;  missile0y  =  nowhere

	LDA #nowhere
	STA missile0y
.hero_action_end
 ; hero_action_end

.
 ; 

.L0662 ;  rem 'Deterimine hero's motion and direction from the joystick

.L0663 ;  rem 'In NESW order  

.L0664 ;  rem ' hero_flags = (hero_flags / 16) * 16 preserves the higher nibble (I hope)

.L0665 ;  rem '

.L0666 ;  rem 'For the player collision detection, we calculate the playfield position 

.L0667 ;  rem 'at any of the four points around the feet of the player, and check if 

.L0668 ;  rem 'if anything is there with pfread, and prevent motion if it is so.  

.L0669 ;  rem ' Note that we do allow Signe to turn her facing even when she cannot move.

.
 ; 

.L0670 ;  if !joy0up then goto joy0up_end

 lda #$10
 bit SWCHA
	BEQ .skipL0670
.condpart81
 jmp .joy0up_end

.skipL0670
.L0671 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

; complex statement detected
	LDA hero_flags
	lsr
	lsr
	lsr
	lsr
	asl
	asl
	asl
	asl
	STA hero_flags
.L0672 ;  hero_flags{0}  =  1

	LDA hero_flags
	ORA #1
	STA hero_flags
.L0673 ;  rem 'Check top middle point 

.L0674 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0675 ;  temp2  =   ( hero_y  -  hero_high  -  1  -  1 )   /  8

; complex statement detected
	LDA hero_y
	SEC
	SBC #hero_high
	SEC
	SBC #1
	SEC
	SBC #1
	lsr
	lsr
	lsr
	STA temp2
.L0676 ;  if pfread ( temp1 ,  temp2 )  goto joy0up_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point13-1)
 pha
 lda #<(ret_point13-1)
 pha
 lda #>(pfread-1)
 pha
 lda #<(pfread-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point13
 if ((* - .joy0up_end) < 127) && ((* - .joy0up_end) > -128)
	BEQ .joy0up_end
 else
	bne .4skipjoy0up_end
	jmp .joy0up_end
.4skipjoy0up_end
 endif
.L0677 ;  hero_y  =  hero_y  -  1

	DEC hero_y
.joy0up_end
 ; joy0up_end

.L0678 ;  if !joy0right then goto joy0right_end

 lda #$80
 bit SWCHA
	BEQ .skipL0678
.condpart82
 jmp .joy0right_end

.skipL0678
.L0679 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

; complex statement detected
	LDA hero_flags
	lsr
	lsr
	lsr
	lsr
	asl
	asl
	asl
	asl
	STA hero_flags
.L0680 ;  hero_flags{1}  =  1

	LDA hero_flags
	ORA #2
	STA hero_flags
.L0681 ;  temp1  =   ( hero_x  +  hero_wide  +  1  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_wide
	CLC
	ADC #1
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0682 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

; complex statement detected
	LDA hero_y
	SEC
	SBC #hero_half_high
	SEC
	SBC #1
	lsr
	lsr
	lsr
	STA temp2
.L0683 ;  rem 'Prevent motion if something is there

.L0684 ;  if pfread ( temp1 ,  temp2 )  goto joy0right_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point14-1)
 pha
 lda #<(ret_point14-1)
 pha
 lda #>(pfread-1)
 pha
 lda #<(pfread-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point14
 if ((* - .joy0right_end) < 127) && ((* - .joy0right_end) > -128)
	BEQ .joy0right_end
 else
	bne .5skipjoy0right_end
	jmp .joy0right_end
.5skipjoy0right_end
 endif
.L0685 ;  hero_x  =  hero_x  +  1

	INC hero_x
.joy0right_end
 ; joy0right_end

.L0686 ;  if !joy0down then goto joy0down_end

 lda #$20
 bit SWCHA
	BEQ .skipL0686
.condpart83
 jmp .joy0down_end

.skipL0686
.L0687 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

; complex statement detected
	LDA hero_flags
	lsr
	lsr
	lsr
	lsr
	asl
	asl
	asl
	asl
	STA hero_flags
.L0688 ;  hero_flags{2}  =  1

	LDA hero_flags
	ORA #4
	STA hero_flags
.L0689 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0690 ;  temp2  =   ( hero_y  +  1  -  1 )   /  8

; complex statement detected
	LDA hero_y
	CLC
	ADC #1
	SEC
	SBC #1
	lsr
	lsr
	lsr
	STA temp2
.L0691 ;  rem 'Check bottom middle point. don't move if it's blocked by the playfield.

.L0692 ;  if pfread ( temp1 ,  temp2 )  then goto joy0down_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point15-1)
 pha
 lda #<(ret_point15-1)
 pha
 lda #>(pfread-1)
 pha
 lda #<(pfread-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point15
	BNE .skipL0692
.condpart84
 jmp .joy0down_end

.skipL0692
.L0693 ;  hero_y  =  hero_y  +  1

	INC hero_y
.joy0down_end
 ; joy0down_end

.L0694 ;  if !joy0left then goto joy0left_end

 lda #$40
 bit SWCHA
	BEQ .skipL0694
.condpart85
 jmp .joy0left_end

.skipL0694
.L0695 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

; complex statement detected
	LDA hero_flags
	lsr
	lsr
	lsr
	lsr
	asl
	asl
	asl
	asl
	STA hero_flags
.L0696 ;  hero_flags{3}  =  1

	LDA hero_flags
	ORA #8
	STA hero_flags
.L0697 ;  temp1  =   ( hero_x  -  1  -  17 )   /  4

; complex statement detected
	LDA hero_x
	SEC
	SBC #1
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0698 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

; complex statement detected
	LDA hero_y
	SEC
	SBC #hero_half_high
	SEC
	SBC #1
	lsr
	lsr
	lsr
	STA temp2
.L0699 ;  rem 'Check point to the west of the feet 

.L0700 ;  if pfread ( temp1 ,  temp2 )  goto joy0left_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point16-1)
 pha
 lda #<(ret_point16-1)
 pha
 lda #>(pfread-1)
 pha
 lda #<(pfread-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point16
 if ((* - .joy0left_end) < 127) && ((* - .joy0left_end) > -128)
	BEQ .joy0left_end
 else
	bne .6skipjoy0left_end
	jmp .joy0left_end
.6skipjoy0left_end
 endif
.L0701 ;  hero_x  =  hero_x  -  1

	DEC hero_x
.joy0left_end
 ; joy0left_end

.
 ; 

.L0702 ;  rem 'Monster "AI":

.L0703 ;  rem 'Animate missile if it has been launched

.L0704 ;  if missile1y  =  nowhere then goto missile_move_end

	LDA missile1y
	CMP #nowhere
     BNE .skipL0704
.condpart86
 jmp .missile_move_end

.skipL0704
.L0705 ;  if item_flags{3} then missile1x  =  missile1x  -  2  :  goto missile_move_end

	LDA item_flags
	AND #8
	BEQ .skipL0705
.condpart87
	LDA missile1x
	SEC
	SBC #2
	STA missile1x
 jmp .missile_move_end

.skipL0705
.L0706 ;  if item_flags{0} then missile1y  =  missile1y  -  2  :  goto missile_move_end

	LDA item_flags
	LSR
	BCC .skipL0706
.condpart88
	LDA missile1y
	SEC
	SBC #2
	STA missile1y
 jmp .missile_move_end

.skipL0706
.L0707 ;  if item_flags{1} then missile1x  =  missile1x  +  2  :  goto missile_move_end

	LDA item_flags
	AND #2
	BEQ .skipL0707
.condpart89
	LDA missile1x
	CLC
	ADC #2
	STA missile1x
 jmp .missile_move_end

.skipL0707
.L0708 ;  if item_flags{2} then missile1y  =  missile1y  +  2  :  goto missile_move_end

	LDA item_flags
	AND #4
	BEQ .skipL0708
.condpart90
	LDA missile1y
	CLC
	ADC #2
	STA missile1y
 jmp .missile_move_end

.skipL0708
.missile_move_end
 ; missile_move_end

.
 ; 

.L0709 ;  rem 'Move the item if it's a mobile (id lower than 128)

.L0710 ;  if item_kind{7} then goto item_move_end

	BIT item_kind
	BPL .skipL0710
.condpart91
 jmp .item_move_end

.skipL0710
.L0711 ;  temp1  =  rand

 sta temp7
 lda #>(ret_point17-1)
 pha
 lda #<(ret_point17-1)
 pha
 lda #>(randomize-1)
 pha
 lda #<(randomize-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point17
	STA temp1
.L0712 ;  rem ' Shoot a missile sometimes...

.L0713 ;  temp6  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp6
.L0714 ;  rem 'But not if the monster cannot shoot (bit 0)

.L0715 ;  if !temp6{0} then goto activate_missile_end

	LDA temp6
	LSR
	BCS .skipL0715
.condpart92
 jmp .activate_missile_end

.skipL0715
.L0716 ;  if !game_timer{4} then goto activate_missile_end

	LDA game_timer
	AND #16
	BNE .skipL0716
.condpart93
 jmp .activate_missile_end

.skipL0716
.L0717 ;  if !game_timer{7} then goto activate_missile_end

	BIT game_timer
	BMI .skipL0717
.condpart94
 jmp .activate_missile_end

.skipL0717
.L0718 ;  if !temp1{0} then goto activate_missile_end

	LDA temp1
	LSR
	BCS .skipL0718
.condpart95
 jmp .activate_missile_end

.skipL0718
.L0719 ;  if missile1y  <>  nowhere then goto activate_missile_end

	LDA missile1y
	CMP #nowhere
     BEQ .skipL0719
.condpart96
 jmp .activate_missile_end

.skipL0719
.L0720 ;  rem 'Make a shot sound

.L0721 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0722 ;  AUDC1 = 14

	LDA #14
	STA AUDC1
.L0723 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0724 ;  AUDF1 = 20

	LDA #20
	STA AUDF1
.L0725 ;  rem 'Place the missile  

.L0726 ;  missile1x  =  item_x  +  4

	LDA item_x
	CLC
	ADC #4
	STA missile1x
.L0727 ;  missile1y  =  item_y  -  4

	LDA item_y
	SEC
	SBC #4
	STA missile1y
.L0728 ;  temp4  =  item_x  +  8

	LDA item_x
	CLC
	ADC #8
	STA temp4
.L0729 ;  if hero_x  >  temp4 then item_flags{1}  =  1

	LDA temp4
	CMP hero_x
     BCS .skipL0729
.condpart97
	LDA item_flags
	ORA #2
	STA item_flags
.skipL0729
.L0730 ;  temp4  =  item_x  -  8

	LDA item_x
	SEC
	SBC #8
	STA temp4
.L0731 ;  if hero_x  <  temp4 then item_flags{3}  =  1

	LDA hero_x
	CMP temp4
     BCS .skipL0731
.condpart98
	LDA item_flags
	ORA #8
	STA item_flags
.skipL0731
.L0732 ;  if hero_y  >  item_y then item_flags{2}  =  1

	LDA item_y
	CMP hero_y
     BCS .skipL0732
.condpart99
	LDA item_flags
	ORA #4
	STA item_flags
.skipL0732
.L0733 ;  if hero_y  <  item_y then item_flags{0}  =  1

	LDA hero_y
	CMP item_y
     BCS .skipL0733
.condpart100
	LDA item_flags
	ORA #1
	STA item_flags
.skipL0733
.L0734 ;  missile1height = 4

	LDA #4
	STA missile1height
.activate_missile_end
 ; activate_missile_end

.L0735 ;  rem 'Agressive chasing if aggressive flag is set 

.L0736 ;  if temp6{2} then goto monster_chase

	LDA temp6
	AND #4
	BEQ .skipL0736
.condpart101
 jmp .monster_chase

.skipL0736
.L0737 ;  rem 'Don't move if monster is static

.L0738 ;  if temp6{3} then goto item_move_end

	LDA temp6
	AND #8
	BEQ .skipL0738
.condpart102
 jmp .item_move_end

.skipL0738
.L0739 ;  rem 'Only move half the time

.L0740 ;  if game_timer{2} goto item_move_end

	LDA game_timer
	AND #4
 if ((* - .item_move_end) < 127) && ((* - .item_move_end) > -128)
	BNE .item_move_end
 else
	beq .7skipitem_move_end
	jmp .item_move_end
.7skipitem_move_end
 endif
.
 ; 

.L0741 ;  rem 'Only move if the 4th and 5th bit are not set

.L0742 ;  if temp1{1} then goto item_move_end

	LDA temp1
	AND #2
	BEQ .skipL0742
.condpart103
 jmp .item_move_end

.skipL0742
.L0743 ;  rem 'Random walking monsters don't give chase

.L0744 ;  if temp6{1} then goto monster_random_walk

	LDA temp6
	AND #2
	BEQ .skipL0744
.condpart104
 jmp .monster_random_walk

.skipL0744
.monster_chase
 ; monster_chase

.L0745 ;  if hero_x  >  item_x then item_x  =  item_x  +  1 else item_x  =  item_x  -  1

	LDA item_x
	CMP hero_x
     BCS .skipL0745
.condpart105
	INC item_x
 jmp .skipelse0
.skipL0745
	DEC item_x
.skipelse0
.L0746 ;  if hero_y  >  item_y then item_y  =  item_y  +  1 else item_y  =  item_y  -  1

	LDA item_y
	CMP hero_y
     BCS .skipL0746
.condpart106
	INC item_y
 jmp .skipelse1
.skipL0746
	DEC item_y
.skipelse1
.L0747 ;  if temp6{2} then goto item_move_end

	LDA temp6
	AND #4
	BEQ .skipL0747
.condpart107
 jmp .item_move_end

.skipL0747
.L0748 ;  rem 'Aggressive ennemies don't random walk

.monster_random_walk
 ; monster_random_walk

.L0749 ;  rem 'Use bits of random number for random walk.

.L0750 ;  if temp1{0} then item_y  =  item_y  -  1

	LDA temp1
	LSR
	BCC .skipL0750
.condpart108
	DEC item_y
.skipL0750
.L0751 ;  if temp1{4} then item_x  =  item_x  -  1

	LDA temp1
	AND #16
	BEQ .skipL0751
.condpart109
	DEC item_x
.skipL0751
.L0752 ;  if temp1{0}  ||  temp1{4} then goto item_move_end

	LDA temp1
	LSR
	BCC .skipL0752
.condpart110
 jmp .condpart111
.skipL0752
	LDA temp1
	AND #16
	BEQ .skip6OR
.condpart111
 jmp .item_move_end

.skip6OR
.L0753 ;  if temp1{2} then item_y  =  item_y  +  1

	LDA temp1
	AND #4
	BEQ .skipL0753
.condpart112
	INC item_y
.skipL0753
.L0754 ;  if temp1{6} then item_x  =  item_x  +  1

	BIT temp1
	BVC .skipL0754
.condpart113
	INC item_x
.skipL0754
.item_move_end
 ; item_move_end

.
 ; 

.L0755 ;  rem 'Activate missile if needed

.
 ; 

.L0756 ;  rem 'Debug by showing flags in score field

.L0757 ;  rem sc0 = hero_flags

.L0758 ;  rem temp2 = monster_info[item_kind]

.L0759 ;  rem sc1 = temp2

.L0760 ;  rem sc2 = hero_room

.
 ; 

.L0761 ;  rem 'Music, disabled for now.

.L0762 ;  rem 'Is it time to update the note? 

.L0763 ;  rem if music_timer = 0 then gosub music_change_note

.L0764 ;  rem music_timer = music_timer - 1

.
 ; 

.
 ; 

.
 ; 

.L0765 ;  rem 'Collision op hero with edge of field, transfer to other field.

.L0766 ;  if field_left  <  hero_x then goto exit_left_done

	LDA #field_left
	CMP hero_x
     BCS .skipL0766
.condpart114
 jmp .exit_left_done

.skipL0766
.L0767 ;  hero_x  =  field_right_enter

	LDA #field_right_enter
	STA hero_x
.L0768 ;  hero_oldx  =  field_right

	LDA #field_right
	STA hero_oldx
.L0769 ;  hero_room  =  hero_room  -  1

	DEC hero_room
.L0770 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point18-1)
 pha
 lda #<(ret_point18-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point18
.L0771 ;  goto exit_done

 jmp .exit_done

.exit_left_done
 ; exit_left_done

.L0772 ;  if hero_x  <  field_right then goto exit_right_done

	LDA hero_x
	CMP #field_right
     BCS .skipL0772
.condpart115
 jmp .exit_right_done

.skipL0772
.L0773 ;  hero_x  =  field_left_enter

	LDA #field_left_enter
	STA hero_x
.L0774 ;  hero_oldx  =  field_left

	LDA #field_left
	STA hero_oldx
.L0775 ;  hero_room  =  hero_room  +  1

	INC hero_room
.L0776 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point19-1)
 pha
 lda #<(ret_point19-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point19
.L0777 ;  goto exit_done

 jmp .exit_done

.exit_right_done
 ; exit_right_done

.L0778 ;  if hero_y  >  field_top then goto exit_top_done

	LDA #field_top
	CMP hero_y
     BCS .skipL0778
.condpart116
 jmp .exit_top_done

.skipL0778
.L0779 ;  hero_y  =  field_bottom_enter

	LDA #field_bottom_enter
	STA hero_y
.L0780 ;  hero_oldy  =  field_bottom

	LDA #field_bottom
	STA hero_oldy
.L0781 ;  hero_room  =  hero_room  -  8

	LDA hero_room
	SEC
	SBC #8
	STA hero_room
.L0782 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point20-1)
 pha
 lda #<(ret_point20-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point20
.L0783 ;  goto exit_done

 jmp .exit_done

.exit_top_done
 ; exit_top_done

.L0784 ;  if hero_y  <  field_bottom then goto exit_bottom_done

	LDA hero_y
	CMP #field_bottom
     BCS .skipL0784
.condpart117
 jmp .exit_bottom_done

.skipL0784
.L0785 ;  hero_y  =  field_top_enter

	LDA #field_top_enter
	STA hero_y
.L0786 ;  hero_oldy  =  field_top

	LDA #field_top
	STA hero_oldy
.L0787 ;  hero_room  =  hero_room  +  8

	LDA hero_room
	CLC
	ADC #8
	STA hero_room
.L0788 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point21-1)
 pha
 lda #<(ret_point21-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point21
.L0789 ;  goto exit_done

 jmp .exit_done

.exit_bottom_done
 ; exit_bottom_done

.exit_done
 ; exit_done

.
 ; 

.L0790 ;  rem 'Collision of item with edge of field, bounce back.

.L0791 ;  if item_x  <  field_left then item_x = field_left_enter

	LDA item_x
	CMP #field_left
     BCS .skipL0791
.condpart118
	LDA #field_left_enter
	STA item_x
.skipL0791
.L0792 ;  if item_x  >  field_right then item_x = field_right_enter

	LDA #field_right
	CMP item_x
     BCS .skipL0792
.condpart119
	LDA #field_right_enter
	STA item_x
.skipL0792
.L0793 ;  if item_y  <  field_top then item_y = field_top_enter

	LDA item_y
	CMP #field_top
     BCS .skipL0793
.condpart120
	LDA #field_top_enter
	STA item_y
.skipL0793
.L0794 ;  if item_y  >  field_bottom then item_x = field_bottom_enter

	LDA #field_bottom
	CMP item_y
     BCS .skipL0794
.condpart121
	LDA #field_bottom_enter
	STA item_x
.skipL0794
.
 ; 

.
 ; 

.L0795 ;  rem 'Draw acting player in position 2

.L0796 ;  if joy0fire goto player_animate_2

 lda #$80
 bit INPT4
 if ((* - .player_animate_2) < 127) && ((* - .player_animate_2) > -128)
	BEQ .player_animate_2
 else
	bne .8skipplayer_animate_2
	jmp .player_animate_2
.8skipplayer_animate_2
 endif
.L0797 ;  rem 'Draw non moving player not animated

.L0798 ;  if hero_x  =  hero_oldx  &&  hero_y  =  hero_oldy then goto player_animate_1

	LDA hero_x
	CMP hero_oldx
     BNE .skipL0798
.condpart122
	LDA hero_y
	CMP hero_oldy
     BNE .skip122then
.condpart123
 jmp .player_animate_1

.skip122then
.skipL0798
.L0799 ;  rem 'Draw and animate player every so many ticks

.L0800 ;  if !game_timer{4} then goto player_animate_2

	LDA game_timer
	AND #16
	BNE .skipL0800
.condpart124
 jmp .player_animate_2

.skipL0800
.player_animate_1
 ; player_animate_1

.L0801 ;  if hero_flags{0} then gosub hero_draw_n

	LDA hero_flags
	LSR
	BCC .skipL0801
.condpart125
 jsr .hero_draw_n

.skipL0801
.L0802 ;  if hero_flags{1} then gosub hero_draw_e

	LDA hero_flags
	AND #2
	BEQ .skipL0802
.condpart126
 jsr .hero_draw_e

.skipL0802
.L0803 ;  if hero_flags{2} then gosub hero_draw_s

	LDA hero_flags
	AND #4
	BEQ .skipL0803
.condpart127
 jsr .hero_draw_s

.skipL0803
.L0804 ;  if hero_flags{3} then gosub hero_draw_w

	LDA hero_flags
	AND #8
	BEQ .skipL0804
.condpart128
 jsr .hero_draw_w

.skipL0804
.L0805 ;  goto player_animate_end

 jmp .player_animate_end

.player_animate_2
 ; player_animate_2

.L0806 ;  if hero_flags{0} then gosub hero_draw_n2

	LDA hero_flags
	LSR
	BCC .skipL0806
.condpart129
 jsr .hero_draw_n2

.skipL0806
.L0807 ;  if hero_flags{1} then gosub hero_draw_e2

	LDA hero_flags
	AND #2
	BEQ .skipL0807
.condpart130
 jsr .hero_draw_e2

.skipL0807
.L0808 ;  if hero_flags{2} then gosub hero_draw_s2

	LDA hero_flags
	AND #4
	BEQ .skipL0808
.condpart131
 jsr .hero_draw_s2

.skipL0808
.L0809 ;  if hero_flags{3} then gosub hero_draw_w2

	LDA hero_flags
	AND #8
	BEQ .skipL0809
.condpart132
 jsr .hero_draw_w2

.skipL0809
.player_animate_end
 ; player_animate_end

.
 ; 

.L0810 ;  rem 'Reflect sprite if moving west, otherwise, don't

.L0811 ;  if hero_flags{3} then REFP0  =  8 else REFP0  =  0

	LDA hero_flags
	AND #8
	BEQ .skipL0811
.condpart133
	LDA #8
	STA REFP0
 jmp .skipelse2
.skipL0811
	LDA #0
	STA REFP0
.skipelse2
.
 ; 

.
 ; 

.
 ; 

.L0812 ;  rem draw screen

.L0813 ;  drawscreen

 sta temp7
 lda #>(ret_point22-1)
 pha
 lda #<(ret_point22-1)
 pha
 lda #>(drawscreen-1)
 pha
 lda #<(drawscreen-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point22
.
 ; 

.L0814 ;  rem And continue main loop

.L0815 ;  goto main_loop

 jmp .main_loop

.
 ; 

.L0816 ;  rem 'Vblank sub. Do something useful in here...

.vblank
 ; vblank

.L0817 ;  rem 'Do something here...

.L0818 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.L0819 ;  rem 'Gosub that correctly displays MP

.set_mp_display
 ; set_mp_display

.L0820 ;  lives = hero_mp  *  32

	LDA hero_mp
	asl
	asl
	asl
	asl
	asl
	STA lives
.L0821 ;  lives:

	LDA #<lives__L0821
	STA lifepointer
	LDA lifepointer+1
	AND #$E0
	ORA #(>lives__L0821)&($1F)
	STA lifepointer+1
.L0822 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.
 ; 

.L0823 ;  rem 'Display inventory ggosub

.show_inventory
 ; show_inventory

.L0824 ;  sc0  =  0

	LDA #0
	STA sc0
.L0825 ;  sc1  =  0

	LDA #0
	STA sc1
.L0826 ;  sc2  =  0

	LDA #0
	STA sc2
.L0827 ;  rem 'Add up score to display Numen Leaves

.L0828 ;  if hero_items{0} then sc0  =  sc0  +  16

	LDA hero_items
	LSR
	BCC .skipL0828
.condpart134
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0828
.L0829 ;  if hero_items{1} then sc0  =  sc0  +  16

	LDA hero_items
	AND #2
	BEQ .skipL0829
.condpart135
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0829
.L0830 ;  if hero_items{2} then sc0  =  sc0  +  16

	LDA hero_items
	AND #4
	BEQ .skipL0830
.condpart136
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0830
.L0831 ;  rem 'Display shield if we have it

.L0832 ;  if hero_items{3} then sc0  =  sc0  +  4

	LDA hero_items
	AND #8
	BEQ .skipL0832
.condpart137
	LDA sc0
	CLC
	ADC #4
	STA sc0
.skipL0832
.L0833 ;  rem 'Display Armor

.L0834 ;  if hero_items{4} then sc1  =  sc1  +  80

	LDA hero_items
	AND #16
	BEQ .skipL0834
.condpart138
	LDA sc1
	CLC
	ADC #80
	STA sc1
.skipL0834
.L0835 ;  rem 'Display Sword

.L0836 ;  if hero_items{5} then sc1  =  sc1  +  6

	LDA hero_items
	AND #32
	BEQ .skipL0836
.condpart139
	LDA sc1
	CLC
	ADC #6
	STA sc1
.skipL0836
.L0837 ;  if hero_items{6} then sc2  =  sc2  +  112

	BIT hero_items
	BVC .skipL0837
.condpart140
	LDA sc2
	CLC
	ADC #112
	STA sc2
.skipL0837
.L0838 ;  if hero_items{7} then sc2  =  sc2  +  8

	BIT hero_items
	BPL .skipL0838
.condpart141
	LDA sc2
	CLC
	ADC #8
	STA sc2
.skipL0838
.L0839 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.
 ; 

.L0840 ;  rem 'Item Pickup gosub

.item_pickup
 ; item_pickup

.L0841 ;  rem 'All sorts of item pickup effects, should be in a gosub, I guess

.L0842 ;  rem 'Win game if finding victory item

.L0843 ;  rem 'Make a pickup sound

.L0844 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0845 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0846 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0847 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0848 ;  if item_kind  =  item_victory then goto game_win bank5

	LDA item_kind
	CMP #item_victory
     BNE .skipL0848
.condpart142
 sta temp7
 lda #>(.game_win-1)
 pha
 lda #<(.game_win-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #5
 jmp BS_jsr
.skipL0848
.L0849 ;  if item_kind  =  item_leaf1 then hero_items{0}  =  1

	LDA item_kind
	CMP #item_leaf1
     BNE .skipL0849
.condpart143
	LDA hero_items
	ORA #1
	STA hero_items
.skipL0849
.L0850 ;  if item_kind  =  item_leaf2 then hero_items{1}  =  1

	LDA item_kind
	CMP #item_leaf2
     BNE .skipL0850
.condpart144
	LDA hero_items
	ORA #2
	STA hero_items
.skipL0850
.L0851 ;  if item_kind  =  item_leaf3 then hero_items{2}  =  1

	LDA item_kind
	CMP #item_leaf3
     BNE .skipL0851
.condpart145
	LDA hero_items
	ORA #4
	STA hero_items
.skipL0851
.L0852 ;  if item_kind  =  item_shield then hero_items{3}  =  1

	LDA item_kind
	CMP #item_shield
     BNE .skipL0852
.condpart146
	LDA hero_items
	ORA #8
	STA hero_items
.skipL0852
.L0853 ;  if item_kind  =  item_armor then hero_items{4}  =  1

	LDA item_kind
	CMP #item_armor
     BNE .skipL0853
.condpart147
	LDA hero_items
	ORA #16
	STA hero_items
.skipL0853
.L0854 ;  if item_kind  =  item_sword then hero_items{5}  =  1

	LDA item_kind
	CMP #item_sword
     BNE .skipL0854
.condpart148
	LDA hero_items
	ORA #32
	STA hero_items
.skipL0854
.L0855 ;  if item_kind  =  item_bookheal then hero_items{6}  =  1

	LDA item_kind
	CMP #item_bookheal
     BNE .skipL0855
.condpart149
	LDA hero_items
	ORA #64
	STA hero_items
.skipL0855
.L0856 ;  if item_kind  =  item_bookstrike then hero_items{7}  =  1

	LDA item_kind
	CMP #item_bookstrike
     BNE .skipL0856
.condpart150
	LDA hero_items
	ORA #128
	STA hero_items
.skipL0856
.L0857 ;  if item_kind  <>  item_healhp then goto no_healhp

	LDA item_kind
	CMP #item_healhp
     BEQ .skipL0857
.condpart151
 jmp .no_healhp

.skipL0857
.L0858 ;  hero_hp  =  hero_hp  +  8

	LDA hero_hp
	CLC
	ADC #8
	STA hero_hp
.L0859 ;  temp2  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA temp2
.L0860 ;  if hero_hp  >  temp2 then hero_hp  =  temp2

	LDA temp2
	CMP hero_hp
     BCS .skipL0860
.condpart152
	LDA temp2
	STA hero_hp
.skipL0860
.no_healhp
 ; no_healhp

.L0861 ;  if item_kind  <>  item_healmp then goto no_healmp

	LDA item_kind
	CMP #item_healmp
     BEQ .skipL0861
.condpart153
 jmp .no_healmp

.skipL0861
.L0862 ;  hero_mp  =  hero_mp  +  1

	INC hero_mp
.L0863 ;  temp2  =  hero_level  /  16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA temp2
.L0864 ;  if hero_mp  >  temp2 then hero_mp  =  temp2

	LDA temp2
	CMP hero_mp
     BCS .skipL0864
.condpart154
	LDA temp2
	STA hero_mp
.skipL0864
.L0865 ;  gosub set_mp_display

 jsr .set_mp_display

.no_healmp
 ; no_healmp

.L0866 ;  if item_kind  <>  item_healallhp then goto no_healallhp

	LDA item_kind
	CMP #item_healallhp
     BEQ .skipL0866
.condpart155
 jmp .no_healallhp

.skipL0866
.L0867 ;  hero_hp  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.no_healallhp
 ; no_healallhp

.L0868 ;  if item_kind  <>  item_healallmp then goto no_healallmp

	LDA item_kind
	CMP #item_healallmp
     BEQ .skipL0868
.condpart156
 jmp .no_healallmp

.skipL0868
.L0869 ;  rem 'Actually, this is buggy for levels above 80

.L0870 ;  hero_mp  =  hero_level / 16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0871 ;  gosub set_mp_display

 jsr .set_mp_display

.no_healallmp
 ; no_healallmp

.L0872 ;  rem " All done with item effects 

.
 ; 

.L0873 ;  rem ' Remove item

.L0874 ;  item_kind  =  item_none

	LDA #item_none
	STA item_kind
.L0875 ;  item_hp  =  0

	LDA #0
	STA item_hp
.L0876 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L0877 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
.L0878 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.
 ; 

.hero_draw_n
 ; hero_draw_n

.L0879 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0880 ;  player0:

	LDA #<playerL0880_0

	STA player0pointerlo
	LDA #>playerL0880_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0881 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.hero_draw_s
 ; hero_draw_s

.L0882 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0883 ;  player0:

	LDA #<playerL0883_0

	STA player0pointerlo
	LDA #>playerL0883_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0884 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.hero_draw_w
 ; hero_draw_w

.L0885 ;  REFP0 = 8

	LDA #8
	STA REFP0
.L0886 ;  goto hero_draw_e_start

 jmp .hero_draw_e_start

.hero_draw_e
 ; hero_draw_e

.L0887 ;  REFP0 = 0

	LDA #0
	STA REFP0
.hero_draw_e_start
 ; hero_draw_e_start

.L0888 ;  player0:

	LDA #<playerL0888_0

	STA player0pointerlo
	LDA #>playerL0888_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0889 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.hero_draw_n2
 ; hero_draw_n2

.L0890 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0891 ;  player0:

	LDA #<playerL0891_0

	STA player0pointerlo
	LDA #>playerL0891_0

	STA player0pointerhi
	LDA #14
	STA player0height
.L0892 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.hero_draw_s2
 ; hero_draw_s2

.L0893 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0894 ;  player0:

	LDA #<playerL0894_0

	STA player0pointerlo
	LDA #>playerL0894_0

	STA player0pointerhi
	LDA #14
	STA player0height
.L0895 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.hero_draw_w2
 ; hero_draw_w2

.L0896 ;  REFP0 = 8

	LDA #8
	STA REFP0
.L0897 ;  goto hero_draw_e2_start

 jmp .hero_draw_e2_start

.hero_draw_e2
 ; hero_draw_e2

.L0898 ;  REFP0 = 0

	LDA #0
	STA REFP0
.hero_draw_e2_start
 ; hero_draw_e2_start

.L0899 ;  player0:

	LDA #<playerL0899_0

	STA player0pointerlo
	LDA #>playerL0899_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0900 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.L0901 ;  bank 2

 echo "    ",[(start_bank1 - *)]d , "bytes of ROM space left in bank 1")
 ORG $1FF4-bscode_length
 RORG $1FF4-bscode_length
start_bank1 ldx #$ff
 txs
 lda #>(start-1)
 pha
 lda #<(start-1)
 pha
 pha
 txa
 pha
 tsx
 lda 4,x ; get high byte of return address
 rol   
 rol
 rol
 rol
 and #bs_mask ;1 3 or 7 for F8/F6/F4
 tax
 inx
 lda bankswitch_hotspot-1,x
 pla
 tax
 pla
 rts
 if ((* & $1FFF) > ((bankswitch_hotspot & $1FFF) - 1))
   echo "WARNING: size parameter in banksw.asm too small - the program probably will not work."
   echo "Change to",[(*-begin_bscode+1)&$FF]d,"and try again."
 endif
 ORG $1FFC
 RORG $1FFC
 .word start_bank1
 .word start_bank1
 ORG $2000
 RORG $3000
.L0902 ;  set kernel_options pfcolors

.room_draw
 ; room_draw

.L0903 ;  rem 'Reset the monster missile and item flags

.L0904 ;  missile1x  =  nowhere

	LDA #nowhere
	STA missile1x
.L0905 ;  missile1y  =  nowhere

	LDA #nowhere
	STA missile1y
.L0906 ;  item_flags  =  0

	LDA #0
	STA item_flags
.L0907 ;  rem 'Set up item and draw it

.L0908 ;  goto item_setup bank4

 sta temp7
 lda #>(.item_setup-1)
 pha
 lda #<(.item_setup-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #4
 jmp BS_jsr
.item_setup_done
 ; item_setup_done

.L0909 ;  if hero_room  >  39 goto room_draw_40_local

	LDA #39
	CMP hero_room
 if ((* - .room_draw_40_local) < 127) && ((* - .room_draw_40_local) > -128)
	bcc .room_draw_40_local
 else
	bcs .9skiproom_draw_40_local
	jmp .room_draw_40_local
.9skiproom_draw_40_local
 endif
.L0910 ;  if hero_room  >  63 goto room_draw_64

	LDA #63
	CMP hero_room
 if ((* - .room_draw_64) < 127) && ((* - .room_draw_64) > -128)
	bcc .room_draw_64
 else
	bcs .10skiproom_draw_64
	jmp .room_draw_64
.10skiproom_draw_64
 endif
.L0911 ;  on hero_room goto r00 r01 r02 r03 r04 r05 r06 r07 r08 r09 r10 r11 r12 r13 r14 r15 r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31 r32 r33 r34 r35 r36 r37 r38 r39 r40

	LDX hero_room
	LDA .L0911jumptablehi,x
	PHA
	LDA .L0911jumptablelo,x
	PHA
	RTS
.L0911jumptablehi
	.byte >(.r00-1)
	.byte >(.r01-1)
	.byte >(.r02-1)
	.byte >(.r03-1)
	.byte >(.r04-1)
	.byte >(.r05-1)
	.byte >(.r06-1)
	.byte >(.r07-1)
	.byte >(.r08-1)
	.byte >(.r09-1)
	.byte >(.r10-1)
	.byte >(.r11-1)
	.byte >(.r12-1)
	.byte >(.r13-1)
	.byte >(.r14-1)
	.byte >(.r15-1)
	.byte >(.r16-1)
	.byte >(.r17-1)
	.byte >(.r18-1)
	.byte >(.r19-1)
	.byte >(.r20-1)
	.byte >(.r21-1)
	.byte >(.r22-1)
	.byte >(.r23-1)
	.byte >(.r24-1)
	.byte >(.r25-1)
	.byte >(.r26-1)
	.byte >(.r27-1)
	.byte >(.r28-1)
	.byte >(.r29-1)
	.byte >(.r30-1)
	.byte >(.r31-1)
	.byte >(.r32-1)
	.byte >(.r33-1)
	.byte >(.r34-1)
	.byte >(.r35-1)
	.byte >(.r36-1)
	.byte >(.r37-1)
	.byte >(.r38-1)
	.byte >(.r39-1)
	.byte >(.r40-1)
.L0911jumptablelo
	.byte <(.r00-1)
	.byte <(.r01-1)
	.byte <(.r02-1)
	.byte <(.r03-1)
	.byte <(.r04-1)
	.byte <(.r05-1)
	.byte <(.r06-1)
	.byte <(.r07-1)
	.byte <(.r08-1)
	.byte <(.r09-1)
	.byte <(.r10-1)
	.byte <(.r11-1)
	.byte <(.r12-1)
	.byte <(.r13-1)
	.byte <(.r14-1)
	.byte <(.r15-1)
	.byte <(.r16-1)
	.byte <(.r17-1)
	.byte <(.r18-1)
	.byte <(.r19-1)
	.byte <(.r20-1)
	.byte <(.r21-1)
	.byte <(.r22-1)
	.byte <(.r23-1)
	.byte <(.r24-1)
	.byte <(.r25-1)
	.byte <(.r26-1)
	.byte <(.r27-1)
	.byte <(.r28-1)
	.byte <(.r29-1)
	.byte <(.r30-1)
	.byte <(.r31-1)
	.byte <(.r32-1)
	.byte <(.r33-1)
	.byte <(.r34-1)
	.byte <(.r35-1)
	.byte <(.r36-1)
	.byte <(.r37-1)
	.byte <(.r38-1)
	.byte <(.r39-1)
	.byte <(.r40-1)
.L0912 ;  rem r41 r42 r43 r44 r45 r46 r47 r48 r49 r50

.L0913 ;  rem r51 r52 r53 r54 r55 r56 r57 r58 r59 r60 r61 r62 r63 r64

.room_draw_40_local
 ; room_draw_40_local

.L0914 ;  goto room_draw_40 bank3

 sta temp7
 lda #>(.room_draw_40-1)
 pha
 lda #<(.room_draw_40-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #3
 jmp BS_jsr
.room_draw_64
 ; room_draw_64

.L0915 ;  rem 'If we get here, the room number s not valid. Go to room room_start (49)

.L0916 ;  hero_room  =  room_start

	LDA #room_start
	STA hero_room
.L0917 ;  goto r00

 jmp .r00

.L0918 ;  rem r64

.L0919 ;  rem r65 r66 r67 r68 r69 r70 r71 r72 r73 r74 r75 r76 r77 r78 r79 r80 r81 r82 r83 r84 r85 r86 r87 r88 r89 r90 r91 r92 r93 r94 r95 r96 r97 r98 r99 r100

.room_draw_end
 ; room_draw_end

.L0920 ;  rem 'Clear bottom scrolling playfield row, so collision detection works properly

.L0921 ;  var44  =  0

	LDA #0
	STA var44
.L0922 ;  var45  =  0

	LDA #0
	STA var45
.L0923 ;  var46  =  0

	LDA #0
	STA var46
.L0924 ;  var47  =  0

	LDA #0
	STA var47
.L0925 ;  return otherbank

	JMP BS_return
.r00
 ; r00

.L0926 ;  pfcolors:

 lda # skyblue
 sta COLUPF
 lda #>(pfcolorlabel139-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-84)
 sta pfcolortable
.L0927 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel0
PF_data0
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %11111111, %00111111, %00111111, %11111111
pflabel0
	lda PF_data0,x
	sta playfield,x
	dex
	bpl pflabel0
.L0928 ;  goto room_draw_end

 jmp .room_draw_end

.r01
 ; r01

.L0929 ;  if quest_flags{1} then COLUBK = gray else COLUBK = black

	LDA quest_flags
	AND #2
	BEQ .skipL0929
.condpart157
	LDA gray
	STA COLUBK
 jmp .skipelse3
.skipL0929
	LDA black
	STA COLUBK
.skipelse3
.L0930 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel139-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-83)
 sta pfcolortable
.L0931 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel1
PF_data1
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000001, %00000001, %00000001, %00000001
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %11111111, %11111111, %11111111, %11111111
pflabel1
	lda PF_data1,x
	sta playfield,x
	dex
	bpl pflabel1
.L0932 ;  goto room_draw_end

 jmp .room_draw_end

.r02
 ; r02

.L0933 ;  if quest_flags{1} then COLUBK = gray else COLUBK = black

	LDA quest_flags
	AND #2
	BEQ .skipL0933
.condpart158
	LDA gray
	STA COLUBK
 jmp .skipelse4
.skipL0933
	LDA black
	STA COLUBK
.skipelse4
.L0934 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel139-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-82)
 sta pfcolortable
.L0935 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel2
PF_data2
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %10000000, %00000110, %00000000, %10001100
	.byte %10000000, %00000110, %00000000, %10001100
	.byte %10001100, %11000110, %11111111, %10001100
	.byte %00001100, %01000110, %00000000, %10000000
	.byte %00001100, %01000110, %00000000, %10000000
	.byte %00001100, %01000000, %00000011, %11111111
	.byte %11111111, %11111111, %11100000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %11111111, %00111111, %00111111, %11111111
pflabel2
	lda PF_data2,x
	sta playfield,x
	dex
	bpl pflabel2
.L0936 ;  goto room_draw_end

 jmp .room_draw_end

.r03
 ; r03

.L0937 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L0938 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel139-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-81)
 sta pfcolortable
.L0939 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel3
PF_data3
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %10000000, %11111100, %11111100, %00000000
	.byte %10000000, %11111100, %11111100, %00000000
	.byte %10000000, %11111100, %11111100, %00000000
	.byte %10000000, %11111100, %11111100, %00000000
	.byte %10000000, %11111100, %11111100, %00000000
	.byte %10000000, %11111110, %11111110, %00000000
	.byte %11000000, %11111111, %11111111, %00000000
	.byte %11000001, %11111111, %11111111, %00000001
	.byte %11000011, %11111111, %11111111, %00000111
	.byte %11000001, %11111111, %11111111, %00000001
pflabel3
	lda PF_data3,x
	sta playfield,x
	dex
	bpl pflabel3
.L0940 ;  goto room_draw_end

 jmp .room_draw_end

.r04
 ; r04

.L0941 ;  COLUBK = gray

	LDA gray
	STA COLUBK
.L0942 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel156-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-84)
 sta pfcolortable
.L0943 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel4
PF_data4
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %10000000, %00000000, %10000000
	.byte %00110000, %10110000, %01100000, %10011000
	.byte %00110000, %11001000, %10010000, %10011000
	.byte %00000000, %10110000, %01100000, %10000000
	.byte %00000000, %10000000, %00000000, %10000000
	.byte %00110000, %00000011, %00000110, %10011000
	.byte %00110000, %00000011, %00000110, %10011000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
pflabel4
	lda PF_data4,x
	sta playfield,x
	dex
	bpl pflabel4
.L0944 ;  goto room_draw_end

 jmp .room_draw_end

.r05
 ; r05

.L0945 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0946 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel156-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-83)
 sta pfcolortable
.L0947 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel5
PF_data5
	.byte %11111111, %11111100, %11110011, %11111111
	.byte %10111100, %11111110, %11111000, %11111111
	.byte %10000000, %11111111, %11111110, %10000000
	.byte %10000001, %11111111, %11111100, %10000000
	.byte %10000000, %11111111, %11111100, %10000000
	.byte %10000000, %11110000, %11000000, %10000000
	.byte %10000000, %11110000, %11000000, %10000000
	.byte %10000000, %00001000, %00100000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
pflabel5
	lda PF_data5,x
	sta playfield,x
	dex
	bpl pflabel5
.L0948 ;  goto room_draw_end

 jmp .room_draw_end

.r06
 ; r06

.L0949 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0950 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel156-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-82)
 sta pfcolortable
.L0951 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel6
PF_data6
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11000000, %00000110, %00000000, %00000000
	.byte %11000000, %00000110, %00000000, %00000000
	.byte %11000110, %10000110, %11111111, %11111111
	.byte %11000110, %10000000, %10000000, %00010000
	.byte %11000110, %10000000, %10000000, %00010000
	.byte %11000111, %10000111, %10000001, %11110001
	.byte %11000111, %00000000, %00000001, %00000001
	.byte %11000011, %00000000, %00000001, %00000001
	.byte %11000011, %11111000, %11110001, %00000001
	.byte %11000011, %11111000, %11110001, %11000001
pflabel6
	lda PF_data6,x
	sta playfield,x
	dex
	bpl pflabel6
.L0952 ;  goto room_draw_end

 jmp .room_draw_end

.r07
 ; r07

.L0953 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0954 ;  pfcolors:

 lda # darkgreen
 sta COLUPF
 lda #>(pfcolorlabel156-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-81)
 sta pfcolortable
.L0955 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel7
PF_data7
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %00000000, %00100000, %00000000, %10000000
	.byte %00000000, %00100000, %00000000, %10000000
	.byte %11111111, %00100001, %01111111, %10000011
	.byte %00000000, %00100000, %01110000, %10000000
	.byte %00000000, %00100000, %01110000, %10000000
	.byte %11111100, %00111000, %01110001, %10000011
	.byte %00000100, %00111000, %01110001, %10000011
	.byte %00000100, %00111000, %01110001, %10000011
	.byte %00000100, %00111000, %01110001, %10000011
	.byte %11000100, %00111000, %01110001, %10000011
pflabel7
	lda PF_data7,x
	sta playfield,x
	dex
	bpl pflabel7
.L0956 ;  goto room_draw_end

 jmp .room_draw_end

.r08
 ; r08

.L0957 ;  COLUBK = brown

	LDA brown
	STA COLUBK
.L0958 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel173-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-84)
 sta pfcolortable
.L0959 ;  if quest_flags{3} goto r08_open

	LDA quest_flags
	AND #8
 if ((* - .r08_open) < 127) && ((* - .r08_open) > -128)
	BNE .r08_open
 else
	beq .11skipr08_open
	jmp .r08_open
.11skipr08_open
 endif
.L0960 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel8
PF_data8
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %00111111, %00111111, %11111111
	.byte %11110000, %00000000, %00000000, %11111110
	.byte %11000000, %00000000, %00000000, %11111100
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %11111000, %00000000, %00000001, %11111111
	.byte %11111110, %00000000, %00000111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel8
	lda PF_data8,x
	sta playfield,x
	dex
	bpl pflabel8
.L0961 ;  goto r08_end

 jmp .r08_end

.r08_open
 ; r08_open

.L0962 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel9
PF_data9
	.byte %11111111, %00111111, %00111111, %11111111
	.byte %11111111, %00111111, %00111111, %11111111
	.byte %11111111, %00111111, %00111111, %11111111
	.byte %11110000, %00000000, %00000000, %11111110
	.byte %11000000, %00000000, %00000000, %11111100
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %11111000, %00000000, %00000001, %11111111
	.byte %11111110, %00000000, %00000111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel9
	lda PF_data9,x
	sta playfield,x
	dex
	bpl pflabel9
.r08_end
 ; r08_end

.L0963 ;  goto room_draw_end

 jmp .room_draw_end

.r09
 ; r09

.L0964 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0965 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel173-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-83)
 sta pfcolortable
.L0966 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel10
PF_data10
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11100000, %00000000
	.byte %11111111, %00000111, %00000000, %00000000
	.byte %11111111, %00000001, %00000000, %11100000
	.byte %11111110, %11110000, %11111111, %11111111
	.byte %00011000, %00111000, %00000000, %00000000
	.byte %00000000, %00011000, %00000000, %00000000
	.byte %00000011, %00001111, %00111100, %11111100
	.byte %11000111, %00000000, %11111110, %00000000
	.byte %11111110, %11000000, %11111111, %00000000
	.byte %11111100, %11100000, %11111111, %11100001
pflabel10
	lda PF_data10,x
	sta playfield,x
	dex
	bpl pflabel10
.L0967 ;  goto room_draw_end

 jmp .room_draw_end

.r10
 ; r10

.L0968 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0969 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel173-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-82)
 sta pfcolortable
.L0970 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel11
PF_data11
	.byte %11111111, %00111111, %00111111, %11111111
	.byte %00000000, %00110000, %00000000, %10000000
	.byte %00000000, %01100000, %00000000, %10000000
	.byte %11100000, %11000000, %00000110, %10000000
	.byte %11111111, %10000011, %10001100, %11000000
	.byte %00000000, %00000000, %00011000, %11110000
	.byte %00000000, %00000000, %00110000, %00111100
	.byte %11111111, %11111111, %11100000, %00001111
	.byte %00000000, %00000000, %00000111, %00000111
	.byte %00000000, %00000000, %00001111, %00000000
	.byte %11111111, %11111111, %11111110, %00000000
pflabel11
	lda PF_data11,x
	sta playfield,x
	dex
	bpl pflabel11
.L0971 ;  goto room_draw_end

 jmp .room_draw_end

.r11
 ; r11

.L0972 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0973 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel173-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-81)
 sta pfcolortable
.L0974 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel12
PF_data12
	.byte %11000000, %11111110, %11111110, %00000000
	.byte %11000000, %11111110, %11111110, %00000000
	.byte %11000000, %11111100, %11111100, %00000000
	.byte %11000000, %11111100, %11111100, %00000000
	.byte %11000000, %11111100, %11111100, %00000000
	.byte %11000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
pflabel12
	lda PF_data12,x
	sta playfield,x
	dex
	bpl pflabel12
.L0975 ;  goto room_draw_end

 jmp .room_draw_end

.r12
 ; r12

.L0976 ;  COLUBK = gray

	LDA gray
	STA COLUBK
.L0977 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel190-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-84)
 sta pfcolortable
.L0978 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel13
PF_data13
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00110000, %00000011, %00000110, %10011000
	.byte %00110000, %00000011, %00000110, %10011000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00110000, %00000011, %00000110, %10011000
	.byte %00110000, %00000011, %00000110, %10011000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
pflabel13
	lda PF_data13,x
	sta playfield,x
	dex
	bpl pflabel13
.L0979 ;  goto room_draw_end

 jmp .room_draw_end

.r13
 ; r13

.L0980 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0981 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-83)
 sta pfcolortable
.L0982 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel14
PF_data14
	.byte %11111111, %00111111, %01111111, %11111111
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %11111111, %11111111, %11111111, %11111111
pflabel14
	lda PF_data14,x
	sta playfield,x
	dex
	bpl pflabel14
.L0983 ;  goto room_draw_end

 jmp .room_draw_end

.r14
 ; r14

.L0984 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0985 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-82)
 sta pfcolortable
.L0986 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel15
PF_data15
	.byte %11100011, %11111000, %11110001, %11000001
	.byte %11100011, %11111000, %11110001, %00000001
	.byte %11100011, %00000000, %00000001, %00000001
	.byte %11100011, %00000000, %00000001, %00000001
	.byte %11100011, %11111111, %11110001, %11111111
	.byte %11100000, %00000000, %00000000, %00000000
	.byte %11100000, %00000000, %00000000, %00000000
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %11111111, %11111111, %11111111, %11111111
pflabel15
	lda PF_data15,x
	sta playfield,x
	dex
	bpl pflabel15
.L0987 ;  goto room_draw_end

 jmp .room_draw_end

.r15
 ; r15

.L0988 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-81)
 sta pfcolortable
.L0989 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel16
PF_data16
	.byte %11000111, %00111000, %01110001, %10000011
	.byte %00000111, %00111000, %01110001, %10000011
	.byte %00000111, %00000000, %00000000, %10000000
	.byte %00000111, %00000000, %00000000, %10000000
	.byte %11111111, %11111111, %11111111, %10000111
	.byte %00001110, %00000000, %00000000, %10000000
	.byte %00001110, %00000000, %00000000, %10000000
	.byte %11001110, %11111000, %11111111, %11111111
	.byte %00001110, %00001000, %00000000, %10000000
	.byte %00001110, %00001000, %00000000, %10000000
	.byte %11111110, %11111000, %11111111, %10000011
pflabel16
	lda PF_data16,x
	sta playfield,x
	dex
	bpl pflabel16
.L0990 ;  goto room_draw_end

 jmp .room_draw_end

.r16
 ; r16

.L0991 ;  COLUBK = yellow

	LDA yellow
	STA COLUBK
.L0992 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel207-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-84)
 sta pfcolortable
.L0993 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel17
PF_data17
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10000000, %00000000, %00000000, %10000000
pflabel17
	lda PF_data17,x
	sta playfield,x
	dex
	bpl pflabel17
.L0994 ;  goto room_draw_end

 jmp .room_draw_end

.r17
 ; r17

.L0995 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0996 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel207-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-83)
 sta pfcolortable
.L0997 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel18
PF_data18
	.byte %11111100, %11100000, %11111111, %11100001
	.byte %11111111, %10000000, %11111110, %11000000
	.byte %11111111, %00000011, %00001100, %00000000
	.byte %11111111, %00011111, %00000110, %00000000
	.byte %11111111, %11111111, %00000011, %00000000
	.byte %11111111, %11111111, %11000011, %00000000
	.byte %11111111, %11111111, %00001111, %00000000
	.byte %11111111, %01111111, %00011110, %00000000
	.byte %11111111, %11111111, %10000000, %00000000
	.byte %11111111, %11111111, %11100000, %00000000
	.byte %11111111, %11111111, %11111100, %00000000
pflabel18
	lda PF_data18,x
	sta playfield,x
	dex
	bpl pflabel18
.L0998 ;  goto room_draw_end

 jmp .room_draw_end

.r18
 ; r18

.L0999 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01000 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel207-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-82)
 sta pfcolortable
.L01001 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel19
PF_data19
	.byte %11111111, %11111111, %11111110, %00000000
	.byte %11111111, %11111111, %11100000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00011111, %11111111, %10000000, %00000000
	.byte %00111111, %11111111, %11000000, %00000000
	.byte %01111111, %11111111, %11100000, %00000000
	.byte %00111111, %11111111, %11000000, %00000000
	.byte %00110011, %00111000, %11000000, %00000000
	.byte %00111111, %11111000, %11000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
pflabel19
	lda PF_data19,x
	sta playfield,x
	dex
	bpl pflabel19
.L01002 ;  goto room_draw_end

 jmp .room_draw_end

.r19
 ; r19

.L01003 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01004 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel207-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-81)
 sta pfcolortable
.L01005 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel20
PF_data20
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111110, %00000000
	.byte %00000000, %11111100, %11111110, %00000000
	.byte %00000000, %11111110, %11111110, %00000000
	.byte %00000000, %11111110, %11111110, %00000000
	.byte %00000000, %11111110, %11111100, %00000000
	.byte %00000000, %11111110, %11111100, %00000000
	.byte %00000000, %11111110, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
pflabel20
	lda PF_data20,x
	sta playfield,x
	dex
	bpl pflabel20
.L01006 ;  goto room_draw_end

 jmp .room_draw_end

.r20
 ; r20

.L01007 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01008 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-84)
 sta pfcolortable
.L01009 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel21
PF_data21
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00001010, %11000001, %01101100, %10101000
	.byte %00001111, %11000001, %11111100, %11111000
	.byte %00001000, %01000001, %00000100, %10001000
	.byte %00001000, %01111111, %00000111, %10001111
	.byte %00001000, %00000000, %00000000, %00000000
	.byte %00001000, %00000000, %00000000, %00000000
	.byte %00001000, %11111111, %11111111, %11111111
	.byte %00001000, %00000000, %01100000, %00000000
	.byte %00001000, %00000000, %01100000, %00000000
	.byte %00001000, %00000000, %01100000, %11000000
pflabel21
	lda PF_data21,x
	sta playfield,x
	dex
	bpl pflabel21
.L01010 ;  goto room_draw_end

 jmp .room_draw_end

.r21
 ; r21

.L01011 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01012 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-83)
 sta pfcolortable
.L01013 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel22
PF_data22
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %00001010, %10000001, %01101000, %10101000
	.byte %00001111, %10000001, %11111000, %11111000
	.byte %00001000, %11111111, %00001111, %10001111
	.byte %11111000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %11111000, %00000000, %00000000, %10000000
	.byte %00001000, %00000000, %00000000, %10000000
	.byte %00001000, %00000000, %00000000, %10000000
	.byte %11001111, %11111111, %11111111, %11111111
pflabel22
	lda PF_data22,x
	sta playfield,x
	dex
	bpl pflabel22
.L01014 ;  goto room_draw_end

 jmp .room_draw_end

.r22
 ; r22

.L01015 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01016 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-82)
 sta pfcolortable
.L01017 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel23
PF_data23
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %01110001, %00111111, %11111111
	.byte %11111110, %00000000, %00001111, %11111111
	.byte %11111100, %00000000, %00000011, %11111111
	.byte %11110000, %00000000, %00000000, %11111111
	.byte %11110000, %00000000, %00000000, %11111000
	.byte %11100000, %00000000, %00000000, %11110000
	.byte %11000010, %00000000, %00000100, %11100000
	.byte %11000111, %00000001, %00001111, %00000000
	.byte %10000010, %00000000, %00000100, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
pflabel23
	lda PF_data23,x
	sta playfield,x
	dex
	bpl pflabel23
.L01018 ;  goto room_draw_end

 jmp .room_draw_end

.r23
 ; r23

.L01019 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01020 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-81)
 sta pfcolortable
.L01021 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel24
PF_data24
	.byte %11111110, %11111100, %11111111, %10000011
	.byte %11111110, %11111100, %11111111, %10000011
	.byte %11111110, %11111000, %11111111, %10000000
	.byte %11111110, %11110000, %11111110, %10000000
	.byte %11111110, %00000000, %00000000, %10000000
	.byte %11111110, %00000000, %00000000, %10000000
	.byte %11111111, %11111111, %00111111, %11111111
	.byte %00000100, %00010000, %00001000, %10000000
	.byte %00001111, %01111000, %00011110, %10000000
	.byte %00000100, %00010000, %00001000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
pflabel24
	lda PF_data24,x
	sta playfield,x
	dex
	bpl pflabel24
.L01022 ;  goto room_draw_end

 jmp .room_draw_end

.r24
 ; r24

.L01023 ;  COLUBK = yellow

	LDA yellow
	STA COLUBK
.L01024 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel241-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-84)
 sta pfcolortable
.L01025 ;  if quest_flags{5} then goto r24_open

	LDA quest_flags
	AND #32
	BEQ .skipL01025
.condpart159
 jmp .r24_open

.skipL01025
.L01026 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel25
PF_data25
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10000000, %11000000, %11000000, %10000000
	.byte %10001100, %11100000, %11100000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000001, %00111111, %00111111, %11000001
pflabel25
	lda PF_data25,x
	sta playfield,x
	dex
	bpl pflabel25
.L01027 ;  goto r24_end

 jmp .r24_end

.r24_open
 ; r24_open

.L01028 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel26
PF_data26
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10000000, %11000000, %11000000, %10000000
	.byte %10001100, %11100000, %11100000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10001100, %00000000, %00000000, %10001100
	.byte %10000000, %00000000, %00000000, %10000000
	.byte %10000001, %00111111, %00111111, %11000001
pflabel26
	lda PF_data26,x
	sta playfield,x
	dex
	bpl pflabel26
.r24_end
 ; r24_end

.L01029 ;  goto room_draw_end

 jmp .room_draw_end

.r25
 ; r25

.L01030 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01031 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel241-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-83)
 sta pfcolortable
.L01032 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel27
PF_data27
	.byte %11111111, %11111111, %11111100, %00000000
	.byte %11111111, %11111111, %11110000, %00000000
	.byte %11111111, %11111111, %11011111, %00011111
	.byte %11111111, %01111111, %00111111, %00111111
	.byte %11111111, %00011111, %01111111, %01111111
	.byte %11111111, %00000111, %00111111, %00111111
	.byte %11111111, %00000001, %00111001, %00110001
	.byte %11111100, %00000000, %00111111, %00110001
	.byte %11111000, %00000000, %00000000, %00000000
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %11000000, %00000000, %00000000, %00000000
pflabel27
	lda PF_data27,x
	sta playfield,x
	dex
	bpl pflabel27
.L01033 ;  goto room_draw_end

 jmp .room_draw_end

.r26
 ; r26

.L01034 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01035 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel241-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-82)
 sta pfcolortable
.L01036 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel28
PF_data28
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00011111, %10000011, %11111111, %00001111
	.byte %00111111, %11000111, %11111111, %00011111
	.byte %01111111, %11101111, %11111111, %00111111
	.byte %00111111, %11000111, %11111111, %00011111
	.byte %00110100, %11000110, %00110001, %00011001
	.byte %00111100, %11000110, %11110001, %00011111
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
pflabel28
	lda PF_data28,x
	sta playfield,x
	dex
	bpl pflabel28
.L01037 ;  goto room_draw_end

 jmp .room_draw_end

.r27
 ; r27

.L01038 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01039 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel241-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-81)
 sta pfcolortable
.L01040 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel29
PF_data29
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111110, %00000000
	.byte %00000000, %11111100, %11111110, %00000000
	.byte %00000000, %11111100, %11111110, %00000000
	.byte %00000000, %11111100, %11111110, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
pflabel29
	lda PF_data29,x
	sta playfield,x
	dex
	bpl pflabel29
.L01041 ;  goto room_draw_end

 jmp .room_draw_end

.r28
 ; r28

.L01042 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01043 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-84)
 sta pfcolortable
.L01044 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel30
PF_data30
	.byte %00001000, %00000000, %01100000, %11000000
	.byte %00001111, %00111111, %01100000, %11111111
	.byte %00001000, %00000000, %01100000, %11000000
	.byte %00001000, %00000000, %01100000, %11000000
	.byte %00001000, %11111111, %11111111, %11000111
	.byte %00001000, %00000001, %00000000, %11000100
	.byte %00001000, %00000000, %00000000, %11000000
	.byte %00001100, %11111000, %11111111, %11100001
	.byte %00001111, %00011111, %00000001, %11111111
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
pflabel30
	lda PF_data30,x
	sta playfield,x
	dex
	bpl pflabel30
.L01045 ;  goto room_draw_end

 jmp .room_draw_end

.r29
 ; r29

.L01046 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01047 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-83)
 sta pfcolortable
.L01048 ;  if quest_flags{6} then goto r29_open

	BIT quest_flags
	BVC .skipL01048
.condpart160
 jmp .r29_open

.skipL01048
.L01049 ;  if item_flags{3} then goto r29_open

	LDA item_flags
	AND #8
	BEQ .skipL01049
.condpart161
 jmp .r29_open

.skipL01049
.L01050 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel31
PF_data31
	.byte %11001111, %11111111, %11111111, %11111111
	.byte %11001000, %00000000, %00000000, %11000000
	.byte %11001000, %00000000, %00000000, %11000000
	.byte %11001001, %11111111, %11111111, %11000001
	.byte %11001001, %10111111, %10111111, %11000001
	.byte %11001001, %10101111, %10101111, %11000001
	.byte %11000001, %11101111, %11101111, %11000001
	.byte %11000001, %10101111, %10101111, %11000001
	.byte %11111111, %00000001, %00000001, %11111111
	.byte %00000000, %00000000, %00000000, %11000000
	.byte %00000000, %00000000, %00000000, %11000000
pflabel31
	lda PF_data31,x
	sta playfield,x
	dex
	bpl pflabel31
.L01051 ;  goto room_draw_end

 jmp .room_draw_end

.r29_open
 ; r29_open

.L01052 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel32
PF_data32
	.byte %11001111, %11111111, %11111111, %11111111
	.byte %11001000, %00000000, %00000000, %11000000
	.byte %11001000, %00000000, %00000000, %11000000
	.byte %11001001, %01111111, %01111111, %11000001
	.byte %11001001, %00011111, %00011111, %11000001
	.byte %11001001, %00001111, %00001111, %11000001
	.byte %11000001, %00001111, %00001111, %11000001
	.byte %11000001, %00001111, %00001111, %11000001
	.byte %11111111, %00000001, %00000001, %11111111
	.byte %00000000, %00000000, %00000000, %11000000
	.byte %00000000, %00000000, %00000000, %11000000
pflabel32
	lda PF_data32,x
	sta playfield,x
	dex
	bpl pflabel32
.L01053 ;  goto room_draw_end

 jmp .room_draw_end

.r30
 ; r30

.L01054 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-82)
 sta pfcolortable
.L01055 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel33
PF_data33
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %11000000, %01000001, %00001000, %00000100
	.byte %11100001, %11100011, %00011100, %00001110
	.byte %11000011, %11110111, %10111110, %00011111
	.byte %10000000, %01000001, %00001000, %00000100
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %11000100, %00010000, %00010000, %00000010
	.byte %10001110, %00111000, %00111000, %00000111
	.byte %10011111, %01111100, %01111101, %00001111
	.byte %10000100, %00010000, %00010000, %00000010
	.byte %10000000, %00000000, %00000000, %11000000
	.byte %11000000, %00000000, %00000000, %11000000
pflabel33
	lda PF_data33,x
	sta playfield,x
	dex
	bpl pflabel33
.L01056 ;  goto room_draw_end

 jmp .room_draw_end

.r31
 ; r31

.L01057 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-81)
 sta pfcolortable
.L01058 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel34
PF_data34
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000001, %00000011, %00000000, %10000111
	.byte %00000011, %00000111, %10000001, %10001111
	.byte %00000011, %10000111, %11000001, %10001111
	.byte %00000001, %11000011, %11100000, %10000111
	.byte %00000000, %00000001, %10000000, %10000010
	.byte %00000000, %00000001, %00000000, %10000010
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %11100000, %00000000, %00000000, %11000000
pflabel34
	lda PF_data34,x
	sta playfield,x
	dex
	bpl pflabel34
.L01059 ;  goto room_draw_end

 jmp .room_draw_end

.r32
 ; r32

.L01060 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01061 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-84)
 sta pfcolortable
.L01062 ;  if quest_flags{2} then goto r32_open

	LDA quest_flags
	AND #4
	BEQ .skipL01062
.condpart162
 jmp .r32_open

.skipL01062
.L01063 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel35
PF_data35
	.byte %10000011, %11111111, %11111111, %11000001
	.byte %10000111, %11111111, %11111111, %11000011
	.byte %10001111, %11111111, %11111111, %11000111
	.byte %10011111, %11111111, %11111111, %11001111
	.byte %10111111, %11111111, %11111111, %11111111
	.byte %10011001, %10011001, %10011001, %00011001
	.byte %10011001, %10011001, %10011001, %00011001
	.byte %10011001, %10011001, %10011001, %00011001
	.byte %10111111, %11111111, %11111111, %00111111
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
pflabel35
	lda PF_data35,x
	sta playfield,x
	dex
	bpl pflabel35
.L01064 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r32_open
 ; r32_open

.L01065 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel36
PF_data36
	.byte %10000011, %00111111, %00111111, %11000001
	.byte %10000111, %00111111, %00111111, %11000011
	.byte %10001111, %00011111, %00011111, %11000111
	.byte %10011111, %00011111, %00011111, %11001111
	.byte %10111111, %00111111, %00111111, %11111111
	.byte %10011001, %00011001, %00011001, %00011001
	.byte %10011001, %00011001, %00011001, %00011001
	.byte %10011001, %00011001, %00011001, %00011001
	.byte %10111111, %00011111, %00011111, %00111111
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
pflabel36
	lda PF_data36,x
	sta playfield,x
	dex
	bpl pflabel36
.L01066 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r33
 ; r33

.L01067 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-83)
 sta pfcolortable
.L01068 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel37
PF_data37
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %11000000, %10000000, %11000000, %00000000
	.byte %11000000, %01100000, %00110000, %00000000
	.byte %11000000, %10000000, %11000000, %00000000
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %00001111, %11111111, %00001111, %00111111
	.byte %00011111, %11111111, %10011111, %01111111
	.byte %00001111, %11111111, %00001111, %00111111
	.byte %00001101, %10110001, %00001101, %00110001
	.byte %00001111, %11110001, %00001111, %00110001
	.byte %00000000, %00000000, %00000000, %00000000
pflabel37
	lda PF_data37,x
	sta playfield,x
	dex
	bpl pflabel37
.L01069 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r34
 ; r34

.L01070 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01071 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-82)
 sta pfcolortable
.L01072 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel38
PF_data38
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00011111, %11111111, %00000011, %00011111
	.byte %00111111, %11111111, %10000111, %00111111
	.byte %01111111, %11111111, %11001111, %01111111
	.byte %00111111, %11111111, %10000111, %00111111
	.byte %00110011, %10011000, %10000110, %00100011
	.byte %00111111, %11111000, %10000111, %00100011
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
pflabel38
	lda PF_data38,x
	sta playfield,x
	dex
	bpl pflabel38
.L01073 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r35
 ; r35

.L01074 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01075 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel275-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-81)
 sta pfcolortable
.L01076 ;  if quest_flags{4} then goto r35_open

	LDA quest_flags
	AND #16
	BEQ .skipL01076
.condpart163
 jmp .r35_open

.skipL01076
.L01077 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel39
PF_data39
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111000, %11111000, %00000000
	.byte %00000000, %11110000, %11110000, %00000000
	.byte %00000000, %11110000, %11110000, %00000000
	.byte %00000000, %11111000, %11111000, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111110, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
pflabel39
	lda PF_data39,x
	sta playfield,x
	dex
	bpl pflabel39
.L01078 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r35_open
 ; r35_open

.L01079 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel40
PF_data40
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111000, %11111000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %11111000, %11111000, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111110, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111100, %00000000
pflabel40
	lda PF_data40,x
	sta playfield,x
	dex
	bpl pflabel40
.L01080 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r36
 ; r36

.L01081 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01082 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-84)
 sta pfcolortable
.L01083 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel41
PF_data41
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00111111, %11110011, %11110011, %00111111
	.byte %00111111, %11110011, %11110011, %00111111
	.byte %00111111, %11110011, %11110011, %00111111
	.byte %00111111, %11110011, %11110011, %00111111
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00111111, %11110011, %11110011, %00111111
	.byte %00111111, %11110011, %11110011, %00111111
	.byte %00111111, %11110011, %11110011, %00111111
	.byte %00111111, %11110011, %11110011, %00111111
	.byte %00000000, %00000000, %00000000, %00000000
pflabel41
	lda PF_data41,x
	sta playfield,x
	dex
	bpl pflabel41
.L01084 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r37
 ; r37

.L01085 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01086 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-83)
 sta pfcolortable
.L01087 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel42
PF_data42
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00111111, %11111001, %11111001, %00111111
	.byte %00111111, %11111001, %11111001, %00111111
	.byte %00111111, %11111001, %11111001, %00111111
	.byte %00111111, %11111001, %11111001, %00111111
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00111111, %11111001, %11111001, %00111111
	.byte %00111111, %11111001, %11111001, %00111111
	.byte %00111111, %11111001, %11111001, %00111111
	.byte %00111111, %11111001, %11111001, %00111111
	.byte %00000000, %00000000, %00000000, %00000000
pflabel42
	lda PF_data42,x
	sta playfield,x
	dex
	bpl pflabel42
.L01088 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r38
 ; r38

.L01089 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01090 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel292-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-82)
 sta pfcolortable
.L01091 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel43
PF_data43
	.byte %11000000, %00000000, %00000000, %11000000
	.byte %00000011, %11111111, %11111111, %00000001
	.byte %00001110, %00000000, %00000001, %00000111
	.byte %00011000, %00000000, %00000000, %00001100
	.byte %00110000, %00000000, %00000000, %00011000
	.byte %00110000, %00000000, %00000000, %00110000
	.byte %00110000, %00000000, %00000000, %00110000
	.byte %00111000, %00000000, %00000000, %00111100
	.byte %00101110, %00000000, %00000000, %00110111
	.byte %00100000, %11111100, %11111111, %00110110
	.byte %00100000, %11110100, %11110111, %00111110
pflabel43
	lda PF_data43,x
	sta playfield,x
	dex
	bpl pflabel43
.L01092 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r39
 ; r39

.L01093 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01094 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-81)
 sta pfcolortable
.L01095 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel44
PF_data44
	.byte %11100000, %00000000, %00000000, %11000000
	.byte %00110001, %00000000, %10000000, %11100001
	.byte %00110011, %10000001, %11000001, %11100011
	.byte %00011001, %00001000, %10000000, %11000001
	.byte %00110000, %00011100, %00000000, %11000000
	.byte %00011000, %00001000, %00010000, %11100000
	.byte %00110000, %00100000, %00111000, %11000000
	.byte %00011000, %01110001, %00010000, %11100001
	.byte %00110001, %00100011, %00000001, %11100011
	.byte %00011000, %00000001, %00000000, %11000001
	.byte %00110000, %00000000, %00000000, %10000000
pflabel44
	lda PF_data44,x
	sta playfield,x
	dex
	bpl pflabel44
.L01096 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.L01097 ;  bank 3

 echo "    ",[(start_bank2 - *)]d , "bytes of ROM space left in bank 2")
 ORG $2FF4-bscode_length
 RORG $3FF4-bscode_length
start_bank2 ldx #$ff
 txs
 lda #>(start-1)
 pha
 lda #<(start-1)
 pha
 pha
 txa
 pha
 tsx
 lda 4,x ; get high byte of return address
 rol   
 rol
 rol
 rol
 and #bs_mask ;1 3 or 7 for F8/F6/F4
 tax
 inx
 lda bankswitch_hotspot-1,x
 pla
 tax
 pla
 rts
 if ((* & $1FFF) > ((bankswitch_hotspot & $1FFF) - 1))
   echo "WARNING: size parameter in banksw.asm too small - the program probably will not work."
   echo "Change to",[(*-begin_bscode+1)&$FF]d,"and try again."
 endif
 ORG $2FFC
 RORG $3FFC
 .word start_bank2
 .word start_bank2
 ORG $3000
 RORG $5000
.room_draw_40
 ; room_draw_40

.L01098 ;  temp1 = hero_room - 40

	LDA hero_room
	SEC
	SBC #40
	STA temp1
.L01099 ;  on temp1 goto r40 r41 r42 r43 r44 r45 r46 r47 r48 r49 r50 r51 r52 r53 r54 r55 r56 r57 r58 r59 r60 r61 r62 r63

	LDX temp1
	LDA .L01099jumptablehi,x
	PHA
	LDA .L01099jumptablelo,x
	PHA
	RTS
.L01099jumptablehi
	.byte >(.r40-1)
	.byte >(.r41-1)
	.byte >(.r42-1)
	.byte >(.r43-1)
	.byte >(.r44-1)
	.byte >(.r45-1)
	.byte >(.r46-1)
	.byte >(.r47-1)
	.byte >(.r48-1)
	.byte >(.r49-1)
	.byte >(.r50-1)
	.byte >(.r51-1)
	.byte >(.r52-1)
	.byte >(.r53-1)
	.byte >(.r54-1)
	.byte >(.r55-1)
	.byte >(.r56-1)
	.byte >(.r57-1)
	.byte >(.r58-1)
	.byte >(.r59-1)
	.byte >(.r60-1)
	.byte >(.r61-1)
	.byte >(.r62-1)
	.byte >(.r63-1)
.L01099jumptablelo
	.byte <(.r40-1)
	.byte <(.r41-1)
	.byte <(.r42-1)
	.byte <(.r43-1)
	.byte <(.r44-1)
	.byte <(.r45-1)
	.byte <(.r46-1)
	.byte <(.r47-1)
	.byte <(.r48-1)
	.byte <(.r49-1)
	.byte <(.r50-1)
	.byte <(.r51-1)
	.byte <(.r52-1)
	.byte <(.r53-1)
	.byte <(.r54-1)
	.byte <(.r55-1)
	.byte <(.r56-1)
	.byte <(.r57-1)
	.byte <(.r58-1)
	.byte <(.r59-1)
	.byte <(.r60-1)
	.byte <(.r61-1)
	.byte <(.r62-1)
	.byte <(.r63-1)
.L01100 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r40
 ; r40

.L01101 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel309-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-84)
 sta pfcolortable
.L01102 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel45
PF_data45
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %11100000, %00000000, %00000000, %00000000
	.byte %11111000, %00000000, %00000000, %00000000
	.byte %11111110, %00000000, %00000000, %00000000
	.byte %11111111, %00000011, %11100000, %00000000
	.byte %11111111, %10000000, %11110000, %00000000
	.byte %11111100, %11100000, %11111100, %00000000
	.byte %11111100, %11000000, %11110000, %00000000
	.byte %11111000, %00000000, %11111100, %00000000
	.byte %11100000, %00000000, %00111110, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
pflabel45
	lda PF_data45,x
	sta playfield,x
	dex
	bpl pflabel45
.L01103 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r41
 ; r41

.L01104 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01105 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel309-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-83)
 sta pfcolortable
.L01106 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel46
PF_data46
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00100000, %00000000, %00000000
	.byte %00000000, %01110000, %00000000, %00000000
	.byte %00000000, %11111000, %00000000, %00000000
	.byte %00000000, %00100000, %00000000, %00000000
	.byte %00000100, %00000000, %00000000, %00000000
	.byte %00001110, %00000000, %00010000, %00000000
	.byte %00001111, %00000000, %00111000, %00000000
	.byte %00011111, %00000000, %01111100, %00000000
	.byte %00000100, %00000000, %00010000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
pflabel46
	lda PF_data46,x
	sta playfield,x
	dex
	bpl pflabel46
.L01107 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r42
 ; r42

.L01108 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01109 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel309-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-82)
 sta pfcolortable
.L01110 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel47
PF_data47
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000100
	.byte %00000000, %00000000, %00000000, %00001110
	.byte %00000000, %00000000, %00000000, %00011111
	.byte %00000000, %00000000, %00000000, %00000100
	.byte %00000100, %00000000, %00000000, %00000000
	.byte %00001110, %00000000, %00010000, %00000000
	.byte %00001111, %00000000, %00111000, %00000000
	.byte %00011111, %00000000, %01111100, %00000000
	.byte %00000100, %00000000, %00010000, %11000000
	.byte %00000000, %00000000, %00000000, %11000000
pflabel47
	lda PF_data47,x
	sta playfield,x
	dex
	bpl pflabel47
.L01111 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r43
 ; r43

.L01112 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01113 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel309-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-81)
 sta pfcolortable
.L01114 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel48
PF_data48
	.byte %00000000, %11111100, %11111100, %00000000
	.byte %00000000, %11111100, %11111110, %00000000
	.byte %00000000, %11111100, %11111110, %00000000
	.byte %00000000, %11111100, %11111111, %00000000
	.byte %00000000, %11111110, %11111111, %00000000
	.byte %00000000, %11111110, %11111111, %00000011
	.byte %00000001, %11111111, %11111111, %00001111
	.byte %00000111, %11111111, %11111111, %00011111
	.byte %00111111, %11111111, %11111111, %01111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel48
	lda PF_data48,x
	sta playfield,x
	dex
	bpl pflabel48
.L01115 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r44
 ; r44

.L01116 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01117 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel326-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-84)
 sta pfcolortable
.L01118 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel49
PF_data49
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000001, %00100000, %00000000
	.byte %00000001, %00000011, %01110000, %00000000
	.byte %00000011, %00000111, %11111000, %00000000
	.byte %00000111, %10001110, %11011100, %00000010
	.byte %00001110, %11011100, %10001110, %00000111
	.byte %00000000, %00000000, %00000001, %00001111
	.byte %00000000, %00000000, %00000011, %00011101
	.byte %00000000, %00000000, %00000111, %00111000
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %11000000, %00000000, %00000000, %00000000
pflabel49
	lda PF_data49,x
	sta playfield,x
	dex
	bpl pflabel49
.L01119 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r45
 ; r45

.L01120 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01121 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel326-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-83)
 sta pfcolortable
.L01122 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel50
PF_data50
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00011100, %11000111, %11000011, %00111001
	.byte %00100011, %11101000, %11100100, %01000110
	.byte %00000011, %11100000, %11100000, %00000011
	.byte %00000001, %01100001, %01100000, %00000011
	.byte %00000001, %01100001, %01100001, %00000001
	.byte %00000000, %00000011, %00000001, %00000001
	.byte %00000000, %00000000, %00000000, %00000000
pflabel50
	lda PF_data50,x
	sta playfield,x
	dex
	bpl pflabel50
.L01123 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r46
 ; r46

.L01124 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01125 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel326-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-82)
 sta pfcolortable
.L01126 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel51
PF_data51
	.byte %00100000, %01110100, %01110111, %00111111
	.byte %00100001, %01111111, %01111111, %00111110
	.byte %00100000, %11111100, %11110111, %00111110
	.byte %00111000, %11100000, %11110111, %00110111
	.byte %00110111, %00000000, %00111111, %00110111
	.byte %00111110, %00000111, %00000111, %00111110
	.byte %00111110, %00111111, %00000000, %00111111
	.byte %00110111, %11110111, %11100000, %00111000
	.byte %00110111, %01111111, %01111100, %00100000
	.byte %00111110, %11111111, %11111111, %00100001
	.byte %00111110, %11110111, %11110100, %00111000
	.byte %00110111, %01110111, %01110100, %00111000
pflabel51
	lda PF_data51,x
	sta playfield,x
	dex
	bpl pflabel51
.L01127 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r47
 ; r47

.L01128 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel326-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-81)
 sta pfcolortable
.L01129 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel52
PF_data52
	.byte %00110000, %00000000, %00000000, %10000000
	.byte %00011001, %00010000, %00100000, %10000001
	.byte %00110011, %00111001, %01110001, %11000011
	.byte %00011001, %00010000, %00100000, %11000001
	.byte %00110000, %10000010, %00000100, %11100000
	.byte %00011000, %11000111, %10001110, %11100000
	.byte %00011000, %10000010, %00000100, %11110000
	.byte %00110001, %00010000, %00100000, %11100001
	.byte %00110011, %00111001, %01110001, %11000011
	.byte %00110001, %00010000, %00100000, %10000001
	.byte %00011000, %00000000, %00000000, %10000000
pflabel52
	lda PF_data52,x
	sta playfield,x
	dex
	bpl pflabel52
.L01130 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r48
 ; r48

.L01131 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01132 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel343-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-84)
 sta pfcolortable
.L01133 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel53
PF_data53
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00010000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000010, %00000000, %00000000, %00010000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00001000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10010000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %10000000, %00000000
pflabel53
	lda PF_data53,x
	sta playfield,x
	dex
	bpl pflabel53
.L01134 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r49
 ; r49

.L01135 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01136 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel343-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-83)
 sta pfcolortable
.L01137 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel54
PF_data54
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000001, %11111111, %11111111, %00000001
	.byte %00000011, %11111111, %11111111, %00000011
	.byte %00000111, %11111111, %11111111, %00000111
	.byte %00000011, %11111111, %11111111, %00000011
	.byte %00000011, %00111001, %01111001, %00000011
	.byte %00000011, %00111111, %01111111, %00000011
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
pflabel54
	lda PF_data54,x
	sta playfield,x
	dex
	bpl pflabel54
.L01138 ;  COLUPF = red

	LDA red
	STA COLUPF
.L01139 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r50
 ; r50

.L01140 ;  COLUBK  =  sand  +  0

	LDA #sand
	CLC
	ADC #0
	STA COLUBK
.L01141 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel343-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-82)
 sta pfcolortable
.L01142 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel55
PF_data55
	.byte %00000000, %00000000, %00000000, %11000000
	.byte %00000000, %00000000, %00000000, %11100000
	.byte %00000000, %00000000, %00000000, %11111000
	.byte %00000000, %00000000, %00000000, %11111110
	.byte %00000000, %00000000, %00000000, %11111111
	.byte %00000000, %00000000, %00000011, %11111111
	.byte %00000000, %00000000, %00000111, %11111111
	.byte %00000000, %00000000, %00011111, %11111111
	.byte %00000000, %00000000, %00111111, %11111111
	.byte %00000000, %00000000, %00111111, %11111111
	.byte %00000000, %00000000, %01111111, %11111111
	.byte %00000000, %00000000, %11111111, %11111111
pflabel55
	lda PF_data55,x
	sta playfield,x
	dex
	bpl pflabel55
.L01143 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r51
 ; r51

.L01144 ;  rem 'black ship top

.L01145 ;  COLUBK  =  144  +  2

	LDA #144
	CLC
	ADC #2
	STA COLUBK
.L01146 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel343-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-81)
 sta pfcolortable
.L01147 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel56
PF_data56
	.byte %00000000, %11100000, %11111111, %00000011
	.byte %00000000, %11111000, %11111111, %00000011
	.byte %00000000, %11111100, %11111111, %00000011
	.byte %00000000, %11111111, %11111111, %00000011
	.byte %00000011, %11111111, %11111111, %00000011
	.byte %00000111, %11111111, %11111111, %00000011
	.byte %00000000, %10000000, %10000000, %00000000
	.byte %00001111, %11111111, %11111111, %00111111
	.byte %00011000, %10000000, %10000000, %01100000
	.byte %00111000, %10000000, %10000000, %01100000
	.byte %01111000, %10000000, %10000000, %11100000
pflabel56
	lda PF_data56,x
	sta playfield,x
	dex
	bpl pflabel56
.L01148 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r52
 ; r52

.L01149 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01150 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel360-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-84)
 sta pfcolortable
.L01151 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel57
PF_data57
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %11100000, %00000000, %00000000, %00000000
	.byte %11110000, %00000000, %00000000, %00000000
	.byte %11111000, %00000000, %00000000, %00000000
	.byte %11111110, %00000000, %00000000, %00000000
	.byte %11111111, %00000111, %00000000, %00000000
	.byte %11111111, %00111111, %00000000, %00000000
	.byte %11111111, %11111111, %10000000, %00000000
	.byte %11111111, %11111111, %11111000, %00000000
	.byte %11111111, %11111111, %11111111, %00000011
	.byte %11111111, %11111111, %11111111, %11111111
pflabel57
	lda PF_data57,x
	sta playfield,x
	dex
	bpl pflabel57
.L01152 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r53
 ; r53

.L01153 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01154 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel360-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-83)
 sta pfcolortable
.L01155 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel58
PF_data58
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000011, %00111001, %00001110, %00001110
	.byte %00000100, %01000110, %00010001, %00010001
	.byte %00000000, %00000110, %00000001, %00000001
	.byte %00000000, %00000011, %00000011, %00000000
	.byte %00000001, %00000001, %00000110, %00000000
	.byte %00000011, %00000000, %00001100, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %11000000, %00000000, %00000000, %00000000
pflabel58
	lda PF_data58,x
	sta playfield,x
	dex
	bpl pflabel58
.L01156 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r54
 ; r54

.L01157 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01158 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel360-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-82)
 sta pfcolortable
.L01159 ;  if quest_flags{3} then goto r54_open

	LDA quest_flags
	AND #8
	BEQ .skipL01159
.condpart164
 jmp .r54_open

.skipL01159
.L01160 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel59
PF_data59
	.byte %00110111, %11110111, %11110111, %00110111
	.byte %00110111, %01111111, %01111111, %00110111
	.byte %00111110, %01111111, %01111111, %00111110
	.byte %00111110, %11110111, %11110111, %00111110
	.byte %00110111, %11110111, %11110111, %00110111
	.byte %00110111, %11011111, %00011111, %00110111
	.byte %00111110, %01111111, %10101111, %00011110
	.byte %00011110, %10110111, %01010111, %00001110
	.byte %00001111, %10110111, %01010111, %00000111
	.byte %00000011, %11111111, %11111111, %11000001
	.byte %00000000, %00000000, %00000000, %11000000
pflabel59
	lda PF_data59,x
	sta playfield,x
	dex
	bpl pflabel59
.L01161 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r54_open
 ; r54_open

.L01162 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel60
PF_data60
	.byte %00110111, %11110111, %11111100, %00111000
	.byte %00110111, %00011111, %00000000, %00111111
	.byte %00111110, %00011111, %00000111, %00111110
	.byte %00111110, %00010111, %00111111, %00111110
	.byte %00110111, %00010111, %11110111, %00110111
	.byte %00110111, %00011111, %00011111, %00110111
	.byte %00111110, %01111111, %01111111, %00011110
	.byte %00011110, %00110111, %00110111, %00001110
	.byte %00001111, %00110111, %00110111, %00000111
	.byte %00000011, %00111111, %00111111, %11000001
	.byte %00000000, %00000000, %00000000, %11000000
pflabel60
	lda PF_data60,x
	sta playfield,x
	dex
	bpl pflabel60
.L01163 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r55
 ; r55

.L01164 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01165 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel360-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-81)
 sta pfcolortable
.L01166 ;  if quest_flags{7} then goto r55_open

	BIT quest_flags
	BPL .skipL01166
.condpart165
 jmp .r55_open

.skipL01166
.L01167 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel61
PF_data61
	.byte %00011000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000001, %00111111, %00000000, %10000000
	.byte %00000011, %01111111, %00000000, %10000000
	.byte %00000001, %00101010, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00111111, %11111111
	.byte %00011110, %00000000, %11110000, %11111100
	.byte %11111111, %11110001, %11111100, %11111110
	.byte %11111111, %11111111, %11111111, %11111111
pflabel61
	lda PF_data61,x
	sta playfield,x
	dex
	bpl pflabel61
.L01168 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r55_open
 ; r55_open

.L01169 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel62
PF_data62
	.byte %00011000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000001, %00111111, %00000000, %10000000
	.byte %00000011, %01111111, %00000000, %10000000
	.byte %00000001, %00101010, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %11110000
	.byte %00011110, %00000000, %11110000, %11111100
	.byte %11001100, %00000000, %01000000, %11100000
	.byte %11000000, %00000000, %00000000, %10000000
pflabel62
	lda PF_data62,x
	sta playfield,x
	dex
	bpl pflabel62
.L01170 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r56
 ; r56

.L01171 ;  rem 'Shield on island

.L01172 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01173 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel377-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-84)
 sta pfcolortable
.L01174 ;  if !quest_flags{6} then goto r56_closed

	BIT quest_flags
	BVS .skipL01174
.condpart166
 jmp .r56_closed

.skipL01174
.L01175 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel63
PF_data63
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %11100000, %00000000, %00000000, %00000000
	.byte %11111000, %00000000, %00000000, %00000000
	.byte %11111001, %00000001, %00000000, %00000000
	.byte %11111001, %00111111, %00000000, %00000000
	.byte %11110000, %11111111, %11000000, %00000000
	.byte %11100000, %11111110, %11111000, %00000000
	.byte %11100000, %11111110, %11111111, %00000011
	.byte %11110000, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel63
	lda PF_data63,x
	sta playfield,x
	dex
	bpl pflabel63
.L01176 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r56_closed
 ; r56_closed

.L01177 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel64
PF_data64
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %11100000, %00000000, %00000000, %00000000
	.byte %11111100, %00000000, %00000000, %00000000
	.byte %11111111, %00000001, %00000000, %00000000
	.byte %11111111, %00111111, %00000000, %00000000
	.byte %11110000, %11111111, %11000000, %00000000
	.byte %11100000, %11111110, %11111000, %00000000
	.byte %11100000, %11111110, %11111111, %00000011
	.byte %11110000, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel64
	lda PF_data64,x
	sta playfield,x
	dex
	bpl pflabel64
.r56_end
 ; r56_end

.L01178 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r57
 ; r57

.L01179 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01180 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel377-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-83)
 sta pfcolortable
.L01181 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel65
PF_data65
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000010, %00000000
	.byte %00000000, %00000000, %00000111, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000111, %11110001, %11000000, %00011110
	.byte %11111111, %11111111, %11110001, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel65
	lda PF_data65,x
	sta playfield,x
	dex
	bpl pflabel65
.L01182 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r58
 ; r58

.L01183 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01184 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel377-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-82)
 sta pfcolortable
.L01185 ;  if quest_flags{2} goto r58_open

	LDA quest_flags
	AND #4
 if ((* - .r58_open) < 127) && ((* - .r58_open) > -128)
	BNE .r58_open
 else
	beq .12skipr58_open
	jmp .r58_open
.12skipr58_open
 endif
.L01186 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel66
PF_data66
	.byte %00000000, %00000000, %11111111, %11111111
	.byte %00000000, %00000000, %11111111, %11111111
	.byte %00000000, %10000000, %11111111, %11111111
	.byte %00000000, %11000000, %11111111, %11111111
	.byte %00000000, %11000000, %11111111, %11111111
	.byte %00000000, %11100000, %11111111, %11111111
	.byte %00000000, %11111100, %11111111, %11111111
	.byte %00000000, %11111111, %11111111, %11111111
	.byte %00001111, %11111111, %11111111, %00001111
	.byte %11111111, %11111111, %11111111, %00011111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel66
	lda PF_data66,x
	sta playfield,x
	dex
	bpl pflabel66
.L01187 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r58_open
 ; r58_open

.L01188 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel67
PF_data67
	.byte %00000000, %00000000, %11111111, %11111111
	.byte %00000000, %00000000, %00011111, %11111111
	.byte %00000000, %00000000, %00000111, %11111111
	.byte %00000000, %00000000, %00000001, %11111111
	.byte %00000000, %00000000, %00000000, %11111110
	.byte %00000000, %00000000, %00000000, %11111000
	.byte %00000000, %00000000, %00000000, %11100000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel67
	lda PF_data67,x
	sta playfield,x
	dex
	bpl pflabel67
.L01189 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r59
 ; r59

.L01190 ;  COLUBK  =  144  +  1

	LDA #144
	CLC
	ADC #1
	STA COLUBK
.L01191 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel377-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-81)
 sta pfcolortable
.L01192 ;  rem 'black ship bottom, when switch3 isn't pressed

.L01193 ;  if quest_flags{3} goto black_ship_open

	LDA quest_flags
	AND #8
 if ((* - .black_ship_open) < 127) && ((* - .black_ship_open) > -128)
	BNE .black_ship_open
 else
	beq .13skipblack_ship_open
	jmp .black_ship_open
.13skipblack_ship_open
 endif
.L01194 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel68
PF_data68
	.byte %01111001, %10000001, %10001111, %11100001
	.byte %11111000, %00000000, %00001000, %11100001
	.byte %01111000, %00000000, %00000000, %11100000
	.byte %00111000, %00000000, %00000000, %01111000
	.byte %00011111, %01111111, %00111111, %01111111
	.byte %00001111, %01111111, %00111111, %01111111
	.byte %11111111, %01111111, %00111111, %00111111
	.byte %00000000, %00000000, %00110000, %00000000
	.byte %00000000, %00000000, %00110000, %00000000
	.byte %11111111, %11111111, %11110000, %00000000
	.byte %11111111, %11111111, %11110000, %00000000
pflabel68
	lda PF_data68,x
	sta playfield,x
	dex
	bpl pflabel68
.L01195 ;  goto black_ship_end

 jmp .black_ship_end

.black_ship_open
 ; black_ship_open

.L01196 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel69
PF_data69
	.byte %01111001, %10000001, %10001111, %01100001
	.byte %11111000, %00000000, %00001000, %01100001
	.byte %01111000, %00000000, %00000000, %01100000
	.byte %00111000, %00000000, %00000000, %01100000
	.byte %00011111, %01111111, %00111111, %00111111
	.byte %00001111, %01111111, %00111111, %00111111
	.byte %11111111, %01111111, %00111111, %11111111
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel69
	lda PF_data69,x
	sta playfield,x
	dex
	bpl pflabel69
.black_ship_end
 ; black_ship_end

.L01197 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r60
 ; r60

.L01198 ;  rem 'Boss 3, Kraken room.

.L01199 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L01200 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel394-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-84)
 sta pfcolortable
.L01201 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel70
PF_data70
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11000000, %00000000, %00000000, %11000000
	.byte %11000000, %00000000, %00000000, %11000000
	.byte %00000000, %00000000, %00000000, %11000000
	.byte %00000000, %00000000, %00000000, %11000000
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel70
	lda PF_data70,x
	sta playfield,x
	dex
	bpl pflabel70
.L01202 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r61
 ; r61

.L01203 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01204 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel394-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-83)
 sta pfcolortable
.L01205 ;  if !hero_items{5} then goto r61_closed

	LDA hero_items
	AND #32
	BNE .skipL01205
.condpart167
 jmp .r61_closed

.skipL01205
.L01206 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel71
PF_data71
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %11100000, %00000000, %00000000, %00000000
	.byte %11111000, %00000000, %00000000, %00000000
	.byte %11111001, %00000001, %00000000, %00000000
	.byte %11111001, %00111111, %00000000, %00000000
	.byte %11110000, %11111111, %11000000, %00000000
	.byte %11100000, %11110010, %11111000, %00000000
	.byte %11100000, %11001110, %00111111, %00000011
	.byte %11110000, %00111111, %11111111, %11111111
	.byte %11111111, %11001111, %00111111, %11111111
pflabel71
	lda PF_data71,x
	sta playfield,x
	dex
	bpl pflabel71
.L01207 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r61_closed
 ; r61_closed

.L01208 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel72
PF_data72
	.byte %11000000, %00000000, %00000000, %00000000
	.byte %11111000, %00000000, %00000000, %00000000
	.byte %11111111, %00000000, %00000000, %00000000
	.byte %11111111, %00001111, %00000000, %00000000
	.byte %11111111, %00111111, %00000000, %00000000
	.byte %11111111, %11111111, %00000000, %00000000
	.byte %11110000, %11111111, %11000000, %00000000
	.byte %11100000, %11111110, %11111000, %00000000
	.byte %11100000, %11111110, %11111111, %00000011
	.byte %11110000, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel72
	lda PF_data72,x
	sta playfield,x
	dex
	bpl pflabel72
.L01209 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r62
 ; r62

.L01210 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01211 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel394-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-82)
 sta pfcolortable
.L01212 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel73
PF_data73
	.byte %00000000, %00000000, %00000000, %10000000
	.byte %00000000, %00000000, %00000000, %11000000
	.byte %00000000, %00000000, %00000000, %11000000
	.byte %00000000, %00000000, %00000000, %11110000
	.byte %00000000, %00000000, %00000000, %11100000
	.byte %00000000, %00000000, %00000000, %11110000
	.byte %00000000, %00000000, %00000000, %11111100
	.byte %00000000, %00000000, %00000000, %11111110
	.byte %00011100, %11110000, %10000000, %11111111
	.byte %11111111, %11111100, %11110001, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel73
	lda PF_data73,x
	sta playfield,x
	dex
	bpl pflabel73
.L01213 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.r63
 ; r63

.L01214 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01215 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel394-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-81)
 sta pfcolortable
.L01216 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel74
PF_data74
	.byte %11000000, %00000000, %00000000, %10000000
	.byte %11000000, %00000000, %00000000, %11000000
	.byte %11000000, %00000000, %00000000, %11000000
	.byte %11100000, %00000000, %00000000, %11110000
	.byte %11100000, %00000000, %00000000, %11100000
	.byte %11100000, %00000000, %00000000, %11110000
	.byte %11110000, %00000000, %00000000, %11111100
	.byte %11110000, %00000000, %00000000, %11111110
	.byte %11111110, %11110000, %10000000, %11111111
	.byte %11111111, %11111111, %11110001, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel74
	lda PF_data74,x
	sta playfield,x
	dex
	bpl pflabel74
.L01217 ;  goto room_draw_end bank2

 sta temp7
 lda #>(.room_draw_end-1)
 pha
 lda #<(.room_draw_end-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.L01218 ;  bank 4

 echo "    ",[(start_bank3 - *)]d , "bytes of ROM space left in bank 3")
 ORG $3FF4-bscode_length
 RORG $5FF4-bscode_length
start_bank3 ldx #$ff
 txs
 lda #>(start-1)
 pha
 lda #<(start-1)
 pha
 pha
 txa
 pha
 tsx
 lda 4,x ; get high byte of return address
 rol   
 rol
 rol
 rol
 and #bs_mask ;1 3 or 7 for F8/F6/F4
 tax
 inx
 lda bankswitch_hotspot-1,x
 pla
 tax
 pla
 rts
 if ((* & $1FFF) > ((bankswitch_hotspot & $1FFF) - 1))
   echo "WARNING: size parameter in banksw.asm too small - the program probably will not work."
   echo "Change to",[(*-begin_bscode+1)&$FF]d,"and try again."
 endif
 ORG $3FFC
 RORG $5FFC
 .word start_bank3
 .word start_bank3
 ORG $4000
 RORG $7000
.L01219 ;  rem 'Item sprites as well as initial data for item and sprites are in bank 4

.L01220 ;  rem 'Items/monsters/mobiles in any given room

.L01221 ;  rem '8 items per line means it corresponds with map layout

.L01222 ;  data room_items

	JMP .skipL01222
room_items
	.byte   monster_leaf2_boss, monster_leaf1_boss, monster_bat, item_switch4

	.byte   monster_zombie, item_victory, monster_treant, monster_wolf

	.byte   item_switch1, monster_mushroom, monster_scorpio, monster_mushroom

	.byte   monster_zombie, monster_ikaza, monster_wolf, monster_treant 

	.byte   monster_sword_boss, monster_scorpio, monster_grunt, monster_fish 

	.byte   monster_knight, monster_armor_boss, item_switch2, monster_curse

	.byte   item_switch7, monster_archer, monster_grunt, monster_rabid

	.byte   monster_archer, monster_grunt, monster_lion, monster_snake

	.byte   item_none, monster_mushroom, monster_grunt, monster_fish 

	.byte   monster_cannon, monster_cannon, monster_mage, monster_lion 

	.byte   monster_scorpio, monster_slime, monster_bat, monster_snake

	.byte   monster_archer, monster_knight, monster_spider, monster_mushroom

	.byte   monster_flower, item_healhp, monster_slime, item_switch3 

	.byte   monster_scorpio, monster_crab, item_none, monster_fish

	.byte   item_shield, monster_crab, monster_scorpio, item_none 

	.byte   monster_leaf3_boss, item_switch6, monster_crab, monster_strike_boss

.skipL01222
.L01223 ;  data item_hplist

	JMP .skipL01223
item_hplist
	.byte     4, 4, 8, 8, 8, 16, 16, 16, 16, 32, 32, 32, 32, 64, 32, 32

	.byte    64, 80,100, 4, 64, 32, 64, 64, 64, 80, 80, 80, 80,100,100,120

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

.skipL01223
.L01224 ;  rem 'Shorthand for the center of screen positions.

.L01225 ;  const hc = 73

.L01226 ;  const vc = 49

.L01227 ;  data item_xlist

	JMP .skipL01227
item_xlist
	.byte   hc, hc, 84, 37,109, 70, hc - 8, hc 

	.byte   hc, hc, hc, 37, hc, hc, hc, hc 

	.byte   hc,113,113, 37, hc + 12, hc, hc, hc 

	.byte   hc, hc, 63,113, hc,120, hc, hc 

	.byte   hc, 37, 90, 37, 53, 53, hc, hc 

	.byte   77, hc, hc,123, 66, hc, hc, 61 

	.byte   hc, hc, hc,113, hc, hc, hc, hc 

	.byte   37, hc, 37, 37, hc, 37, hc, hc 

.skipL01227
.L01228 ;  data item_ylist

	JMP .skipL01228
item_ylist
	.byte   vc, vc, vc, 25, 70, 65, vc, vc 

	.byte   vc, vc, vc, vc, vc, vc, vc, vc + 4 

	.byte   vc, vc, vc, vc, vc + 1, vc, vc, vc + 8 

	.byte   vc, vc, 70, vc, vc + 8, vc, vc, vc - 16 

	.byte   vc, 29, 70, vc, 41, 41, vc, vc 

	.byte   33, vc, vc, vc, 70, 25, vc, vc 

	.byte   vc, 55, vc, 80, vc, vc, vc, vc 

	.byte   73, vc, vc, vc, vc, 73, vc, vc 

.skipL01228
.item_setup
 ; item_setup

.L01229 ;  item_kind  =  room_items[hero_room]

	LDX hero_room
	LDA room_items,x
	STA item_kind
.L01230 ;  gosub item_setup_kind

 jsr .item_setup_kind

.L01231 ;  goto item_setup_done bank2

 sta temp7
 lda #>(.item_setup_done-1)
 pha
 lda #<(.item_setup_done-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
.L01232 ;  rem 'You must set item_kind before gosubbing to this one  

.item_setup_kind
 ; item_setup_kind

.L01233 ;  rem 'Initialize item/mobile, and it's position, HP and speed

.L01234 ;  rem 'First, handle switches

.L01235 ;  if item_kind  <  item_switch1 then goto item_switch_end

	LDA item_kind
	CMP #item_switch1
     BCS .skipL01235
.condpart168
 jmp .item_switch_end

.skipL01235
.L01236 ;  if item_kind  >  item_switch7 then goto item_switch_end

	LDA #item_switch7
	CMP item_kind
     BCS .skipL01236
.condpart169
 jmp .item_switch_end

.skipL01236
.L01237 ;  rem 'set switch to on if needed

.L01238 ;  if quest_flags{1}  &&  item_kind  =  item_switch1 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #2
	BEQ .skipL01238
.condpart170
	LDA item_kind
	CMP #item_switch1
     BNE .skip170then
.condpart171
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip170then
.skipL01238
.L01239 ;  if quest_flags{2}  &&  item_kind  =  item_switch2 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #4
	BEQ .skipL01239
.condpart172
	LDA item_kind
	CMP #item_switch2
     BNE .skip172then
.condpart173
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip172then
.skipL01239
.L01240 ;  if quest_flags{3}  &&  item_kind  =  item_switch3 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #8
	BEQ .skipL01240
.condpart174
	LDA item_kind
	CMP #item_switch3
     BNE .skip174then
.condpart175
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip174then
.skipL01240
.L01241 ;  if quest_flags{4}  &&  item_kind  =  item_switch4 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #16
	BEQ .skipL01241
.condpart176
	LDA item_kind
	CMP #item_switch4
     BNE .skip176then
.condpart177
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip176then
.skipL01241
.L01242 ;  if quest_flags{5}  &&  item_kind  =  item_switch5 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #32
	BEQ .skipL01242
.condpart178
	LDA item_kind
	CMP #item_switch5
     BNE .skip178then
.condpart179
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip178then
.skipL01242
.L01243 ;  if quest_flags{6}  &&  item_kind  =  item_switch6 then item_kind  =  item_kind  +  item_switch_on

	BIT quest_flags
	BVC .skipL01243
.condpart180
	LDA item_kind
	CMP #item_switch6
     BNE .skip180then
.condpart181
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip180then
.skipL01243
.L01244 ;  if quest_flags{7}  &&  item_kind  =  item_switch7 then item_kind  =  item_kind  +  item_switch_on

	BIT quest_flags
	BPL .skipL01244
.condpart182
	LDA item_kind
	CMP #item_switch7
     BNE .skip182then
.condpart183
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip182then
.skipL01244
.item_switch_end
 ; item_switch_end

.L01245 ;  rem 'Next, handle bosses that may already be dead.

.L01246 ;  rem 'Ikaza

.L01247 ;  if item_kind  <  monster_leaf1_boss then goto item_boss_end

	LDA item_kind
	CMP #monster_leaf1_boss
     BCS .skipL01247
.condpart184
 jmp .item_boss_end

.skipL01247
.L01248 ;  if item_kind  >  monster_ikaza then goto item_boss_end

	LDA #monster_ikaza
	CMP item_kind
     BCS .skipL01248
.condpart185
 jmp .item_boss_end

.skipL01248
.L01249 ;  if quest_flags{0}  &&  item_kind  =  monster_ikaza then item_kind  =  item_none

	LDA quest_flags
	LSR
	BCC .skipL01249
.condpart186
	LDA item_kind
	CMP #monster_ikaza
     BNE .skip186then
.condpart187
	LDA #item_none
	STA item_kind
.skip186then
.skipL01249
.L01250 ;  rem 'Leaf 1 dropped by boss 1

.L01251 ;  if hero_items{0}  &&  item_kind  =  monster_leaf1_boss then item_kind  =  item_none

	LDA hero_items
	LSR
	BCC .skipL01251
.condpart188
	LDA item_kind
	CMP #monster_leaf1_boss
     BNE .skip188then
.condpart189
	LDA #item_none
	STA item_kind
.skip188then
.skipL01251
.L01252 ;  rem 'Leaf 2 dropped by boss 2

.L01253 ;  if hero_items{1}  &&  item_kind  =  monster_leaf2_boss then item_kind  =  item_none

	LDA hero_items
	AND #2
	BEQ .skipL01253
.condpart190
	LDA item_kind
	CMP #monster_leaf2_boss
     BNE .skip190then
.condpart191
	LDA #item_none
	STA item_kind
.skip190then
.skipL01253
.L01254 ;  rem 'Leaf 3 dropped by boss 3

.L01255 ;  if hero_items{2}  &&  item_kind  =  monster_leaf3_boss then item_kind  =  item_none

	LDA hero_items
	AND #4
	BEQ .skipL01255
.condpart192
	LDA item_kind
	CMP #monster_leaf3_boss
     BNE .skip192then
.condpart193
	LDA #item_none
	STA item_kind
.skip192then
.skipL01255
.L01256 ;  rem 'Armor Dropped by armor boss

.L01257 ;  if hero_items{4}  &&  item_kind  =  monster_armor_boss then item_kind  =  item_none

	LDA hero_items
	AND #16
	BEQ .skipL01257
.condpart194
	LDA item_kind
	CMP #monster_armor_boss
     BNE .skip194then
.condpart195
	LDA #item_none
	STA item_kind
.skip194then
.skipL01257
.L01258 ;  rem 'Sword dropped by sword boss

.L01259 ;  if hero_items{5}  &&  item_kind  =  monster_sword_boss then item_kind  =  item_none

	LDA hero_items
	AND #32
	BEQ .skipL01259
.condpart196
	LDA item_kind
	CMP #monster_sword_boss
     BNE .skip196then
.condpart197
	LDA #item_none
	STA item_kind
.skip196then
.skipL01259
.L01260 ;  rem 'Strike book dropped by strike book boss

.L01261 ;  if hero_items{7}  &&  item_kind  =  monster_strike_boss then item_kind  =  item_none

	BIT hero_items
	BPL .skipL01261
.condpart198
	LDA item_kind
	CMP #monster_strike_boss
     BNE .skip198then
.condpart199
	LDA #item_none
	STA item_kind
.skip198then
.skipL01261
.L01262 ;  rem 'Heal book and shield are not dropped by a boss, but just found

.item_boss_end
 ; item_boss_end

.L01263 ;  rem 'Finally handle the case of the shield and the heal book that should drop only once

.L01264 ;  if hero_items{3}  &&  item_kind  =  item_shield then item_kind  =  item_none

	LDA hero_items
	AND #8
	BEQ .skipL01264
.condpart200
	LDA item_kind
	CMP #item_shield
     BNE .skip200then
.condpart201
	LDA #item_none
	STA item_kind
.skip200then
.skipL01264
.L01265 ;  if hero_items{6}  &&  item_kind  =  item_bookheal then item_kind  =  item_none

	BIT hero_items
	BVC .skipL01265
.condpart202
	LDA item_kind
	CMP #item_bookheal
     BNE .skip202then
.condpart203
	LDA #item_none
	STA item_kind
.skip202then
.skipL01265
.L01266 ;  temp1  =  item_kind_mask  &  item_kind

	LDA #item_kind_mask
	AND item_kind
	STA temp1
.L01267 ;  item_hp  =  item_hplist[temp1]

	LDX temp1
	LDA item_hplist,x
	STA item_hp
.L01268 ;  if temp1  >  31 goto item_draw_32

	LDA #31
	CMP temp1
 if ((* - .item_draw_32) < 127) && ((* - .item_draw_32) > -128)
	bcc .item_draw_32
 else
	bcs .14skipitem_draw_32
	jmp .item_draw_32
.14skipitem_draw_32
 endif
.L01269 ;  on temp1 goto i00 i01 i02 i03 i04 i05 i06 i07 i08 i09 i10 i11 i12 i13 i14 i15 i16 i17 i18 i19 i20 i21 i22 i23 i24 i25 i26 i27 i28 i29 i30 i31

	LDX temp1
	LDA .L01269jumptablehi,x
	PHA
	LDA .L01269jumptablelo,x
	PHA
	RTS
.L01269jumptablehi
	.byte >(.i00-1)
	.byte >(.i01-1)
	.byte >(.i02-1)
	.byte >(.i03-1)
	.byte >(.i04-1)
	.byte >(.i05-1)
	.byte >(.i06-1)
	.byte >(.i07-1)
	.byte >(.i08-1)
	.byte >(.i09-1)
	.byte >(.i10-1)
	.byte >(.i11-1)
	.byte >(.i12-1)
	.byte >(.i13-1)
	.byte >(.i14-1)
	.byte >(.i15-1)
	.byte >(.i16-1)
	.byte >(.i17-1)
	.byte >(.i18-1)
	.byte >(.i19-1)
	.byte >(.i20-1)
	.byte >(.i21-1)
	.byte >(.i22-1)
	.byte >(.i23-1)
	.byte >(.i24-1)
	.byte >(.i25-1)
	.byte >(.i26-1)
	.byte >(.i27-1)
	.byte >(.i28-1)
	.byte >(.i29-1)
	.byte >(.i30-1)
	.byte >(.i31-1)
.L01269jumptablelo
	.byte <(.i00-1)
	.byte <(.i01-1)
	.byte <(.i02-1)
	.byte <(.i03-1)
	.byte <(.i04-1)
	.byte <(.i05-1)
	.byte <(.i06-1)
	.byte <(.i07-1)
	.byte <(.i08-1)
	.byte <(.i09-1)
	.byte <(.i10-1)
	.byte <(.i11-1)
	.byte <(.i12-1)
	.byte <(.i13-1)
	.byte <(.i14-1)
	.byte <(.i15-1)
	.byte <(.i16-1)
	.byte <(.i17-1)
	.byte <(.i18-1)
	.byte <(.i19-1)
	.byte <(.i20-1)
	.byte <(.i21-1)
	.byte <(.i22-1)
	.byte <(.i23-1)
	.byte <(.i24-1)
	.byte <(.i25-1)
	.byte <(.i26-1)
	.byte <(.i27-1)
	.byte <(.i28-1)
	.byte <(.i29-1)
	.byte <(.i30-1)
	.byte <(.i31-1)
.L01270 ;  goto item_draw_done

 jmp .item_draw_done

.item_draw_32
 ; item_draw_32

.L01271 ;  temp1  =  temp1  -  32

	LDA temp1
	SEC
	SBC #32
	STA temp1
.L01272 ;  on temp1 goto i32 i33 i34 i35 i36 i37 i38 i39 i40 i41 i42 i43 i44 i45 i46 i47 i48 i49 i50 i51 i52 i53 i54 i55 i56 i57 i58 i59 i60 i61 i62 i63

	LDX temp1
	LDA .L01272jumptablehi,x
	PHA
	LDA .L01272jumptablelo,x
	PHA
	RTS
.L01272jumptablehi
	.byte >(.i32-1)
	.byte >(.i33-1)
	.byte >(.i34-1)
	.byte >(.i35-1)
	.byte >(.i36-1)
	.byte >(.i37-1)
	.byte >(.i38-1)
	.byte >(.i39-1)
	.byte >(.i40-1)
	.byte >(.i41-1)
	.byte >(.i42-1)
	.byte >(.i43-1)
	.byte >(.i44-1)
	.byte >(.i45-1)
	.byte >(.i46-1)
	.byte >(.i47-1)
	.byte >(.i48-1)
	.byte >(.i49-1)
	.byte >(.i50-1)
	.byte >(.i51-1)
	.byte >(.i52-1)
	.byte >(.i53-1)
	.byte >(.i54-1)
	.byte >(.i55-1)
	.byte >(.i56-1)
	.byte >(.i57-1)
	.byte >(.i58-1)
	.byte >(.i59-1)
	.byte >(.i60-1)
	.byte >(.i61-1)
	.byte >(.i62-1)
	.byte >(.i63-1)
.L01272jumptablelo
	.byte <(.i32-1)
	.byte <(.i33-1)
	.byte <(.i34-1)
	.byte <(.i35-1)
	.byte <(.i36-1)
	.byte <(.i37-1)
	.byte <(.i38-1)
	.byte <(.i39-1)
	.byte <(.i40-1)
	.byte <(.i41-1)
	.byte <(.i42-1)
	.byte <(.i43-1)
	.byte <(.i44-1)
	.byte <(.i45-1)
	.byte <(.i46-1)
	.byte <(.i47-1)
	.byte <(.i48-1)
	.byte <(.i49-1)
	.byte <(.i50-1)
	.byte <(.i51-1)
	.byte <(.i52-1)
	.byte <(.i53-1)
	.byte <(.i54-1)
	.byte <(.i55-1)
	.byte <(.i56-1)
	.byte <(.i57-1)
	.byte <(.i58-1)
	.byte <(.i59-1)
	.byte <(.i60-1)
	.byte <(.i61-1)
	.byte <(.i62-1)
	.byte <(.i63-1)
.item_draw_done
 ; item_draw_done

.L01273 ;  if item_kind  <>  item_none goto item_not_none

	LDA item_kind
	CMP #item_none
 if ((* - .item_not_none) < 127) && ((* - .item_not_none) > -128)
	BNE .item_not_none
 else
	beq .15skipitem_not_none
	jmp .item_not_none
.15skipitem_not_none
 endif
.L01274 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L01275 ;  item_oldx  =  nowhere

	LDA #nowhere
	STA item_oldx
.L01276 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
.L01277 ;  item_oldy  =  nowhere

	LDA #nowhere
	STA item_oldy
.L01278 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.item_not_none
 ; item_not_none

.L01279 ;  rem temp1 = item_kind & item_kind_mask

.L01280 ;  rem 'look up location in table.

.L01281 ;  item_oldx  =  item_xlist[hero_room]

	LDX hero_room
	LDA item_xlist,x
	STA item_oldx
.L01282 ;  item_x  =  item_xlist[hero_room]

	LDX hero_room
	LDA item_xlist,x
	STA item_x
.L01283 ;  item_oldy  =  item_ylist[hero_room]

	LDX hero_room
	LDA item_ylist,x
	STA item_oldy
.L01284 ;  item_y  =  item_ylist[hero_room]

	LDX hero_room
	LDA item_ylist,x
	STA item_y
.L01285 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.i00
 ; i00

.L01286 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01287 ;  player1:

	LDA #<playerL01287_1

	STA player1pointerlo
	LDA #>playerL01287_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01288 ;  goto item_draw_done

 jmp .item_draw_done

.i01
 ; i01

.L01289 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01290 ;  player1:

	LDA #<playerL01290_1

	STA player1pointerlo
	LDA #>playerL01290_1

	STA player1pointerhi
	LDA #5
	STA player1height
.L01291 ;  goto item_draw_done

 jmp .item_draw_done

.i02
 ; i02

.L01292 ;  COLUP1 = red

	LDA red
	STA COLUP1
.L01293 ;  player1:

	LDA #<playerL01293_1

	STA player1pointerlo
	LDA #>playerL01293_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01294 ;  goto item_draw_done

 jmp .item_draw_done

.i03
 ; i03

.L01295 ;  rem 'bat.xpm

.L01296 ;  player1:

	LDA #<playerL01296_1

	STA player1pointerlo
	LDA #>playerL01296_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01297 ;  goto item_draw_done

 jmp .item_draw_done

.i04
 ; i04

.L01298 ;  rem 'scorpio.xpm

.L01299 ;  player1:

	LDA #<playerL01299_1

	STA player1pointerlo
	LDA #>playerL01299_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01300 ;  goto item_draw_done

 jmp .item_draw_done

.i05
 ; i05

.L01301 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01302 ;  rem 'rabid.xpm

.L01303 ;  player1:

	LDA #<playerL01303_1

	STA player1pointerlo
	LDA #>playerL01303_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01304 ;  goto item_draw_done

 jmp .item_draw_done

.i06
 ; i06

.L01305 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01306 ;  rem 'spider.xpm

.L01307 ;  player1:

	LDA #<playerL01307_1

	STA player1pointerlo
	LDA #>playerL01307_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01308 ;  goto item_draw_done

 jmp .item_draw_done

.i07
 ; i07

.L01309 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01310 ;  rem 'snake.xpm

.L01311 ;  player1:

	LDA #<playerL01311_1

	STA player1pointerlo
	LDA #>playerL01311_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01312 ;  goto item_draw_done

 jmp .item_draw_done

.i08
 ; i08

.L01313 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01314 ;  rem 'fish.xpm

.L01315 ;  player1:

	LDA #<playerL01315_1

	STA player1pointerlo
	LDA #>playerL01315_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01316 ;  goto item_draw_done

 jmp .item_draw_done

.i09
 ; i09

.L01317 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01318 ;  rem 'lion.xpm

.L01319 ;  player1:

	LDA #<playerL01319_1

	STA player1pointerlo
	LDA #>playerL01319_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01320 ;  goto item_draw_done

 jmp .item_draw_done

.i10
 ; i10

.L01321 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01322 ;  rem 'wolf.xpm

.L01323 ;  player1:

	LDA #<playerL01323_1

	STA player1pointerlo
	LDA #>playerL01323_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01324 ;  goto item_draw_done

 jmp .item_draw_done

.i11
 ; i11

.L01325 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01326 ;  rem 'captain.xpm, used for monster_grunt. (captain and grunt were doubles)

.L01327 ;  player1:

	LDA #<playerL01327_1

	STA player1pointerlo
	LDA #>playerL01327_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01328 ;  goto item_draw_done

 jmp .item_draw_done

.i12
 ; i12

.L01329 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01330 ;  rem 'archer.xpm

.L01331 ;  player1:

	LDA #<playerL01331_1

	STA player1pointerlo
	LDA #>playerL01331_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01332 ;  goto item_draw_done

 jmp .item_draw_done

.i13
 ; i13

.L01333 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01334 ;  rem 'knight.xpm

.L01335 ;  player1:

	LDA #<playerL01335_1

	STA player1pointerlo
	LDA #>playerL01335_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01336 ;  goto item_draw_done

 jmp .item_draw_done

.i14
 ; i14

.L01337 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01338 ;  rem 'cannon.xpm

.L01339 ;  player1:

	LDA #<playerL01339_1

	STA player1pointerlo
	LDA #>playerL01339_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01340 ;  goto item_draw_done

 jmp .item_draw_done

.i15
 ; i15

.L01341 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01342 ;  rem 'zombie.xpm

.L01343 ;  player1:

	LDA #<playerL01343_1

	STA player1pointerlo
	LDA #>playerL01343_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01344 ;  goto item_draw_done

 jmp .item_draw_done

.i16
 ; i16

.L01345 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01346 ;  rem 'skeleton.xpm

.L01347 ;  player1:

	LDA #<playerL01347_1

	STA player1pointerlo
	LDA #>playerL01347_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01348 ;  goto item_draw_done

 jmp .item_draw_done

.i17
 ; i17

.L01349 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01350 ;  rem 'ghost.xpm

.L01351 ;  player1:

	LDA #<playerL01351_1

	STA player1pointerlo
	LDA #>playerL01351_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01352 ;  goto item_draw_done

 jmp .item_draw_done

.i18
 ; i18

.L01353 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01354 ;  rem 'bitmap/mage.xpm

.L01355 ;  player1:

	LDA #<playerL01355_1

	STA player1pointerlo
	LDA #>playerL01355_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01356 ;  goto item_draw_done

 jmp .item_draw_done

.i19
 ; i19

.L01357 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01358 ;  rem 'flower.xpm

.L01359 ;  player1:

	LDA #<playerL01359_1

	STA player1pointerlo
	LDA #>playerL01359_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01360 ;  goto item_draw_done

 jmp .item_draw_done

.i20
 ; i20

.L01361 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01362 ;  rem 'treant.xpm

.L01363 ;  player1:

	LDA #<playerL01363_1

	STA player1pointerlo
	LDA #>playerL01363_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01364 ;  goto item_draw_done

 jmp .item_draw_done

.i21
 ; i21

.L01365 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01366 ;  rem 'muschroom.xpm

.L01367 ;  player1:

	LDA #<playerL01367_1

	STA player1pointerlo
	LDA #>playerL01367_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01368 ;  goto item_draw_done

 jmp .item_draw_done

.i22
 ; i22

.L01369 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01370 ;  rem 'book.xpm

.L01371 ;  player1:

	LDA #<playerL01371_1

	STA player1pointerlo
	LDA #>playerL01371_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01372 ;  goto item_draw_done

 jmp .item_draw_done

.i23
 ; i23

.L01373 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01374 ;  rem 'sword.xpm. It's a fake Numen Sword that hurts the one who touches it.

.L01375 ;  player1:

	LDA #<playerL01375_1

	STA player1pointerlo
	LDA #>playerL01375_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01376 ;  goto item_draw_done

 jmp .item_draw_done

.i24
 ; i24

.L01377 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01378 ;  rem 'Curse, entrance blocking item

.L01379 ;  player1:

	LDA #<playerL01379_1

	STA player1pointerlo
	LDA #>playerL01379_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01380 ;  goto item_draw_done

 jmp .item_draw_done

.i25
 ; i25

.L01381 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01382 ;  rem 'boss1.xpm

.L01383 ;  player1:

	LDA #<playerL01383_1

	STA player1pointerlo
	LDA #>playerL01383_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01384 ;  goto item_draw_done

 jmp .item_draw_done

.i26
 ; i26

.L01385 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01386 ;  rem 'boss2.xpm

.L01387 ;  player1:

	LDA #<playerL01387_1

	STA player1pointerlo
	LDA #>playerL01387_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01388 ;  goto item_draw_done

 jmp .item_draw_done

.i27
 ; i27

.L01389 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01390 ;  rem 'boss3.xpm

.L01391 ;  player1:

	LDA #<playerL01391_1

	STA player1pointerlo
	LDA #>playerL01391_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01392 ;  goto item_draw_done

 jmp .item_draw_done

.i28
 ; i28

.L01393 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01394 ;  rem 'armorboss.xpm

.L01395 ;  player1:

	LDA #<playerL01395_1

	STA player1pointerlo
	LDA #>playerL01395_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01396 ;  goto item_draw_done

 jmp .item_draw_done

.i29
 ; i29

.L01397 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01398 ;  rem 'Sword boss is a player frame 

.L01399 ;  player1:

	LDA #<playerL01399_1

	STA player1pointerlo
	LDA #>playerL01399_1

	STA player1pointerhi
	LDA #13
	STA player1height
.L01400 ;  goto item_draw_done

 jmp .item_draw_done

.i30
 ; i30

.L01401 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01402 ;  rem 'Strike boss is a one point sprite which is difficult to hit

.L01403 ;  player1:

	LDA #<playerL01403_1

	STA player1pointerlo
	LDA #>playerL01403_1

	STA player1pointerhi
	LDA #1
	STA player1height
.L01404 ;  goto item_draw_done

 jmp .item_draw_done

.i31
 ; i31

.L01405 ;  rem 'ikaza.xpm

.L01406 ;  rem 'ikaza.xpm

.L01407 ;  player1:

	LDA #<playerL01407_1

	STA player1pointerlo
	LDA #>playerL01407_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01408 ;  goto item_draw_done

 jmp .item_draw_done

.i32
 ; i32

.i33
 ; i33

.i34
 ; i34

.L01409 ;  rem 'leaf.xpm

.L01410 ;  player1:

	LDA #<playerL01410_1

	STA player1pointerlo
	LDA #>playerL01410_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01411 ;  goto item_draw_done

 jmp .item_draw_done

.i35
 ; i35

.L01412 ;  rem 'armor.xpm

.L01413 ;  player1:

	LDA #<playerL01413_1

	STA player1pointerlo
	LDA #>playerL01413_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01414 ;  goto item_draw_done

 jmp .item_draw_done

.i36
 ; i36

.L01415 ;  rem 'shield.xpm

.L01416 ;  player1:

	LDA #<playerL01416_1

	STA player1pointerlo
	LDA #>playerL01416_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01417 ;  goto item_draw_done

 jmp .item_draw_done

.i37
 ; i37

.L01418 ;  rem 'sword.xpm

.L01419 ;  player1:

	LDA #<playerL01419_1

	STA player1pointerlo
	LDA #>playerL01419_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01420 ;  goto item_draw_done

 jmp .item_draw_done

.i38
 ; i38

.L01421 ;  rem 'healbook.xpm

.L01422 ;  player1:

	LDA #<playerL01422_1

	STA player1pointerlo
	LDA #>playerL01422_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01423 ;  goto item_draw_done

 jmp .item_draw_done

.i39
 ; i39

.L01424 ;  rem 'strikebook.xpm

.L01425 ;  player1:

	LDA #<playerL01425_1

	STA player1pointerlo
	LDA #>playerL01425_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01426 ;  goto item_draw_done

 jmp .item_draw_done

.i40
 ; i40

.L01427 ;  rem 'healhp.xpm

.L01428 ;  player1:

	LDA #<playerL01428_1

	STA player1pointerlo
	LDA #>playerL01428_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01429 ;  goto item_draw_done

 jmp .item_draw_done

.i41
 ; i41

.L01430 ;  rem 'healmp.xpm

.L01431 ;  player1:

	LDA #<playerL01431_1

	STA player1pointerlo
	LDA #>playerL01431_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01432 ;  goto item_draw_done

 jmp .item_draw_done

.i42
 ; i42

.L01433 ;  rem 'healallhp.xpm

.L01434 ;  player1:

	LDA #<playerL01434_1

	STA player1pointerlo
	LDA #>playerL01434_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01435 ;  goto item_draw_done

 jmp .item_draw_done

.i43
 ; i43

.L01436 ;  rem 'healallmp.xpm

.L01437 ;  player1:

	LDA #<playerL01437_1

	STA player1pointerlo
	LDA #>playerL01437_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01438 ;  goto item_draw_done

 jmp .item_draw_done

.i44
 ; i44

.i45
 ; i45

.i46
 ; i46

.i47
 ; i47

.i48
 ; i48

.i49
 ; i49

.i50
 ; i50

.L01439 ;  rem 'bitmap/switch.xpm

.L01440 ;  player1:

	LDA #<playerL01440_1

	STA player1pointerlo
	LDA #>playerL01440_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01441 ;  goto item_draw_done

 jmp .item_draw_done

.i51
 ; i51

.i52
 ; i52

.i53
 ; i53

.i54
 ; i54

.i55
 ; i55

.i56
 ; i56

.i57
 ; i57

.L01442 ;  rem 'bitmap/switch_on.xpm

.L01443 ;  player1:

	LDA #<playerL01443_1

	STA player1pointerlo
	LDA #>playerL01443_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01444 ;  goto item_draw_done

 jmp .item_draw_done

.i58
 ; i58

.i59
 ; i59

.i60
 ; i60

.i61
 ; i61

.i62
 ; i62

.L01445 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01446 ;  player1:

	LDA #<playerL01446_1

	STA player1pointerlo
	LDA #>playerL01446_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01447 ;  goto item_draw_done

 jmp .item_draw_done

.i63
 ; i63

.L01448 ;  COLUP1 = pink

	LDA pink
	STA COLUP1
.L01449 ;  player1:

	LDA #<playerL01449_1

	STA player1pointerlo
	LDA #>playerL01449_1

	STA player1pointerhi
	LDA #13
	STA player1height
.L01450 ;  goto item_draw_done

 jmp .item_draw_done

.L01451 ;  bank 5

 echo "    ",[(start_bank4 - *)]d , "bytes of ROM space left in bank 4")
 ORG $4FF4-bscode_length
 RORG $7FF4-bscode_length
start_bank4 ldx #$ff
 txs
 lda #>(start-1)
 pha
 lda #<(start-1)
 pha
 pha
 txa
 pha
 tsx
 lda 4,x ; get high byte of return address
 rol   
 rol
 rol
 rol
 and #bs_mask ;1 3 or 7 for F8/F6/F4
 tax
 inx
 lda bankswitch_hotspot-1,x
 pla
 tax
 pla
 rts
 if ((* & $1FFF) > ((bankswitch_hotspot & $1FFF) - 1))
   echo "WARNING: size parameter in banksw.asm too small - the program probably will not work."
   echo "Change to",[(*-begin_bscode+1)&$FF]d,"and try again."
 endif
 ORG $4FFC
 RORG $7FFC
 .word start_bank4
 .word start_bank4
 ORG $5000
 RORG $9000
.L01452 ;  rem ' Bank 5 contains the game over, game win and game intro, screen

.L01453 ;  rem 'as well as a music playing routine for them.

.game_over
 ; game_over

.L01454 ;  gosub music_restart

 jsr .music_restart

.L01455 ;  music_which = 1

	LDA #1
	STA music_which
.L01456 ;  COLUBK  =  red

	LDA red
	STA COLUBK
.L01457 ;  hero_x = 70

	LDA #70
	STA hero_x
.L01458 ;  hero_y = 80

	LDA #80
	STA hero_y
.L01459 ;  player0:

	LDA #<playerL01459_0

	STA player0pointerlo
	LDA #>playerL01459_0

	STA player0pointerhi
	LDA #59
	STA player0height
.game_over_loop
 ; game_over_loop

.L01460 ;  rem 'Teleport to Sygne's home on reset button, with some losses

.L01461 ;  if !switchreset then goto reset_go_end

 lda #1
 bit SWCHB
	BEQ .skipL01461
.condpart204
 jmp .reset_go_end

.skipL01461
.L01462 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L01463 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L01464 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L01465 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L01466 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L01467 ;  hero_hp  =  hero_level  +  hero_base_hp

	LDA hero_level
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L01468 ;  hero_next  =  hero_level

	LDA hero_level
	STA hero_next
.L01469 ;  hero_mp  =  0

	LDA #0
	STA hero_mp
.L01470 ;  rem ' Restart the game with stats somewhat reduced, 

.L01471 ;  gosub music_restart

 jsr .music_restart

.L01472 ;  gosub set_mp_display bank1

 sta temp7
 lda #>(ret_point23-1)
 pha
 lda #<(ret_point23-1)
 pha
 lda #>(.set_mp_display-1)
 pha
 lda #<(.set_mp_display-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #1
 jmp BS_jsr
ret_point23
.L01473 ;  gosub hero_draw_s bank1

 sta temp7
 lda #>(ret_point24-1)
 pha
 lda #<(ret_point24-1)
 pha
 lda #>(.hero_draw_s-1)
 pha
 lda #<(.hero_draw_s-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #1
 jmp BS_jsr
ret_point24
.L01474 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point25-1)
 pha
 lda #<(ret_point25-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point25
.L01475 ;  goto main_loop bank1

 sta temp7
 lda #>(.main_loop-1)
 pha
 lda #<(.main_loop-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #1
 jmp BS_jsr
.reset_go_end
 ; reset_go_end

.L01476 ;  COLUP0 = white

	LDA white
	STA COLUP0
.L01477 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01478 ;  rem 'Make monster look x3 size, to maintain boss size and suggest something for non bosses.  

.L01479 ;  NUSIZ1  =  $07

	LDA #$07
	STA NUSIZ1
.special_effects_go_end
 ; special_effects_go_end

.L01480 ;  gosub music_play

 jsr .music_play

.L01481 ;  drawscreen

 sta temp7
 lda #>(ret_point26-1)
 pha
 lda #<(ret_point26-1)
 pha
 lda #>(drawscreen-1)
 pha
 lda #<(drawscreen-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point26
.L01482 ;  goto game_over_loop

 jmp .game_over_loop

.L01483 ;  data music_data

	JMP .skipL01483
music_data
	.byte   24,30,27,30,30,30,27,30,24,28,-1,2,24,28,-1,2,24,60

	.byte   27,28,-1,2,27,28,-1,2,27,60,24,30,20,28,-1,2,20,60

	.byte   24,30,27,30,30,30,27,30,24,28,-1,2,24,28,-1,2,24,28,-1,2

	.byte   24,30,27,28,-1,2,27,30,24,30,27,30,30,120

.skipL01483
.game_win
 ; game_win

.L01484 ;  gosub music_restart

 jsr .music_restart

.L01485 ;  music_which = 2

	LDA #2
	STA music_which
.L01486 ;  COLUBK  =  white

	LDA white
	STA COLUBK
.L01487 ;  COLUPF  =  yellow

	LDA yellow
	STA COLUPF
.L01488 ;  item_x = 80

	LDA #80
	STA item_x
.L01489 ;  item_y = 80

	LDA #80
	STA item_y
.L01490 ;  hero_x = 84

	LDA #84
	STA hero_x
.L01491 ;  hero_y = 80

	LDA #80
	STA hero_y
.L01492 ;  player1:

	LDA #<playerL01492_1

	STA player1pointerlo
	LDA #>playerL01492_1

	STA player1pointerhi
	LDA #73
	STA player1height
.game_win_loop
 ; game_win_loop

.L01493 ;  COLUBK  =  white

	LDA white
	STA COLUBK
.L01494 ;  COLUP1  =  rand

 sta temp7
 lda #>(ret_point27-1)
 pha
 lda #<(ret_point27-1)
 pha
 lda #>(randomize-1)
 pha
 lda #<(randomize-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point27
	STA COLUP1
.L01495 ;  COLUP0  =  rand

 sta temp7
 lda #>(ret_point28-1)
 pha
 lda #<(ret_point28-1)
 pha
 lda #>(randomize-1)
 pha
 lda #<(randomize-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point28
	STA COLUP0
.L01496 ;  REFP0  =  8

	LDA #8
	STA REFP0
.L01497 ;  gosub music_play

 jsr .music_play

.L01498 ;  drawscreen

 sta temp7
 lda #>(ret_point29-1)
 pha
 lda #<(ret_point29-1)
 pha
 lda #>(drawscreen-1)
 pha
 lda #<(drawscreen-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point29
.L01499 ;  rem 'Go back to house, but level up to 90 if not so already

.L01500 ;  if !switchreset then goto reset_win_end

 lda #1
 bit SWCHB
	BEQ .skipL01500
.condpart205
 jmp .reset_win_end

.skipL01500
.L01501 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L01502 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L01503 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L01504 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L01505 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L01506 ;  if hero_level  <  90 then hero_level  =  90

	LDA hero_level
	CMP #90
     BCS .skipL01506
.condpart206
	LDA #90
	STA hero_level
.skipL01506
.L01507 ;  hero_hp  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L01508 ;  hero_mp  =  hero_level  /  16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L01509 ;  gosub music_restart

 jsr .music_restart

.L01510 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point30-1)
 pha
 lda #<(ret_point30-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point30
.L01511 ;  gosub hero_draw_s bank1

 sta temp7
 lda #>(ret_point31-1)
 pha
 lda #<(ret_point31-1)
 pha
 lda #>(.hero_draw_s-1)
 pha
 lda #<(.hero_draw_s-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #1
 jmp BS_jsr
ret_point31
.L01512 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point32-1)
 pha
 lda #<(ret_point32-1)
 pha
 lda #>(.room_draw-1)
 pha
 lda #<(.room_draw-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #2
 jmp BS_jsr
ret_point32
.L01513 ;  goto main_loop bank1

 sta temp7
 lda #>(.main_loop-1)
 pha
 lda #<(.main_loop-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #1
 jmp BS_jsr
.reset_win_end
 ; reset_win_end

.L01514 ;  goto game_win_loop

 jmp .game_win_loop

.intro_screen
 ; intro_screen

.L01515 ;  gosub music_restart

 jsr .music_restart

.L01516 ;  music_which = 0

	LDA #0
	STA music_which
.L01517 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L01518 ;  COLUPF  =  white

	LDA white
	STA COLUPF
.L01519 ;  pfcolors:

 lda # yellow
 sta COLUPF
 lda #>(pfcolorlabel1303-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel1303-84)
 sta pfcolortable
.L01520 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel75
PF_data75
	.byte %00100001, %01001111, %00100111, %01111000
	.byte %01010000, %01000010, %00100100, %00001001
	.byte %10001000, %01000010, %00100111, %01111000
	.byte %11111000, %01000010, %00100100, %00001001
	.byte %10001000, %10000010, %11000100, %01111001
	.byte %00000000, %00000000, %00000000, %00000000
	.byte %11100111, %10011001, %11000110, %00010010
	.byte %10010100, %10100100, %00101001, %00010110
	.byte %11100111, %10100101, %11001111, %00011010
	.byte %10010100, %10100100, %00101001, %00010010
	.byte %11100111, %10011001, %00101001, %00010010
pflabel75
	lda PF_data75,x
	sta playfield,x
	dex
	bpl pflabel75
.L01521 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.intro_loop
 ; intro_loop

.L01522 ;  rem COLUP1 = rand

.L01523 ;  rem COLUP0 = rand

.L01524 ;  rem REFP0 = 8

.L01525 ;  gosub music_play

 jsr .music_play

.L01526 ;  drawscreen

 sta temp7
 lda #>(ret_point33-1)
 pha
 lda #<(ret_point33-1)
 pha
 lda #>(drawscreen-1)
 pha
 lda #<(drawscreen-1)
 pha
 lda temp7
 pha
 txa
 pha
 ldx #8
 jmp BS_jsr
ret_point33
.L01527 ;  if switchreset  ||  joy0fire then return

 lda #1
 bit SWCHB
	BNE .skipL01527
.condpart207
 jmp .condpart208
.skipL01527
 lda #$80
 bit INPT4
	BNE .skip24OR
.condpart208
	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.skip24OR
.L01528 ;  goto intro_loop

 jmp .intro_loop

.music_notes_intro_p
 ; music_notes_intro_p

.L01529 ;  data music_notes_intro

	JMP .skipL01529
music_notes_intro
	.byte   15, 30, 18, 30, 23, 30, 27, 30, 0, 8

	.byte   15, 30, 18, 30, 23, 30, 27, 30, 0, 8

	.byte   30, 30, 23, 30, 20, 30, 27, 30, 0, 8

	.byte   30, 30, 23, 30, 20, 30, 27, 30, 0, 8

	.byte   18, 120, 23, 120, 27, 120, 0, 120

	.byte   23, 120, 18, 120, 15, 120, 0, 120

.skipL01529
.music_notes_gameover_p
 ; music_notes_gameover_p

.L01530 ;  data music_notes_gameover

	JMP .skipL01530
music_notes_gameover
	.byte   24, 120, 18, 120, 20, 120, 24, 120, 0, 120

	.byte   18, 120, 16, 120, 20, 120, 18, 120, 0, 120

.skipL01530
.music_notes_victory_p
 ; music_notes_victory_p

.L01531 ;  data music_notes_victory

	JMP .skipL01531
music_notes_victory
	.byte   30, 30, 24, 30, 20, 30, 15, 30

	.byte   20, 30, 24, 30, 30, 60, 0, 120

	.byte   30, 30, 27, 30, 24, 15, 20, 30

	.byte   15, 30, 24, 30, 30, 60, 0, 120

.skipL01531
.L01532 ;  data music_sizes

	JMP .skipL01532
music_sizes
	.byte   56, 20, 32

.skipL01532
.L01533 ;  rem 'restarts the music  

.music_restart
 ; music_restart

.L01534 ;  music_timer = 1

	LDA #1
	STA music_timer
.L01535 ;  music_pointer = 0

	LDA #0
	STA music_pointer
.L01536 ;  AUDV0 = 0

	LDA #0
	STA AUDV0
.L01537 ;  AUDV1 = 0

	LDA #0
	STA AUDV1
.L01538 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.music_play
 ; music_play

.L01539 ;  rem ' Update music timer and change note if needed

.L01540 ;  rem ' If we get here, the timer is not 0 yet. 

.L01541 ;  rem ' Go on to the next note, and leave it at that   

.L01542 ;  if music_timer  =  0 then goto music_do_note_change

	LDA music_timer
	CMP #0
     BNE .skipL01542
.condpart209
 jmp .music_do_note_change

.skipL01542
.L01543 ;  goto music_no_note_change

 jmp .music_no_note_change

.music_do_note_change
 ; music_do_note_change

.L01544 ;  gosub music_change_note

 jsr .music_change_note

.music_no_note_change
 ; music_no_note_change

.L01545 ;  music_timer  =  music_timer  -  1

	DEC music_timer
.L01546 ;  rem ' COLUBK = music_timer

.L01547 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.music_change_note
 ; music_change_note

.L01548 ;  rem ' choose the not from the right music table

.L01549 ;  if music_which  =  0 then temp1  =  music_notes_intro[music_pointer]

	LDA music_which
	CMP #0
     BNE .skipL01549
.condpart210
	LDX music_pointer
	LDA music_notes_intro,x
	STA temp1
.skipL01549
.L01550 ;  if music_which  =  1 then temp1  =  music_notes_gameover[music_pointer]

	LDA music_which
	CMP #1
     BNE .skipL01550
.condpart211
	LDX music_pointer
	LDA music_notes_gameover,x
	STA temp1
.skipL01550
.L01551 ;  if music_which  =  2 then temp1  =  music_notes_victory[music_pointer]

	LDA music_which
	CMP #2
     BNE .skipL01551
.condpart212
	LDX music_pointer
	LDA music_notes_victory,x
	STA temp1
.skipL01551
.L01552 ;  AUDF0  =  temp1

	LDA temp1
	STA AUDF0
.L01553 ;  AUDF1  =  temp1

	LDA temp1
	STA AUDF1
.L01554 ;  AUDC0  =  4

	LDA #4
	STA AUDC0
.L01555 ;  AUDC1  =  12

	LDA #12
	STA AUDC1
.L01556 ;  if temp1  =  0 then AUDV0  =  0 else AUDV0  =  4

	LDA temp1
	CMP #0
     BNE .skipL01556
.condpart213
	LDA #0
	STA AUDV0
 jmp .skipelse5
.skipL01556
	LDA #4
	STA AUDV0
.skipelse5
.L01557 ;  if temp1  =  0 then AUDV1  =  0 else AUDV1  =  4

	LDA temp1
	CMP #0
     BNE .skipL01557
.condpart214
	LDA #0
	STA AUDV1
 jmp .skipelse6
.skipL01557
	LDA #4
	STA AUDV1
.skipelse6
.L01558 ;  music_pointer  =  music_pointer  +  1

	INC music_pointer
.L01559 ;  rem ' Get right timig for note from right music

.L01560 ;  if music_which  =  0 then music_timer  =  music_notes_intro[music_pointer]

	LDA music_which
	CMP #0
     BNE .skipL01560
.condpart215
	LDX music_pointer
	LDA music_notes_intro,x
	STA music_timer
.skipL01560
.L01561 ;  if music_which  =  1 then music_timer  =  music_notes_gameover[music_pointer]

	LDA music_which
	CMP #1
     BNE .skipL01561
.condpart216
	LDX music_pointer
	LDA music_notes_gameover,x
	STA music_timer
.skipL01561
.L01562 ;  if music_which  =  2 then music_timer  =  music_notes_victory[music_pointer]

	LDA music_which
	CMP #2
     BNE .skipL01562
.condpart217
	LDX music_pointer
	LDA music_notes_victory,x
	STA music_timer
.skipL01562
.L01563 ;  music_pointer  =  music_pointer  +  1

	INC music_pointer
.L01564 ;  temp3  =  music_sizes[music_which]

	LDX music_which
	LDA music_sizes,x
	STA temp3
.L01565 ;  if music_which  =  0 then temp3  =  56

	LDA music_which
	CMP #0
     BNE .skipL01565
.condpart218
	LDA #56
	STA temp3
.skipL01565
.L01566 ;  if music_which  =  1 then temp3  =  20

	LDA music_which
	CMP #1
     BNE .skipL01566
.condpart219
	LDA #20
	STA temp3
.skipL01566
.L01567 ;  if music_which  =  2 then temp3  =  32

	LDA music_which
	CMP #2
     BNE .skipL01567
.condpart220
	LDA #32
	STA temp3
.skipL01567
.L01568 ;  if music_pointer  >=  temp3 then music_pointer  =  0

	LDA music_pointer
	CMP temp3
     BCC .skipL01568
.condpart221
	LDA #0
	STA music_pointer
.skipL01568
.L01569 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.L01570 ;  bank 6

 echo "    ",[(start_bank5 - *)]d , "bytes of ROM space left in bank 5")
 ORG $5FF4-bscode_length
 RORG $9FF4-bscode_length
start_bank5 ldx #$ff
 txs
 lda #>(start-1)
 pha
 lda #<(start-1)
 pha
 pha
 txa
 pha
 tsx
 lda 4,x ; get high byte of return address
 rol   
 rol
 rol
 rol
 and #bs_mask ;1 3 or 7 for F8/F6/F4
 tax
 inx
 lda bankswitch_hotspot-1,x
 pla
 tax
 pla
 rts
 if ((* & $1FFF) > ((bankswitch_hotspot & $1FFF) - 1))
   echo "WARNING: size parameter in banksw.asm too small - the program probably will not work."
   echo "Change to",[(*-begin_bscode+1)&$FF]d,"and try again."
 endif
 ORG $5FFC
 RORG $9FFC
 .word start_bank5
 .word start_bank5
 ORG $6000
 RORG $B000
.L01571 ;  bank 7

 echo "    ",[(start_bank6 - *)]d , "bytes of ROM space left in bank 6")
 ORG $6FF4-bscode_length
 RORG $BFF4-bscode_length
start_bank6 ldx #$ff
 txs
 lda #>(start-1)
 pha
 lda #<(start-1)
 pha
 pha
 txa
 pha
 tsx
 lda 4,x ; get high byte of return address
 rol   
 rol
 rol
 rol
 and #bs_mask ;1 3 or 7 for F8/F6/F4
 tax
 inx
 lda bankswitch_hotspot-1,x
 pla
 tax
 pla
 rts
 if ((* & $1FFF) > ((bankswitch_hotspot & $1FFF) - 1))
   echo "WARNING: size parameter in banksw.asm too small - the program probably will not work."
   echo "Change to",[(*-begin_bscode+1)&$FF]d,"and try again."
 endif
 ORG $6FFC
 RORG $BFFC
 .word start_bank6
 .word start_bank6
 ORG $7000
 RORG $D000
.L01572 ;  bank 8

 echo "    ",[(start_bank7 - *)]d , "bytes of ROM space left in bank 7")
 ORG $7FF4-bscode_length
 RORG $DFF4-bscode_length
start_bank7 ldx #$ff
 txs
 lda #>(start-1)
 pha
 lda #<(start-1)
 pha
 pha
 txa
 pha
 tsx
 lda 4,x ; get high byte of return address
 rol   
 rol
 rol
 rol
 and #bs_mask ;1 3 or 7 for F8/F6/F4
 tax
 inx
 lda bankswitch_hotspot-1,x
 pla
 tax
 pla
 rts
 if ((* & $1FFF) > ((bankswitch_hotspot & $1FFF) - 1))
   echo "WARNING: size parameter in banksw.asm too small - the program probably will not work."
   echo "Change to",[(*-begin_bscode+1)&$FF]d,"and try again."
 endif
 ORG $7FFC
 RORG $DFFC
 .word start_bank7
 .word start_bank7
 ORG $8000
 RORG $F000
; This is a 2-line kernel!
kernel
 sta WSYNC
 lda #255
 sta TIM64T

 lda #1
 sta VDELBL
 sta VDELP0
 ldx ballheight
 inx
 inx
 stx temp4
 lda player1y
 sta temp3

 ifconst shakescreen
   jsr doshakescreen
 else
   ldx missile0height
   inx
 endif

 inx
 stx stack1

 lda bally
 sta stack2

 lda player0y
 ldx #0
 sta WSYNC
 stx GRP0
 stx GRP1
 stx PF1
 stx PF2
 stx CXCLR
 ifconst readpaddle
   stx paddle
 else
   sleep 3
 endif

 sta temp2,x

 ;store these so they can be retrieved later
 ifnconst pfres
   ldx #128-44
 else
   ldx #132-pfres*4
 endif

 inc player1y

 lda missile0y
 sta temp5
 lda missile1y
 sta temp6

 lda playfieldpos
 sta temp1
 
 ifconst pfrowheight
 lda #pfrowheight+2
 else
 ifnconst pfres
   lda #10
 else
   lda #(96/pfres)+2 ; try to come close to the real size
 endif
 endif

 clc
 sbc playfieldpos
 sta playfieldpos
 jmp .startkernel

.skipDrawP0
 lda #0
 tay
 jmp .continueP0

.skipDrawP1
 lda #0
 tay
 jmp .continueP1

.kerloop ; enter at cycle 59??

continuekernel
 sleep 2
continuekernel2
 lda ballheight
 
 ifconst pfres
 ldy playfield+pfres*4-132,x
 sty PF1 ;3
 ldy playfield+pfres*4-131,x
 sty PF2 ;3
 ldy playfield+pfres*4-129,x
 sty PF1 ; 3 too early?
 ldy playfield+pfres*4-130,x
 sty PF2 ;3
 else
 ldy playfield+44-128,x ;4
 sty PF1 ;3
 ldy playfield+45-128,x ;4
 sty PF2 ;3
 ldy playfield+47-128,x ;4
 sty PF1 ; 3 too early?
 ldy playfield+46-128,x;4
 sty PF2 ;3
 endif

 dcp bally
 rol
 rol
; rol
; rol
goback
 sta ENABL 
.startkernel
 lda player1height ;3
 dcp player1y ;5
 bcc .skipDrawP1 ;2
 ldy player1y ;3
 lda (player1pointer),y ;5; player0pointer must be selected carefully by the compiler
			; so it doesn't cross a page boundary!

.continueP1
 sta GRP1 ;3

 ifnconst player1colors
   lda missile1height ;3
   dcp missile1y ;5
   rol;2
   rol;2
   sta ENAM1 ;3
 else
   lda (player1color),y
   sta COLUP1
 ifnconst playercolors
   sleep 7
 else
   lda.w player0colorstore
   sta COLUP0
 endif
 endif

 ifconst pfres
 lda playfield+pfres*4-132,x 
 sta PF1 ;3
 lda playfield+pfres*4-131,x 
 sta PF2 ;3
 lda playfield+pfres*4-129,x 
 sta PF1 ; 3 too early?
 lda playfield+pfres*4-130,x 
 sta PF2 ;3
 else
 lda playfield+44-128,x ;4
 sta PF1 ;3
 lda playfield+45-128,x ;4
 sta PF2 ;3
 lda playfield+47-128,x ;4
 sta PF1 ; 3 too early?
 lda playfield+46-128,x;4
 sta PF2 ;3
 endif 
; sleep 3

 lda player0height
 dcp player0y
 bcc .skipDrawP0
 ldy player0y
 lda (player0pointer),y
.continueP0
 sta GRP0

 ifnconst no_blank_lines
 ifnconst playercolors
   lda missile0height ;3
   dcp missile0y ;5
   sbc stack1
   sta ENAM0 ;3
 else
   lda (player0color),y
   sta player0colorstore
   sleep 6
 endif
   dec temp1
   bne continuekernel
 else
   dec temp1
   beq altkernel2
 ifconst readpaddle
   ldy currentpaddle
   lda INPT0,y
   bpl noreadpaddle
   inc paddle
   jmp continuekernel2
noreadpaddle
   sleep 2
   jmp continuekernel
 else
 ifnconst playercolors 
 ifconst PFcolors
   txa
   tay
   lda (pfcolortable),y
 ifnconst backgroundchange
   sta COLUPF
 else
   sta COLUBK
 endif
   jmp continuekernel
 else
   sleep 12
 endif
 else
   lda (player0color),y
   sta player0colorstore
   sleep 4
 endif
   jmp continuekernel
 endif
altkernel2
   txa
   sbx #252
   bmi lastkernelline
 ifconst pfrowheight
 lda #pfrowheight
 else
 ifnconst pfres
   lda #8
 else
   lda #(96/pfres) ; try to come close to the real size
 endif
 endif
   sta temp1
   jmp continuekernel
 endif

altkernel

 ifconst PFmaskvalue
   lda #PFmaskvalue
 else
   lda #0
 endif
 sta PF1
 sta PF2


 ;sleep 3

 ;28 cycles to fix things
 ;minus 11=17

; lax temp4
; clc
 txa
 sbx #252

 bmi lastkernelline

 ifconst PFcolorandheight
   ldy playfieldcolorandheight-87,x
 ifnconst backgroundchange
   sty COLUPF
 else
   sty COLUBK
 endif
   lda playfieldcolorandheight-88,x
   sta.w temp1
 endif
 ifconst PFheights
   lsr
   lsr
   tay
   lda (pfheighttable),y
   sta.w temp1
 endif
 ifconst PFcolors
   tay
   lda (pfcolortable),y
 ifnconst backgroundchange
   sta COLUPF
 else
   sta COLUBK
 endif
 ifconst pfrowheight
 lda #pfrowheight
 else
 ifnconst pfres
   lda #8
 else
   lda #(96/pfres) ; try to come close to the real size
 endif
 endif
   sta temp1
 endif
 ifnconst PFcolorandheight
 ifnconst PFcolors
 ifnconst PFheights
 ifnconst no_blank_lines
 ; read paddle 0
 ; lo-res paddle read
  ; bit INPT0
  ; bmi paddleskipread
  ; inc paddle0
;donepaddleskip
   sleep 10
 ifconst pfrowheight
   lda #pfrowheight
 else
 ifnconst pfres
   lda #8
 else
   lda #(96/pfres) ; try to come close to the real size
 endif
 endif
   sta temp1
 endif
 endif
 endif
 endif
 

 lda ballheight
 dcp bally
 sbc temp4


 jmp goback


 ifnconst no_blank_lines
lastkernelline
 ifnconst PFcolors
   sleep 10
 else
   ldy #124
   lda (pfcolortable),y
   sta COLUPF
 endif

 ifconst PFheights
 ldx #1
 sleep 4
 else
 ldx playfieldpos
 sleep 3
 endif

 jmp enterlastkernel

 else
lastkernelline
 
 ifconst PFheights
 ldx #1
; sleep 5
 sleep 4
 else
   ldx playfieldpos
; sleep 4
 sleep 3
 endif

   cpx #1
   bne .enterfromNBL
   jmp no_blank_lines_bailout
 endif

 if ((<*)>$d5)
 align 256
 endif
 ; this is a kludge to prevent page wrapping - fix!!!

.skipDrawlastP1
 sleep 2
 lda #0
 jmp .continuelastP1

.endkerloop ; enter at cycle 59??
 nop

.enterfromNBL
 ifconst pfres
 ldy.w playfield+pfres*4-4
 sty PF1 ;3
 ldy.w playfield+pfres*4-3
 sty PF2 ;3
 ldy.w playfield+pfres*4-1
 sty PF1 ; possibly too early?
 ldy.w playfield+pfres*4-2
 sty PF2 ;3
 else
 ldy.w playfield+44
 sty PF1 ;3
 ldy.w playfield+45
 sty PF2 ;3
 ldy.w playfield+47
 sty PF1 ; possibly too early?
 ldy.w playfield+46
 sty PF2 ;3
 endif

enterlastkernel
 lda ballheight

; tya
 dcp bally
; sleep 4

; sbc stack3
 rol
 rol
 sta ENABL 

 lda player1height ;3
 dcp player1y ;5
 bcc .skipDrawlastP1
 ldy player1y ;3
 lda (player1pointer),y ;5; player0pointer must be selected carefully by the compiler
			; so it doesn't cross a page boundary!

.continuelastP1
 sta GRP1 ;3

 ifnconst player1colors
   lda missile1height ;3
   dcp missile1y ;5
 else
   lda (player1color),y
   sta COLUP1
 endif

 dex
 ;dec temp4 ; might try putting this above PF writes
 beq endkernel


 ifconst pfres
 ldy.w playfield+pfres*4-4
 sty PF1 ;3
 ldy.w playfield+pfres*4-3
 sty PF2 ;3
 ldy.w playfield+pfres*4-1
 sty PF1 ; possibly too early?
 ldy.w playfield+pfres*4-2
 sty PF2 ;3
 else
 ldy.w playfield+44
 sty PF1 ;3
 ldy.w playfield+45
 sty PF2 ;3
 ldy.w playfield+47
 sty PF1 ; possibly too early?
 ldy.w playfield+46
 sty PF2 ;3
 endif

 ifnconst player1colors
   rol;2
   rol;2
   sta ENAM1 ;3
 else
 ifnconst playercolors
   sleep 7
 else
   lda.w player0colorstore
   sta COLUP0
 endif
 endif
 
 lda.w player0height
 dcp player0y
 bcc .skipDrawlastP0
 ldy player0y
 lda (player0pointer),y
.continuelastP0
 sta GRP0



 ifnconst no_blank_lines
   lda missile0height ;3
   dcp missile0y ;5
   sbc stack1
   sta ENAM0 ;3
   jmp .endkerloop
 else
 ifconst readpaddle
   ldy currentpaddle
   lda INPT0,y
   bpl noreadpaddle2
   inc paddle
   jmp .endkerloop
noreadpaddle2
   sleep 4
   jmp .endkerloop
 else ; no_blank_lines and no paddle reading
 sleep 14
 jmp .endkerloop
 endif
 endif


;  ifconst donepaddleskip
;paddleskipread
 ; this is kind of lame, since it requires 4 cycles from a page boundary crossing
 ; plus we get a lo-res paddle read
; bmi donepaddleskip
;  endif

.skipDrawlastP0
 sleep 2
 lda #0
 jmp .continuelastP0

 ifconst no_blank_lines
no_blank_lines_bailout
 ldx #0
 endif

endkernel
 ; 6 digit score routine
 stx PF1
 stx PF2
 stx PF0
 clc

 ifconst pfrowheight
 lda #pfrowheight+2
 else
 ifnconst pfres
   lda #10
 else
   lda #(96/pfres)+2 ; try to come close to the real size
 endif
 endif

 sbc playfieldpos
 sta playfieldpos
 txa

 ifconst shakescreen
   bit shakescreen
   bmi noshakescreen2
   ldx #$3D
noshakescreen2
 endif

   sta WSYNC,x

;                STA WSYNC ;first one, need one more
 sta REFP0
 sta REFP1
                STA GRP0
                STA GRP1
 ;               STA PF1
   ;             STA PF2
 sta HMCLR
 sta ENAM0
 sta ENAM1
 sta ENABL

 lda temp2 ;restore variables that were obliterated by kernel
 sta player0y
 lda temp3
 sta player1y
 ifnconst player1colors
   lda temp6
   sta missile1y
 endif
 ifnconst playercolors
 ifnconst readpaddle
   lda temp5
   sta missile0y
 endif
 endif
 lda stack2
 sta bally

 ifconst no_blank_lines
 sta WSYNC
 endif

 lda INTIM
 clc
 ifnconst vblank_time
 adc #43+12+87
 else
 adc #vblank_time+12+87
 endif
; sta WSYNC
 sta TIM64T

 ifconst minikernel
 jsr minikernel
 endif

 ; now reassign temp vars for score pointers

; score pointers contain:
; score1-5: lo1,lo2,lo3,lo4,lo5,lo6
; swap lo2->temp1
; swap lo4->temp3
; swap lo6->temp5
 ifnconst noscore
 lda scorepointers+1
; ldy temp1
 sta temp1
; sty scorepointers+1

 lda scorepointers+3
; ldy temp3
 sta temp3
; sty scorepointers+3


 sta HMCLR
 tsx
 stx stack1 
 ldx #$10
 stx HMP0

 sta WSYNC
 ldx #0
                STx GRP0
                STx GRP1 ; seems to be needed because of vdel

 lda scorepointers+5
; ldy temp5
 sta temp5,x
; sty scorepointers+5
 lda #>scoretable
 sta scorepointers+1
 sta scorepointers+3
 sta scorepointers+5,x
 sta temp2,x
 sta temp4,x
 sta temp6,x
                LDY #7
                STA RESP0
                STA RESP1


        LDA #$03
        STA NUSIZ0
        STA NUSIZ1,x
        STA VDELP0
        STA VDELP1
        LDA #$20
        STA HMP1
               LDA scorecolor 
;               STA HMCLR
;               STA WSYNC; second one
                STA HMOVE ; cycle 73 ?

                STA COLUP0
                STA COLUP1
 lda  (scorepointers),y
 sta  GRP0
 ifconst pfscore
 lda pfscorecolor
 sta COLUPF
 endif
 lda  (scorepointers+8),y
 sta WSYNC
 sleep 2
 jmp beginscore

 if ((<*)>$d4)
 align 256 ; kludge that potentially wastes space!  should be fixed!
 endif

loop2
 lda  (scorepointers),y     ;+5  68  204
 sta  GRP0            ;+3  71  213      D1     --      --     --
 ifconst pfscore
 lda.w pfscore1
 sta PF1
 else
 sleep 7
 endif
 ; cycle 0
 lda  (scorepointers+$8),y  ;+5   5   15
beginscore
 sta  GRP1            ;+3   8   24      D1     D1      D2     --
 lda  (scorepointers+$6),y  ;+5  13   39
 sta  GRP0            ;+3  16   48      D3     D1      D2     D2
 lax  (scorepointers+$2),y  ;+5  29   87
 txs
 lax  (scorepointers+$4),y  ;+5  36  108
 sleep 3

 ifconst pfscore
 lda pfscore2
 sta PF1
 else
 sleep 6
 endif

 lda  (scorepointers+$A),y  ;+5  21   63
 stx  GRP1            ;+3  44  132      D3     D3      D4     D2!
 tsx
 stx  GRP0            ;+3  47  141      D5     D3!     D4     D4
 sta  GRP1            ;+3  50  150      D5     D5      D6     D4!
 sty  GRP0            ;+3  53  159      D4*    D5!     D6     D6
 dey
 bpl  loop2           ;+2  60  180

 ldx stack1 
 txs
; lda scorepointers+1
 ldy temp1
; sta temp1
 sty scorepointers+1

                LDA #0   
 sta PF1
               STA GRP0
                STA GRP1
        STA VDELP0
        STA VDELP1;do we need these
        STA NUSIZ0
        STA NUSIZ1

; lda scorepointers+3
 ldy temp3
; sta temp3
 sty scorepointers+3

; lda scorepointers+5
 ldy temp5
; sta temp5
 sty scorepointers+5
 endif ;noscore
 LDA #%11000010
 sta WSYNC
 STA VBLANK
 RETURN

 ifconst shakescreen
doshakescreen
   bit shakescreen
   bmi noshakescreen
   sta WSYNC
noshakescreen
   ldx missile0height
   inx
   rts
 endif

start
 sei
 cld
 ldy #0
 lda $D0
 cmp #$2C               ;check RAM location #1
 bne MachineIs2600
 lda $D1
 cmp #$A9               ;check RAM location #2
 bne MachineIs2600
 dey
MachineIs2600
 ldx #0
 txa
clearmem
 inx
 txs
 pha
 bne clearmem
 sty temp1
 ifconst pfrowheight
 lda pfrowheight
 else
 ifconst pfres
 lda #(96/pfres)
 else
 lda #8
 endif
 endif
 sta playfieldpos
 ldx #5
initscore
 lda #<scoretable
 sta scorepointers,x 
 dex
 bpl initscore
 lda #1
 sta CTRLPF
 ora INTIM
 sta rand

 ifconst multisprite
   jsr multisprite_setup
 endif

 ifnconst bankswitch
   jmp game
 else
   lda #>(game-1)
   pha
   lda #<(game-1)
   pha
   pha
   pha
   ldx #1
   jmp BS_jsr
 endif
; playfield drawing routines
; you get a 32x12 bitmapped display in a single color :)
; 0-31 and 0-11

pfclear ; clears playfield - or fill with pattern
 ifconst pfres
 ldx #pfres*4-1
 else
 ldx #47
 endif
pfclear_loop
 ifnconst superchip
 sta playfield,x
 else
 sta playfield-128,x
 endif
 dex
 bpl pfclear_loop
 RETURN
 
setuppointers
 stx temp2 ; store on.off.flip value
 tax ; put x-value in x 
 lsr
 lsr
 lsr ; divide x pos by 8 
 sta temp1
 tya
 asl
 asl ; multiply y pos by 4
 clc
 adc temp1 ; add them together to get actual memory location offset
 tay ; put the value in y
 lda temp2 ; restore on.off.flip value
 rts

pfread
;x=xvalue, y=yvalue
 jsr setuppointers
 lda setbyte,x
 and playfield,y
 eor setbyte,x
; beq readzero
; lda #1
; readzero
 RETURN

pfpixel
;x=xvalue, y=yvalue, a=0,1,2
 jsr setuppointers

 ifconst bankswitch
 lda temp2 ; load on.off.flip value (0,1, or 2)
 beq pixelon_r  ; if "on" go to on
 lsr
 bcs pixeloff_r ; value is 1 if true
 lda playfield,y ; if here, it's "flip"
 eor setbyte,x
 ifconst superchip
 sta playfield-128,y
 else
 sta playfield,y
 endif
 RETURN
pixelon_r
 lda playfield,y
 ora setbyte,x
 ifconst superchip
 sta playfield-128,y
 else
 sta playfield,y
 endif
 RETURN
pixeloff_r
 lda setbyte,x
 eor #$ff
 and playfield,y
 ifconst superchip
 sta playfield-128,y
 else
 sta playfield,y
 endif
 RETURN

 else
 jmp plotpoint
 endif

pfhline
;x=xvalue, y=yvalue, a=0,1,2, temp3=endx
 jsr setuppointers
 jmp noinc
keepgoing
 inx
 txa
 and #7
 bne noinc
 iny
noinc
 jsr plotpoint
 cpx temp3
 bmi keepgoing
 RETURN

pfvline
;x=xvalue, y=yvalue, a=0,1,2, temp3=endx
 jsr setuppointers
 sty temp1 ; store memory location offset
 inc temp3 ; increase final x by 1 
 lda temp3
 asl
 asl ; multiply by 4
 sta temp3 ; store it
 ; Thanks to Michael Rideout for fixing a bug in this code
 ; right now, temp1=y=starting memory location, temp3=final
 ; x should equal original x value
keepgoingy
 jsr plotpoint
 iny
 iny
 iny
 iny
 cpy temp3
 bmi keepgoingy
 RETURN

plotpoint
 lda temp2 ; load on.off.flip value (0,1, or 2)
 beq pixelon  ; if "on" go to on
 lsr
 bcs pixeloff ; value is 1 if true
 lda playfield,y ; if here, it's "flip"
 eor setbyte,x
  ifconst superchip
 sta playfield-128,y
 else
 sta playfield,y
 endif
 rts
pixelon
 lda playfield,y
 ora setbyte,x
 ifconst superchip
 sta playfield-128,y
 else
 sta playfield,y
 endif
 rts
pixeloff
 lda setbyte,x
 eor #$ff
 and playfield,y
 ifconst superchip
 sta playfield-128,y
 else
 sta playfield,y
 endif
 rts

setbyte
 .byte $80
 .byte $40
 .byte $20
 .byte $10
 .byte $08
 .byte $04
 .byte $02
 .byte $01
 .byte $01
 .byte $02
 .byte $04
 .byte $08
 .byte $10
 .byte $20
 .byte $40
 .byte $80
 .byte $80
 .byte $40
 .byte $20
 .byte $10
 .byte $08
 .byte $04
 .byte $02
 .byte $01
 .byte $01
 .byte $02
 .byte $04
 .byte $08
 .byte $10
 .byte $20
 .byte $40
 .byte $80
pfscroll ;(a=0 left, 1 right, 2 up, 4 down, 6=upup, 12=downdown)
 bne notleft
;left
 ifconst pfres
 ldx #pfres*4
 else
 ldx #48
 endif
leftloop
 lda playfield-1,x
 lsr

 ifconst superchip
 lda playfield-2,x
 rol
 sta playfield-130,x
 lda playfield-3,x
 ror
 sta playfield-131,x
 lda playfield-4,x
 rol
 sta playfield-132,x
 lda playfield-1,x
 ror
 sta playfield-129,x
 else
 rol playfield-2,x
 ror playfield-3,x
 rol playfield-4,x
 ror playfield-1,x
 endif

 txa
 sbx #4
 bne leftloop
 RETURN

notleft
 lsr
 bcc notright
;right

 ifconst pfres
 ldx #pfres*4
 else
 ldx #48
 endif
rightloop
 lda playfield-4,x
 lsr
 ifconst superchip
 lda playfield-3,x
 rol
 sta playfield-131,x
 lda playfield-2,x
 ror
 sta playfield-130,x
 lda playfield-1,x
 rol
 sta playfield-129,x
 lda playfield-4,x
 ror
 sta playfield-132,x
 else
 rol playfield-3,x
 ror playfield-2,x
 rol playfield-1,x
 ror playfield-4,x
 endif
 txa
 sbx #4
 bne rightloop
  RETURN

notright
 lsr
 bcc notup
;up
 lsr
 bcc onedecup
 dec playfieldpos
onedecup
 dec playfieldpos
 beq shiftdown 
 bpl noshiftdown2 
shiftdown
  ifconst pfrowheight
 lda #pfrowheight
 else
 ifnconst pfres
   lda #8
 else
   lda #(96/pfres) ; try to come close to the real size
 endif
 endif

 sta playfieldpos
 lda playfield+3
 sta temp4
 lda playfield+2
 sta temp3
 lda playfield+1
 sta temp2
 lda playfield
 sta temp1
 ldx #0
up2
 lda playfield+4,x
 ifconst superchip
 sta playfield-128,x
 lda playfield+5,x
 sta playfield-127,x
 lda playfield+6,x
 sta playfield-126,x
 lda playfield+7,x
 sta playfield-125,x
 else
 sta playfield,x
 lda playfield+5,x
 sta playfield+1,x
 lda playfield+6,x
 sta playfield+2,x
 lda playfield+7,x
 sta playfield+3,x
 endif
 txa
 sbx #252
 ifconst pfres
 cpx #(pfres-1)*4
 else
 cpx #44
 endif
 bne up2

 lda temp4
 
 ifconst superchip
 ifconst pfres
 sta playfield+pfres*4-129
 lda temp3
 sta playfield+pfres*4-130
 lda temp2
 sta playfield+pfres*4-131
 lda temp1
 sta playfield+pfres*4-132
 else
 sta playfield+47-128
 lda temp3
 sta playfield+46-128
 lda temp2
 sta playfield+45-128
 lda temp1
 sta playfield+44-128
 endif
 else
 ifconst pfres
 sta playfield+pfres*4-1
 lda temp3
 sta playfield+pfres*4-2
 lda temp2
 sta playfield+pfres*4-3
 lda temp1
 sta playfield+pfres*4-4
 else
 sta playfield+47
 lda temp3
 sta playfield+46
 lda temp2
 sta playfield+45
 lda temp1
 sta playfield+44
 endif
 endif
noshiftdown2
 RETURN


notup
;down
 lsr
 bcs oneincup
 inc playfieldpos
oneincup
 inc playfieldpos
 lda playfieldpos

  ifconst pfrowheight
 cmp #pfrowheight+1
 else
 ifnconst pfres
   cmp #9
 else
   cmp #(96/pfres)+1 ; try to come close to the real size
 endif
 endif

 bcc noshiftdown 
 lda #1
 sta playfieldpos

 ifconst pfres
 lda playfield+pfres*4-1
 sta temp4
 lda playfield+pfres*4-2
 sta temp3
 lda playfield+pfres*4-3
 sta temp2
 lda playfield+pfres*4-4
 else
 lda playfield+47
 sta temp4
 lda playfield+46
 sta temp3
 lda playfield+45
 sta temp2
 lda playfield+44
 endif

 sta temp1

 ifconst pfres
 ldx #(pfres-1)*4
 else
 ldx #44
 endif
down2
 lda playfield-1,x
 ifconst superchip
 sta playfield-125,x
 lda playfield-2,x
 sta playfield-126,x
 lda playfield-3,x
 sta playfield-127,x
 lda playfield-4,x
 sta playfield-128,x
 else
 sta playfield+3,x
 lda playfield-2,x
 sta playfield+2,x
 lda playfield-3,x
 sta playfield+1,x
 lda playfield-4,x
 sta playfield,x
 endif
 txa
 sbx #4
 bne down2

 lda temp4
 ifconst superchip
 sta playfield-125
 lda temp3
 sta playfield-126
 lda temp2
 sta playfield-127
 lda temp1
 sta playfield-128
 else
 sta playfield+3
 lda temp3
 sta playfield+2
 lda temp2
 sta playfield+1
 lda temp1
 sta playfield
 endif
noshiftdown
 RETURN
;standard routines needed for pretty much all games
; just the random number generator is left - maybe we should remove this asm file altogether?
; repositioning code and score pointer setup moved to overscan
; read switches, joysticks now compiler generated (more efficient)

randomize
	lda rand
	lsr
 ifconst rand16
	rol rand16
 endif
	bcc noeor
	eor #$B4
noeor
	sta rand
 ifconst rand16
	eor rand16
 endif
	RETURN
drawscreen
 ifconst debugscore
   ldx #14
   lda INTIM ; display # cycles left in the score

 ifconst mincycles
 lda mincycles 
 cmp INTIM
 lda mincycles
 bcc nochange
 lda INTIM
 sta mincycles
nochange
 endif

;   cmp #$2B
;   bcs no_cycles_left
   bmi cycles_left
   ldx #64
   eor #$ff ;make negative
cycles_left
   stx scorecolor
   and #$7f ; clear sign bit
   tax
   lda scorebcd,x
   sta score+2
   lda scorebcd1,x
   sta score+1
   jmp done_debugscore   
scorebcd
 .byte $00, $64, $28, $92, $56, $20, $84, $48, $12, $76, $40
 .byte $04, $68, $32, $96, $60, $24, $88, $52, $16, $80, $44
 .byte $08, $72, $36, $00, $64, $28, $92, $56, $20, $84, $48
 .byte $12, $76, $40, $04, $68, $32, $96, $60, $24, $88
scorebcd1
 .byte 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 6
 .byte 7, 7, 8, 8, 9, $10, $10, $11, $12, $12, $13
 .byte $14, $14, $15, $16, $16, $17, $17, $18, $19, $19, $20
 .byte $21, $21, $22, $23, $23, $24, $24, $25, $26, $26
done_debugscore
 endif

 ifconst debugcycles
   lda INTIM ; if we go over, it mucks up the background color
;   cmp #$2B
;   BCC overscan
   bmi overscan
   sta COLUBK
   bcs doneoverscan
 endif

 
overscan
 lda INTIM ;wait for sync
 bmi overscan
doneoverscan
;do VSYNC
 lda #2
 sta WSYNC
 sta VSYNC
 STA WSYNC
 STA WSYNC
 LDA #0
 STA WSYNC
 STA VSYNC
 sta VBLANK
 ifnconst overscan_time
 lda #37+128
 else
 lda #overscan_time+128
 endif
 sta TIM64T

 ifconst legacy
 if legacy < 100
 ldx #4
adjustloop
 lda player0x,x
 sec
 sbc #14 ;?
 sta player0x,x
 dex
 bpl adjustloop
 endif
 endif
 if (<*)>$F0
 ;align 256, $ea
 ; the above puts in zeros. Why? replaced by repeat below:
 repeat ($100-(<*))
 nop
 repend
 endif
  sta WSYNC
  ldx #4
  SLEEP 3
HorPosLoop       ;     5
  lda player0x,X  ;+4   9
  sec           ;+2  11
DivideLoop
  sbc #15
  bcs DivideLoop;+4  15
  sta temp1,X    ;+4  19
  sta RESP0,X   ;+4  23
  sta WSYNC
  dex
  bpl HorPosLoop;+5   5
                ;     4

  ldx #4
  ldy temp1,X
  lda repostable-256,Y
  sta HMP0,X    ;+14 18

  dex
  ldy temp1,X
  lda repostable-256,Y
  sta HMP0,X    ;+14 32

  dex
  ldy temp1,X
  lda repostable-256,Y
  sta HMP0,X    ;+14 46

  dex
  ldy temp1,X
  lda repostable-256,Y
  sta HMP0,X    ;+14 60

  dex
  ldy temp1,X
  lda repostable-256,Y
  sta HMP0,X    ;+14 74

  sta WSYNC
 
  sta HMOVE     ;+3   3


 ifconst legacy
 if legacy < 100
 ldx #4
adjustloop2
 lda player0x,x
 clc
 adc #14 ;?
 sta player0x,x
 dex
 bpl adjustloop2
 endif
 endif




;set score pointers
 lax score+2
 jsr scorepointerset
 sty scorepointers+5
 stx scorepointers+2
 lax score+1
 jsr scorepointerset
 sty scorepointers+4
 stx scorepointers+1
 lax score
 jsr scorepointerset
 sty scorepointers+3
 stx scorepointers

vblk
; run possible vblank bB code
 ifconst vblank_bB_code
   jsr vblank_bB_code
 endif
vblk2
 LDA INTIM
 bmi vblk2
 jmp kernel
 

    .byte $80,$70,$60,$50,$40,$30,$20,$10,$00
    .byte $F0,$E0,$D0,$C0,$B0,$A0,$90
repostable

scorepointerset
 and #$0F
 asl
 asl
 asl
 adc #<scoretable
 tay 
 txa
 arr #$F0
 tax
 sbx #<(256-<scoretable)
 rts
;bB.asm
; bB.asm file is split here
.L01573 ;  inline 6lives_statusbar.asm

 include 6lives_statusbar.asm

 if (<*) > (<(*+8))
	repeat ($100-<*)
	.byte 0
	repend
	endif
lives__L0306
	.byte   %00010000
	.byte   %00010000
	.byte   %01010100
	.byte   %10111010
	.byte   %01010100
	.byte   %00010000
	.byte   %00010000
	.byte   %00010000
 if (<*) > (<(*+8))
	repeat ($100-<*)
	.byte 0
	repend
	endif
lives__L0821
	.byte   %00010000
	.byte   %00010000
	.byte   %01010100
	.byte   %10111010
	.byte   %01010100
	.byte   %00010000
	.byte   %00010000
	.byte   %00010000
 if (<*) > (<(*+14))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL0880_0

	.byte 0
	.byte   %01101100
	.byte   %01101100
	.byte   %01101100
	.byte   %01111100
	.byte   %10111010
	.byte   %01111100
	.byte   %10111010
	.byte   %11111110
	.byte   %11111110
	.byte   %11111110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+14))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL0883_0

	.byte 0
	.byte   %01101100
	.byte   %01101100
	.byte   %01111100
	.byte   %01111100
	.byte   %10111010
	.byte   %01111100
	.byte   %10111010
	.byte   %11111110
	.byte   %11010110
	.byte   %11010110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+14))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL0888_0

	.byte 0
	.byte   %00011100
	.byte   %10011000
	.byte   %10011000
	.byte   %10111000
	.byte   %10111100
	.byte   %10011100
	.byte   %10111000
	.byte   %11111100
	.byte   %11110100
	.byte   %11110110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+15))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL0891_0

	.byte 0
	.byte   %01100000
	.byte   %01100000
	.byte   %01101100
	.byte   %01101100
	.byte   %01111101
	.byte   %00111010
	.byte   %10111100
	.byte   %11111010
	.byte   %11111110
	.byte   %11111110
	.byte   %11111110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+15))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL0894_0

	.byte 0
	.byte   %01100000
	.byte   %01100000
	.byte   %01101100
	.byte   %01111100
	.byte   %01111101
	.byte   %00111010
	.byte   %10111100
	.byte   %11111010
	.byte   %11111110
	.byte   %11010110
	.byte   %11010110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+14))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL0899_0

	.byte 0
	.byte   %11000011
	.byte   %01100110
	.byte   %00111100
	.byte   %10111001
	.byte   %11111110
	.byte   %10111100
	.byte   %10111000
	.byte   %11111100
	.byte   %11110100
	.byte   %11110110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel139
 .byte  skyblue, black, black, riverblue
 .byte  skyblue, black, black, riverblue
 .byte  skyblue, black, black, riverblue
 .byte  red, black, black, riverblue
 .byte  red, black, black, riverblue
 .byte  red, black, black, white
 .byte  red, black, black, riverblue
 .byte  red, black, black, riverblue
 .byte  red, black, black, riverblue
 .byte  red, black, black, riverblue
 .byte  skyblue, black, black, riverblue
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel156
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, brown, green, green
 .byte  white, brown, green, green
 .byte  white, brown, green, green
 .byte  white, brown, green, green
 .byte  white, brown, green, green
 .byte  white, brown, green, green
 .byte  white, green, green, green
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel173
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, red, red, riverblue
 .byte  red, black, black, riverblue
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel190
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 .byte  white, green, green, green
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel207
 .byte  white, brown, red, riverblue
 .byte  white, brown, white, riverblue
 .byte  white, brown, white, riverblue
 .byte  white, red, red + 8, riverblue
 .byte  white, red, red + 6, riverblue
 .byte  white, red, red + 4, riverblue
 .byte  white, red, white, riverblue
 .byte  white, red, white, riverblue
 .byte  white, red, white, riverblue
 .byte  white, red, white, riverblue
 .byte  white, red, red, riverblue
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel224
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, brown, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  green, green, green, green
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel241
 .byte  white, red, white, riverblue
 .byte  white, red + 8, red + 8, riverblue
 .byte  white, red + 6, red + 6, riverblue
 .byte  white, red + 4, red + 4, riverblue
 .byte  white, white, white, riverblue
 .byte  white, white, white, riverblue
 .byte  white, white, white, riverblue
 .byte  white, white, white, riverblue
 .byte  white, white, red, riverblue
 .byte  white, white, red, riverblue
 .byte  white, red, red, riverblue
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel258
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, brown, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, brown
 .byte  gray, gray, green, brown
 .byte  gray, gray, brown, brown
 .byte  gray, gray, green, green
 .byte  green, green, green, green
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel275
 .byte  white, white, white, riverblue
 .byte  white, white, red + 8, riverblue
 .byte  white, white, red + 6, riverblue
 .byte  white, red + 8, red + 4, riverblue
 .byte  white, red + 6, white, riverblue
 .byte  white, white, white, riverblue
 .byte  white, white, white, riverblue
 .byte  white, white, white, riverblue
 .byte  white, white, red, riverblue
 .byte  white, white, red, riverblue
 .byte  white, white, red, riverblue
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel292
 .byte  yellow, yellow, gray, green
 .byte  green, green, gray, green
 .byte  yellow, yellow, gray, green
 .byte  green, green, gray, green
 .byte  green, green, gray, green
 .byte  yellow, yellow, gray, green
 .byte  green, green, gray, green
 .byte  yellow, yellow, gray, green
 .byte  green, green, gray, green
 .byte  yellow, yellow, gray, green
 .byte  green, green, gray,0
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel309
 .byte  brown, green, green, riverblue
 .byte  brown, green, green, riverblue
 .byte  brown, green, green, riverblue
 .byte  brown, brown, brown, 144 + 8
 .byte  brown, green, green, 144 + 8
 .byte  brown, green, green, 144 + 4
 .byte  brown, green, green, 144 + 4
 .byte  brown, green, green, 144 + 2
 .byte  brown, brown, brown, 144 + 2
 .byte  brown, 144, 144, 144
 .byte  brown, green, green, 144
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel326
 .byte  red, brown, gray, green
 .byte  white, brown, gray, green
 .byte  white, green, gray, green
 .byte  white, green, gray, green
 .byte  white, green, gray, green
 .byte  white, brown, gray, green
 .byte  white, brown, gray, green
 .byte  white, brown, gray, green
 .byte  144, brown, gray, green
 .byte  144, brown, gray, green
 .byte  red, green, gray, green
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel343
 .byte  brown, white, riverblue, white
 .byte  brown, red + 8, riverblue, white
 .byte  brown, red + 6, riverblue, white
 .byte  brown, red + 4, 144 + 8, white
 .byte  brown, white, 144 + 7, white
 .byte  brown, white, 144 + 6, black
 .byte  brown, white, 144 + 5, black
 .byte  brown, white, 144 + 4, black
 .byte  brown, green, 144 + 3, black
 .byte  brown, green, 144, black
 .byte  brown, red, riverblue, black
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel360
 .byte  144, green, gray, red
 .byte  144, green, gray, white
 .byte  144, brown, gray, white
 .byte  144, brown, gray, white
 .byte  144, brown, gray, white
 .byte  144, brown, gray, red
 .byte  144, 144, gray, red
 .byte  144, 144, gray, 144
 .byte  144, 144, gray, 144
 .byte  144, 144, 144, 144
 .byte  144, green, gray, 144
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel377
 .byte  144, red, 144, black
 .byte  144, red, 144, black
 .byte  144, red, 144, black
 .byte  144, red, 144, black
 .byte  144, red, 144, black
 .byte  144, red, 144, black
 .byte  144, red, 144, black
 .byte  144, 144, 144, black
 .byte  144, 144, 144, black
 .byte  144, 144, 144, 144
 .byte  144, 144, 144, black
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel394
 .byte  144, riverblue, 144, 144
 .byte  144, riverblue, 144, 144
 .byte  144, riverblue, 144, 144
 .byte  144, 144 + 8, 144, 144
 .byte  144, 144 + 8, 144, 144
 .byte  144, 144 + 4, 144, 144
 .byte  144, 144 + 4, 144, 144
 .byte  144, 144 + 2, 144, 144
 .byte  144, 144 + 2, 144, 144
 .byte  144, 144, 144, 144
 .byte  144, 144, 144, 144
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01287_1

	.byte 0
	.byte   %10000001
	.byte   %01000010
	.byte   %00100100
	.byte   %00011000
	.byte   %00011000
	.byte   %00100100
	.byte   %01000010
	.byte   %10000001
 if (<*) > (<(*+6))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01290_1

	.byte 0
	.byte   %01111100
	.byte   %11111110
	.byte   %11010110
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01293_1

	.byte 0
	.byte   %00100100
	.byte   %01011010
	.byte   %10111101
	.byte   %01111110
	.byte   %01011010
	.byte   %01000010
	.byte   %10100101
	.byte   %10100101
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01296_1

	.byte 0
	.byte   %01000010
	.byte   %10000001
	.byte   %10011001
	.byte   %10111101
	.byte   %11111111
	.byte   %11100111
	.byte   %11000011
	.byte   %01000010
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01299_1

	.byte 0
	.byte   %00101000
	.byte   %00010010
	.byte   %00011111
	.byte   %00111100
	.byte   %01100101
	.byte   %10000010
	.byte   %10010001
	.byte   %01100000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01303_1

	.byte 0
	.byte   %00011000
	.byte   %00111100
	.byte   %01111110
	.byte   %11011011
	.byte   %11111111
	.byte   %01111110
	.byte   %11011011
	.byte   %11000011
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01307_1

	.byte 0
	.byte   %10100101
	.byte   %10011001
	.byte   %01011010
	.byte   %00111100
	.byte   %01100110
	.byte   %10111101
	.byte   %10100101
	.byte   %00100100
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01311_1

	.byte 0
	.byte   %00111110
	.byte   %01000011
	.byte   %00111000
	.byte   %00000100
	.byte   %00111000
	.byte   %01000000
	.byte   %00110000
	.byte   %00001000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01315_1

	.byte 0
	.byte   %10001100
	.byte   %11011110
	.byte   %11111101
	.byte   %11111110
	.byte   %11111111
	.byte   %11111011
	.byte   %11011110
	.byte   %10001100
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01319_1

	.byte 0
	.byte   %11011011
	.byte   %01100110
	.byte   %01000010
	.byte   %11111111
	.byte   %11011011
	.byte   %11111111
	.byte   %01011010
	.byte   %00100100
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01323_1

	.byte 0
	.byte   %10100101
	.byte   %10100101
	.byte   %01111110
	.byte   %01111100
	.byte   %10000111
	.byte   %10001101
	.byte   %10001110
	.byte   %00000100
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01327_1

	.byte 0
	.byte   %01101100
	.byte   %01101100
	.byte   %01101100
	.byte   %01101100
	.byte   %00101000
	.byte   %10111011
	.byte   %01111111
	.byte   %01111101
	.byte   %01101101
	.byte   %01010100
	.byte   %11111110
	.byte   %00111000
	.byte   %01010100
	.byte   %10010010
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01331_1

	.byte 0
	.byte   %01101100
	.byte   %01101100
	.byte   %01101100
	.byte   %01101100
	.byte   %00101010
	.byte   %00110110
	.byte   %01101001
	.byte   %01010001
	.byte   %01111111
	.byte   %01010001
	.byte   %01101001
	.byte   %00110110
	.byte   %01111110
	.byte   %01010100
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01335_1

	.byte 0
	.byte   %01101100
	.byte   %01101100
	.byte   %01101100
	.byte   %01111100
	.byte   %10111010
	.byte   %11111110
	.byte   %11111110
	.byte   %11111110
	.byte   %00111000
	.byte   %01111100
	.byte   %11010110
	.byte   %10010010
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
	.byte   %01010100
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01339_1

	.byte 0
	.byte   %00011100
	.byte   %00001000
	.byte   %10001000
	.byte   %11111001
	.byte   %10011111
	.byte   %00010001
	.byte   %00010000
	.byte   %00111000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01343_1

	.byte 0
	.byte   %00101100
	.byte   %01101000
	.byte   %00101000
	.byte   %00101100
	.byte   %01101000
	.byte   %10111001
	.byte   %01111001
	.byte   %01011001
	.byte   %01111101
	.byte   %01101110
	.byte   %01111110
	.byte   %00111000
	.byte   %01011100
	.byte   %01110110
	.byte   %00111110
	.byte   %00011000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01347_1

	.byte 0
	.byte   %01101100
	.byte   %00101000
	.byte   %00101000
	.byte   %00101000
	.byte   %10101010
	.byte   %10010010
	.byte   %10111010
	.byte   %01010100
	.byte   %00111000
	.byte   %00010000
	.byte   %00101000
	.byte   %01111100
	.byte   %01010100
	.byte   %10010010
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01351_1

	.byte 0
	.byte   %10101010
	.byte   %11111110
	.byte   %11111110
	.byte   %11111110
	.byte   %01111100
	.byte   %01111100
	.byte   %01111100
	.byte   %11111110
	.byte   %11111110
	.byte   %10111010
	.byte   %00111000
	.byte   %01111100
	.byte   %11010110
	.byte   %10111010
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01355_1

	.byte 0
	.byte   %00101001
	.byte   %01111101
	.byte   %01111101
	.byte   %01111101
	.byte   %00111001
	.byte   %10111011
	.byte   %10010011
	.byte   %01111101
	.byte   %00010001
	.byte   %00111001
	.byte   %01111101
	.byte   %01010101
	.byte   %01111101
	.byte   %00111001
	.byte   %00111010
	.byte   %00010001
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01359_1

	.byte 0
	.byte   %00010000
	.byte   %00111000
	.byte   %01010100
	.byte   %00010000
	.byte   %00101000
	.byte   %01000100
	.byte   %00101000
	.byte   %00010000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01363_1

	.byte 0
	.byte   %10101010
	.byte   %01111100
	.byte   %00111000
	.byte   %00111000
	.byte   %00111000
	.byte   %00111000
	.byte   %00111000
	.byte   %00111000
	.byte   %01111000
	.byte   %10111100
	.byte   %00111010
	.byte   %01111000
	.byte   %10111100
	.byte   %00010010
	.byte   %00110000
	.byte   %01001000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01367_1

	.byte 0
	.byte   %01111110
	.byte   %00111100
	.byte   %00011000
	.byte   %00011000
	.byte   %00011000
	.byte   %11111111
	.byte   %01111110
	.byte   %00111100
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01371_1

	.byte 0
	.byte   %11111111
	.byte   %10100101
	.byte   %10100101
	.byte   %10010101
	.byte   %11001101
	.byte   %10110101
	.byte   %10000101
	.byte   %11111111
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01375_1

	.byte 0
	.byte   %10010000
	.byte   %01100000
	.byte   %01110000
	.byte   %10111000
	.byte   %00011100
	.byte   %00001110
	.byte   %00000111
	.byte   %00000011
 if (<*) > (<(*+25))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01379_1

	.byte 0
	.byte   %00111100
	.byte   %01111110
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11000111
	.byte   %11000111
	.byte   %11111111
	.byte   %10010011
	.byte   %10010011
	.byte   %10010011
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %11111111
	.byte   %01111110
	.byte   %00111100
 if (<*) > (<(*+25))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01383_1

	.byte 0
	.byte   %01000100
	.byte   %10101010
	.byte   %10101010
	.byte   %01000100
	.byte   %01000100
	.byte   %00000000
	.byte   %00101000
	.byte   %00111000
	.byte   %01111100
	.byte   %11111110
	.byte   %11010110
	.byte   %10111010
	.byte   %11111110
	.byte   %01111101
	.byte   %01111100
	.byte   %01111100
	.byte   %01111100
	.byte   %10111010
	.byte   %00111000
	.byte   %01111000
	.byte   %00111000
	.byte   %00010100
	.byte   %00010000
	.byte   %00010000
 if (<*) > (<(*+25))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01387_1

	.byte 0
	.byte   %01011010
	.byte   %01011010
	.byte   %01011010
	.byte   %10011001
	.byte   %10111101
	.byte   %10011001
	.byte   %10111101
	.byte   %10011001
	.byte   %10011001
	.byte   %10111101
	.byte   %11011011
	.byte   %10111101
	.byte   %10111101
	.byte   %11100111
	.byte   %11000011
	.byte   %10011001
	.byte   %10100101
	.byte   %10011001
	.byte   %10111101
	.byte   %10100101
	.byte   %10000001
	.byte   %01000010
	.byte   %01000010
	.byte   %01000010
 if (<*) > (<(*+25))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01391_1

	.byte 0
	.byte   %10100101
	.byte   %10100101
	.byte   %10100101
	.byte   %10100101
	.byte   %10100101
	.byte   %10100101
	.byte   %10111101
	.byte   %01111110
	.byte   %11011011
	.byte   %10011001
	.byte   %10011001
	.byte   %10011001
	.byte   %10011001
	.byte   %11011011
	.byte   %11111111
	.byte   %01111110
	.byte   %01111110
	.byte   %01111110
	.byte   %00111100
	.byte   %00111100
	.byte   %00111100
	.byte   %00011000
	.byte   %00011000
	.byte   %00011000
 if (<*) > (<(*+25))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01395_1

	.byte 0
	.byte   %00011000
	.byte   %00111100
	.byte   %01011010
	.byte   %01111110
	.byte   %00011000
	.byte   %00111100
	.byte   %10111101
	.byte   %11111111
	.byte   %01111110
	.byte   %01111110
	.byte   %11011011
	.byte   %11100111
	.byte   %11111111
	.byte   %01011010
	.byte   %01100110
	.byte   %11111111
	.byte   %10111101
	.byte   %00111100
	.byte   %00111100
	.byte   %00001000
	.byte   %00011000
	.byte   %00010000
	.byte   %00001000
	.byte   %00001000
 if (<*) > (<(*+14))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01399_1

	.byte 0
	.byte   %01101100
	.byte   %01101100
	.byte   %01111100
	.byte   %01111100
	.byte   %10111010
	.byte   %01111100
	.byte   %10111010
	.byte   %11111110
	.byte   %11010110
	.byte   %11010110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+2))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01403_1

	.byte 0
	.byte   %00010000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01407_1

	.byte 0
	.byte   %00101000
	.byte   %11111110
	.byte   %01111100
	.byte   %01111100
	.byte   %00111000
	.byte   %10111010
	.byte   %11010110
	.byte   %01111100
	.byte   %01010100
	.byte   %00111000
	.byte   %01111100
	.byte   %11010110
	.byte   %10111010
	.byte   %01111100
	.byte   %01111100
	.byte   %01010100
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01410_1

	.byte 0
	.byte   %00000000
	.byte   %00000000
	.byte   %00111000
	.byte   %01111100
	.byte   %11111111
	.byte   %01111100
	.byte   %00111000
	.byte   %00000000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01413_1

	.byte 0
	.byte   %01111110
	.byte   %01011010
	.byte   %10100101
	.byte   %11011011
	.byte   %11111111
	.byte   %01111110
	.byte   %00011000
	.byte   %00000000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01416_1

	.byte 0
	.byte   %00010000
	.byte   %00111000
	.byte   %01010100
	.byte   %11010110
	.byte   %10111010
	.byte   %11010110
	.byte   %10010010
	.byte   %11111110
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01419_1

	.byte 0
	.byte   %10010000
	.byte   %01100000
	.byte   %01110000
	.byte   %10111000
	.byte   %00011100
	.byte   %00001110
	.byte   %00000111
	.byte   %00000011
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01422_1

	.byte 0
	.byte   %11111110
	.byte   %10000010
	.byte   %10000010
	.byte   %10010010
	.byte   %10111010
	.byte   %10101010
	.byte   %10000010
	.byte   %11111110
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01425_1

	.byte 0
	.byte   %11111110
	.byte   %10010010
	.byte   %10001010
	.byte   %11111110
	.byte   %10100010
	.byte   %10010010
	.byte   %10001010
	.byte   %11111110
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01428_1

	.byte 0
	.byte   %00000000
	.byte   %00000000
	.byte   %00010000
	.byte   %00111000
	.byte   %01111100
	.byte   %01111100
	.byte   %00101000
	.byte   %00000000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01431_1

	.byte 0
	.byte   %00000000
	.byte   %00000000
	.byte   %00010000
	.byte   %01010100
	.byte   %10111010
	.byte   %01010100
	.byte   %00010000
	.byte   %00000000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01434_1

	.byte 0
	.byte   %00000000
	.byte   %00010000
	.byte   %00111000
	.byte   %01111100
	.byte   %11111110
	.byte   %11111110
	.byte   %01101100
	.byte   %00000000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01437_1

	.byte 0
	.byte   %00000000
	.byte   %00010000
	.byte   %01010100
	.byte   %10111010
	.byte   %10010010
	.byte   %10111010
	.byte   %01010100
	.byte   %00010000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01440_1

	.byte 0
	.byte   %00000000
	.byte   %11111111
	.byte   %00111110
	.byte   %00001000
	.byte   %00010000
	.byte   %00100000
	.byte   %11000000
	.byte   %11000000
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01443_1

	.byte 0
	.byte   %00000000
	.byte   %11111111
	.byte   %01111100
	.byte   %00010000
	.byte   %00001000
	.byte   %00000100
	.byte   %00000011
	.byte   %00000011
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01446_1

	.byte 0
	.byte   %10000001
	.byte   %01000010
	.byte   %00100100
	.byte   %00011000
	.byte   %00011000
	.byte   %00100100
	.byte   %01000010
	.byte   %10000001
 if (<*) > (<(*+14))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01449_1

	.byte 0
	.byte   %00011100
	.byte   %11111110
	.byte   %01111100
	.byte   %10111000
	.byte   %10111110
	.byte   %10011110
	.byte   %10111000
	.byte   %11111100
	.byte   %11110100
	.byte   %11110110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
 if (<*) > (<(*+60))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01459_0

	.byte 0
	.byte   %1000001
	.byte   %1000001
	.byte   %1000010
	.byte   %1111110
	.byte   %1000001
	.byte   %1111110
	.byte   %0000000
	.byte   %1111111
	.byte   %1000000
	.byte   %1111111
	.byte   %1000000
	.byte   %1111111
	.byte   %0000000
	.byte   %0001000
	.byte   %0010100
	.byte   %0100010
	.byte   %1000001
	.byte   %1000001
	.byte   %0000000
	.byte   %0011100
	.byte   %0100010
	.byte   %1000001
	.byte   %1000001
	.byte   %0100010
	.byte   %0011100
	.byte   %0000000
	.byte   %0000000
	.byte   %0000000
	.byte   %0000000
	.byte   %0000000
	.byte   %0000000
	.byte   %0000000
	.byte   %0000000
	.byte   %1111111
	.byte   %1000000
	.byte   %1111111
	.byte   %1000000
	.byte   %1111111
	.byte   %0000000
	.byte   %1000001
	.byte   %1000001
	.byte   %1001001
	.byte   %1010101
	.byte   %1100011
	.byte   %1000001
	.byte   %0000000
	.byte   %1000001
	.byte   %1000001
	.byte   %1111111
	.byte   %0100010
	.byte   %0010100
	.byte   %0001000
	.byte   %0000000
	.byte   %0011110
	.byte   %0100010
	.byte   %1000111
	.byte   %1000000
	.byte   %0100010
	.byte   %0011100
 if (<*) > (<(*+74))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01492_1

	.byte 0
	.byte   %00011100
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
	.byte   %10111110
	.byte   %10011110
	.byte   %10111000
	.byte   %11111100
	.byte   %11110100
	.byte   %11110110
	.byte   %11111110
	.byte   %01111100
	.byte   %00111000
	.byte   %00000000
	.byte   %00010000
	.byte   %00111000
	.byte   %01111100
	.byte   %11111110
	.byte   %11111110
	.byte   %01101100
	.byte   %00000000
	.byte   %00000000
	.byte   %00000000
	.byte   %11111000
	.byte   %10000100
	.byte   %10000010
	.byte   %10000010
	.byte   %10000100
	.byte   %11111000
	.byte   %00000000
	.byte   %00000000
	.byte   %10000110
	.byte   %10001010
	.byte   %10010010
	.byte   %10100010
	.byte   %11000010
	.byte   %00000000
	.byte   %00000000
	.byte   %11111110
	.byte   %10000000
	.byte   %11111110
	.byte   %10000000
	.byte   %11111110
	.byte   %00000000
	.byte   %00000000
	.byte   %00000000
	.byte   %00000000
	.byte   %11111110
	.byte   %10000000
	.byte   %11111110
	.byte   %10000000
	.byte   %11111110
	.byte   %00000000
	.byte   %10000010
	.byte   %10000010
	.byte   %11111110
	.byte   %10000010
	.byte   %10000010
	.byte   %00000000
	.byte   %00010000
	.byte   %00010000
	.byte   %00010000
	.byte   %00010000
	.byte   %11111110
	.byte   %00000000
	.byte   %00000000
	.byte   %00000000
	.byte   %00010000
	.byte   %00111000
	.byte   %01111100
	.byte   %11111110
	.byte   %11111110
	.byte   %01101100
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel1303
 .byte  yellow - 2,0,0,0
 .byte  yellow - 4,0,0,0
 .byte  yellow - 8,0,0,0
 .byte  yellow - 10,0,0,0
 .byte  yellow - 12,0,0,0
 .byte  yellow - 2,0,0,0
 .byte  yellow - 4,0,0,0
 .byte  yellow - 8,0,0,0
 .byte  yellow - 10,0,0,0
 .byte  yellow - 12,0,0,0
       echo "    ",[(scoretable - *)]d , "bytes of ROM space left in bank 8")
 
 
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
; every bank has this stuff at the same place
; this code can switch to/from any bank at any entry point
; and can preserve register values
; note: lines not starting with a space are not placed in all banks
;
; line below tells the compiler how long this is - do not remove
;size=32
begin_bscode
 ldx #$ff
 txs
 lda #>(start-1)
 pha
 lda #<(start-1)
 pha
BS_return
 pha
 txa
 pha
 tsx
 lda 4,x ; get high byte of return address
 rol   
 rol
 rol
 rol
 and #bs_mask ;1 3 or 7 for F8/F6/F4
 tax
 inx
BS_jsr
 lda bankswitch_hotspot-1,x
 pla
 tax
 pla
 rts
 if ((* & $1FFF) > ((bankswitch_hotspot & $1FFF) - 1))
   echo "WARNING: size parameter in banksw.asm too small - the program probably will not work."
   echo "Change to",[(*-begin_bscode+1)&$FF]d,"and try again."
 endif
 ifconst bankswitch
   if bankswitch == 8
     ORG $2FFC
     RORG $FFFC
   endif
   if bankswitch == 16
     ORG $4FFC
     RORG $FFFC
   endif
   if bankswitch == 32
     ORG $8FFC
     RORG $FFFC
   endif
 else
   ifconst ROM2k
     ORG $F7FC
   else
     ORG $FFFC
   endif
 endif
 .word start
 .word start
