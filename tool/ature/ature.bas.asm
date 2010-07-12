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

.
 ; 

.L083 ;  rem 'Initial values

.L084 ;  rem 'Normal start

.L085 ;  const room_start = 49

.L086 ;  rem '49

.L087 ;  rem 'On the east side of the river

.L088 ;  rem 'const room_start=4

.
 ; 

.L089 ;  const hero_start_x = 75

.L090 ;  const hero_start_y = 75

.L091 ;  const item_start_x = 60

.L092 ;  const item_start_y = 40

.L093 ;  const item_start_hp = 1

.L094 ;  const hero_base_hp = 11

.L095 ;  const hero_high = 8

.L096 ;  const hero_half_high = 4

.L097 ;  const hero_feet_high = 4

.L098 ;  const hero_wide = 8

.L099 ;  const hero_half_wide = 4

.
 ; 

.L0100 ;  rem 'Hero's position, speed and room

.L0101 ;  dim hero_x = player0x

.L0102 ;  dim hero_y = player0y

.
 ; 

.L0103 ;  rem 'Score, split up in individual digits

.L0104 ;  dim sc0 = score

.L0105 ;  dim sc1 = score + 1

.L0106 ;  dim sc2 = score + 2

.
 ; 

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

.L0126 ;  dim hero_hp = j

.L0127 ;  rem ' statusbarlength

.L0128 ;  dim hero_mp = o

.L0129 ;  dim hero_next = k

.L0130 ;  dim hero_level = l

.L0131 ;  rem 'Quest flags contain important achievments of Signe.

.L0132 ;  dim quest_flags = h

.L0133 ;  rem 'quest_flags{0} -> Defeated Ikaza

.L0134 ;  rem 'quest_flags{1} -> Flipped Switch 1

.L0135 ;  rem 'quest_flags{2} -> Flipped Switch 2

.L0136 ;  rem 'quest_flags{3} -> Flipped Switch 3

.L0137 ;  rem 'quest_flags{4} -> Flipped Switch 4

.L0138 ;  rem 'quest_flags{5} -> Flipped Switch 5

.L0139 ;  rem 'quest_flags{6} -> Flipped Switch 6

.L0140 ;  rem 'quest_flags{7} -> Flipped Switch 7

.
 ; 

.L0141 ;  rem 'Other flags: Reserved for future extension

.L0142 ;  rem 'quest_flags{1} -> Defeated Armor Boss

.L0143 ;  rem 'quest_flags{2} -> Defeated Sword Boss 

.L0144 ;  rem 'quest_flags{3} -> Defeated 

.L0145 ;  rem 'quest_flags{0} -> Defeated Leaf Boss 1

.L0146 ;  rem 'quest_flags{1} -> Defeated Leaf Boss 2

.L0147 ;  rem 'quest_flags{2} -> Defeated Leaf Boss 3

.L0148 ;  rem 'status bar color in Z, which si otherwise used for debugging

.L0149 ;  rem ' Can't sue changes top platfield row color somehow. :p

.L0150 ;  rem ' dim statusbarcolor=z

.
 ; 

.L0151 ;  const strike_cost = 1

.L0152 ;  const last_monster = 32

.
 ; 

.L0153 ;  const hero_speed = 1

.L0154 ;  const hero_speed_negative = # - 1

.
 ; 

.L0155 ;  rem item/monster position speed and kind

.
 ; 

.L0156 ;  dim item_x = player1x

.L0157 ;  dim item_y = player1y

.L0158 ;  dim item_kind = s

.L0159 ;  dim item_hp = i

.L0160 ;  dim item_oldx = t

.L0161 ;  dim item_oldy = u

.L0162 ;  rem 'Active item flags: 

.L0163 ;  dim item_flags = f

.L0164 ;  rem 'item_flags{0} -> Monster missile flies north

.L0165 ;  rem 'item_flags{1} -> Monster missile flies east

.L0166 ;  rem 'item_flags{2} -> Monster missile flies south

.L0167 ;  rem 'item_flags{3} -> Monster missile flies west

.L0168 ;  rem 'Game timer. Controls animation. Keeps on ticking, by overflow.

.L0169 ;  dim game_timer = e

.L0170 ;  rem 'Timer and pointer for music and sound

.L0171 ;  dim music_pointer = m

.L0172 ;  dim music_timer = n

.L0173 ;  dim music_which = d

.L0174 ;  dim sound_timer = p

.L0175 ;  rem 'Used variables: d e f g h i k l m n o p q r s t u v w

.L0176 ;  rem 'Used in alpha order:    

.L0177 ;  rem 'Unused variables: a b c j x y z

.
 ; 

.
 ; 

.
 ; 

.L0178 ;  const timer_loop = 128

.L0179 ;  const timer_second = 60

.
 ; 

.
 ; 

.L0180 ;  rem 'Item and monster kinds. 

.L0181 ;  rem 'Monster kind is divided as follows: the 6 bits record the looks 

.L0182 ;  rem 'and the 'index' for data tables of the monster or item

.L0183 ;  rem 'The high 2 bits are used for 2 flag variables: 

.L0184 ;  rem 'If item_kind{7} is set it's an immobile item, otherwise, a monster

.L0185 ;  rem '6 is yet to be determined

.L0186 ;  const item_kind_mask = %00111111

.L0187 ;  const is_item_mask = %10000000

.L0188 ;  const item_none = %10000000

.L0189 ;  const monster_slime = 1

.L0190 ;  const monster_crab = 2

.L0191 ;  const monster_bat = 3

.L0192 ;  const monster_scorpio = 4

.L0193 ;  rem 'The famous rabid rabbit ^_^

.L0194 ;  const monster_rabid = 5

.L0195 ;  const monster_spider = 6

.L0196 ;  const monster_snake = 7

.L0197 ;  const monster_fish = 8

.L0198 ;  const monster_lion = 9

.L0199 ;  const monster_wolf = 10

.L0200 ;  const monster_grunt = 11

.L0201 ;  const monster_archer = 12

.L0202 ;  const monster_knight = 13

.L0203 ;  const monster_cannon = 14

.L0204 ;  const monster_zombie = 15

.L0205 ;  const monster_skeleton = 16

.L0206 ;  const monster_ghost = 17

.L0207 ;  const monster_mage = 18

.L0208 ;  const monster_flower = 19

.L0209 ;  const monster_treant = 20

.L0210 ;  const monster_mushroom = 21

.L0211 ;  const monster_book = 22

.L0212 ;  const monster_sword = 23

.L0213 ;  rem 'Monster curse blocks the way in rooms. It's invulnerable,

.L0214 ;  rem 'but can be removed by game events.

.L0215 ;  const monster_curse = 24

.L0216 ;  const monster_first_boss = 25

.L0217 ;  const monster_leaf1_boss = 25

.L0218 ;  const monster_leaf2_boss = 26

.L0219 ;  const monster_leaf3_boss = 27

.L0220 ;  const monster_armor_boss = 28

.L0221 ;  rem 'Boss that guards the Numen Sword

.L0222 ;  const monster_sword_boss = 29

.L0223 ;  rem 'Boss that guards the Strike book

.L0224 ;  const monster_strike_boss = 30

.L0225 ;  const monster_ikaza = 31

.
 ; 

.L0226 ;  const item_inventory_mask = 160

.L0227 ;  const item_leaf1 = 160

.L0228 ;  const item_leaf2 = 161

.L0229 ;  const item_leaf3 = 162

.L0230 ;  const item_armor = 163

.L0231 ;  const item_shield = 164

.L0232 ;  const item_sword = 165

.L0233 ;  const item_bookheal = 166

.L0234 ;  const item_bookstrike = 167

.L0235 ;  const item_healhp = 168

.L0236 ;  const item_healmp = 169

.L0237 ;  const item_healallhp = 170

.L0238 ;  const item_healallmp = 171

.L0239 ;  const item_first_switch = 172

.L0240 ;  const item_switch_on = 7

.L0241 ;  const item_switch1 = 172

.L0242 ;  const item_switch2 = 173

.L0243 ;  const item_switch3 = 174

.L0244 ;  const item_switch4 = 175

.L0245 ;  const item_switch5 = 176

.L0246 ;  const item_switch6 = 177

.L0247 ;  const item_switch7 = 178

.L0248 ;  const item_last_switch = 178

.L0249 ;  const item_first_switch_on = 179

.L0250 ;  const item_switch1_on = 179

.L0251 ;  const item_switch2_on = 180

.L0252 ;  const item_switch3_on = 181

.L0253 ;  const item_switch4_on = 182

.L0254 ;  const item_switch5_on = 183

.L0255 ;  const item_switch6_on = 184

.L0256 ;  const item_switch7_on = 185

.L0257 ;  const item_last_switch_on = 182

.L0258 ;  const item_victory = 191

.L0259 ;  const monster_normal = 0

.L0260 ;  rem 'Bit 0 in monster flags

.L0261 ;  const monster_shoots = 1

.L0262 ;  rem 'Bit 1 in monster flags

.L0263 ;  const monster_randomwalk = 2

.L0264 ;  rem 'Bit 2 in monster flags

.L0265 ;  const monster_aggressive = 4

.L0266 ;  rem 'Bit 3 in monster flags

.L0267 ;  const monster_static = 8

.L0268 ;  rem 'Bit 4 in monster flags

.L0269 ;  const monster_alwaysdrop = 16

.L0270 ;  rem 'Bit 5 in monster flags

.L0271 ;  const monster_nowalls = 32

.L0272 ;  rem 'Bit 6 in monster flags

.L0273 ;  const monster_huge = 64

.L0274 ;  rem 'Bit 7 in monster flags. Will triple if huge is also set

.L0275 ;  rem 'Currently, double nor triple doesn't work well with the AI

.L0276 ;  const monster_double = 128

.L0277 ;  const monster_boss_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0278 ;  const monster_boss2_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0279 ;  const monster_boss3_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0280 ;  const monster_swordboss_info = monster_alwaysdrop  +  monster_aggressive  +  monster_nowalls

.L0281 ;  const monster_cannon_info = monster_shoots  +  monster_static  +  monster_nowalls

.
 ; 

.L0282 ;  rem ' Music wave form to use.

.L0283 ;  const music_instrument = 12

.L0284 ;  const music_notes = 69

.
 ; 

.
 ; 

.L0285 ;  rem 'Set back to 1 when not debugging.

.L0286 ;  const hero_start_level = 20

.L0287 ;  rem 'set back to 0 when not debugging

.L0288 ;  const hero_start_items = 

.
 ; 

.L0289 ;  rem ' Colors to use for the item display 

.L0290 ;  data item_colors

	JMP .skipL0290
item_colors
	.byte   black, green, red, blue, black, green, red, blue 

	.byte   black, green, red, blue, black, green, red, blue 

	.byte   black, green, red, blue, black, green, red, blue 

	.byte   black, black, red, red, black, orange, red, blue 

	.byte   black, green, red, blue, black, green, red, blue 

	.byte   red , green, red, green, black, green, red, blue 

	.byte   white, yellow, orange, black, green, red, blue, white 

	.byte   yellow, orange, blue, black, green, red, blue, blue 

.skipL0290
.
 ; 

.L0291 ;  rem 'Damage modifier for the MONSTER/item. 0 means no damage

.L0292 ;  rem 'There is no monster nr 0, that's for item_none.

.L0293 ;  rem 'The curse deals massive damage to be sure it works.

.L0294 ;  data item_damage

	JMP .skipL0294
item_damage
	.byte    0, 1, 1, 1, 1, 2, 2, 2

	.byte    2, 4, 4, 4, 4, 6, 6, 6

	.byte   10, 16, 16, 1, 16, 4, 16, 16

	.byte  128, 20, 22, 24, 48, 48, 48, 64

	.byte    0, 0, 0, 0, 0, 0, 0, 0

	.byte    0, 0, 0, 0, 0, 0, 0, 0

	.byte    0, 0, 0, 0, 0, 0, 0, 0

	.byte    0, 0, 0, 0, 0, 0, 0, 0

.skipL0294
.
 ; 

.L0295 ;  rem ' 

.L0296 ;  rem ' Monster information, bitflags (monster 0 doesn't exist, but is in this list!)

.L0297 ;  data monster_info

	JMP .skipL0297
monster_info
	.byte   monster_normal, monster_normal, monster_nowalls+monster_shoots, monster_nowalls

	.byte   monster_normal, monster_nowalls, monster_normal, monster_aggressive

	.byte   monster_shoots + monster_nowalls, monster_normal, monster_normal, monster_normal

	.byte   monster_shoots, monster_aggressive, monster_cannon_info, monster_randomwalk

	.byte   monster_normal, monster_nowalls, monster_nowalls + monster_shoots, monster_cannon_info

	.byte   monster_normal, monster_normal, monster_cannon_info, monster_static + monster_nowalls

	.byte   monster_static + monster_huge + monster_nowalls, monster_boss_info, monster_boss_info, monster_boss3_info

	.byte   monster_boss_info, monster_swordboss_info, monster_boss_info, monster_aggressive + monster_shoots

.skipL0297
.
 ; 

.L0298 ;  rem ' Items the monster drops, or for items, what item flag to set

.L0299 ;  data monster_drops

	JMP .skipL0299
monster_drops
	.byte   item_healhp, item_healhp, item_healhp, item_healmp 

	.byte   item_healhp, item_healhp, item_healhp, item_healhp 

	.byte   item_healhp, item_healhp, item_healhp, item_healhp 

	.byte   item_healhp, item_healallhp, item_healhp, monster_skeleton 

	.byte   monster_ghost, item_switch5, monster_book, item_healmp 

	.byte   item_healallmp, item_healhp, item_bookheal, item_sword 

	.byte   item_healhp, item_leaf1, item_leaf2 , item_leaf3 

	.byte   item_armor, monster_sword, item_bookstrike, item_healallhp 

.skipL0299
.
 ; 

.
 ; 

.L0300 ;  rem VOLUME DOWN AND SET CHANNELS

.L0301 ;  AUDV0 = 0

	LDA #0
	STA AUDV0
.L0302 ;  AUDV1 = 0

	LDA #0
	STA AUDV1
.L0303 ;  AUDC0 = music_instrument

	LDA #music_instrument
	STA AUDC0
.
 ; 

.L0304 ;  rem INITIALIZE POINTERS AND TIMERS

.
 ; 

.L0305 ;  rem 'lives control

.L0306 ;  lifecolor = black

	LDA black
	STA lifecolor
.L0307 ;  rem 'Lives are used for the MP

.L0308 ;  lives = 32

	LDA #32
	STA lives
.L0309 ;  lives:

	LDA #<lives__L0309
	STA lifepointer
	LDA lifepointer+1
	AND #$E0
	ORA #(>lives__L0309)&($1F)
	STA lifepointer+1
.
 ; 

.L0310 ;  rem 'Go to intro screen, but not now while we are debugging.

.L0311 ;  gosub intro_screen bank5

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
.L0312 ;  goto main_loop_start

 jmp .main_loop_start

.
 ; 

.main_loop_start
 ; main_loop_start

.L0313 ;  rem ' Set up initial values.

.L0314 ;  gosub music_restart bank5

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
.L0315 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L0316 ;  lifecolor = yellow

	LDA yellow
	STA lifecolor
.L0317 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L0318 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L0319 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L0320 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L0321 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L0322 ;  quest_flags = 0

	LDA #0
	STA quest_flags
.L0323 ;  hero_level = hero_start_level

	LDA #hero_start_level
	STA hero_level
.L0324 ;  hero_next =  ( hero_level  /  2 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	CLC
	ADC #1
	STA hero_next
.L0325 ;  hero_hp =  ( hero_level  *  2 )   +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0326 ;  rem 'MP is between 1 and 6

.L0327 ;  hero_mp =  ( hero_level  /  16 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0328 ;  gosub set_mp_display

 jsr .set_mp_display

.L0329 ;  hero_flags  =  0

	LDA #0
	STA hero_flags
.L0330 ;  item_flags  =  0

	LDA #0
	STA item_flags
.L0331 ;  game_timer  =  0

	LDA #0
	STA game_timer
.L0332 ;  item_kind  =  item_none

	LDA #item_none
	STA item_kind
.L0333 ;  hero_items  =  hero_start_items

	LDA #hero_start_items
	STA hero_items
.L0334 ;  rem 'Player starts facing south, able to attack

.L0335 ;  hero_flags{2}  =  1

	LDA hero_flags
	ORA #4
	STA hero_flags
.L0336 ;  hero_flags{4}  =  1

	LDA hero_flags
	ORA #16
	STA hero_flags
.L0337 ;  rem player0x=75

.L0338 ;  rem player0y=75

.L0339 ;  item_x = item_start_x

	LDA #item_start_x
	STA item_x
.L0340 ;  item_oldx = item_start_x

	LDA #item_start_x
	STA item_oldx
.L0341 ;  item_y = item_start_y

	LDA #item_start_y
	STA item_y
.L0342 ;  item_oldy = item_start_x

	LDA #item_start_x
	STA item_oldy
.L0343 ;  missile1x = nowhere

	LDA #nowhere
	STA missile1x
.L0344 ;  missile1y = nowhere

	LDA #nowhere
	STA missile1y
.L0345 ;  scorecolor = 64

	LDA #64
	STA scorecolor
.L0346 ;  item_hp = item_start_hp

	LDA #item_start_hp
	STA item_hp
.L0347 ;  gosub show_inventory

 jsr .show_inventory

.L0348 ;  gosub hero_draw_s

 jsr .hero_draw_s

.L0349 ;  gosub room_draw bank2

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
.L0350 ;  drawscreen

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

.L0351 ;  rem ' set status bar length from hero hp

.L0352 ;  statusbarlength  =  hero_hp

	LDA hero_hp
	STA statusbarlength
.L0353 ;  COLUP0  =  skin

	LDA #skin
	STA COLUP0
.L0354 ;  temp1  =  item_kind  &  item_kind_mask

	LDA item_kind
	AND #item_kind_mask
	STA temp1
.L0355 ;  COLUP1  =  item_colors[temp1]

	LDX temp1
	LDA item_colors,x
	STA COLUP1
.L0356 ;  rem 'Special effects for the item (double or triple size, double or triple instances)

.L0357 ;  if item_kind{7} goto special_effects_end

	BIT item_kind
 if ((* - .special_effects_end) < 127) && ((* - .special_effects_end) > -128)
	bmi .special_effects_end
 else
	bpl .0skipspecial_effects_end
	jmp .special_effects_end
.0skipspecial_effects_end
 endif
.L0358 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0359 ;  if temp2{6} then NUSIZ1  =  $07

	BIT temp2
	BVC .skipL0359
.condpart0
	LDA #$07
	STA NUSIZ1
.skipL0359
.L0360 ;  if temp2{7} then NUSIZ1  =  $02

	BIT temp2
	BPL .skipL0360
.condpart1
	LDA #$02
	STA NUSIZ1
.skipL0360
.L0361 ;  if temp2{7}  &&  temp2{7} then NUSIZ1  = $06

	BIT temp2
	BPL .skipL0361
.condpart2
	BIT temp2
	BPL .skip2then
.condpart3
	LDA #$06
	STA NUSIZ1
.skip2then
.skipL0361
.special_effects_end
 ; special_effects_end

.L0362 ;  PF0 = $00

	LDA #$00
	STA PF0
.
 ; 

.
 ; 

.L0363 ;  rem 'Collisions with the walls. For now, we use push-back collision 

.L0364 ;  rem 'for the monsters, and preventive collision detection for the player. 

.L0365 ;  rem 'This is inconsistent, but push-back is easier to manage with regards to 

.L0366 ;  rem 'and matters less for AI, so I'll leave it at that for now.

.L0367 ;  if !collision(player0,playfield) then goto herofield_collide_end

	BIT CXP0FB
	BMI .skipL0367
.condpart4
 jmp .herofield_collide_end

.skipL0367
.L0368 ;  rem 'Could do something here ??? Some overlap is OK, IMO

.herofield_collide_end
 ; herofield_collide_end

.
 ; 

.L0369 ;  rem 'Collisions with the walls

.L0370 ;  rem 'For the item

.L0371 ;  if !collision(player1,playfield) then goto itemfield_collide_end

	BIT CXP1FB
	BMI .skipL0371
.condpart5
 jmp .itemfield_collide_end

.skipL0371
.L0372 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0373 ;  rem 'Some monsters ignore walls

.L0374 ;  if temp2{5} then goto itemfield_collide_end

	LDA temp2
	AND #32
	BEQ .skipL0374
.condpart6
 jmp .itemfield_collide_end

.skipL0374
.
 ; 

.L0375 ;  if item_y  =  item_oldy  &&  item_x  =  item_oldx then goto itemfield_collide_still

	LDA item_y
	CMP item_oldy
     BNE .skipL0375
.condpart7
	LDA item_x
	CMP item_oldx
     BNE .skip7then
.condpart8
 jmp .itemfield_collide_still

.skip7then
.skipL0375
.L0376 ;  item_y  =  item_oldy

	LDA item_oldy
	STA item_y
.L0377 ;  item_x  =  item_oldx

	LDA item_oldx
	STA item_x
.L0378 ;  goto itemfield_collide_end

 jmp .itemfield_collide_end

.itemfield_collide_still
 ; itemfield_collide_still

.L0379 ;  rem if item_y < field_vcenter then item_y = item_oldy - 1 else item_y = item_oldy + 1

.L0380 ;  rem if item_x < field_hcenter then item_x = item_oldx - 1 else item_x = hero_oldx + 1

.itemfield_collide_end
 ; itemfield_collide_end

.
 ; 

.
 ; 

.L0381 ;  rem 'Remember curent position which should be OK here, 

.L0382 ;  rem 'and with that I mean not colliding with the walls. 

.L0383 ;  hero_oldx  =  hero_x

	LDA hero_x
	STA hero_oldx
.L0384 ;  hero_oldy  =  hero_y

	LDA hero_y
	STA hero_oldy
.L0385 ;  item_oldx  =  item_x

	LDA item_x
	STA item_oldx
.L0386 ;  item_oldy  =  item_y

	LDA item_y
	STA item_oldy
.
 ; 

.
 ; 

.L0387 ;  rem 'Teleport to Sygne's home on reset button

.L0388 ;  if !switchreset then goto reset_end

 lda #1
 bit SWCHB
	BEQ .skipL0388
.condpart9
 jmp .reset_end

.skipL0388
.L0389 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L0390 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L0391 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L0392 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L0393 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L0394 ;  gosub room_draw bank2

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

.L0395 ;  rem 'Update game timer, and let it overflow back to 0 .

.L0396 ;  game_timer  =  game_timer  +  1

	INC game_timer
.L0397 ;  rem 'Update sound timer, and set to silence if sound is done

.L0398 ;  if sound_timer  <  1 then goto sound_end

	LDA sound_timer
	CMP #1
     BCS .skipL0398
.condpart10
 jmp .sound_end

.skipL0398
.L0399 ;  sound_timer  =  sound_timer  -  1

	DEC sound_timer
.L0400 ;  if sound_timer  >  1 then goto sound_end

	LDA #1
	CMP sound_timer
     BCS .skipL0400
.condpart11
 jmp .sound_end

.skipL0400
.L0401 ;  AUDV1  =  0

	LDA #0
	STA AUDV1
.L0402 ;  AUDC1  =  0

	LDA #0
	STA AUDC1
.L0403 ;  AUDF1  =  0

	LDA #0
	STA AUDF1
.L0404 ;  sound_timer  =  0

	LDA #0
	STA sound_timer
.sound_end
 ; sound_end

.
 ; 

.L0405 ;  rem 'Collision between hero and monster/item or the monster's missile

.L0406 ;  if !collision(player0,missile1) then goto no_missile_collide

	BIT CXM1P
	BMI .skipL0406
.condpart12
 jmp .no_missile_collide

.skipL0406
.L0407 ;  rem 'Ennemy missile hit player, remove it

.L0408 ;  missile1y  =  nowhere

	LDA #nowhere
	STA missile1y
.L0409 ;  missile1x  =  nowhere

	LDA #nowhere
	STA missile1x
.L0410 ;  item_flags =  0

	LDA #0
	STA item_flags
.L0411 ;  rem ' If the player doesn't have a shield, jump to monster collide

.L0412 ;  rem 'Otherwise ignore the projectile and remove it  

.L0413 ;  if !hero_items{3} then goto monster_collide

	LDA hero_items
	AND #8
	BNE .skipL0413
.condpart13
 jmp .monster_collide

.skipL0413
.no_missile_collide
 ; no_missile_collide

.L0414 ;  if !collision(player0,player1) then goto item_collide_end

	BIT CXPPMM
	BMI .skipL0414
.condpart14
 jmp .item_collide_end

.skipL0414
.L0415 ;  rem 'Distinguish between item or monster collide

.L0416 ;  if item_kind{7} then goto item_collide

	BIT item_kind
	BPL .skipL0416
.condpart15
 jmp .item_collide

.skipL0416
.L0417 ;  rem 'Collision with monster or monster missile

.monster_collide
 ; monster_collide

.L0418 ;  COLUP0  =  red

	LDA red
	STA COLUP0
.L0419 ;  rem 'Make a hit sound

.L0420 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0421 ;  AUDC1 = 6

	LDA #6
	STA AUDC1
.L0422 ;  AUDF1 = 10

	LDA #10
	STA AUDF1
.L0423 ;  AUDV1 = 14

	LDA #14
	STA AUDV1
.
 ; 

.L0424 ;  rem 'Push back the hero, but only if the playfield is free behind her.

.L0425 ;  rem 'Also don't push back if too close to the edge of the screen.

.
 ; 

.
 ; 

.L0426 ;  if !hero_flags{0} then goto hero_hit_north_end

	LDA hero_flags
	LSR
	BCS .skipL0426
.condpart16
 jmp .hero_hit_north_end

.skipL0426
.L0427 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0428 ;  temp2  =   ( hero_y  +  8  -  1 )   /  8

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
.L0429 ;  if pfread ( temp1 ,  temp2 )  goto hero_hit_north_end

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
.L0430 ;  hero_y  =  hero_y  +  8

	LDA hero_y
	CLC
	ADC #8
	STA hero_y
.L0431 ;  rem 'Still inside the screen?  

.L0432 ;  if hero_y  <  field_bottom then goto hero_hit_north_end

	LDA hero_y
	CMP #field_bottom
     BCS .skipL0432
.condpart17
 jmp .hero_hit_north_end

.skipL0432
.L0433 ;  rem 'if we get here, we'd be pushed out of the screen. Prevent this.

.L0434 ;  hero_y  =  field_bottom_enter

	LDA #field_bottom_enter
	STA hero_y
.hero_hit_north_end
 ; hero_hit_north_end

.
 ; 

.L0435 ;  if !hero_flags{1} then goto hero_hit_east_end

	LDA hero_flags
	AND #2
	BNE .skipL0435
.condpart18
 jmp .hero_hit_east_end

.skipL0435
.L0436 ;  temp1  =   ( hero_x  -  8  -  17 )   /  4

; complex statement detected
	LDA hero_x
	SEC
	SBC #8
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0437 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

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
.L0438 ;  if pfread ( temp1 ,  temp2 )  goto hero_hit_east_end

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
.L0439 ;  hero_x  =  hero_x  -  8

	LDA hero_x
	SEC
	SBC #8
	STA hero_x
.L0440 ;  rem 'Still inside the screen?  

.L0441 ;  if field_left  <  hero_x then goto hero_hit_east_end

	LDA #field_left
	CMP hero_x
     BCS .skipL0441
.condpart19
 jmp .hero_hit_east_end

.skipL0441
.L0442 ;  rem 'if we get here, we'd be pushed out of the screen. Prevent this.

.L0443 ;  hero_x  =  field_left_enter

	LDA #field_left_enter
	STA hero_x
.hero_hit_east_end
 ; hero_hit_east_end

.
 ; 

.L0444 ;  if !hero_flags{2} then goto hero_hit_south_end

	LDA hero_flags
	AND #4
	BNE .skipL0444
.condpart20
 jmp .hero_hit_south_end

.skipL0444
.L0445 ;  if pfread ( temp1 ,  temp2 )  then goto hero_hit_south_end

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
	BNE .skipL0445
.condpart21
 jmp .hero_hit_south_end

.skipL0445
.L0446 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0447 ;  temp2  =   ( hero_y  -  hero_high  -  8  -  1 )   /  8

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
.L0448 ;  hero_y  =  hero_y  -  8

	LDA hero_y
	SEC
	SBC #8
	STA hero_y
.L0449 ;  if hero_y  >  field_top then goto hero_hit_south_end

	LDA #field_top
	CMP hero_y
     BCS .skipL0449
.condpart22
 jmp .hero_hit_south_end

.skipL0449
.L0450 ;  rem 'if we get here, we'd be pushed out of the screen. Prevent this.

.L0451 ;  hero_y  =  field_top_enter

	LDA #field_top_enter
	STA hero_y
.hero_hit_south_end
 ; hero_hit_south_end

.
 ; 

.L0452 ;  if !hero_flags{3} then goto hero_hit_west_end

	LDA hero_flags
	AND #8
	BNE .skipL0452
.condpart23
 jmp .hero_hit_west_end

.skipL0452
.L0453 ;  temp1  =   ( hero_x  +  hero_wide  +  8  -  17 )   /  4

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
.L0454 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

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
.L0455 ;  if pfread ( temp1 ,  temp2 )  then goto hero_hit_west_end

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
	BNE .skipL0455
.condpart24
 jmp .hero_hit_west_end

.skipL0455
.L0456 ;  hero_x  =  hero_x  +  8

	LDA hero_x
	CLC
	ADC #8
	STA hero_x
.L0457 ;  rem 'Still inside the screen?  

.L0458 ;  if hero_x  <  field_right then goto hero_hit_west_end

	LDA hero_x
	CMP #field_right
     BCS .skipL0458
.condpart25
 jmp .hero_hit_west_end

.skipL0458
.L0459 ;  rem 'if we get here, we'd be pushed out of the screen. Prevent this.

.L0460 ;  hero_x  =  field_right_enter

	LDA #field_right_enter
	STA hero_x
.hero_hit_west_end
 ; hero_hit_west_end

.
 ; 

.L0461 ;  temp1  =  item_kind  &  item_kind_mask

	LDA item_kind
	AND #item_kind_mask
	STA temp1
.L0462 ;  temp1  =  item_damage[temp1]

	LDX temp1
	LDA item_damage,x
	STA temp1
.L0463 ;  rem 'Halve damage if hero has the armor 

.L0464 ;  if hero_items{4} then temp1  =  temp1  /  2

	LDA hero_items
	AND #16
	BEQ .skipL0464
.condpart26
	LDA temp1
	lsr
	STA temp1
.skipL0464
.
 ; 

.L0465 ;  rem 'Game over if damage is higher than health

.L0466 ;  if temp1  <  hero_hp then goto do_damage

	LDA temp1
	CMP hero_hp
     BCS .skipL0466
.condpart27
 jmp .do_damage

.skipL0466
.L0467 ;  rem 'But escape by the skin of the teeth if Signe has the healing book and 

.L0468 ;  rem 'Numen to use it.

.L0469 ;  if hero_items{6}  &&  hero_mp  >  0 then goto use_heal_spell

	BIT hero_items
	BVC .skipL0469
.condpart28
	LDA #0
	CMP hero_mp
     BCS .skip28then
.condpart29
 jmp .use_heal_spell

.skip28then
.skipL0469
.L0470 ;  hero_hp  =  0

	LDA #0
	STA hero_hp
.L0471 ;  goto game_over bank5

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

.L0472 ;  COLUP0  =  green

	LDA green
	STA COLUP0
.L0473 ;  hero_mp  =  hero_mp  -  1

	DEC hero_mp
.L0474 ;  gosub set_mp_display

 jsr .set_mp_display

.L0475 ;  hero_hp  =   ( hero_level  *  2 )   +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0476 ;  goto item_collide_end

 jmp .item_collide_end

.do_damage
 ; do_damage

.L0477 ;  hero_hp  =  hero_hp  -  temp1

	LDA hero_hp
	SEC
	SBC temp1
	STA hero_hp
.L0478 ;  goto item_collide_end

 jmp .item_collide_end

.item_collide
 ; item_collide

.L0479 ;  rem 'If colliding with an item, pick it up if it is not a switch.

.L0480 ;  if item_kind  <  item_switch1 then goto item_do_collide

	LDA item_kind
	CMP #item_switch1
     BCS .skipL0480
.condpart30
 jmp .item_do_collide

.skipL0480
.L0481 ;  if item_kind  >  item_switch7_on then goto item_do_collide

	LDA #item_switch7_on
	CMP item_kind
     BCS .skipL0481
.condpart31
 jmp .item_do_collide

.skipL0481
.L0482 ;  goto item_collide_end

 jmp .item_collide_end

.item_do_collide
 ; item_do_collide

.L0483 ;  gosub item_pickup

 jsr .item_pickup

.L0484 ;  gosub show_inventory

 jsr .show_inventory

.item_collide_end
 ; item_collide_end

.
 ; 

.L0485 ;  rem 'Collision between monster missile and field. 

.L0486 ;  rem 'Or between sword and monster missile.  Remove missile.

.L0487 ;  rem 'Remove misile if monster is gone.

.L0488 ;  if item_kind  >  last_monster goto missile_remove

	LDA #last_monster
	CMP item_kind
 if ((* - .missile_remove) < 127) && ((* - .missile_remove) > -128)
	bcc .missile_remove
 else
	bcs .3skipmissile_remove
	jmp .missile_remove
.3skipmissile_remove
 endif
.L0489 ;  if collision(missile0,missile1) then goto missile_remove

	BIT CXPPMM
	BVC .skipL0489
.condpart32
 jmp .missile_remove

.skipL0489
.L0490 ;  if missile1x  >  field_right then goto missile_remove

	LDA #field_right
	CMP missile1x
     BCS .skipL0490
.condpart33
 jmp .missile_remove

.skipL0490
.L0491 ;  if missile1x  <  field_left then goto missile_remove

	LDA missile1x
	CMP #field_left
     BCS .skipL0491
.condpart34
 jmp .missile_remove

.skipL0491
.L0492 ;  if missile1y  <  field_top then goto missile_remove

	LDA missile1y
	CMP #field_top
     BCS .skipL0492
.condpart35
 jmp .missile_remove

.skipL0492
.L0493 ;  if missile1y  >  field_bottom then goto missile_remove

	LDA #field_bottom
	CMP missile1y
     BCS .skipL0493
.condpart36
 jmp .missile_remove

.skipL0493
.L0494 ;  if !collision(missile1,playfield) then goto missile_remove_end

	BIT CXM1FB
	BMI .skipL0494
.condpart37
 jmp .missile_remove_end

.skipL0494
.L0495 ;  rem 'Don't remove the misile even when coliding with the playfield if the

.L0496 ;  rem 'monster ignores the walls.

.L0497 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0498 ;  rem 'temp2 = temp2 & monster_nowalls

.L0499 ;  if temp2{5} then goto missile_remove_end

	LDA temp2
	AND #32
	BEQ .skipL0499
.condpart38
 jmp .missile_remove_end

.skipL0499
.missile_remove
 ; missile_remove

.L0500 ;  missile1y  =  nowhere

	LDA #nowhere
	STA missile1y
.L0501 ;  missile1x  =  nowhere

	LDA #nowhere
	STA missile1x
.L0502 ;  item_flags =  0

	LDA #0
	STA item_flags
.missile_remove_end
 ; missile_remove_end

.
 ; 

.L0503 ;  rem 'Collision between monster and hero's attack

.L0504 ;  if !collision(player1,missile0) then goto slash_collide_end

	BIT CXM0P
	BMI .skipL0504
.condpart39
 jmp .slash_collide_end

.skipL0504
.L0505 ;  rem 'Distinguish between item or monster collide

.L0506 ;  if item_kind{7} then goto slash_item_collide

	BIT item_kind
	BPL .skipL0506
.condpart40
 jmp .slash_item_collide

.skipL0506
.
 ; 

.L0507 ;  rem 'The curse monster cannot be harmed normally. 

.L0508 ;  rem 'It can be harmed if Signe has the three leaves, or the Numen Sword. 

.L0509 ;  rem 'Do the special handling here.

.L0510 ;  if item_kind  <>  monster_curse then goto monster_no_curse

	LDA item_kind
	CMP #monster_curse
     BEQ .skipL0510
.condpart41
 jmp .monster_no_curse

.skipL0510
.L0511 ;  rem 'Signe has the sword? The curse can be damaged.

.L0512 ;  if hero_items{5} then goto monster_no_curse

	LDA hero_items
	AND #32
	BEQ .skipL0512
.condpart42
 jmp .monster_no_curse

.skipL0512
.L0513 ;  rem 'Signe has the three leaves? The curse can be damaged. 

.L0514 ;  if hero_items{0}  &&  hero_items{1}  &&  hero_items{2} then goto monster_no_curse

	LDA hero_items
	LSR
	BCC .skipL0514
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
.skipL0514
.L0515 ;  rem 'If we get here, Signe's unable to do any damage.

.L0516 ;  rem 'Make a deflected sound

.L0517 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0518 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0519 ;  AUDV1 = 14

	LDA #14
	STA AUDV1
.L0520 ;  AUDF1 = 1

	LDA #1
	STA AUDF1
.L0521 ;  goto slash_collide_end

 jmp .slash_collide_end

.monster_no_curse
 ; monster_no_curse

.L0522 ;  rem 'Make a hit sound once

.L0523 ;  sound_timer = 1

	LDA #1
	STA sound_timer
.L0524 ;  AUDC1 = 8

	LDA #8
	STA AUDC1
.L0525 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0526 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0527 ;  hero_flags{4}  =  0

	LDA hero_flags
	AND #239
	STA hero_flags
.L0528 ;  rem COLUP1 = red

.L0529 ;  rem 'Push back monster

.L0530 ;  if hero_flags{0} then item_y  =  item_y  -  4

	LDA hero_flags
	LSR
	BCC .skipL0530
.condpart46
	LDA item_y
	SEC
	SBC #4
	STA item_y
.skipL0530
.L0531 ;  if hero_flags{1} then item_x  =  item_x  +  4

	LDA hero_flags
	AND #2
	BEQ .skipL0531
.condpart47
	LDA item_x
	CLC
	ADC #4
	STA item_x
.skipL0531
.L0532 ;  if hero_flags{2} then item_y  =  item_y  +  4

	LDA hero_flags
	AND #4
	BEQ .skipL0532
.condpart48
	LDA item_y
	CLC
	ADC #4
	STA item_y
.skipL0532
.L0533 ;  if hero_flags{3} then item_x  =  item_x  -  4

	LDA hero_flags
	AND #8
	BEQ .skipL0533
.condpart49
	LDA item_x
	SEC
	SBC #4
	STA item_x
.skipL0533
.L0534 ;  rem 'Damage is level / 2 + 1

.L0535 ;  temp1  =   ( hero_level  /  2 ) 

; complex statement detected
	LDA hero_level
	lsr
	STA temp1
.L0536 ;  temp1  =  temp1  +  1

	INC temp1
.L0537 ;  rem 'Double damage with the Numen Sword

.L0538 ;  if hero_items{5} then temp1  =  temp1  *  2

	LDA hero_items
	AND #32
	BEQ .skipL0538
.condpart50
	LDA temp1
	asl
	STA temp1
.skipL0538
.L0539 ;  rem 'Prevent damage overflow

.L0540 ;  if temp1  <  0 then temp1  =  0  -  temp1

	LDA temp1
	CMP #0
     BCS .skipL0540
.condpart51
	LDA #0
	SEC
	SBC temp1
	STA temp1
.skipL0540
.L0541 ;  if temp1  <  item_hp then goto item_survived

	LDA temp1
	CMP item_hp
     BCS .skipL0541
.condpart52
 jmp .item_survived

.skipL0541
.L0542 ;  rem 'Item / monster killed or destroyed

.L0543 ;  if item_kind  =  monster_ikaza then quest_flags{0}  =  1

	LDA item_kind
	CMP #monster_ikaza
     BNE .skipL0543
.condpart53
	LDA quest_flags
	ORA #1
	STA quest_flags
.skipL0543
.L0544 ;  rem 'Make sure Ikaza cannot come back

.L0545 ;  rem 'Give experience and level up if needed

.L0546 ;  rem 'Experience received is danger level of the monster 

.L0547 ;  temp2  =  item_damage[item_kind]

	LDX item_kind
	LDA item_damage,x
	STA temp2
.L0548 ;  rem 'No experience for killing the curse, as it's a sitting duck.

.L0549 ;  if item_kind  =  monster_curse then goto give_experience_end

	LDA item_kind
	CMP #monster_curse
     BNE .skipL0549
.condpart54
 jmp .give_experience_end

.skipL0549
.L0550 ;  if temp2  <  hero_next then goto give_experience

	LDA temp2
	CMP hero_next
     BCS .skipL0550
.condpart55
 jmp .give_experience

.skipL0550
.L0551 ;  rem 'Level up, but not more than 99

.L0552 ;  if hero_level  >  98 then goto give_experience_end

	LDA #98
	CMP hero_level
     BCS .skipL0552
.condpart56
 jmp .give_experience_end

.skipL0552
.L0553 ;  rem 'Level up sound

.L0554 ;  COLUP0 = blue

	LDA blue
	STA COLUP0
.L0555 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0556 ;  AUDF1 = 24

	LDA #24
	STA AUDF1
.L0557 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0558 ;  sound_timer = 8

	LDA #8
	STA sound_timer
.L0559 ;  hero_level  =  hero_level  +  1

	INC hero_level
.L0560 ;  hero_hp  =   ( hero_level  *  2 )   +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0561 ;  hero_mp  =   ( hero_level  /  16 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0562 ;  gosub set_mp_display

 jsr .set_mp_display

.L0563 ;  hero_next  =   ( hero_level  /  2 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	CLC
	ADC #1
	STA hero_next
.L0564 ;  COLUP0  =  turfy

	LDA #turfy
	STA COLUP0
.L0565 ;  goto give_experience_end

 jmp .give_experience_end

.give_experience
 ; give_experience

.L0566 ;  hero_next  =  hero_next  -  temp2

	LDA hero_next
	SEC
	SBC temp2
	STA hero_next
.give_experience_end
 ; give_experience_end

.L0567 ;  rem 'Drop an item, perhaps

.L0568 ;  temp6  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp6
.L0569 ;  rem 'Bit 4: monster always drops item 

.L0570 ;  if temp6{4} then goto drop_item

	LDA temp6
	AND #16
	BEQ .skipL0570
.condpart57
 jmp .drop_item

.skipL0570
.L0571 ;  if rand  >  128 then goto drop_item

	LDA #128
	CMP rand
     BCS .skipL0571
.condpart58
 jmp .drop_item

.skipL0571
.L0572 ;  rem 'Drop occasional, not always, depeding on monster flags

.drop_no_item
 ; drop_no_item

.L0573 ;  item_kind  =  item_none

	LDA #item_none
	STA item_kind
.L0574 ;  item_hp  =  0

	LDA #0
	STA item_hp
.L0575 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L0576 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
.L0577 ;  goto drop_item_done

 jmp .drop_item_done

.drop_item
 ; drop_item

.L0578 ;  temp2  =  monster_drops[item_kind]

	LDX item_kind
	LDA monster_drops,x
	STA temp2
.L0579 ;  item_kind  =  temp2

	LDA temp2
	STA item_kind
.L0580 ;  gosub item_setup_kind bank4

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

.L0581 ;  goto slash_collide_end

 jmp .slash_collide_end

.item_survived
 ; item_survived

.L0582 ;  item_hp  =  item_hp  -  temp1

	LDA item_hp
	SEC
	SBC temp1
	STA item_hp
.L0583 ;  rem 'Push item around

.L0584 ;  if hero_flags{0} then item_y  =  item_y  -  4

	LDA hero_flags
	LSR
	BCC .skipL0584
.condpart59
	LDA item_y
	SEC
	SBC #4
	STA item_y
.skipL0584
.L0585 ;  if hero_flags{1} then item_x  =  item_x  +  4

	LDA hero_flags
	AND #2
	BEQ .skipL0585
.condpart60
	LDA item_x
	CLC
	ADC #4
	STA item_x
.skipL0585
.L0586 ;  if hero_flags{2} then item_y  =  item_y  +  4

	LDA hero_flags
	AND #4
	BEQ .skipL0586
.condpart61
	LDA item_y
	CLC
	ADC #4
	STA item_y
.skipL0586
.L0587 ;  if hero_flags{3} then item_x  =  item_x  -  4

	LDA hero_flags
	AND #8
	BEQ .skipL0587
.condpart62
	LDA item_x
	SEC
	SBC #4
	STA item_x
.skipL0587
.L0588 ;  goto slash_collide_end

 jmp .slash_collide_end

.slash_item_collide
 ; slash_item_collide

.L0589 ;  rem 'Make a hit sound once

.L0590 ;  sound_timer = 1

	LDA #1
	STA sound_timer
.L0591 ;  AUDC1 = 8

	LDA #8
	STA AUDC1
.L0592 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0593 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0594 ;  if item_kind  <  item_switch1 then goto slash_item_pickup

	LDA item_kind
	CMP #item_switch1
     BCS .skipL0594
.condpart63
 jmp .slash_item_pickup

.skipL0594
.L0595 ;  if item_kind  >  item_switch7_on then goto slash_item_pickup

	LDA #item_switch7_on
	CMP item_kind
     BCS .skipL0595
.condpart64
 jmp .slash_item_pickup

.skipL0595
.L0596 ;  rem 'If colliding with an item that's not a switch, pick it up.

.L0597 ;  rem 'Otherwise it's a switch that's hit.  

.L0598 ;  goto hit_switch

 jmp .hit_switch

.slash_item_pickup
 ; slash_item_pickup

.L0599 ;  gosub item_pickup

 jsr .item_pickup

.L0600 ;  gosub show_inventory

 jsr .show_inventory

.L0601 ;  goto slash_collide_end

 jmp .slash_collide_end

.hit_switch
 ; hit_switch

.L0602 ;  rem 'Do nothing with the already triggered switches. 

.L0603 ;  rem '(Turn them off again, perhaps?)

.L0604 ;  if item_kind  >  item_switch7 then goto slash_collide_end

	LDA #item_switch7
	CMP item_kind
     BCS .skipL0604
.condpart65
 jmp .slash_collide_end

.skipL0604
.L0605 ;  rem 'Handle hitting of the switch: set the quest flag and change the display.

.L0606 ;  if item_kind  =  item_switch1 then quest_flags{1}  =  1

	LDA item_kind
	CMP #item_switch1
     BNE .skipL0606
.condpart66
	LDA quest_flags
	ORA #2
	STA quest_flags
.skipL0606
.L0607 ;  if item_kind  =  item_switch2 then quest_flags{2}  =  1

	LDA item_kind
	CMP #item_switch2
     BNE .skipL0607
.condpart67
	LDA quest_flags
	ORA #4
	STA quest_flags
.skipL0607
.L0608 ;  if item_kind  =  item_switch3 then quest_flags{3}  =  1

	LDA item_kind
	CMP #item_switch3
     BNE .skipL0608
.condpart68
	LDA quest_flags
	ORA #8
	STA quest_flags
.skipL0608
.L0609 ;  if item_kind  =  item_switch4 then quest_flags{4}  =  1

	LDA item_kind
	CMP #item_switch4
     BNE .skipL0609
.condpart69
	LDA quest_flags
	ORA #16
	STA quest_flags
.skipL0609
.L0610 ;  if item_kind  =  item_switch5 then quest_flags{5}  =  1

	LDA item_kind
	CMP #item_switch5
     BNE .skipL0610
.condpart70
	LDA quest_flags
	ORA #32
	STA quest_flags
.skipL0610
.L0611 ;  if item_kind  =  item_switch6 then quest_flags{6}  =  1

	LDA item_kind
	CMP #item_switch6
     BNE .skipL0611
.condpart71
	LDA quest_flags
	ORA #64
	STA quest_flags
.skipL0611
.L0612 ;  if item_kind  =  item_switch7 then quest_flags{7}  =  1

	LDA item_kind
	CMP #item_switch7
     BNE .skipL0612
.condpart72
	LDA quest_flags
	ORA #128
	STA quest_flags
.skipL0612
.L0613 ;  gosub item_setup_kind bank4

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

.L0614 ;  rem 'Player action though button

.L0615 ;  if !joy0fire then goto hero_action_done

 lda #$80
 bit INPT4
	BEQ .skipL0615
.condpart73
 jmp .hero_action_done

.skipL0615
.L0616 ;  rem 'Skip all this if action is blocked.

.L0617 ;  if !hero_flags{4} then goto hero_action_blocked

	LDA hero_flags
	AND #16
	BNE .skipL0617
.condpart74
 jmp .hero_action_blocked

.skipL0617
.L0618 ;  if !hero_items{7} then goto hero_act_nostrike

	BIT hero_items
	BMI .skipL0618
.condpart75
 jmp .hero_act_nostrike

.skipL0618
.L0619 ;  if hero_mp  <  strike_cost then goto hero_act_nostrike

	LDA hero_mp
	CMP #strike_cost
     BCS .skipL0619
.condpart76
 jmp .hero_act_nostrike

.skipL0619
.L0620 ;  rem ' The b switch controls whether the book of striking is in use or not.

.L0621 ;  if switchleftb then goto hero_act_nostrike

 lda #$40
 bit SWCHB
	BNE .skipL0621
.condpart77
 jmp .hero_act_nostrike

.skipL0621
.L0622 ;  if item_kind  <  last_monster then goto hero_act_strike

	LDA item_kind
	CMP #last_monster
     BCS .skipL0622
.condpart78
 jmp .hero_act_strike

.skipL0622
.L0623 ;  if item_kind  >=  item_switch1  &&  item_kind  <=  item_switch7 then goto hero_act_strike

	LDA item_kind
	CMP #item_switch1
     BCC .skipL0623
.condpart79
	LDA #item_switch7
	CMP item_kind
     BCC .skip79then
.condpart80
 jmp .hero_act_strike

.skip79then
.skipL0623
.hero_act_nostrike
 ; hero_act_nostrike

.L0624 ;  temp1  =  hero_flags  &  %00001111

	LDA hero_flags
	AND #%00001111
	STA temp1
.L0625 ;  rem 'Use this to jump to the right attack action.

.L0626 ;  rem '          0     1=north 2=east 3=ne   4=south  5=n+s(emu onlu)  6=se 7=nse 

.L0627 ;  rem ' 8 = west

.L0628 ;  rem ' 9 10 11 12 13 14 15(all sides, emu only)    

.L0629 ;  on temp1 goto had hero_act_n hero_act_e hero_act_n hero_act_s hero_act_s hero_act_n hero_act_n hero_act_w hero_act_n hero_act_w hero_act_s had had had

	LDX temp1
	LDA .L0629jumptablehi,x
	PHA
	LDA .L0629jumptablelo,x
	PHA
	RTS
.L0629jumptablehi
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
.L0629jumptablelo
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

.L0630 ;  missile0x = hero_x  +  4

	LDA hero_x
	CLC
	ADC #4
	STA missile0x
.L0631 ;  missile0y = hero_y  -  14

	LDA hero_y
	SEC
	SBC #14
	STA missile0y
.L0632 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0633 ;  NUSIZ0 = $00

	LDA #$00
	STA NUSIZ0
.L0634 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_e
 ; hero_act_e

.L0635 ;  missile0x = hero_x  +  9

	LDA hero_x
	CLC
	ADC #9
	STA missile0x
.L0636 ;  missile0y = hero_y  -  4

	LDA hero_y
	SEC
	SBC #4
	STA missile0y
.L0637 ;  missile0height = 0

	LDA #0
	STA missile0height
.L0638 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0639 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_s
 ; hero_act_s

.L0640 ;  missile0x = hero_x  +  4

	LDA hero_x
	CLC
	ADC #4
	STA missile0x
.L0641 ;  missile0y = hero_y  +  8

	LDA hero_y
	CLC
	ADC #8
	STA missile0y
.L0642 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0643 ;  NUSIZ0 = $00

	LDA #$00
	STA NUSIZ0
.L0644 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_w
 ; hero_act_w

.L0645 ;  missile0x = hero_x  -  7

	LDA hero_x
	SEC
	SBC #7
	STA missile0x
.L0646 ;  missile0y = hero_y  -  4

	LDA hero_y
	SEC
	SBC #4
	STA missile0y
.L0647 ;  missile0height = 0

	LDA #0
	STA missile0height
.L0648 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0649 ;  goto hero_action_end

 jmp .hero_action_end

.L0650 ;  rem NUSIZ0=$00

.hero_act_strike
 ; hero_act_strike

.L0651 ;  hero_mp  =  hero_mp  -  strike_cost

	LDA hero_mp
	SEC
	SBC #strike_cost
	STA hero_mp
.L0652 ;  gosub set_mp_display

 jsr .set_mp_display

.L0653 ;  rem 'Strike will hit the ennemy wherever it is. A guaranteed hit I'd say ^_^

.L0654 ;  missile0x = item_x

	LDA item_x
	STA missile0x
.L0655 ;  missile0y = item_y

	LDA item_y
	STA missile0y
.L0656 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0657 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0658 ;  COLUP0 = rand

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
.L0659 ;  COLUP1 = red

	LDA red
	STA COLUP1
.L0660 ;  goto hero_action_end

 jmp .hero_action_end

.had
 ; had

.hero_action_done
 ; hero_action_done

.L0661 ;  hero_flags{4}  =  1

	LDA hero_flags
	ORA #16
	STA hero_flags
.L0662 ;  rem 'Allow attacks again

.hero_action_blocked
 ; hero_action_blocked

.L0663 ;  rem 'If the action is blocked or released, hide the missile

.L0664 ;  missile0x  =  nowhere

	LDA #nowhere
	STA missile0x
.L0665 ;  missile0y  =  nowhere

	LDA #nowhere
	STA missile0y
.hero_action_end
 ; hero_action_end

.
 ; 

.L0666 ;  rem 'Deterimine hero's motion and direction from the joystick

.L0667 ;  rem 'In NESW order  

.L0668 ;  rem ' hero_flags = (hero_flags / 16) * 16 preserves the higher nibble (I hope)

.L0669 ;  rem '

.L0670 ;  rem 'For the player collision detection, we calculate the playfield position 

.L0671 ;  rem 'at any of the four points around the feet of the player, and check if 

.L0672 ;  rem 'if anything is there with pfread, and prevent motion if it is so.  

.L0673 ;  rem ' Note that we do allow Signe to turn her facing even when she cannot move.

.
 ; 

.L0674 ;  if !joy0up then goto joy0up_end

 lda #$10
 bit SWCHA
	BEQ .skipL0674
.condpart81
 jmp .joy0up_end

.skipL0674
.L0675 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

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
.L0676 ;  hero_flags{0}  =  1

	LDA hero_flags
	ORA #1
	STA hero_flags
.L0677 ;  rem 'Check top middle point 

.L0678 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0679 ;  temp2  =   ( hero_y  -  hero_high  -  1  -  1 )   /  8

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
.L0680 ;  if pfread ( temp1 ,  temp2 )  goto joy0up_end

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
.L0681 ;  hero_y  =  hero_y  -  1

	DEC hero_y
.joy0up_end
 ; joy0up_end

.L0682 ;  if !joy0right then goto joy0right_end

 lda #$80
 bit SWCHA
	BEQ .skipL0682
.condpart82
 jmp .joy0right_end

.skipL0682
.L0683 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

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
.L0684 ;  hero_flags{1}  =  1

	LDA hero_flags
	ORA #2
	STA hero_flags
.L0685 ;  temp1  =   ( hero_x  +  hero_wide  +  1  -  17 )   /  4

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
.L0686 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

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
.L0687 ;  rem 'Prevent motion if something is there

.L0688 ;  if pfread ( temp1 ,  temp2 )  goto joy0right_end

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
.L0689 ;  hero_x  =  hero_x  +  1

	INC hero_x
.joy0right_end
 ; joy0right_end

.L0690 ;  if !joy0down then goto joy0down_end

 lda #$20
 bit SWCHA
	BEQ .skipL0690
.condpart83
 jmp .joy0down_end

.skipL0690
.L0691 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

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
.L0692 ;  hero_flags{2}  =  1

	LDA hero_flags
	ORA #4
	STA hero_flags
.L0693 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0694 ;  temp2  =   ( hero_y  +  1  -  1 )   /  8

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
.L0695 ;  rem 'Check bottom middle point. don't move if it's blocked by the playfield.

.L0696 ;  if pfread ( temp1 ,  temp2 )  then goto joy0down_end

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
	BNE .skipL0696
.condpart84
 jmp .joy0down_end

.skipL0696
.L0697 ;  hero_y  =  hero_y  +  1

	INC hero_y
.joy0down_end
 ; joy0down_end

.L0698 ;  if !joy0left then goto joy0left_end

 lda #$40
 bit SWCHA
	BEQ .skipL0698
.condpart85
 jmp .joy0left_end

.skipL0698
.L0699 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

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
.L0700 ;  hero_flags{3}  =  1

	LDA hero_flags
	ORA #8
	STA hero_flags
.L0701 ;  temp1  =   ( hero_x  -  1  -  17 )   /  4

; complex statement detected
	LDA hero_x
	SEC
	SBC #1
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0702 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

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
.L0703 ;  rem 'Check point to the west of the feet 

.L0704 ;  if pfread ( temp1 ,  temp2 )  goto joy0left_end

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
.L0705 ;  hero_x  =  hero_x  -  1

	DEC hero_x
.joy0left_end
 ; joy0left_end

.
 ; 

.L0706 ;  rem 'Monster "AI":

.L0707 ;  rem 'Animate missile if it has been launched

.L0708 ;  if missile1y  =  nowhere then goto missile_move_end

	LDA missile1y
	CMP #nowhere
     BNE .skipL0708
.condpart86
 jmp .missile_move_end

.skipL0708
.L0709 ;  if item_flags{3} then missile1x  =  missile1x  -  2  :  goto missile_move_end

	LDA item_flags
	AND #8
	BEQ .skipL0709
.condpart87
	LDA missile1x
	SEC
	SBC #2
	STA missile1x
 jmp .missile_move_end

.skipL0709
.L0710 ;  if item_flags{0} then missile1y  =  missile1y  -  2  :  goto missile_move_end

	LDA item_flags
	LSR
	BCC .skipL0710
.condpart88
	LDA missile1y
	SEC
	SBC #2
	STA missile1y
 jmp .missile_move_end

.skipL0710
.L0711 ;  if item_flags{1} then missile1x  =  missile1x  +  2  :  goto missile_move_end

	LDA item_flags
	AND #2
	BEQ .skipL0711
.condpart89
	LDA missile1x
	CLC
	ADC #2
	STA missile1x
 jmp .missile_move_end

.skipL0711
.L0712 ;  if item_flags{2} then missile1y  =  missile1y  +  2  :  goto missile_move_end

	LDA item_flags
	AND #4
	BEQ .skipL0712
.condpart90
	LDA missile1y
	CLC
	ADC #2
	STA missile1y
 jmp .missile_move_end

.skipL0712
.missile_move_end
 ; missile_move_end

.
 ; 

.L0713 ;  rem 'Move the item if it's a mobile (id lower than 128)

.L0714 ;  if item_kind{7} then goto item_move_end

	BIT item_kind
	BPL .skipL0714
.condpart91
 jmp .item_move_end

.skipL0714
.L0715 ;  temp1  =  rand

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
.L0716 ;  rem ' Shoot a missile sometimes...

.L0717 ;  temp6  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp6
.L0718 ;  rem 'But not if the monster cannot shoot (bit 0)

.L0719 ;  if !temp6{0} then goto activate_missile_end

	LDA temp6
	LSR
	BCS .skipL0719
.condpart92
 jmp .activate_missile_end

.skipL0719
.L0720 ;  if !game_timer{4} then goto activate_missile_end

	LDA game_timer
	AND #16
	BNE .skipL0720
.condpart93
 jmp .activate_missile_end

.skipL0720
.L0721 ;  if !game_timer{7} then goto activate_missile_end

	BIT game_timer
	BMI .skipL0721
.condpart94
 jmp .activate_missile_end

.skipL0721
.L0722 ;  if !temp1{0} then goto activate_missile_end

	LDA temp1
	LSR
	BCS .skipL0722
.condpart95
 jmp .activate_missile_end

.skipL0722
.L0723 ;  if missile1y  <>  nowhere then goto activate_missile_end

	LDA missile1y
	CMP #nowhere
     BEQ .skipL0723
.condpart96
 jmp .activate_missile_end

.skipL0723
.L0724 ;  rem 'Make a shot sound

.L0725 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0726 ;  AUDC1 = 14

	LDA #14
	STA AUDC1
.L0727 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0728 ;  AUDF1 = 20

	LDA #20
	STA AUDF1
.L0729 ;  rem 'Place the missile  

.L0730 ;  missile1x  =  item_x  +  4

	LDA item_x
	CLC
	ADC #4
	STA missile1x
.L0731 ;  missile1y  =  item_y  -  4

	LDA item_y
	SEC
	SBC #4
	STA missile1y
.L0732 ;  temp4  =  item_x  +  8

	LDA item_x
	CLC
	ADC #8
	STA temp4
.L0733 ;  if hero_x  >  temp4 then item_flags{1}  =  1

	LDA temp4
	CMP hero_x
     BCS .skipL0733
.condpart97
	LDA item_flags
	ORA #2
	STA item_flags
.skipL0733
.L0734 ;  temp4  =  item_x  -  8

	LDA item_x
	SEC
	SBC #8
	STA temp4
.L0735 ;  if hero_x  <  temp4 then item_flags{3}  =  1

	LDA hero_x
	CMP temp4
     BCS .skipL0735
.condpart98
	LDA item_flags
	ORA #8
	STA item_flags
.skipL0735
.L0736 ;  if hero_y  >  item_y then item_flags{2}  =  1

	LDA item_y
	CMP hero_y
     BCS .skipL0736
.condpart99
	LDA item_flags
	ORA #4
	STA item_flags
.skipL0736
.L0737 ;  if hero_y  <  item_y then item_flags{0}  =  1

	LDA hero_y
	CMP item_y
     BCS .skipL0737
.condpart100
	LDA item_flags
	ORA #1
	STA item_flags
.skipL0737
.L0738 ;  missile1height = 4

	LDA #4
	STA missile1height
.activate_missile_end
 ; activate_missile_end

.L0739 ;  rem 'Agressive chasing if aggressive flag is set 

.L0740 ;  if temp6{2} then goto monster_chase

	LDA temp6
	AND #4
	BEQ .skipL0740
.condpart101
 jmp .monster_chase

.skipL0740
.L0741 ;  rem 'Don't move if monster is static

.L0742 ;  if temp6{3} then goto item_move_end

	LDA temp6
	AND #8
	BEQ .skipL0742
.condpart102
 jmp .item_move_end

.skipL0742
.L0743 ;  rem 'Only move half the time

.L0744 ;  if game_timer{2} goto item_move_end

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

.L0745 ;  rem 'Only move if the 4th and 5th bit are not set

.L0746 ;  if temp1{1} then goto item_move_end

	LDA temp1
	AND #2
	BEQ .skipL0746
.condpart103
 jmp .item_move_end

.skipL0746
.L0747 ;  rem 'Random walking monsters don't give chase

.L0748 ;  if temp6{1} then goto monster_random_walk

	LDA temp6
	AND #2
	BEQ .skipL0748
.condpart104
 jmp .monster_random_walk

.skipL0748
.monster_chase
 ; monster_chase

.L0749 ;  if hero_x  >  item_x then item_x  =  item_x  +  1 else item_x  =  item_x  -  1

	LDA item_x
	CMP hero_x
     BCS .skipL0749
.condpart105
	INC item_x
 jmp .skipelse0
.skipL0749
	DEC item_x
.skipelse0
.L0750 ;  if hero_y  >  item_y then item_y  =  item_y  +  1 else item_y  =  item_y  -  1

	LDA item_y
	CMP hero_y
     BCS .skipL0750
.condpart106
	INC item_y
 jmp .skipelse1
.skipL0750
	DEC item_y
.skipelse1
.L0751 ;  if temp6{2} then goto item_move_end

	LDA temp6
	AND #4
	BEQ .skipL0751
.condpart107
 jmp .item_move_end

.skipL0751
.L0752 ;  rem 'Aggressive ennemies don't random walk

.monster_random_walk
 ; monster_random_walk

.L0753 ;  rem 'Use bits of random number for random walk.

.L0754 ;  if temp1{0} then item_y  =  item_y  -  1

	LDA temp1
	LSR
	BCC .skipL0754
.condpart108
	DEC item_y
.skipL0754
.L0755 ;  if temp1{4} then item_x  =  item_x  -  1

	LDA temp1
	AND #16
	BEQ .skipL0755
.condpart109
	DEC item_x
.skipL0755
.L0756 ;  if temp1{0}  ||  temp1{4} then goto item_move_end

	LDA temp1
	LSR
	BCC .skipL0756
.condpart110
 jmp .condpart111
.skipL0756
	LDA temp1
	AND #16
	BEQ .skip6OR
.condpart111
 jmp .item_move_end

.skip6OR
.L0757 ;  if temp1{2} then item_y  =  item_y  +  1

	LDA temp1
	AND #4
	BEQ .skipL0757
.condpart112
	INC item_y
.skipL0757
.L0758 ;  if temp1{6} then item_x  =  item_x  +  1

	BIT temp1
	BVC .skipL0758
.condpart113
	INC item_x
.skipL0758
.item_move_end
 ; item_move_end

.
 ; 

.L0759 ;  rem 'Activate missile if needed

.
 ; 

.L0760 ;  rem 'Debug by showing flags in score field

.L0761 ;  rem sc0 = hero_flags

.L0762 ;  rem temp2 = monster_info[item_kind]

.L0763 ;  rem sc1 = temp2

.L0764 ;  rem sc2 = hero_room

.
 ; 

.L0765 ;  rem 'Music, disabled for now.

.L0766 ;  rem 'Is it time to update the note? 

.L0767 ;  rem if music_timer = 0 then gosub music_change_note

.L0768 ;  rem music_timer = music_timer - 1

.
 ; 

.
 ; 

.
 ; 

.L0769 ;  rem 'Collision op hero with edge of field, transfer to other field.

.L0770 ;  if field_left  <  hero_x then goto exit_left_done

	LDA #field_left
	CMP hero_x
     BCS .skipL0770
.condpart114
 jmp .exit_left_done

.skipL0770
.L0771 ;  hero_x  =  field_right_enter

	LDA #field_right_enter
	STA hero_x
.L0772 ;  hero_oldx  =  field_right

	LDA #field_right
	STA hero_oldx
.L0773 ;  hero_room  =  hero_room  -  1

	DEC hero_room
.L0774 ;  gosub room_draw bank2

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
.L0775 ;  goto exit_done

 jmp .exit_done

.exit_left_done
 ; exit_left_done

.L0776 ;  if hero_x  <  field_right then goto exit_right_done

	LDA hero_x
	CMP #field_right
     BCS .skipL0776
.condpart115
 jmp .exit_right_done

.skipL0776
.L0777 ;  hero_x  =  field_left_enter

	LDA #field_left_enter
	STA hero_x
.L0778 ;  hero_oldx  =  field_left

	LDA #field_left
	STA hero_oldx
.L0779 ;  hero_room  =  hero_room  +  1

	INC hero_room
.L0780 ;  gosub room_draw bank2

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
.L0781 ;  goto exit_done

 jmp .exit_done

.exit_right_done
 ; exit_right_done

.L0782 ;  if hero_y  >  field_top then goto exit_top_done

	LDA #field_top
	CMP hero_y
     BCS .skipL0782
.condpart116
 jmp .exit_top_done

.skipL0782
.L0783 ;  hero_y  =  field_bottom_enter

	LDA #field_bottom_enter
	STA hero_y
.L0784 ;  hero_oldy  =  field_bottom

	LDA #field_bottom
	STA hero_oldy
.L0785 ;  hero_room  =  hero_room  -  8

	LDA hero_room
	SEC
	SBC #8
	STA hero_room
.L0786 ;  gosub room_draw bank2

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
.L0787 ;  goto exit_done

 jmp .exit_done

.exit_top_done
 ; exit_top_done

.L0788 ;  if hero_y  <  field_bottom then goto exit_bottom_done

	LDA hero_y
	CMP #field_bottom
     BCS .skipL0788
.condpart117
 jmp .exit_bottom_done

.skipL0788
.L0789 ;  hero_y  =  field_top_enter

	LDA #field_top_enter
	STA hero_y
.L0790 ;  hero_oldy  =  field_top

	LDA #field_top
	STA hero_oldy
.L0791 ;  hero_room  =  hero_room  +  8

	LDA hero_room
	CLC
	ADC #8
	STA hero_room
.L0792 ;  gosub room_draw bank2

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
.L0793 ;  goto exit_done

 jmp .exit_done

.exit_bottom_done
 ; exit_bottom_done

.exit_done
 ; exit_done

.
 ; 

.L0794 ;  rem 'Collision of item with edge of field, bounce back.

.L0795 ;  if item_x  <  field_left then item_x = field_left_enter

	LDA item_x
	CMP #field_left
     BCS .skipL0795
.condpart118
	LDA #field_left_enter
	STA item_x
.skipL0795
.L0796 ;  if item_x  >  field_right then item_x = field_right_enter

	LDA #field_right
	CMP item_x
     BCS .skipL0796
.condpart119
	LDA #field_right_enter
	STA item_x
.skipL0796
.L0797 ;  if item_y  <  field_top then item_y = field_top_enter

	LDA item_y
	CMP #field_top
     BCS .skipL0797
.condpart120
	LDA #field_top_enter
	STA item_y
.skipL0797
.L0798 ;  if item_y  >  field_bottom then item_x = field_bottom_enter

	LDA #field_bottom
	CMP item_y
     BCS .skipL0798
.condpart121
	LDA #field_bottom_enter
	STA item_x
.skipL0798
.
 ; 

.
 ; 

.L0799 ;  rem 'Draw acting player in position 2

.L0800 ;  if joy0fire goto player_animate_2

 lda #$80
 bit INPT4
 if ((* - .player_animate_2) < 127) && ((* - .player_animate_2) > -128)
	BEQ .player_animate_2
 else
	bne .8skipplayer_animate_2
	jmp .player_animate_2
.8skipplayer_animate_2
 endif
.L0801 ;  rem 'Draw non moving player not animated

.L0802 ;  if hero_x  =  hero_oldx  &&  hero_y  =  hero_oldy then goto player_animate_1

	LDA hero_x
	CMP hero_oldx
     BNE .skipL0802
.condpart122
	LDA hero_y
	CMP hero_oldy
     BNE .skip122then
.condpart123
 jmp .player_animate_1

.skip122then
.skipL0802
.L0803 ;  rem 'Draw and animate player every so many ticks

.L0804 ;  if !game_timer{4} then goto player_animate_2

	LDA game_timer
	AND #16
	BNE .skipL0804
.condpart124
 jmp .player_animate_2

.skipL0804
.player_animate_1
 ; player_animate_1

.L0805 ;  if hero_flags{0} then gosub hero_draw_n

	LDA hero_flags
	LSR
	BCC .skipL0805
.condpart125
 jsr .hero_draw_n

.skipL0805
.L0806 ;  if hero_flags{1} then gosub hero_draw_e

	LDA hero_flags
	AND #2
	BEQ .skipL0806
.condpart126
 jsr .hero_draw_e

.skipL0806
.L0807 ;  if hero_flags{2} then gosub hero_draw_s

	LDA hero_flags
	AND #4
	BEQ .skipL0807
.condpart127
 jsr .hero_draw_s

.skipL0807
.L0808 ;  if hero_flags{3} then gosub hero_draw_w

	LDA hero_flags
	AND #8
	BEQ .skipL0808
.condpart128
 jsr .hero_draw_w

.skipL0808
.L0809 ;  goto player_animate_end

 jmp .player_animate_end

.player_animate_2
 ; player_animate_2

.L0810 ;  if hero_flags{0} then gosub hero_draw_n2

	LDA hero_flags
	LSR
	BCC .skipL0810
.condpart129
 jsr .hero_draw_n2

.skipL0810
.L0811 ;  if hero_flags{1} then gosub hero_draw_e2

	LDA hero_flags
	AND #2
	BEQ .skipL0811
.condpart130
 jsr .hero_draw_e2

.skipL0811
.L0812 ;  if hero_flags{2} then gosub hero_draw_s2

	LDA hero_flags
	AND #4
	BEQ .skipL0812
.condpart131
 jsr .hero_draw_s2

.skipL0812
.L0813 ;  if hero_flags{3} then gosub hero_draw_w2

	LDA hero_flags
	AND #8
	BEQ .skipL0813
.condpart132
 jsr .hero_draw_w2

.skipL0813
.player_animate_end
 ; player_animate_end

.
 ; 

.L0814 ;  rem 'Reflect sprite if moving west, otherwise, don't

.L0815 ;  if hero_flags{3} then REFP0  =  8 else REFP0  =  0

	LDA hero_flags
	AND #8
	BEQ .skipL0815
.condpart133
	LDA #8
	STA REFP0
 jmp .skipelse2
.skipL0815
	LDA #0
	STA REFP0
.skipelse2
.
 ; 

.
 ; 

.
 ; 

.L0816 ;  rem draw screen

.L0817 ;  drawscreen

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

.L0818 ;  rem And continue main loop

.L0819 ;  goto main_loop

 jmp .main_loop

.
 ; 

.L0820 ;  rem 'Vblank sub. Do something useful in here...

.vblank
 ; vblank

.L0821 ;  rem 'Do something here...

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

.L0823 ;  rem 'Gosub that correctly displays MP

.set_mp_display
 ; set_mp_display

.L0824 ;  lives = hero_mp  *  32

	LDA hero_mp
	asl
	asl
	asl
	asl
	asl
	STA lives
.L0825 ;  lives:

	LDA #<lives__L0825
	STA lifepointer
	LDA lifepointer+1
	AND #$E0
	ORA #(>lives__L0825)&($1F)
	STA lifepointer+1
.L0826 ;  return

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

.L0827 ;  rem 'Display inventory ggosub

.show_inventory
 ; show_inventory

.L0828 ;  sc0  =  0

	LDA #0
	STA sc0
.L0829 ;  sc1  =  0

	LDA #0
	STA sc1
.L0830 ;  sc2  =  0

	LDA #0
	STA sc2
.L0831 ;  rem 'Add up score to display Numen Leaves

.L0832 ;  if hero_items{0} then sc0  =  sc0  +  16

	LDA hero_items
	LSR
	BCC .skipL0832
.condpart134
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0832
.L0833 ;  if hero_items{1} then sc0  =  sc0  +  16

	LDA hero_items
	AND #2
	BEQ .skipL0833
.condpart135
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0833
.L0834 ;  if hero_items{2} then sc0  =  sc0  +  16

	LDA hero_items
	AND #4
	BEQ .skipL0834
.condpart136
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0834
.L0835 ;  rem 'Display shield if we have it

.L0836 ;  if hero_items{3} then sc0  =  sc0  +  4

	LDA hero_items
	AND #8
	BEQ .skipL0836
.condpart137
	LDA sc0
	CLC
	ADC #4
	STA sc0
.skipL0836
.L0837 ;  rem 'Display Armor

.L0838 ;  if hero_items{4} then sc1  =  sc1  +  80

	LDA hero_items
	AND #16
	BEQ .skipL0838
.condpart138
	LDA sc1
	CLC
	ADC #80
	STA sc1
.skipL0838
.L0839 ;  rem 'Display Sword

.L0840 ;  if hero_items{5} then sc1  =  sc1  +  6

	LDA hero_items
	AND #32
	BEQ .skipL0840
.condpart139
	LDA sc1
	CLC
	ADC #6
	STA sc1
.skipL0840
.L0841 ;  if hero_items{6} then sc2  =  sc2  +  112

	BIT hero_items
	BVC .skipL0841
.condpart140
	LDA sc2
	CLC
	ADC #112
	STA sc2
.skipL0841
.L0842 ;  if hero_items{7} then sc2  =  sc2  +  8

	BIT hero_items
	BPL .skipL0842
.condpart141
	LDA sc2
	CLC
	ADC #8
	STA sc2
.skipL0842
.L0843 ;  return

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

.L0844 ;  rem 'Item Pickup gosub

.item_pickup
 ; item_pickup

.L0845 ;  rem 'All sorts of item pickup effects, should be in a gosub, I guess

.L0846 ;  rem 'Win game if finding victory item

.L0847 ;  rem 'Make a pickup sound

.L0848 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0849 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0850 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0851 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0852 ;  if item_kind  =  item_victory then goto game_win bank5

	LDA item_kind
	CMP #item_victory
     BNE .skipL0852
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
.skipL0852
.L0853 ;  if item_kind  =  item_leaf1 then hero_items{0}  =  1

	LDA item_kind
	CMP #item_leaf1
     BNE .skipL0853
.condpart143
	LDA hero_items
	ORA #1
	STA hero_items
.skipL0853
.L0854 ;  if item_kind  =  item_leaf2 then hero_items{1}  =  1

	LDA item_kind
	CMP #item_leaf2
     BNE .skipL0854
.condpart144
	LDA hero_items
	ORA #2
	STA hero_items
.skipL0854
.L0855 ;  if item_kind  =  item_leaf3 then hero_items{2}  =  1

	LDA item_kind
	CMP #item_leaf3
     BNE .skipL0855
.condpart145
	LDA hero_items
	ORA #4
	STA hero_items
.skipL0855
.L0856 ;  if item_kind  =  item_shield then hero_items{3}  =  1

	LDA item_kind
	CMP #item_shield
     BNE .skipL0856
.condpart146
	LDA hero_items
	ORA #8
	STA hero_items
.skipL0856
.L0857 ;  if item_kind  =  item_armor then hero_items{4}  =  1

	LDA item_kind
	CMP #item_armor
     BNE .skipL0857
.condpart147
	LDA hero_items
	ORA #16
	STA hero_items
.skipL0857
.L0858 ;  if item_kind  =  item_sword then hero_items{5}  =  1

	LDA item_kind
	CMP #item_sword
     BNE .skipL0858
.condpart148
	LDA hero_items
	ORA #32
	STA hero_items
.skipL0858
.L0859 ;  if item_kind  =  item_bookheal then hero_items{6}  =  1

	LDA item_kind
	CMP #item_bookheal
     BNE .skipL0859
.condpart149
	LDA hero_items
	ORA #64
	STA hero_items
.skipL0859
.L0860 ;  if item_kind  =  item_bookstrike then hero_items{7}  =  1

	LDA item_kind
	CMP #item_bookstrike
     BNE .skipL0860
.condpart150
	LDA hero_items
	ORA #128
	STA hero_items
.skipL0860
.L0861 ;  if item_kind  <>  item_healhp then goto no_healhp

	LDA item_kind
	CMP #item_healhp
     BEQ .skipL0861
.condpart151
 jmp .no_healhp

.skipL0861
.L0862 ;  hero_hp  =  hero_hp  +  8

	LDA hero_hp
	CLC
	ADC #8
	STA hero_hp
.L0863 ;  temp2  =   ( hero_level  *  2 )   +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA temp2
.L0864 ;  if hero_hp  >  temp2 then hero_hp  =  temp2

	LDA temp2
	CMP hero_hp
     BCS .skipL0864
.condpart152
	LDA temp2
	STA hero_hp
.skipL0864
.no_healhp
 ; no_healhp

.L0865 ;  if item_kind  <>  item_healmp then goto no_healmp

	LDA item_kind
	CMP #item_healmp
     BEQ .skipL0865
.condpart153
 jmp .no_healmp

.skipL0865
.L0866 ;  hero_mp  =  hero_mp  +  1

	INC hero_mp
.L0867 ;  temp2  =   ( hero_level  /  16 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA temp2
.L0868 ;  if hero_mp  >  temp2 then hero_mp  =  temp2

	LDA temp2
	CMP hero_mp
     BCS .skipL0868
.condpart154
	LDA temp2
	STA hero_mp
.skipL0868
.L0869 ;  gosub set_mp_display

 jsr .set_mp_display

.no_healmp
 ; no_healmp

.L0870 ;  if item_kind  <>  item_healallhp then goto no_healallhp

	LDA item_kind
	CMP #item_healallhp
     BEQ .skipL0870
.condpart155
 jmp .no_healallhp

.skipL0870
.L0871 ;  hero_hp  =   ( hero_level  *  2 )   +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.no_healallhp
 ; no_healallhp

.L0872 ;  if item_kind  <>  item_healallmp then goto no_healallmp

	LDA item_kind
	CMP #item_healallmp
     BEQ .skipL0872
.condpart156
 jmp .no_healallmp

.skipL0872
.L0873 ;  hero_mp  =   ( hero_level  /  16 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0874 ;  gosub set_mp_display

 jsr .set_mp_display

.no_healallmp
 ; no_healallmp

.L0875 ;  rem " All done with item effects 

.
 ; 

.L0876 ;  rem ' Remove item

.L0877 ;  item_kind  =  item_none

	LDA #item_none
	STA item_kind
.L0878 ;  item_hp  =  0

	LDA #0
	STA item_hp
.L0879 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L0880 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
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

.
 ; 

.hero_draw_n
 ; hero_draw_n

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

.hero_draw_s
 ; hero_draw_s

.L0885 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0886 ;  player0:

	LDA #<playerL0886_0

	STA player0pointerlo
	LDA #>playerL0886_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0887 ;  return

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

.L0888 ;  REFP0 = 8

	LDA #8
	STA REFP0
.L0889 ;  goto hero_draw_e_start

 jmp .hero_draw_e_start

.hero_draw_e
 ; hero_draw_e

.L0890 ;  REFP0 = 0

	LDA #0
	STA REFP0
.hero_draw_e_start
 ; hero_draw_e_start

.L0891 ;  player0:

	LDA #<playerL0891_0

	STA player0pointerlo
	LDA #>playerL0891_0

	STA player0pointerhi
	LDA #13
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

.hero_draw_n2
 ; hero_draw_n2

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

.hero_draw_s2
 ; hero_draw_s2

.L0896 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0897 ;  player0:

	LDA #<playerL0897_0

	STA player0pointerlo
	LDA #>playerL0897_0

	STA player0pointerhi
	LDA #14
	STA player0height
.L0898 ;  return

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

.L0899 ;  REFP0 = 8

	LDA #8
	STA REFP0
.L0900 ;  goto hero_draw_e2_start

 jmp .hero_draw_e2_start

.hero_draw_e2
 ; hero_draw_e2

.L0901 ;  REFP0 = 0

	LDA #0
	STA REFP0
.hero_draw_e2_start
 ; hero_draw_e2_start

.L0902 ;  player0:

	LDA #<playerL0902_0

	STA player0pointerlo
	LDA #>playerL0902_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0903 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.L0904 ;  bank 2

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
.L0905 ;  set kernel_options pfcolors

.room_draw
 ; room_draw

.L0906 ;  rem 'Reset the monster missile and item flags

.L0907 ;  missile1x  =  nowhere

	LDA #nowhere
	STA missile1x
.L0908 ;  missile1y  =  nowhere

	LDA #nowhere
	STA missile1y
.L0909 ;  item_flags  =  0

	LDA #0
	STA item_flags
.L0910 ;  rem 'Set up item and draw it

.L0911 ;  goto item_setup bank4

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

.L0912 ;  if hero_room  >  39 goto room_draw_40_local

	LDA #39
	CMP hero_room
 if ((* - .room_draw_40_local) < 127) && ((* - .room_draw_40_local) > -128)
	bcc .room_draw_40_local
 else
	bcs .9skiproom_draw_40_local
	jmp .room_draw_40_local
.9skiproom_draw_40_local
 endif
.L0913 ;  if hero_room  >  63 goto room_draw_64

	LDA #63
	CMP hero_room
 if ((* - .room_draw_64) < 127) && ((* - .room_draw_64) > -128)
	bcc .room_draw_64
 else
	bcs .10skiproom_draw_64
	jmp .room_draw_64
.10skiproom_draw_64
 endif
.L0914 ;  on hero_room goto r00 r01 r02 r03 r04 r05 r06 r07 r08 r09 r10 r11 r12 r13 r14 r15 r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31 r32 r33 r34 r35 r36 r37 r38 r39 r40

	LDX hero_room
	LDA .L0914jumptablehi,x
	PHA
	LDA .L0914jumptablelo,x
	PHA
	RTS
.L0914jumptablehi
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
.L0914jumptablelo
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
.L0915 ;  rem r41 r42 r43 r44 r45 r46 r47 r48 r49 r50

.L0916 ;  rem r51 r52 r53 r54 r55 r56 r57 r58 r59 r60 r61 r62 r63 r64

.room_draw_40_local
 ; room_draw_40_local

.L0917 ;  goto room_draw_40 bank3

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

.L0918 ;  rem 'If we get here, the room number s not valid. Go to room room_start (49)

.L0919 ;  hero_room  =  room_start

	LDA #room_start
	STA hero_room
.L0920 ;  goto r00

 jmp .r00

.L0921 ;  rem r64

.L0922 ;  rem r65 r66 r67 r68 r69 r70 r71 r72 r73 r74 r75 r76 r77 r78 r79 r80 r81 r82 r83 r84 r85 r86 r87 r88 r89 r90 r91 r92 r93 r94 r95 r96 r97 r98 r99 r100

.room_draw_end
 ; room_draw_end

.L0923 ;  rem 'Clear bottom scrolling playfield row, so collision detection works properly

.L0924 ;  var44  =  0

	LDA #0
	STA var44
.L0925 ;  var45  =  0

	LDA #0
	STA var45
.L0926 ;  var46  =  0

	LDA #0
	STA var46
.L0927 ;  var47  =  0

	LDA #0
	STA var47
.L0928 ;  return otherbank

	JMP BS_return
.r00
 ; r00

.L0929 ;  pfcolors:

 lda # skyblue
 sta COLUPF
 lda #>(pfcolorlabel139-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-84)
 sta pfcolortable
.L0930 ;  playfield:

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
.L0931 ;  goto room_draw_end

 jmp .room_draw_end

.r01
 ; r01

.L0932 ;  if quest_flags{1} then COLUBK = gray else COLUBK = black

	LDA quest_flags
	AND #2
	BEQ .skipL0932
.condpart157
	LDA gray
	STA COLUBK
 jmp .skipelse3
.skipL0932
	LDA black
	STA COLUBK
.skipelse3
.L0933 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel139-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-83)
 sta pfcolortable
.L0934 ;  playfield:

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
.L0935 ;  goto room_draw_end

 jmp .room_draw_end

.r02
 ; r02

.L0936 ;  if quest_flags{1} then COLUBK = gray else COLUBK = black

	LDA quest_flags
	AND #2
	BEQ .skipL0936
.condpart158
	LDA gray
	STA COLUBK
 jmp .skipelse4
.skipL0936
	LDA black
	STA COLUBK
.skipelse4
.L0937 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel139-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-82)
 sta pfcolortable
.L0938 ;  playfield:

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
.L0939 ;  goto room_draw_end

 jmp .room_draw_end

.r03
 ; r03

.L0940 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L0941 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel139-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-81)
 sta pfcolortable
.L0942 ;  playfield:

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
.L0943 ;  goto room_draw_end

 jmp .room_draw_end

.r04
 ; r04

.L0944 ;  COLUBK = gray

	LDA gray
	STA COLUBK
.L0945 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel156-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-84)
 sta pfcolortable
.L0946 ;  playfield:

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
.L0947 ;  goto room_draw_end

 jmp .room_draw_end

.r05
 ; r05

.L0948 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0949 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel156-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-83)
 sta pfcolortable
.L0950 ;  playfield:

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
.L0951 ;  goto room_draw_end

 jmp .room_draw_end

.r06
 ; r06

.L0952 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0953 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel156-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-82)
 sta pfcolortable
.L0954 ;  playfield:

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
.L0955 ;  goto room_draw_end

 jmp .room_draw_end

.r07
 ; r07

.L0956 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0957 ;  pfcolors:

 lda # darkgreen
 sta COLUPF
 lda #>(pfcolorlabel156-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-81)
 sta pfcolortable
.L0958 ;  playfield:

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
.L0959 ;  goto room_draw_end

 jmp .room_draw_end

.r08
 ; r08

.L0960 ;  COLUBK = brown

	LDA brown
	STA COLUBK
.L0961 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel173-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-84)
 sta pfcolortable
.L0962 ;  if quest_flags{3} goto r08_open

	LDA quest_flags
	AND #8
 if ((* - .r08_open) < 127) && ((* - .r08_open) > -128)
	BNE .r08_open
 else
	beq .11skipr08_open
	jmp .r08_open
.11skipr08_open
 endif
.L0963 ;  playfield:

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
.L0964 ;  goto r08_end

 jmp .r08_end

.r08_open
 ; r08_open

.L0965 ;  playfield:

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

.L0966 ;  goto room_draw_end

 jmp .room_draw_end

.r09
 ; r09

.L0967 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0968 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel173-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-83)
 sta pfcolortable
.L0969 ;  playfield:

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
.L0970 ;  goto room_draw_end

 jmp .room_draw_end

.r10
 ; r10

.L0971 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0972 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel173-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-82)
 sta pfcolortable
.L0973 ;  playfield:

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
.L0974 ;  goto room_draw_end

 jmp .room_draw_end

.r11
 ; r11

.L0975 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0976 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel173-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-81)
 sta pfcolortable
.L0977 ;  playfield:

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
.L0978 ;  goto room_draw_end

 jmp .room_draw_end

.r12
 ; r12

.L0979 ;  COLUBK = gray

	LDA gray
	STA COLUBK
.L0980 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel190-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-84)
 sta pfcolortable
.L0981 ;  playfield:

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
.L0982 ;  goto room_draw_end

 jmp .room_draw_end

.r13
 ; r13

.L0983 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0984 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-83)
 sta pfcolortable
.L0985 ;  playfield:

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
.L0986 ;  goto room_draw_end

 jmp .room_draw_end

.r14
 ; r14

.L0987 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L0988 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-82)
 sta pfcolortable
.L0989 ;  playfield:

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
.L0990 ;  goto room_draw_end

 jmp .room_draw_end

.r15
 ; r15

.L0991 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-81)
 sta pfcolortable
.L0992 ;  playfield:

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
.L0993 ;  goto room_draw_end

 jmp .room_draw_end

.r16
 ; r16

.L0994 ;  COLUBK = yellow

	LDA yellow
	STA COLUBK
.L0995 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel207-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-84)
 sta pfcolortable
.L0996 ;  playfield:

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
.L0997 ;  goto room_draw_end

 jmp .room_draw_end

.r17
 ; r17

.L0998 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0999 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel207-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-83)
 sta pfcolortable
.L01000 ;  playfield:

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
.L01001 ;  goto room_draw_end

 jmp .room_draw_end

.r18
 ; r18

.L01002 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01003 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel207-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-82)
 sta pfcolortable
.L01004 ;  playfield:

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
.L01005 ;  goto room_draw_end

 jmp .room_draw_end

.r19
 ; r19

.L01006 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01007 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel207-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-81)
 sta pfcolortable
.L01008 ;  playfield:

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
.L01009 ;  goto room_draw_end

 jmp .room_draw_end

.r20
 ; r20

.L01010 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01011 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-84)
 sta pfcolortable
.L01012 ;  playfield:

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
.L01013 ;  goto room_draw_end

 jmp .room_draw_end

.r21
 ; r21

.L01014 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01015 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-83)
 sta pfcolortable
.L01016 ;  playfield:

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
.L01017 ;  goto room_draw_end

 jmp .room_draw_end

.r22
 ; r22

.L01018 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01019 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-82)
 sta pfcolortable
.L01020 ;  playfield:

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
.L01021 ;  goto room_draw_end

 jmp .room_draw_end

.r23
 ; r23

.L01022 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01023 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-81)
 sta pfcolortable
.L01024 ;  playfield:

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
.L01025 ;  goto room_draw_end

 jmp .room_draw_end

.r24
 ; r24

.L01026 ;  COLUBK = yellow

	LDA yellow
	STA COLUBK
.L01027 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel241-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-84)
 sta pfcolortable
.L01028 ;  if quest_flags{5} then goto r24_open

	LDA quest_flags
	AND #32
	BEQ .skipL01028
.condpart159
 jmp .r24_open

.skipL01028
.L01029 ;  playfield:

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
.L01030 ;  goto r24_end

 jmp .r24_end

.r24_open
 ; r24_open

.L01031 ;  playfield:

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

.L01032 ;  goto room_draw_end

 jmp .room_draw_end

.r25
 ; r25

.L01033 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01034 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel241-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-83)
 sta pfcolortable
.L01035 ;  playfield:

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
.L01036 ;  goto room_draw_end

 jmp .room_draw_end

.r26
 ; r26

.L01037 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01038 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel241-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-82)
 sta pfcolortable
.L01039 ;  playfield:

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
.L01040 ;  goto room_draw_end

 jmp .room_draw_end

.r27
 ; r27

.L01041 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01042 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel241-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-81)
 sta pfcolortable
.L01043 ;  playfield:

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
.L01044 ;  goto room_draw_end

 jmp .room_draw_end

.r28
 ; r28

.L01045 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01046 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-84)
 sta pfcolortable
.L01047 ;  playfield:

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
.L01048 ;  goto room_draw_end

 jmp .room_draw_end

.r29
 ; r29

.L01049 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01050 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-83)
 sta pfcolortable
.L01051 ;  if quest_flags{6} then goto r29_open

	BIT quest_flags
	BVC .skipL01051
.condpart160
 jmp .r29_open

.skipL01051
.L01052 ;  if item_flags{3} then goto r29_open

	LDA item_flags
	AND #8
	BEQ .skipL01052
.condpart161
 jmp .r29_open

.skipL01052
.L01053 ;  playfield:

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
.L01054 ;  goto room_draw_end

 jmp .room_draw_end

.r29_open
 ; r29_open

.L01055 ;  playfield:

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
.L01056 ;  goto room_draw_end

 jmp .room_draw_end

.r30
 ; r30

.L01057 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-82)
 sta pfcolortable
.L01058 ;  playfield:

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
.L01059 ;  goto room_draw_end

 jmp .room_draw_end

.r31
 ; r31

.L01060 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-81)
 sta pfcolortable
.L01061 ;  playfield:

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
.L01062 ;  goto room_draw_end

 jmp .room_draw_end

.r32
 ; r32

.L01063 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01064 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-84)
 sta pfcolortable
.L01065 ;  if quest_flags{2} then goto r32_open

	LDA quest_flags
	AND #4
	BEQ .skipL01065
.condpart162
 jmp .r32_open

.skipL01065
.L01066 ;  playfield:

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
.L01067 ;  goto room_draw_end bank2

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

.L01068 ;  playfield:

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
.r33
 ; r33

.L01070 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-83)
 sta pfcolortable
.L01071 ;  playfield:

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
.L01072 ;  goto room_draw_end bank2

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

.L01073 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01074 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-82)
 sta pfcolortable
.L01075 ;  playfield:

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
.L01076 ;  goto room_draw_end bank2

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

.L01077 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01078 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel275-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-81)
 sta pfcolortable
.L01079 ;  if quest_flags{4} then goto r35_open

	LDA quest_flags
	AND #16
	BEQ .skipL01079
.condpart163
 jmp .r35_open

.skipL01079
.L01080 ;  playfield:

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
.L01081 ;  goto room_draw_end bank2

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

.L01082 ;  playfield:

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
.L01083 ;  goto room_draw_end bank2

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

.L01084 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01085 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-84)
 sta pfcolortable
.L01086 ;  playfield:

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
.L01087 ;  goto room_draw_end bank2

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

.L01088 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01089 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-83)
 sta pfcolortable
.L01090 ;  playfield:

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
.L01091 ;  goto room_draw_end bank2

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

.L01092 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01093 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel292-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-82)
 sta pfcolortable
.L01094 ;  playfield:

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
.L01095 ;  goto room_draw_end bank2

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

.L01096 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01097 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-81)
 sta pfcolortable
.L01098 ;  playfield:

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
.L01099 ;  goto room_draw_end bank2

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
.L01100 ;  bank 3

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

.L01101 ;  temp1 = hero_room - 40

	LDA hero_room
	SEC
	SBC #40
	STA temp1
.L01102 ;  on temp1 goto r40 r41 r42 r43 r44 r45 r46 r47 r48 r49 r50 r51 r52 r53 r54 r55 r56 r57 r58 r59 r60 r61 r62 r63

	LDX temp1
	LDA .L01102jumptablehi,x
	PHA
	LDA .L01102jumptablelo,x
	PHA
	RTS
.L01102jumptablehi
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
.L01102jumptablelo
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
.r40
 ; r40

.L01104 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel309-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-84)
 sta pfcolortable
.L01105 ;  playfield:

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
.L01106 ;  goto room_draw_end bank2

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

.L01107 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01108 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel309-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-83)
 sta pfcolortable
.L01109 ;  playfield:

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
.L01110 ;  goto room_draw_end bank2

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

.L01111 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01112 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel309-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-82)
 sta pfcolortable
.L01113 ;  playfield:

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
.L01114 ;  goto room_draw_end bank2

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

.L01115 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01116 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel309-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-81)
 sta pfcolortable
.L01117 ;  playfield:

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
.L01118 ;  goto room_draw_end bank2

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

.L01119 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01120 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel326-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-84)
 sta pfcolortable
.L01121 ;  playfield:

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
.L01122 ;  goto room_draw_end bank2

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

.L01123 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01124 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel326-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-83)
 sta pfcolortable
.L01125 ;  playfield:

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
.L01126 ;  goto room_draw_end bank2

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

.L01127 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01128 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel326-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-82)
 sta pfcolortable
.L01129 ;  playfield:

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
.r47
 ; r47

.L01131 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel326-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-81)
 sta pfcolortable
.L01132 ;  playfield:

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
.L01133 ;  goto room_draw_end bank2

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

.L01134 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01135 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel343-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-84)
 sta pfcolortable
.L01136 ;  playfield:

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
.L01137 ;  goto room_draw_end bank2

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

.L01138 ;  COLUBK = turfy

	LDA #turfy
	STA COLUBK
.L01139 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel343-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-83)
 sta pfcolortable
.L01140 ;  playfield:

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
.L01141 ;  COLUPF = red

	LDA red
	STA COLUPF
.L01142 ;  goto room_draw_end bank2

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

.L01143 ;  COLUBK  =  sand  +  0

	LDA #sand
	CLC
	ADC #0
	STA COLUBK
.L01144 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel343-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-82)
 sta pfcolortable
.L01145 ;  playfield:

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
.L01146 ;  goto room_draw_end bank2

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

.L01147 ;  rem 'black ship top

.L01148 ;  COLUBK  =  144  +  2

	LDA #144
	CLC
	ADC #2
	STA COLUBK
.L01149 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel343-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-81)
 sta pfcolortable
.L01150 ;  playfield:

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
.L01151 ;  goto room_draw_end bank2

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

.L01152 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01153 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel360-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-84)
 sta pfcolortable
.L01154 ;  playfield:

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
.L01155 ;  goto room_draw_end bank2

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

.L01156 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01157 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel360-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-83)
 sta pfcolortable
.L01158 ;  playfield:

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
.L01159 ;  goto room_draw_end bank2

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

.L01160 ;  COLUBK  =  turfy

	LDA #turfy
	STA COLUBK
.L01161 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel360-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-82)
 sta pfcolortable
.L01162 ;  if quest_flags{3} then goto r54_open

	LDA quest_flags
	AND #8
	BEQ .skipL01162
.condpart164
 jmp .r54_open

.skipL01162
.L01163 ;  playfield:

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
.L01164 ;  goto room_draw_end bank2

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

.L01165 ;  playfield:

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
.L01166 ;  goto room_draw_end bank2

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

.L01167 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01168 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel360-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-81)
 sta pfcolortable
.L01169 ;  if quest_flags{7} then goto r55_open

	BIT quest_flags
	BPL .skipL01169
.condpart165
 jmp .r55_open

.skipL01169
.L01170 ;  playfield:

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
.L01171 ;  goto room_draw_end bank2

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

.L01172 ;  playfield:

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
.L01173 ;  goto room_draw_end bank2

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

.L01174 ;  rem 'Shield on island

.L01175 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01176 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel377-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-84)
 sta pfcolortable
.L01177 ;  if !quest_flags{6} then goto r56_closed

	BIT quest_flags
	BVS .skipL01177
.condpart166
 jmp .r56_closed

.skipL01177
.L01178 ;  playfield:

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
.L01179 ;  goto room_draw_end bank2

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

.L01180 ;  playfield:

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

.L01181 ;  goto room_draw_end bank2

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

.L01182 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01183 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel377-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-83)
 sta pfcolortable
.L01184 ;  playfield:

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
.L01185 ;  goto room_draw_end bank2

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

.L01186 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01187 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel377-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-82)
 sta pfcolortable
.L01188 ;  if quest_flags{2} goto r58_open

	LDA quest_flags
	AND #4
 if ((* - .r58_open) < 127) && ((* - .r58_open) > -128)
	BNE .r58_open
 else
	beq .12skipr58_open
	jmp .r58_open
.12skipr58_open
 endif
.L01189 ;  playfield:

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
.L01190 ;  goto room_draw_end bank2

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

.L01191 ;  playfield:

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
.L01192 ;  goto room_draw_end bank2

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

.L01193 ;  COLUBK  =  144  +  1

	LDA #144
	CLC
	ADC #1
	STA COLUBK
.L01194 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel377-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-81)
 sta pfcolortable
.L01195 ;  rem 'black ship bottom, when switch3 isn't pressed

.L01196 ;  if quest_flags{3} goto black_ship_open

	LDA quest_flags
	AND #8
 if ((* - .black_ship_open) < 127) && ((* - .black_ship_open) > -128)
	BNE .black_ship_open
 else
	beq .13skipblack_ship_open
	jmp .black_ship_open
.13skipblack_ship_open
 endif
.L01197 ;  playfield:

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
.L01198 ;  goto black_ship_end

 jmp .black_ship_end

.black_ship_open
 ; black_ship_open

.L01199 ;  playfield:

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

.L01200 ;  goto room_draw_end bank2

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

.L01201 ;  rem 'Boss 3, Kraken room.

.L01202 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L01203 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel394-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-84)
 sta pfcolortable
.L01204 ;  playfield:

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
.L01205 ;  goto room_draw_end bank2

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

.L01206 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01207 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel394-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-83)
 sta pfcolortable
.L01208 ;  if !hero_items{5} then goto r61_closed

	LDA hero_items
	AND #32
	BNE .skipL01208
.condpart167
 jmp .r61_closed

.skipL01208
.L01209 ;  playfield:

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
.L01210 ;  goto room_draw_end bank2

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

.L01211 ;  playfield:

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
.L01212 ;  goto room_draw_end bank2

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

.L01213 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01214 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel394-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-82)
 sta pfcolortable
.L01215 ;  playfield:

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
.L01216 ;  goto room_draw_end bank2

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

.L01217 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01218 ;  pfcolors:

 lda # 144
 sta COLUPF
 lda #>(pfcolorlabel394-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-81)
 sta pfcolortable
.L01219 ;  playfield:

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
.L01220 ;  goto room_draw_end bank2

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
.L01221 ;  bank 4

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
.L01222 ;  rem 'Item sprites as well as initial data for item and sprites are in bank 4

.L01223 ;  rem 'Items/monsters/mobiles in any given room

.L01224 ;  rem '8 items per line means it corresponds with map layout

.L01225 ;  data room_items

	JMP .skipL01225
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

.skipL01225
.L01226 ;  data item_hplist

	JMP .skipL01226
item_hplist
	.byte     4, 4, 8, 8, 8, 16, 16, 16, 16, 32, 32, 32, 32, 64, 32, 32

	.byte    64, 80,100, 4, 64, 32, 64, 64, 64, 80, 80, 80, 80,100,100,120

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

	.byte   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4

.skipL01226
.L01227 ;  rem 'Shorthand for the center of screen positions.

.L01228 ;  const hc = 73

.L01229 ;  const vc = 49

.L01230 ;  data item_xlist

	JMP .skipL01230
item_xlist
	.byte   hc, hc, 84, 37,109, 70, hc - 8, hc 

	.byte   hc, hc, hc, 37, hc, hc, hc, hc 

	.byte   hc,113,113, 37, hc + 12, hc, hc, hc 

	.byte   hc, hc, 63,113, hc,120, hc, hc 

	.byte   hc, 37, 90, 37, 53, 53, hc, hc 

	.byte   77, hc, hc,123, 66, hc, hc, 61 

	.byte   hc, hc, hc,113, hc, hc, hc, hc 

	.byte   37, hc, 37, 37, hc, 37, hc, hc 

.skipL01230
.L01231 ;  data item_ylist

	JMP .skipL01231
item_ylist
	.byte   vc, vc, vc, 25, 70, 65, vc, vc 

	.byte   vc, vc, vc, vc, vc, vc, vc, vc + 4 

	.byte   vc, vc, vc, vc, vc + 1, vc, vc, vc + 8 

	.byte   vc, vc, 70, vc, vc + 8, vc, vc, vc - 16 

	.byte   vc, 29, 70, vc, 41, 41, vc, vc 

	.byte   33, vc, vc, vc, 70, 25, vc, vc 

	.byte   vc, 55, vc, 80, vc, vc, vc, vc 

	.byte   73, vc, vc, vc, vc, 73, vc, vc 

.skipL01231
.item_setup
 ; item_setup

.L01232 ;  item_kind  =  room_items[hero_room]

	LDX hero_room
	LDA room_items,x
	STA item_kind
.L01233 ;  gosub item_setup_kind

 jsr .item_setup_kind

.L01234 ;  goto item_setup_done bank2

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
.L01235 ;  rem 'You must set item_kind before gosubbing to this one  

.item_setup_kind
 ; item_setup_kind

.L01236 ;  rem 'Initialize item/mobile, and it's position, HP and speed

.L01237 ;  rem 'First, handle switches

.L01238 ;  if item_kind  <  item_switch1 then goto item_switch_end

	LDA item_kind
	CMP #item_switch1
     BCS .skipL01238
.condpart168
 jmp .item_switch_end

.skipL01238
.L01239 ;  if item_kind  >  item_switch7 then goto item_switch_end

	LDA #item_switch7
	CMP item_kind
     BCS .skipL01239
.condpart169
 jmp .item_switch_end

.skipL01239
.L01240 ;  rem 'set switch to on if needed

.L01241 ;  if quest_flags{1}  &&  item_kind  =  item_switch1 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #2
	BEQ .skipL01241
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
.skipL01241
.L01242 ;  if quest_flags{2}  &&  item_kind  =  item_switch2 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #4
	BEQ .skipL01242
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
.skipL01242
.L01243 ;  if quest_flags{3}  &&  item_kind  =  item_switch3 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #8
	BEQ .skipL01243
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
.skipL01243
.L01244 ;  if quest_flags{4}  &&  item_kind  =  item_switch4 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #16
	BEQ .skipL01244
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
.skipL01244
.L01245 ;  if quest_flags{5}  &&  item_kind  =  item_switch5 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #32
	BEQ .skipL01245
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
.skipL01245
.L01246 ;  if quest_flags{6}  &&  item_kind  =  item_switch6 then item_kind  =  item_kind  +  item_switch_on

	BIT quest_flags
	BVC .skipL01246
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
.skipL01246
.L01247 ;  if quest_flags{7}  &&  item_kind  =  item_switch7 then item_kind  =  item_kind  +  item_switch_on

	BIT quest_flags
	BPL .skipL01247
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
.skipL01247
.item_switch_end
 ; item_switch_end

.L01248 ;  rem 'Next, handle bosses that may already be dead.

.L01249 ;  rem 'Ikaza

.L01250 ;  if item_kind  <  monster_leaf1_boss then goto item_boss_end

	LDA item_kind
	CMP #monster_leaf1_boss
     BCS .skipL01250
.condpart184
 jmp .item_boss_end

.skipL01250
.L01251 ;  if item_kind  >  monster_ikaza then goto item_boss_end

	LDA #monster_ikaza
	CMP item_kind
     BCS .skipL01251
.condpart185
 jmp .item_boss_end

.skipL01251
.L01252 ;  if quest_flags{0}  &&  item_kind  =  monster_ikaza then item_kind  =  item_none

	LDA quest_flags
	LSR
	BCC .skipL01252
.condpart186
	LDA item_kind
	CMP #monster_ikaza
     BNE .skip186then
.condpart187
	LDA #item_none
	STA item_kind
.skip186then
.skipL01252
.L01253 ;  rem 'Leaf 1 dropped by boss 1

.L01254 ;  if hero_items{0}  &&  item_kind  =  monster_leaf1_boss then item_kind  =  item_none

	LDA hero_items
	LSR
	BCC .skipL01254
.condpart188
	LDA item_kind
	CMP #monster_leaf1_boss
     BNE .skip188then
.condpart189
	LDA #item_none
	STA item_kind
.skip188then
.skipL01254
.L01255 ;  rem 'Leaf 2 dropped by boss 2

.L01256 ;  if hero_items{1}  &&  item_kind  =  monster_leaf2_boss then item_kind  =  item_none

	LDA hero_items
	AND #2
	BEQ .skipL01256
.condpart190
	LDA item_kind
	CMP #monster_leaf2_boss
     BNE .skip190then
.condpart191
	LDA #item_none
	STA item_kind
.skip190then
.skipL01256
.L01257 ;  rem 'Leaf 3 dropped by boss 3

.L01258 ;  if hero_items{2}  &&  item_kind  =  monster_leaf3_boss then item_kind  =  item_none

	LDA hero_items
	AND #4
	BEQ .skipL01258
.condpart192
	LDA item_kind
	CMP #monster_leaf3_boss
     BNE .skip192then
.condpart193
	LDA #item_none
	STA item_kind
.skip192then
.skipL01258
.L01259 ;  rem 'Armor Dropped by armor boss

.L01260 ;  if hero_items{4}  &&  item_kind  =  monster_armor_boss then item_kind  =  item_none

	LDA hero_items
	AND #16
	BEQ .skipL01260
.condpart194
	LDA item_kind
	CMP #monster_armor_boss
     BNE .skip194then
.condpart195
	LDA #item_none
	STA item_kind
.skip194then
.skipL01260
.L01261 ;  rem 'Sword dropped by sword boss

.L01262 ;  if hero_items{5}  &&  item_kind  =  monster_sword_boss then item_kind  =  item_none

	LDA hero_items
	AND #32
	BEQ .skipL01262
.condpart196
	LDA item_kind
	CMP #monster_sword_boss
     BNE .skip196then
.condpart197
	LDA #item_none
	STA item_kind
.skip196then
.skipL01262
.L01263 ;  rem 'Strike book dropped by strike book boss

.L01264 ;  if hero_items{7}  &&  item_kind  =  monster_strike_boss then item_kind  =  item_none

	BIT hero_items
	BPL .skipL01264
.condpart198
	LDA item_kind
	CMP #monster_strike_boss
     BNE .skip198then
.condpart199
	LDA #item_none
	STA item_kind
.skip198then
.skipL01264
.L01265 ;  rem 'Heal book and shield are not dropped by a boss, but just found

.item_boss_end
 ; item_boss_end

.L01266 ;  rem 'Finally handle the case of the shield and the heal book that should drop only once

.L01267 ;  if hero_items{3}  &&  item_kind  =  item_shield then item_kind  =  item_none

	LDA hero_items
	AND #8
	BEQ .skipL01267
.condpart200
	LDA item_kind
	CMP #item_shield
     BNE .skip200then
.condpart201
	LDA #item_none
	STA item_kind
.skip200then
.skipL01267
.L01268 ;  if hero_items{6}  &&  item_kind  =  item_bookheal then item_kind  =  item_none

	BIT hero_items
	BVC .skipL01268
.condpart202
	LDA item_kind
	CMP #item_bookheal
     BNE .skip202then
.condpart203
	LDA #item_none
	STA item_kind
.skip202then
.skipL01268
.L01269 ;  temp1  =  item_kind_mask  &  item_kind

	LDA #item_kind_mask
	AND item_kind
	STA temp1
.L01270 ;  item_hp  =  item_hplist[temp1]

	LDX temp1
	LDA item_hplist,x
	STA item_hp
.L01271 ;  if temp1  >  31 goto item_draw_32

	LDA #31
	CMP temp1
 if ((* - .item_draw_32) < 127) && ((* - .item_draw_32) > -128)
	bcc .item_draw_32
 else
	bcs .14skipitem_draw_32
	jmp .item_draw_32
.14skipitem_draw_32
 endif
.L01272 ;  on temp1 goto i00 i01 i02 i03 i04 i05 i06 i07 i08 i09 i10 i11 i12 i13 i14 i15 i16 i17 i18 i19 i20 i21 i22 i23 i24 i25 i26 i27 i28 i29 i30 i31

	LDX temp1
	LDA .L01272jumptablehi,x
	PHA
	LDA .L01272jumptablelo,x
	PHA
	RTS
.L01272jumptablehi
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
.L01272jumptablelo
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
.L01273 ;  goto item_draw_done

 jmp .item_draw_done

.item_draw_32
 ; item_draw_32

.L01274 ;  temp1  =  temp1  -  32

	LDA temp1
	SEC
	SBC #32
	STA temp1
.L01275 ;  on temp1 goto i32 i33 i34 i35 i36 i37 i38 i39 i40 i41 i42 i43 i44 i45 i46 i47 i48 i49 i50 i51 i52 i53 i54 i55 i56 i57 i58 i59 i60 i61 i62 i63

	LDX temp1
	LDA .L01275jumptablehi,x
	PHA
	LDA .L01275jumptablelo,x
	PHA
	RTS
.L01275jumptablehi
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
.L01275jumptablelo
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

.L01276 ;  if item_kind  <>  item_none goto item_not_none

	LDA item_kind
	CMP #item_none
 if ((* - .item_not_none) < 127) && ((* - .item_not_none) > -128)
	BNE .item_not_none
 else
	beq .15skipitem_not_none
	jmp .item_not_none
.15skipitem_not_none
 endif
.L01277 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L01278 ;  item_oldx  =  nowhere

	LDA #nowhere
	STA item_oldx
.L01279 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
.L01280 ;  item_oldy  =  nowhere

	LDA #nowhere
	STA item_oldy
.L01281 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.item_not_none
 ; item_not_none

.L01282 ;  rem temp1 = item_kind & item_kind_mask

.L01283 ;  rem 'look up location in table.

.L01284 ;  item_oldx  =  item_xlist[hero_room]

	LDX hero_room
	LDA item_xlist,x
	STA item_oldx
.L01285 ;  item_x  =  item_xlist[hero_room]

	LDX hero_room
	LDA item_xlist,x
	STA item_x
.L01286 ;  item_oldy  =  item_ylist[hero_room]

	LDX hero_room
	LDA item_ylist,x
	STA item_oldy
.L01287 ;  item_y  =  item_ylist[hero_room]

	LDX hero_room
	LDA item_ylist,x
	STA item_y
.L01288 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.i00
 ; i00

.L01289 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01290 ;  player1:

	LDA #<playerL01290_1

	STA player1pointerlo
	LDA #>playerL01290_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01291 ;  goto item_draw_done

 jmp .item_draw_done

.i01
 ; i01

.L01292 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01293 ;  player1:

	LDA #<playerL01293_1

	STA player1pointerlo
	LDA #>playerL01293_1

	STA player1pointerhi
	LDA #5
	STA player1height
.L01294 ;  goto item_draw_done

 jmp .item_draw_done

.i02
 ; i02

.L01295 ;  COLUP1 = red

	LDA red
	STA COLUP1
.L01296 ;  player1:

	LDA #<playerL01296_1

	STA player1pointerlo
	LDA #>playerL01296_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01297 ;  goto item_draw_done

 jmp .item_draw_done

.i03
 ; i03

.L01298 ;  rem 'bat.xpm

.L01299 ;  player1:

	LDA #<playerL01299_1

	STA player1pointerlo
	LDA #>playerL01299_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01300 ;  goto item_draw_done

 jmp .item_draw_done

.i04
 ; i04

.L01301 ;  rem 'scorpio.xpm

.L01302 ;  player1:

	LDA #<playerL01302_1

	STA player1pointerlo
	LDA #>playerL01302_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01303 ;  goto item_draw_done

 jmp .item_draw_done

.i05
 ; i05

.L01304 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01305 ;  rem 'rabid.xpm

.L01306 ;  player1:

	LDA #<playerL01306_1

	STA player1pointerlo
	LDA #>playerL01306_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01307 ;  goto item_draw_done

 jmp .item_draw_done

.i06
 ; i06

.L01308 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01309 ;  rem 'spider.xpm

.L01310 ;  player1:

	LDA #<playerL01310_1

	STA player1pointerlo
	LDA #>playerL01310_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01311 ;  goto item_draw_done

 jmp .item_draw_done

.i07
 ; i07

.L01312 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01313 ;  rem 'snake.xpm

.L01314 ;  player1:

	LDA #<playerL01314_1

	STA player1pointerlo
	LDA #>playerL01314_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01315 ;  goto item_draw_done

 jmp .item_draw_done

.i08
 ; i08

.L01316 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01317 ;  rem 'fish.xpm

.L01318 ;  player1:

	LDA #<playerL01318_1

	STA player1pointerlo
	LDA #>playerL01318_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01319 ;  goto item_draw_done

 jmp .item_draw_done

.i09
 ; i09

.L01320 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01321 ;  rem 'lion.xpm

.L01322 ;  player1:

	LDA #<playerL01322_1

	STA player1pointerlo
	LDA #>playerL01322_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01323 ;  goto item_draw_done

 jmp .item_draw_done

.i10
 ; i10

.L01324 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01325 ;  rem 'wolf.xpm

.L01326 ;  player1:

	LDA #<playerL01326_1

	STA player1pointerlo
	LDA #>playerL01326_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01327 ;  goto item_draw_done

 jmp .item_draw_done

.i11
 ; i11

.L01328 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01329 ;  rem 'captain.xpm, used for monster_grunt. (captain and grunt were doubles)

.L01330 ;  player1:

	LDA #<playerL01330_1

	STA player1pointerlo
	LDA #>playerL01330_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01331 ;  goto item_draw_done

 jmp .item_draw_done

.i12
 ; i12

.L01332 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01333 ;  rem 'archer.xpm

.L01334 ;  player1:

	LDA #<playerL01334_1

	STA player1pointerlo
	LDA #>playerL01334_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01335 ;  goto item_draw_done

 jmp .item_draw_done

.i13
 ; i13

.L01336 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01337 ;  rem 'knight.xpm

.L01338 ;  player1:

	LDA #<playerL01338_1

	STA player1pointerlo
	LDA #>playerL01338_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01339 ;  goto item_draw_done

 jmp .item_draw_done

.i14
 ; i14

.L01340 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01341 ;  rem 'cannon.xpm

.L01342 ;  player1:

	LDA #<playerL01342_1

	STA player1pointerlo
	LDA #>playerL01342_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01343 ;  goto item_draw_done

 jmp .item_draw_done

.i15
 ; i15

.L01344 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01345 ;  rem 'zombie.xpm

.L01346 ;  player1:

	LDA #<playerL01346_1

	STA player1pointerlo
	LDA #>playerL01346_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01347 ;  goto item_draw_done

 jmp .item_draw_done

.i16
 ; i16

.L01348 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01349 ;  rem 'skeleton.xpm

.L01350 ;  player1:

	LDA #<playerL01350_1

	STA player1pointerlo
	LDA #>playerL01350_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01351 ;  goto item_draw_done

 jmp .item_draw_done

.i17
 ; i17

.L01352 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01353 ;  rem 'ghost.xpm

.L01354 ;  player1:

	LDA #<playerL01354_1

	STA player1pointerlo
	LDA #>playerL01354_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01355 ;  goto item_draw_done

 jmp .item_draw_done

.i18
 ; i18

.L01356 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01357 ;  rem 'bitmap/mage.xpm

.L01358 ;  player1:

	LDA #<playerL01358_1

	STA player1pointerlo
	LDA #>playerL01358_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01359 ;  goto item_draw_done

 jmp .item_draw_done

.i19
 ; i19

.L01360 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01361 ;  rem 'flower.xpm

.L01362 ;  player1:

	LDA #<playerL01362_1

	STA player1pointerlo
	LDA #>playerL01362_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01363 ;  goto item_draw_done

 jmp .item_draw_done

.i20
 ; i20

.L01364 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01365 ;  rem 'treant.xpm

.L01366 ;  player1:

	LDA #<playerL01366_1

	STA player1pointerlo
	LDA #>playerL01366_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01367 ;  goto item_draw_done

 jmp .item_draw_done

.i21
 ; i21

.L01368 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01369 ;  rem 'muschroom.xpm

.L01370 ;  player1:

	LDA #<playerL01370_1

	STA player1pointerlo
	LDA #>playerL01370_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01371 ;  goto item_draw_done

 jmp .item_draw_done

.i22
 ; i22

.L01372 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01373 ;  rem 'book.xpm

.L01374 ;  player1:

	LDA #<playerL01374_1

	STA player1pointerlo
	LDA #>playerL01374_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01375 ;  goto item_draw_done

 jmp .item_draw_done

.i23
 ; i23

.L01376 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01377 ;  rem 'sword.xpm. It's a fake Numen Sword that hurts the one who touches it.

.L01378 ;  player1:

	LDA #<playerL01378_1

	STA player1pointerlo
	LDA #>playerL01378_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01379 ;  goto item_draw_done

 jmp .item_draw_done

.i24
 ; i24

.L01380 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01381 ;  rem 'Curse, entrance blocking item

.L01382 ;  player1:

	LDA #<playerL01382_1

	STA player1pointerlo
	LDA #>playerL01382_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01383 ;  goto item_draw_done

 jmp .item_draw_done

.i25
 ; i25

.L01384 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01385 ;  rem 'boss1.xpm

.L01386 ;  player1:

	LDA #<playerL01386_1

	STA player1pointerlo
	LDA #>playerL01386_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01387 ;  goto item_draw_done

 jmp .item_draw_done

.i26
 ; i26

.L01388 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01389 ;  rem 'boss2.xpm

.L01390 ;  player1:

	LDA #<playerL01390_1

	STA player1pointerlo
	LDA #>playerL01390_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01391 ;  goto item_draw_done

 jmp .item_draw_done

.i27
 ; i27

.L01392 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01393 ;  rem 'boss3.xpm

.L01394 ;  player1:

	LDA #<playerL01394_1

	STA player1pointerlo
	LDA #>playerL01394_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01395 ;  goto item_draw_done

 jmp .item_draw_done

.i28
 ; i28

.L01396 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01397 ;  rem 'armorboss.xpm

.L01398 ;  player1:

	LDA #<playerL01398_1

	STA player1pointerlo
	LDA #>playerL01398_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01399 ;  goto item_draw_done

 jmp .item_draw_done

.i29
 ; i29

.L01400 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01401 ;  rem 'Sword boss is a player frame 

.L01402 ;  player1:

	LDA #<playerL01402_1

	STA player1pointerlo
	LDA #>playerL01402_1

	STA player1pointerhi
	LDA #13
	STA player1height
.L01403 ;  goto item_draw_done

 jmp .item_draw_done

.i30
 ; i30

.L01404 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01405 ;  rem 'Strike boss is a one point sprite which is difficult to hit

.L01406 ;  player1:

	LDA #<playerL01406_1

	STA player1pointerlo
	LDA #>playerL01406_1

	STA player1pointerhi
	LDA #1
	STA player1height
.L01407 ;  goto item_draw_done

 jmp .item_draw_done

.i31
 ; i31

.L01408 ;  rem 'ikaza.xpm

.L01409 ;  rem 'ikaza.xpm

.L01410 ;  player1:

	LDA #<playerL01410_1

	STA player1pointerlo
	LDA #>playerL01410_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01411 ;  goto item_draw_done

 jmp .item_draw_done

.i32
 ; i32

.i33
 ; i33

.i34
 ; i34

.L01412 ;  rem 'leaf.xpm

.L01413 ;  player1:

	LDA #<playerL01413_1

	STA player1pointerlo
	LDA #>playerL01413_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01414 ;  goto item_draw_done

 jmp .item_draw_done

.i35
 ; i35

.L01415 ;  rem 'armor.xpm

.L01416 ;  player1:

	LDA #<playerL01416_1

	STA player1pointerlo
	LDA #>playerL01416_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01417 ;  goto item_draw_done

 jmp .item_draw_done

.i36
 ; i36

.L01418 ;  rem 'shield.xpm

.L01419 ;  player1:

	LDA #<playerL01419_1

	STA player1pointerlo
	LDA #>playerL01419_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01420 ;  goto item_draw_done

 jmp .item_draw_done

.i37
 ; i37

.L01421 ;  rem 'sword.xpm

.L01422 ;  player1:

	LDA #<playerL01422_1

	STA player1pointerlo
	LDA #>playerL01422_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01423 ;  goto item_draw_done

 jmp .item_draw_done

.i38
 ; i38

.L01424 ;  rem 'healbook.xpm

.L01425 ;  player1:

	LDA #<playerL01425_1

	STA player1pointerlo
	LDA #>playerL01425_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01426 ;  goto item_draw_done

 jmp .item_draw_done

.i39
 ; i39

.L01427 ;  rem 'strikebook.xpm

.L01428 ;  player1:

	LDA #<playerL01428_1

	STA player1pointerlo
	LDA #>playerL01428_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01429 ;  goto item_draw_done

 jmp .item_draw_done

.i40
 ; i40

.L01430 ;  rem 'healhp.xpm

.L01431 ;  player1:

	LDA #<playerL01431_1

	STA player1pointerlo
	LDA #>playerL01431_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01432 ;  goto item_draw_done

 jmp .item_draw_done

.i41
 ; i41

.L01433 ;  rem 'healmp.xpm

.L01434 ;  player1:

	LDA #<playerL01434_1

	STA player1pointerlo
	LDA #>playerL01434_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01435 ;  goto item_draw_done

 jmp .item_draw_done

.i42
 ; i42

.L01436 ;  rem 'healallhp.xpm

.L01437 ;  player1:

	LDA #<playerL01437_1

	STA player1pointerlo
	LDA #>playerL01437_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01438 ;  goto item_draw_done

 jmp .item_draw_done

.i43
 ; i43

.L01439 ;  rem 'healallmp.xpm

.L01440 ;  player1:

	LDA #<playerL01440_1

	STA player1pointerlo
	LDA #>playerL01440_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01441 ;  goto item_draw_done

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

.L01442 ;  rem 'bitmap/switch.xpm

.L01443 ;  player1:

	LDA #<playerL01443_1

	STA player1pointerlo
	LDA #>playerL01443_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01444 ;  goto item_draw_done

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

.L01445 ;  rem 'bitmap/switch_on.xpm

.L01446 ;  player1:

	LDA #<playerL01446_1

	STA player1pointerlo
	LDA #>playerL01446_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01447 ;  goto item_draw_done

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

.L01448 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01449 ;  player1:

	LDA #<playerL01449_1

	STA player1pointerlo
	LDA #>playerL01449_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01450 ;  goto item_draw_done

 jmp .item_draw_done

.i63
 ; i63

.L01451 ;  COLUP1 = pink

	LDA pink
	STA COLUP1
.L01452 ;  player1:

	LDA #<playerL01452_1

	STA player1pointerlo
	LDA #>playerL01452_1

	STA player1pointerhi
	LDA #13
	STA player1height
.L01453 ;  goto item_draw_done

 jmp .item_draw_done

.L01454 ;  bank 5

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
.L01455 ;  rem ' Bank 5 contains the game over, game win and game intro, screen

.L01456 ;  rem 'as well as a music playing routine for them.

.game_over
 ; game_over

.L01457 ;  gosub music_restart

 jsr .music_restart

.L01458 ;  music_which = 1

	LDA #1
	STA music_which
.L01459 ;  COLUBK  =  red

	LDA red
	STA COLUBK
.L01460 ;  hero_x = 70

	LDA #70
	STA hero_x
.L01461 ;  hero_y = 80

	LDA #80
	STA hero_y
.L01462 ;  player0:

	LDA #<playerL01462_0

	STA player0pointerlo
	LDA #>playerL01462_0

	STA player0pointerhi
	LDA #59
	STA player0height
.game_over_loop
 ; game_over_loop

.L01463 ;  rem 'Teleport to Sygne's home on reset button, with some losses

.L01464 ;  if !switchreset then goto reset_go_end

 lda #1
 bit SWCHB
	BEQ .skipL01464
.condpart204
 jmp .reset_go_end

.skipL01464
.L01465 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L01466 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L01467 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L01468 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L01469 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L01470 ;  hero_hp  =  hero_level  +  hero_base_hp

	LDA hero_level
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L01471 ;  hero_next  =  hero_level

	LDA hero_level
	STA hero_next
.L01472 ;  hero_mp  =  0

	LDA #0
	STA hero_mp
.L01473 ;  rem ' Restart the game with stats somewhat reduced, 

.L01474 ;  gosub music_restart

 jsr .music_restart

.L01475 ;  gosub set_mp_display bank1

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
.L01476 ;  gosub hero_draw_s bank1

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
.L01477 ;  gosub room_draw bank2

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
.L01478 ;  goto main_loop bank1

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

.L01479 ;  COLUP0 = white

	LDA white
	STA COLUP0
.L01480 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01481 ;  rem 'Make monster look x3 size, to maintain boss size and suggest something for non bosses.  

.L01482 ;  NUSIZ1  =  $07

	LDA #$07
	STA NUSIZ1
.special_effects_go_end
 ; special_effects_go_end

.L01483 ;  gosub music_play

 jsr .music_play

.L01484 ;  drawscreen

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
.L01485 ;  goto game_over_loop

 jmp .game_over_loop

.L01486 ;  data music_data

	JMP .skipL01486
music_data
	.byte   24,30,27,30,30,30,27,30,24,28,-1,2,24,28,-1,2,24,60

	.byte   27,28,-1,2,27,28,-1,2,27,60,24,30,20,28,-1,2,20,60

	.byte   24,30,27,30,30,30,27,30,24,28,-1,2,24,28,-1,2,24,28,-1,2

	.byte   24,30,27,28,-1,2,27,30,24,30,27,30,30,120

.skipL01486
.game_win
 ; game_win

.L01487 ;  gosub music_restart

 jsr .music_restart

.L01488 ;  music_which = 2

	LDA #2
	STA music_which
.L01489 ;  COLUBK  =  white

	LDA white
	STA COLUBK
.L01490 ;  COLUPF  =  yellow

	LDA yellow
	STA COLUPF
.L01491 ;  item_x = 80

	LDA #80
	STA item_x
.L01492 ;  item_y = 80

	LDA #80
	STA item_y
.L01493 ;  hero_x = 84

	LDA #84
	STA hero_x
.L01494 ;  hero_y = 80

	LDA #80
	STA hero_y
.L01495 ;  player1:

	LDA #<playerL01495_1

	STA player1pointerlo
	LDA #>playerL01495_1

	STA player1pointerhi
	LDA #73
	STA player1height
.game_win_loop
 ; game_win_loop

.L01496 ;  COLUBK  =  white

	LDA white
	STA COLUBK
.L01497 ;  COLUP1  =  rand

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
.L01498 ;  COLUP0  =  rand

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
.L01499 ;  REFP0  =  8

	LDA #8
	STA REFP0
.L01500 ;  gosub music_play

 jsr .music_play

.L01501 ;  drawscreen

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
.L01502 ;  rem 'Go back to house, but level up to 90 if not so already

.L01503 ;  if !switchreset then goto reset_win_end

 lda #1
 bit SWCHB
	BEQ .skipL01503
.condpart205
 jmp .reset_win_end

.skipL01503
.L01504 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L01505 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L01506 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L01507 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L01508 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L01509 ;  rem 'If the player collected all items, give them a new game plus.

.L01510 ;  if hero_items  <>  255 goto no_new_game_plus

	LDA hero_items
	CMP #255
 if ((* - .no_new_game_plus) < 127) && ((* - .no_new_game_plus) > -128)
	BNE .no_new_game_plus
 else
	beq .16skipno_new_game_plus
	jmp .no_new_game_plus
.16skipno_new_game_plus
 endif
.L01511 ;  if hero_level  <  90 then hero_level  =  90

	LDA hero_level
	CMP #90
     BCS .skipL01511
.condpart206
	LDA #90
	STA hero_level
.skipL01511
.L01512 ;  rem 'Take away all items again, and reset the quest flags so the 

.L01513 ;  rem 'player can play again.  

.L01514 ;  quest_flags  =  0

	LDA #0
	STA quest_flags
.L01515 ;  hero_items  =  0

	LDA #0
	STA hero_items
.L01516 ;  goto new_game_plus_end

 jmp .new_game_plus_end

.no_new_game_plus
 ; no_new_game_plus

.L01517 ;  rem 'When no new game plus, just level up 3 levels

.L01518 ;  if hero_level  >  89 goto new_game_plus_end

	LDA #89
	CMP hero_level
 if ((* - .new_game_plus_end) < 127) && ((* - .new_game_plus_end) > -128)
	bcc .new_game_plus_end
 else
	bcs .17skipnew_game_plus_end
	jmp .new_game_plus_end
.17skipnew_game_plus_end
 endif
.L01519 ;  hero_level  =  hero_level  +  3

	LDA hero_level
	CLC
	ADC #3
	STA hero_level
.new_game_plus_end
 ; new_game_plus_end

.L01520 ;  rem 'In any case recalculate hp and mp.

.L01521 ;  hero_hp  =   ( hero_level  *  2 )   +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L01522 ;  hero_mp  =   ( hero_level  /  16 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L01523 ;  gosub music_restart

 jsr .music_restart

.L01524 ;  gosub room_draw bank2

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
.L01525 ;  gosub hero_draw_s bank1

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
.L01526 ;  gosub room_draw bank2

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
.L01527 ;  goto main_loop bank1

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

.L01528 ;  goto game_win_loop

 jmp .game_win_loop

.intro_screen
 ; intro_screen

.L01529 ;  gosub music_restart

 jsr .music_restart

.L01530 ;  music_which = 0

	LDA #0
	STA music_which
.L01531 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L01532 ;  COLUPF  =  white

	LDA white
	STA COLUPF
.L01533 ;  pfcolors:

 lda # yellow
 sta COLUPF
 lda #>(pfcolorlabel1303-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel1303-84)
 sta pfcolortable
.L01534 ;  playfield:

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
.L01535 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.intro_loop
 ; intro_loop

.L01536 ;  rem COLUP1 = rand

.L01537 ;  rem COLUP0 = rand

.L01538 ;  rem REFP0 = 8

.L01539 ;  gosub music_play

 jsr .music_play

.L01540 ;  drawscreen

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
.L01541 ;  if switchreset  ||  joy0fire then return

 lda #1
 bit SWCHB
	BNE .skipL01541
.condpart207
 jmp .condpart208
.skipL01541
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
.L01542 ;  goto intro_loop

 jmp .intro_loop

.music_notes_intro_p
 ; music_notes_intro_p

.L01543 ;  data music_notes_intro

	JMP .skipL01543
music_notes_intro
	.byte   15, 30, 18, 30, 23, 30, 27, 30, 0, 8

	.byte   15, 30, 18, 30, 23, 30, 27, 30, 0, 8

	.byte   30, 30, 23, 30, 20, 30, 27, 30, 0, 8

	.byte   30, 30, 23, 30, 20, 30, 27, 30, 0, 8

	.byte   18, 120, 23, 120, 27, 120, 0, 120

	.byte   23, 120, 18, 120, 15, 120, 0, 120

.skipL01543
.music_notes_gameover_p
 ; music_notes_gameover_p

.L01544 ;  data music_notes_gameover

	JMP .skipL01544
music_notes_gameover
	.byte   24, 120, 18, 120, 20, 120, 24, 120, 0, 120

	.byte   18, 120, 16, 120, 20, 120, 18, 120, 0, 120

.skipL01544
.music_notes_victory_p
 ; music_notes_victory_p

.L01545 ;  data music_notes_victory

	JMP .skipL01545
music_notes_victory
	.byte   30, 30, 24, 30, 20, 30, 15, 30

	.byte   20, 30, 24, 30, 30, 60, 0, 120

	.byte   30, 30, 27, 30, 24, 15, 20, 30

	.byte   15, 30, 24, 30, 30, 60, 0, 120

.skipL01545
.L01546 ;  data music_sizes

	JMP .skipL01546
music_sizes
	.byte   56, 20, 32

.skipL01546
.L01547 ;  rem 'restarts the music  

.music_restart
 ; music_restart

.L01548 ;  music_timer = 1

	LDA #1
	STA music_timer
.L01549 ;  music_pointer = 0

	LDA #0
	STA music_pointer
.L01550 ;  AUDV0 = 0

	LDA #0
	STA AUDV0
.L01551 ;  AUDV1 = 0

	LDA #0
	STA AUDV1
.L01552 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.music_play
 ; music_play

.L01553 ;  rem ' Update music timer and change note if needed

.L01554 ;  rem ' If we get here, the timer is not 0 yet. 

.L01555 ;  rem ' Go on to the next note, and leave it at that   

.L01556 ;  if music_timer  =  0 then goto music_do_note_change

	LDA music_timer
	CMP #0
     BNE .skipL01556
.condpart209
 jmp .music_do_note_change

.skipL01556
.L01557 ;  goto music_no_note_change

 jmp .music_no_note_change

.music_do_note_change
 ; music_do_note_change

.L01558 ;  gosub music_change_note

 jsr .music_change_note

.music_no_note_change
 ; music_no_note_change

.L01559 ;  music_timer  =  music_timer  -  1

	DEC music_timer
.L01560 ;  rem ' COLUBK = music_timer

.L01561 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.music_change_note
 ; music_change_note

.L01562 ;  rem ' choose the not from the right music table

.L01563 ;  if music_which  =  0 then temp1  =  music_notes_intro[music_pointer]

	LDA music_which
	CMP #0
     BNE .skipL01563
.condpart210
	LDX music_pointer
	LDA music_notes_intro,x
	STA temp1
.skipL01563
.L01564 ;  if music_which  =  1 then temp1  =  music_notes_gameover[music_pointer]

	LDA music_which
	CMP #1
     BNE .skipL01564
.condpart211
	LDX music_pointer
	LDA music_notes_gameover,x
	STA temp1
.skipL01564
.L01565 ;  if music_which  =  2 then temp1  =  music_notes_victory[music_pointer]

	LDA music_which
	CMP #2
     BNE .skipL01565
.condpart212
	LDX music_pointer
	LDA music_notes_victory,x
	STA temp1
.skipL01565
.L01566 ;  AUDF0  =  temp1

	LDA temp1
	STA AUDF0
.L01567 ;  AUDF1  =  temp1

	LDA temp1
	STA AUDF1
.L01568 ;  AUDC0  =  4

	LDA #4
	STA AUDC0
.L01569 ;  AUDC1  =  12

	LDA #12
	STA AUDC1
.L01570 ;  if temp1  =  0 then AUDV0  =  0 else AUDV0  =  4

	LDA temp1
	CMP #0
     BNE .skipL01570
.condpart213
	LDA #0
	STA AUDV0
 jmp .skipelse5
.skipL01570
	LDA #4
	STA AUDV0
.skipelse5
.L01571 ;  if temp1  =  0 then AUDV1  =  0 else AUDV1  =  4

	LDA temp1
	CMP #0
     BNE .skipL01571
.condpart214
	LDA #0
	STA AUDV1
 jmp .skipelse6
.skipL01571
	LDA #4
	STA AUDV1
.skipelse6
.L01572 ;  music_pointer  =  music_pointer  +  1

	INC music_pointer
.L01573 ;  rem ' Get right timig for note from right music

.L01574 ;  if music_which  =  0 then music_timer  =  music_notes_intro[music_pointer]

	LDA music_which
	CMP #0
     BNE .skipL01574
.condpart215
	LDX music_pointer
	LDA music_notes_intro,x
	STA music_timer
.skipL01574
.L01575 ;  if music_which  =  1 then music_timer  =  music_notes_gameover[music_pointer]

	LDA music_which
	CMP #1
     BNE .skipL01575
.condpart216
	LDX music_pointer
	LDA music_notes_gameover,x
	STA music_timer
.skipL01575
.L01576 ;  if music_which  =  2 then music_timer  =  music_notes_victory[music_pointer]

	LDA music_which
	CMP #2
     BNE .skipL01576
.condpart217
	LDX music_pointer
	LDA music_notes_victory,x
	STA music_timer
.skipL01576
.L01577 ;  music_pointer  =  music_pointer  +  1

	INC music_pointer
.L01578 ;  temp3  =  music_sizes[music_which]

	LDX music_which
	LDA music_sizes,x
	STA temp3
.L01579 ;  if music_which  =  0 then temp3  =  56

	LDA music_which
	CMP #0
     BNE .skipL01579
.condpart218
	LDA #56
	STA temp3
.skipL01579
.L01580 ;  if music_which  =  1 then temp3  =  20

	LDA music_which
	CMP #1
     BNE .skipL01580
.condpart219
	LDA #20
	STA temp3
.skipL01580
.L01581 ;  if music_which  =  2 then temp3  =  32

	LDA music_which
	CMP #2
     BNE .skipL01581
.condpart220
	LDA #32
	STA temp3
.skipL01581
.L01582 ;  if music_pointer  >=  temp3 then music_pointer  =  0

	LDA music_pointer
	CMP temp3
     BCC .skipL01582
.condpart221
	LDA #0
	STA music_pointer
.skipL01582
.L01583 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.L01584 ;  bank 6

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
.L01585 ;  bank 7

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
.L01586 ;  bank 8

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
.L01587 ;  inline 6lives_statusbar.asm

 include 6lives_statusbar.asm

 if (<*) > (<(*+8))
	repeat ($100-<*)
	.byte 0
	repend
	endif
lives__L0309
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
lives__L0825
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
playerL0883_0

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
playerL0886_0

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
playerL0891_0

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
playerL0894_0

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
playerL0897_0

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
playerL0902_0

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
playerL01290_1

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
playerL01293_1

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
playerL01296_1

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
playerL01299_1

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
playerL01302_1

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
playerL01306_1

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
playerL01310_1

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
playerL01314_1

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
playerL01318_1

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
playerL01322_1

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
playerL01326_1

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
playerL01330_1

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
playerL01334_1

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
playerL01338_1

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
playerL01342_1

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
playerL01346_1

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
playerL01350_1

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
playerL01354_1

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
playerL01358_1

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
playerL01362_1

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
playerL01366_1

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
playerL01370_1

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
playerL01374_1

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
playerL01378_1

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
playerL01382_1

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
playerL01386_1

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
playerL01390_1

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
playerL01394_1

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
playerL01398_1

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
playerL01402_1

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
playerL01406_1

	.byte 0
	.byte   %00010000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01410_1

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
playerL01413_1

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
playerL01416_1

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
playerL01419_1

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
playerL01422_1

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
playerL01425_1

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
playerL01428_1

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
playerL01431_1

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
playerL01434_1

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
playerL01437_1

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
playerL01440_1

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
playerL01443_1

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
playerL01446_1

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
playerL01449_1

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
playerL01452_1

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
playerL01462_0

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
playerL01495_1

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
