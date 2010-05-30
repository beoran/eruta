game
.L00 ;  rem ' Ature Adventure

.L01 ;  rem ' Copyright beoran@rubyforge.org 2009.

.L02 ;  rem ' May be distributed under the Zlib License.

.L03 ;  rem ' You will need to modify Batari Basic from sources to compile this program

.L04 ;  rem ' I'm using much more constants than Batari Basic supports normally.

.L05 ;  rem ' It's much easier to understand what's going on if I use constants galore.

.L06 ;  rem ' Note: I only use integer math, for speed.

.L07 ;  rem ' To do: More enemies, fill in map. Perhaps use underground areas as well.

.L08 ;  rem ' To do: Set quest flags when killing bosses so they don't return.

.L09 ;  rem ' We use playfield colors, 32K and smart branching.

.L010 ;  set kernel_options pfcolors

.L011 ;  set romsize 32k

.L012 ;  set smartbranching on

.L013 ;  rem 'Cycle debugging

.L014 ;  rem set debug cycles

.L015 ;  rem set debug cyclescore

.L016 ;  rem dim mincycles = z

.L017 ;  rem mincycles=255

.
 ; 

.L018 ;  rem 'Constant definitions

.L019 ;  rem 'Commonly used NTSC Colors

.L020 ;  rem 'const pfres=12

.L021 ;  rem 'const pfrowheight=6

.L022 ;  const black = 0

.L023 ;  const gray = 6

.L024 ;  const white = 14

.L025 ;  const yellow = 30

.L026 ;  const ochre = 16

.L027 ;  const brown = 32

.L028 ;  const orange = 32

.L029 ;  const blue = 144

.L030 ;  const skyblue = 174

.L031 ;  const green = 192

.L032 ;  const darkgreen = 191

.L033 ;  const riverblue = 170

.L034 ;  const seablue = 144

.L035 ;  const lightgreen = 200

.L036 ;  const red = 64

.L037 ;  const pink = 78

.L038 ;  const oldskin = 30

.L039 ;  const skin = 254

.L040 ;  const sand = 230

.
 ; 

.
 ; 

.L041 ;  rem ' Note definitions and length const music_voice=12

.L042 ;  const note_c4 = 30

.L043 ;  const note_d4 = 27

.L044 ;  const note_e4 = 24

.L045 ;  const note_f4 = 23

.L046 ;  const note_g4 = 20

.L047 ;  const note_a4 = 18

.L048 ;  const note_b4 = 16

.L049 ;  const note_c5 = 15

.L050 ;  const note_none = 0

.L051 ;  const note_full = 60

.L052 ;  const note_half = 30

.L053 ;  const note_quarter = 15

.L054 ;  const note_fullrest = 58

.L055 ;  const note_halfrest = 28

.L056 ;  const note_quarterest = 13

.L057 ;  const note_ndot = 2

.
 ; 

.L058 ;  rem 'Playfield dimensions

.L059 ;  const field_left = 17

.L060 ;  const field_left_enter = 18

.L061 ;  const field_right = $88

.L062 ;  const field_right_enter = $87

.L063 ;  const field_top = 10

.L064 ;  const field_top_enter = 11

.L065 ;  const field_bottom = 90

.L066 ;  const field_bottom_enter = 89

.L067 ;  const field_hcenter = 61

.L068 ;  const field_vcenter = 44

.
 ; 

.L069 ;  rem ' An object will be positioned at 255 to hide it

.L070 ;  const nowhere = 255

.
 ; 

.L071 ;  rem 'Timer ticks per second, 60 for NTSC

.L072 ;  const ticks_per_second = 60

.
 ; 

.L073 ;  rem 'Score, split up in individual digits

.L074 ;  dim sc0 = score

.L075 ;  dim sc1 = score + 1

.L076 ;  dim sc2 = score + 2

.
 ; 

.L077 ;  rem 'Initial values

.L078 ;  rem 'Normal start

.L079 ;  const room_start = 49

.L080 ;  rem 'On the east side of the river

.L081 ;  rem 'const room_start=4

.
 ; 

.L082 ;  const hero_start_x = 75

.L083 ;  const hero_start_y = 75

.L084 ;  const item_start_x = 60

.L085 ;  const item_start_y = 40

.L086 ;  const item_start_hp = 1

.L087 ;  const hero_base_hp = 11

.L088 ;  const hero_high = 8

.L089 ;  const hero_half_high = 4

.L090 ;  const hero_feet_high = 4

.L091 ;  const hero_wide = 8

.L092 ;  const hero_half_wide = 4

.L093 ;  rem 'Hero's position, speed and room

.L094 ;  dim hero_x = player0x

.L095 ;  dim hero_y = player0y

.
 ; 

.L096 ;  dim hero_oldx = v

.L097 ;  dim hero_oldy = w

.L098 ;  rem 'Flags:

.L099 ;  dim hero_flags = g

.L0100 ;  rem 'hero_flags{0} -> facing north

.L0101 ;  rem 'hero_flags{1} -> facing east

.L0102 ;  rem 'hero_flags{2} -> facing south

.L0103 ;  rem 'hero_flags{3} -> facing west

.L0104 ;  rem 'hero_flags{4} -> set when attacking is allowed, blocked when not

.L0105 ;  dim hero_items = q

.L0106 ;  rem 'hero_items{0} -> Key Leaf 1

.L0107 ;  rem 'hero_items{1} -> Key Leaf 2

.L0108 ;  rem 'hero_items{2} -> Key Leaf 3

.L0109 ;  rem 'hero_items{3} -> Numen Shield

.L0110 ;  rem 'hero_items{4} -> Numen Armor 

.L0111 ;  rem 'hero_items{5} -> Numen Sword

.L0112 ;  rem 'hero_items{6} -> Heal Book

.L0113 ;  rem 'hero_items{7} -> Strike Book

.L0114 ;  dim hero_room = r

.L0115 ;  dim hero_hp = statusbarlength

.L0116 ;  dim hero_mp = o

.L0117 ;  dim hero_next = k

.L0118 ;  dim hero_level = l

.L0119 ;  rem 'Quest flags contain important achievments of Signe.

.L0120 ;  dim quest_flags = h

.L0121 ;  rem 'quest_flags{0} -> Defeated Ikaza

.L0122 ;  rem 'quest_flags{1} -> Flipped Switch 1

.L0123 ;  rem 'quest_flags{2} -> Flipped Switch 2

.L0124 ;  rem 'quest_flags{3} -> Flipped Switch 3

.L0125 ;  rem 'quest_flags{4} -> Flipped Switch 4

.L0126 ;  rem 'quest_flags{5} -> Flipped Switch 5

.L0127 ;  rem 'quest_flags{6} -> Flipped Switch 6

.L0128 ;  rem 'quest_flags{7} -> Flipped Switch 7

.
 ; 

.L0129 ;  rem 'Other flags: Reserved for future extension

.L0130 ;  rem 'quest_flags{1} -> Defeated Armor Boss

.L0131 ;  rem 'quest_flags{2} -> Defeated Sword Boss 

.L0132 ;  rem 'quest_flags{3} -> Defeated 

.L0133 ;  rem 'quest_flags{0} -> Defeated Leaf Boss 1

.L0134 ;  rem 'quest_flags{1} -> Defeated Leaf Boss 2

.L0135 ;  rem 'quest_flags{2} -> Defeated Leaf Boss 3

.
 ; 

.L0136 ;  const strike_cost = 1

.L0137 ;  const last_monster = 32

.
 ; 

.L0138 ;  const hero_speed = 1

.L0139 ;  const hero_speed_negative = # - 1

.
 ; 

.L0140 ;  rem item/monster position speed and kind

.L0141 ;  dim item_x = player1x

.L0142 ;  dim item_y = player1y

.L0143 ;  dim item_kind = s

.L0144 ;  dim item_hp = i

.L0145 ;  dim item_oldx = t

.L0146 ;  dim item_oldy = u

.L0147 ;  rem 'Active item flags: 

.L0148 ;  dim item_flags = f

.L0149 ;  rem 'item_flags{0} -> Monster missile flies north

.L0150 ;  rem 'item_flags{1} -> Monster missile flies east

.L0151 ;  rem 'item_flags{2} -> Monster missile flies south

.L0152 ;  rem 'item_flags{3} -> Monster missile flies west

.
 ; 

.
 ; 

.L0153 ;  rem 'Game timer. Controls animation. Keeps on ticking, by overflow.

.L0154 ;  dim game_timer = e

.L0155 ;  const timer_loop = 128

.L0156 ;  const timer_second = 60

.
 ; 

.
 ; 

.L0157 ;  rem 'Item and monster kinds. 

.L0158 ;  rem 'Monster kind is divided as follows: the 6 bits record the looks 

.L0159 ;  rem 'and the 'index' for data tables of the monster or item

.L0160 ;  rem 'The high 2 bits are used for 2 flag variables: 

.L0161 ;  rem 'If item_kind{7} is set it's an immobile item, otherwise, a monster

.L0162 ;  rem '6 is yet to be determined

.L0163 ;  const item_kind_mask = %00111111

.L0164 ;  const is_item_mask = %10000000

.L0165 ;  const item_none = %10000000

.L0166 ;  const monster_slime = 1

.L0167 ;  const monster_crab = 2

.L0168 ;  const monster_bat = 3

.L0169 ;  const monster_scorpio = 4

.L0170 ;  rem 'The famous rabid rabbit ^_^

.L0171 ;  const monster_rabid = 5

.L0172 ;  const monster_spider = 6

.L0173 ;  const monster_snake = 7

.L0174 ;  const monster_fish = 8

.L0175 ;  const monster_lion = 9

.L0176 ;  const monster_wolf = 10

.L0177 ;  const monster_grunt = 11

.L0178 ;  const monster_archer = 12

.L0179 ;  const monster_knight = 13

.L0180 ;  const monster_cannon = 14

.L0181 ;  const monster_zombie = 15

.L0182 ;  const monster_skeleton = 16

.L0183 ;  const monster_ghost = 17

.L0184 ;  const monster_mage = 18

.L0185 ;  const monster_flower = 19

.L0186 ;  const monster_treant = 20

.L0187 ;  const monster_mushroom = 21

.L0188 ;  const monster_book = 22

.L0189 ;  const monster_sword = 23

.L0190 ;  rem 'Monster curse blocks the way in rooms. It's invulnerable,

.L0191 ;  rem 'but can be removed by game events.

.L0192 ;  const monster_curse = 24

.L0193 ;  const monster_first_boss = 25

.L0194 ;  const monster_leaf1_boss = 25

.L0195 ;  const monster_leaf2_boss = 26

.L0196 ;  const monster_leaf3_boss = 27

.L0197 ;  const monster_armor_boss = 28

.L0198 ;  rem 'Boss that guards the Numen Sword

.L0199 ;  const monster_sword_boss = 29

.L0200 ;  rem 'Boss that guards the Strike book

.L0201 ;  const monster_strike_boss = 30

.L0202 ;  const monster_ikaza = 31

.
 ; 

.L0203 ;  const item_inventory_mask = 160

.L0204 ;  const item_leaf1 = 160

.L0205 ;  const item_leaf2 = 161

.L0206 ;  const item_leaf3 = 162

.L0207 ;  const item_armor = 163

.L0208 ;  const item_shield = 164

.L0209 ;  const item_sword = 165

.L0210 ;  const item_bookheal = 166

.L0211 ;  const item_bookstrike = 167

.L0212 ;  const item_healhp = 168

.L0213 ;  const item_healmp = 169

.L0214 ;  const item_healallhp = 170

.L0215 ;  const item_healallmp = 171

.L0216 ;  const item_first_switch = 172

.L0217 ;  const item_switch_on = 7

.L0218 ;  const item_switch1 = 172

.L0219 ;  const item_switch2 = 173

.L0220 ;  const item_switch3 = 174

.L0221 ;  const item_switch4 = 175

.L0222 ;  const item_switch5 = 176

.L0223 ;  const item_switch6 = 177

.L0224 ;  const item_switch7 = 178

.L0225 ;  const item_last_switch = 178

.L0226 ;  const item_first_switch_on = 179

.L0227 ;  const item_switch1_on = 179

.L0228 ;  const item_switch2_on = 180

.L0229 ;  const item_switch3_on = 181

.L0230 ;  const item_switch4_on = 182

.L0231 ;  const item_switch5_on = 183

.L0232 ;  const item_switch6_on = 184

.L0233 ;  const item_switch7_on = 185

.L0234 ;  const item_last_switch_on = 182

.L0235 ;  const item_victory = 191

.L0236 ;  const monster_normal = 0

.L0237 ;  rem 'Bit 0 in monster flags

.L0238 ;  const monster_shoots = 1

.L0239 ;  rem 'Bit 1 in monster flags

.L0240 ;  const monster_randomwalk = 2

.L0241 ;  rem 'Bit 2 in monster flags

.L0242 ;  const monster_aggressive = 4

.L0243 ;  rem 'Bit 3 in monster flags

.L0244 ;  const monster_static = 8

.L0245 ;  rem 'Bit 4 in monster flags

.L0246 ;  const monster_alwaysdrop = 16

.L0247 ;  rem 'Bit 5 in monster flags

.L0248 ;  const monster_nowalls = 32

.L0249 ;  rem 'Bit 6 in monster flags

.L0250 ;  const monster_huge = 64

.L0251 ;  rem 'Bit 7 in monster flags. Will triple if huge is also set

.L0252 ;  rem 'Currently, double nor triple doesn't work well with the AI

.L0253 ;  const monster_double = 128

.L0254 ;  const monster_boss_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0255 ;  const monster_boss2_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0256 ;  const monster_boss3_info = monster_alwaysdrop  +  monster_shoots  +  monster_huge  +  monster_nowalls

.L0257 ;  const monster_swordboss_info = monster_alwaysdrop  +  monster_aggressive  +  monster_nowalls

.L0258 ;  const monster_cannon_info = monster_shoots  +  monster_static  +  monster_nowalls

.
 ; 

.L0259 ;  rem ' Music wave form to use.

.L0260 ;  const music_instrument = 12

.L0261 ;  const music_notes = 69

.
 ; 

.
 ; 

.
 ; 

.L0262 ;  rem 'Timer and pointer for music and sound

.L0263 ;  dim music_pointer = m

.L0264 ;  dim music_timer = n

.L0265 ;  dim sound_timer = p

.L0266 ;  rem 'Unused variables: a b c d

.
 ; 

.L0267 ;  rem 'Set back to 1 when not debugging.

.L0268 ;  const hero_start_level = 20

.L0269 ;  rem 'set back to 0 when not debugging

.L0270 ;  const hero_start_items = 

.
 ; 

.L0271 ;  rem ' Colors to use for the item display 

.L0272 ;  data item_colors

	JMP .skipL0272
item_colors
	.byte   black, green, red,  blue,  black, green, red,  blue 

	.byte   black, green, red,  blue,  black, green, red,  blue 

	.byte   black, green, red,  blue,  black, green, red,  blue 

	.byte   black, black, red,  red,  black, skin, red,  blue 

	.byte   black, green, red,  blue,  black, green, red,  blue 

	.byte   red  , green, red, green,  black, green, red,  blue 

	.byte   white, yellow, orange, black, green, red,  blue, white  

	.byte   yellow, orange,  blue,  black, green, red,  blue, blue  

.skipL0272
.
 ; 

.L0273 ;  rem 'Damage modifier for the MONSTER/item. 0 means no damage

.L0274 ;  rem 'There is no monster nr 0, that's for item_none. 

.L0275 ;  rem 'The curse deals massive damage to be sure it works.

.L0276 ;  data item_damage

	JMP .skipL0276
item_damage
	.byte    0,  1,  1,  1,  1,  2,  2,  2

	.byte    2,  4,  4,  4,  4,  6,  6,  6

	.byte   10, 16, 16,  1, 16,  4, 16, 16

	.byte  128, 20, 22, 24, 48, 48, 48, 64

	.byte    0,  0,  0,  0,  0,  0,  0,  0

	.byte    0,  0,  0,  0,  0,  0,  0,  0

	.byte    0,  0,  0,  0,  0,  0,  0,  0

	.byte    0,  0,  0,  0,  0,  0,  0,  0

.skipL0276
.
 ; 

.L0277 ;  rem ' 

.L0278 ;  rem ' Monster information, bitflags (monster 0 doesn't exist, but is in this list!)

.L0279 ;  data monster_info

	JMP .skipL0279
monster_info
	.byte   monster_normal, monster_normal, monster_nowalls+monster_shoots, monster_nowalls

	.byte   monster_normal, monster_nowalls, monster_normal, monster_aggressive

	.byte   monster_shoots + monster_nowalls, monster_normal, monster_normal, monster_normal

	.byte   monster_shoots, monster_aggressive,  monster_cannon_info, monster_randomwalk

	.byte   monster_normal, monster_nowalls, monster_nowalls + monster_shoots, monster_cannon_info

	.byte   monster_normal, monster_normal, monster_cannon_info, monster_static + monster_nowalls

	.byte   monster_static + monster_huge + monster_nowalls, monster_boss_info, monster_boss_info, monster_boss3_info

	.byte   monster_boss_info, monster_swordboss_info, monster_boss_info, monster_aggressive + monster_shoots

.skipL0279
.
 ; 

.L0280 ;  rem ' Items the monster drops, or for items, what item flag to set

.L0281 ;  data monster_drops

	JMP .skipL0281
monster_drops
	.byte   item_healhp, item_healhp, item_healhp, item_healmp 

	.byte   item_healhp, item_healhp, item_healhp, item_healhp 

	.byte   item_healhp, item_healhp, item_healhp, item_healhp 

	.byte   item_healhp, item_healallhp, item_healhp, monster_skeleton 

	.byte   monster_ghost, item_switch5, monster_book, item_healmp 

	.byte   item_healallmp, item_healhp,  item_bookheal, item_sword 

	.byte   item_healhp, item_leaf1, item_leaf2 , item_leaf3 

	.byte   item_armor, monster_sword, item_bookstrike, item_healallhp 

.skipL0281
.
 ; 

.
 ; 

.L0282 ;  rem VOLUME DOWN AND SET CHANNELS

.L0283 ;  AUDV0 = 0

	LDA #0
	STA AUDV0
.L0284 ;  AUDV1 = 0

	LDA #0
	STA AUDV1
.L0285 ;  AUDC0 = music_instrument

	LDA #music_instrument
	STA AUDC0
.
 ; 

.L0286 ;  rem INITIALIZE POINTERS AND TIMERS

.L0287 ;  music_pointer = 0

	LDA #0
	STA music_pointer
.L0288 ;  music_timer = 0

	LDA #0
	STA music_timer
.
 ; 

.L0289 ;  rem 'lives control

.L0290 ;  lifecolor = black

	LDA black
	STA lifecolor
.L0291 ;  rem 'Lives are used for the MP

.L0292 ;  lives = 32

	LDA #32
	STA lives
.L0293 ;  lives:

	LDA #<lives__L0293
	STA lifepointer
	LDA lifepointer+1
	AND #$E0
	ORA #(>lives__L0293)&($1F)
	STA lifepointer+1
.
 ; 

.L0294 ;  rem 'Go to intro screen, but not now while we are debugging.

.L0295 ;  gosub intro_screen bank5

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
.L0296 ;  goto main_loop_start

 jmp .main_loop_start

.
 ; 

.main_loop_start
 ; main_loop_start

.L0297 ;  rem ' Set up initial values.

.L0298 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L0299 ;  lifecolor = yellow

	LDA yellow
	STA lifecolor
.L0300 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L0301 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L0302 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L0303 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L0304 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L0305 ;  quest_flags = 0

	LDA #0
	STA quest_flags
.L0306 ;  hero_level = hero_start_level

	LDA #hero_start_level
	STA hero_level
.L0307 ;  hero_next =  ( hero_level  /  2 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	CLC
	ADC #1
	STA hero_next
.L0308 ;  hero_hp = hero_level * 2 + hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0309 ;  rem 'MP is between 1 and 6

.L0310 ;  hero_mp = hero_level / 16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0311 ;  gosub set_mp_display

 jsr .set_mp_display

.L0312 ;  hero_flags  =  0

	LDA #0
	STA hero_flags
.L0313 ;  item_flags  =  0

	LDA #0
	STA item_flags
.L0314 ;  game_timer  =  0

	LDA #0
	STA game_timer
.L0315 ;  item_kind = item_none

	LDA #item_none
	STA item_kind
.L0316 ;  hero_items  =  hero_start_items

	LDA #hero_start_items
	STA hero_items
.L0317 ;  rem 'Player starts facing south, able to attack

.L0318 ;  hero_flags{2}  =  1

	LDA hero_flags
	ORA #4
	STA hero_flags
.L0319 ;  hero_flags{4}  =  1

	LDA hero_flags
	ORA #16
	STA hero_flags
.L0320 ;  rem player0x=75

.L0321 ;  rem player0y=75

.L0322 ;  item_x = item_start_x

	LDA #item_start_x
	STA item_x
.L0323 ;  item_oldx = item_start_x

	LDA #item_start_x
	STA item_oldx
.L0324 ;  item_y = item_start_y

	LDA #item_start_y
	STA item_y
.L0325 ;  item_oldy = item_start_x

	LDA #item_start_x
	STA item_oldy
.L0326 ;  missile1x = nowhere

	LDA #nowhere
	STA missile1x
.L0327 ;  missile1y = nowhere

	LDA #nowhere
	STA missile1y
.L0328 ;  music_pointer = 0

	LDA #0
	STA music_pointer
.L0329 ;  music_timer = 0

	LDA #0
	STA music_timer
.L0330 ;  scorecolor = yellow

	LDA yellow
	STA scorecolor
.L0331 ;  item_hp = item_start_hp

	LDA #item_start_hp
	STA item_hp
.L0332 ;  gosub show_inventory

 jsr .show_inventory

.L0333 ;  gosub hero_draw_s

 jsr .hero_draw_s

.L0334 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point2-1)
 pha
 lda #<(ret_point2-1)
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
ret_point2
.L0335 ;  drawscreen

 sta temp7
 lda #>(ret_point3-1)
 pha
 lda #<(ret_point3-1)
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
ret_point3
.main_loop
 ; main_loop

.L0336 ;  COLUP0  =  skin

	LDA #skin
	STA COLUP0
.L0337 ;  temp1  =  item_kind  &  item_kind_mask

	LDA item_kind
	AND #item_kind_mask
	STA temp1
.L0338 ;  COLUP1  =  item_colors[temp1]

	LDX temp1
	LDA item_colors,x
	STA COLUP1
.L0339 ;  rem 'Special effects for the item (double or triple size, double or triple instances)

.L0340 ;  if item_kind{7} goto special_effects_end

	BIT item_kind
 if ((* - .special_effects_end) < 127) && ((* - .special_effects_end) > -128)
	bmi .special_effects_end
 else
	bpl .0skipspecial_effects_end
	jmp .special_effects_end
.0skipspecial_effects_end
 endif
.L0341 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0342 ;  if temp2{6} then NUSIZ1  =  $07

	BIT temp2
	BVC .skipL0342
.condpart0
	LDA #$07
	STA NUSIZ1
.skipL0342
.L0343 ;  if temp2{7} then NUSIZ1  =  $02

	BIT temp2
	BPL .skipL0343
.condpart1
	LDA #$02
	STA NUSIZ1
.skipL0343
.L0344 ;  if temp2{7}  &&  temp2{7} then NUSIZ1  = $06

	BIT temp2
	BPL .skipL0344
.condpart2
	BIT temp2
	BPL .skip2then
.condpart3
	LDA #$06
	STA NUSIZ1
.skip2then
.skipL0344
.special_effects_end
 ; special_effects_end

.L0345 ;  PF0 = $00

	LDA #$00
	STA PF0
.
 ; 

.
 ; 

.L0346 ;  rem 'Collisions with the walls. For now, we use push-back collision 

.L0347 ;  rem 'for the monsters, and preventive collision detection for the player. 

.L0348 ;  rem 'This is inconsistent, but push-back is easier to manage with regards to 

.L0349 ;  rem 'and matters less for AI, so I'll leave it at that for now.

.L0350 ;  if !collision(player0,playfield) then goto herofield_collide_end

	BIT CXP0FB
	BMI .skipL0350
.condpart4
 jmp .herofield_collide_end

.skipL0350
.L0351 ;  rem 'Could do something here ??? Some overlap is OK, IMO

.herofield_collide_end
 ; herofield_collide_end

.
 ; 

.L0352 ;  rem 'Collisions with the walls

.L0353 ;  rem 'For the item

.L0354 ;  if !collision(player1,playfield) then goto itemfield_collide_end

	BIT CXP1FB
	BMI .skipL0354
.condpart5
 jmp .itemfield_collide_end

.skipL0354
.L0355 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0356 ;  rem 'Some monsters ignore walls

.L0357 ;  if temp2{5} then goto itemfield_collide_end

	LDA temp2
	AND #32
	BEQ .skipL0357
.condpart6
 jmp .itemfield_collide_end

.skipL0357
.
 ; 

.L0358 ;  if item_y  =  item_oldy  &&  item_x  =  item_oldx then goto itemfield_collide_still

	LDA item_y
	CMP item_oldy
     BNE .skipL0358
.condpart7
	LDA item_x
	CMP item_oldx
     BNE .skip7then
.condpart8
 jmp .itemfield_collide_still

.skip7then
.skipL0358
.L0359 ;  item_y  =  item_oldy

	LDA item_oldy
	STA item_y
.L0360 ;  item_x  =  item_oldx

	LDA item_oldx
	STA item_x
.L0361 ;  goto itemfield_collide_end

 jmp .itemfield_collide_end

.itemfield_collide_still
 ; itemfield_collide_still

.L0362 ;  rem if item_y < field_vcenter then item_y  = item_oldy - 1 else item_y  = item_oldy + 1

.L0363 ;  rem if item_x < field_hcenter then item_x  = item_oldx - 1 else item_x  = hero_oldx + 1

.itemfield_collide_end
 ; itemfield_collide_end

.
 ; 

.
 ; 

.L0364 ;  rem 'Remember curent position which should be OK here, 

.L0365 ;  rem 'and with that I mean not colliding with the walls. 

.L0366 ;  hero_oldx  =  hero_x

	LDA hero_x
	STA hero_oldx
.L0367 ;  hero_oldy  =  hero_y

	LDA hero_y
	STA hero_oldy
.L0368 ;  item_oldx  =  item_x

	LDA item_x
	STA item_oldx
.L0369 ;  item_oldy  =  item_y

	LDA item_y
	STA item_oldy
.
 ; 

.
 ; 

.L0370 ;  rem 'Teleport to Sygne's home on reset button

.L0371 ;  if !switchreset then goto reset_end

 lda #1
 bit SWCHB
	BEQ .skipL0371
.condpart9
 jmp .reset_end

.skipL0371
.L0372 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L0373 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L0374 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L0375 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L0376 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L0377 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point4-1)
 pha
 lda #<(ret_point4-1)
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
ret_point4
.reset_end
 ; reset_end

.
 ; 

.L0378 ;  rem 'Update game timer, and let it overflow back to 0 .

.L0379 ;  game_timer  =  game_timer  +  1

	INC game_timer
.L0380 ;  rem 'Update sound timer, and set to silence if sound is done

.L0381 ;  if sound_timer  <  1 then goto sound_end

	LDA sound_timer
	CMP #1
     BCS .skipL0381
.condpart10
 jmp .sound_end

.skipL0381
.L0382 ;  sound_timer  =  sound_timer  -  1

	DEC sound_timer
.L0383 ;  if sound_timer  >  1 then goto sound_end

	LDA #1
	CMP sound_timer
     BCS .skipL0383
.condpart11
 jmp .sound_end

.skipL0383
.L0384 ;  AUDV1  =  0

	LDA #0
	STA AUDV1
.L0385 ;  AUDC1  =  0

	LDA #0
	STA AUDC1
.L0386 ;  AUDF1  =  0

	LDA #0
	STA AUDF1
.L0387 ;  sound_timer  =  0

	LDA #0
	STA sound_timer
.sound_end
 ; sound_end

.
 ; 

.L0388 ;  rem 'Collision between hero and monster/item or the monster's missile

.L0389 ;  if !collision(player0,missile1) then goto no_missile_collide

	BIT CXM1P
	BMI .skipL0389
.condpart12
 jmp .no_missile_collide

.skipL0389
.L0390 ;  rem 'Ennemy missile hit player, remove it

.L0391 ;  missile1y  =  nowhere

	LDA #nowhere
	STA missile1y
.L0392 ;  missile1x  =  nowhere

	LDA #nowhere
	STA missile1x
.L0393 ;  item_flags =  0

	LDA #0
	STA item_flags
.L0394 ;  rem ' If the player doesn't have a shield, jump to monster collide

.L0395 ;  rem 'Otherwise ignore the projectile and remove it  

.L0396 ;  if !hero_items{3} then goto monster_collide

	LDA hero_items
	AND #8
	BNE .skipL0396
.condpart13
 jmp .monster_collide

.skipL0396
.no_missile_collide
 ; no_missile_collide

.L0397 ;  if !collision(player0,player1) then goto item_collide_end

	BIT CXPPMM
	BMI .skipL0397
.condpart14
 jmp .item_collide_end

.skipL0397
.L0398 ;  rem 'Distinguish between item or monster collide

.L0399 ;  if item_kind{7} then goto item_collide

	BIT item_kind
	BPL .skipL0399
.condpart15
 jmp .item_collide

.skipL0399
.L0400 ;  rem 'Collision with monster or monster missile

.monster_collide
 ; monster_collide

.L0401 ;  COLUP0  =  red

	LDA #red
	STA COLUP0
.L0402 ;  rem 'Make a hit sound

.L0403 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0404 ;  AUDC1 = 6

	LDA #6
	STA AUDC1
.L0405 ;  AUDF1 = 10

	LDA #10
	STA AUDF1
.L0406 ;  AUDV1 = 14

	LDA #14
	STA AUDV1
.
 ; 

.L0407 ;  rem 'Push back the hero, but only if the playfield is free behind her.

.
 ; 

.L0408 ;  if !hero_flags{0} then goto hero_hit_north_end

	LDA hero_flags
	LSR
	BCS .skipL0408
.condpart16
 jmp .hero_hit_north_end

.skipL0408
.L0409 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0410 ;  temp2  =   ( hero_y  +  8  -  1 )   /  8

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
.L0411 ;  if pfread ( temp1 ,  temp2 )  goto hero_hit_north_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point5-1)
 pha
 lda #<(ret_point5-1)
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
ret_point5
 if ((* - .hero_hit_north_end) < 127) && ((* - .hero_hit_north_end) > -128)
	BEQ .hero_hit_north_end
 else
	bne .1skiphero_hit_north_end
	jmp .hero_hit_north_end
.1skiphero_hit_north_end
 endif
.L0412 ;  hero_y  =  hero_y  +  8

	LDA hero_y
	CLC
	ADC #8
	STA hero_y
.hero_hit_north_end
 ; hero_hit_north_end

.
 ; 

.L0413 ;  if !hero_flags{1} then goto hero_hit_east_end

	LDA hero_flags
	AND #2
	BNE .skipL0413
.condpart17
 jmp .hero_hit_east_end

.skipL0413
.L0414 ;  temp1  =   ( hero_x  -  8  -  17 )   /  4

; complex statement detected
	LDA hero_x
	SEC
	SBC #8
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0415 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

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
.L0416 ;  if pfread ( temp1 ,  temp2 )  goto hero_hit_east_end

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
 if ((* - .hero_hit_east_end) < 127) && ((* - .hero_hit_east_end) > -128)
	BEQ .hero_hit_east_end
 else
	bne .2skiphero_hit_east_end
	jmp .hero_hit_east_end
.2skiphero_hit_east_end
 endif
.L0417 ;  hero_x  =  hero_x  -  8

	LDA hero_x
	SEC
	SBC #8
	STA hero_x
.hero_hit_east_end
 ; hero_hit_east_end

.
 ; 

.L0418 ;  if !hero_flags{2} then goto hero_hit_south_end

	LDA hero_flags
	AND #4
	BNE .skipL0418
.condpart18
 jmp .hero_hit_south_end

.skipL0418
.L0419 ;  if pfread ( temp1 ,  temp2 )  then goto hero_hit_south_end

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
	BNE .skipL0419
.condpart19
 jmp .hero_hit_south_end

.skipL0419
.L0420 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0421 ;  temp2  =   ( hero_y  -  hero_high  -  8  -  1 )   /  8

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
.L0422 ;  hero_y  =  hero_y  -  8

	LDA hero_y
	SEC
	SBC #8
	STA hero_y
.hero_hit_south_end
 ; hero_hit_south_end

.
 ; 

.L0423 ;  if !hero_flags{3} then goto hero_hit_west_end

	LDA hero_flags
	AND #8
	BNE .skipL0423
.condpart20
 jmp .hero_hit_west_end

.skipL0423
.L0424 ;  temp1  =   ( hero_x  +  hero_wide  +  8  -  17 )   /  4

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
.L0425 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

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
.L0426 ;  if pfread ( temp1 ,  temp2 )  then goto hero_hit_west_end

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
	BNE .skipL0426
.condpart21
 jmp .hero_hit_west_end

.skipL0426
.L0427 ;  hero_x  =  hero_x  +  8

	LDA hero_x
	CLC
	ADC #8
	STA hero_x
.hero_hit_west_end
 ; hero_hit_west_end

.
 ; 

.L0428 ;  temp1  =  item_kind  &  item_kind_mask

	LDA item_kind
	AND #item_kind_mask
	STA temp1
.L0429 ;  temp1  =  item_damage[temp1]

	LDX temp1
	LDA item_damage,x
	STA temp1
.L0430 ;  rem 'Halve damage if hero has the armor 

.L0431 ;  if hero_items{4} then temp1  =  temp1  /  2

	LDA hero_items
	AND #16
	BEQ .skipL0431
.condpart22
	LDA temp1
	lsr
	STA temp1
.skipL0431
.
 ; 

.L0432 ;  rem 'Game over if damage is higher than health

.L0433 ;  if temp1  <  hero_hp then goto do_damage

	LDA temp1
	CMP hero_hp
     BCS .skipL0433
.condpart23
 jmp .do_damage

.skipL0433
.L0434 ;  rem 'But escape by the skin of the teeth if Signe has the healing book and 

.L0435 ;  rem 'Numen to use it.

.L0436 ;  if hero_items{6}  &&  hero_mp  >  0 then goto use_heal_spell

	BIT hero_items
	BVC .skipL0436
.condpart24
	LDA #0
	CMP hero_mp
     BCS .skip24then
.condpart25
 jmp .use_heal_spell

.skip24then
.skipL0436
.L0437 ;  hero_hp  =  0

	LDA #0
	STA hero_hp
.L0438 ;  goto game_over bank5

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

.L0439 ;  COLUP0  =  green

	LDA #green
	STA COLUP0
.L0440 ;  hero_mp  =  hero_mp  -  1

	DEC hero_mp
.L0441 ;  gosub set_mp_display

 jsr .set_mp_display

.L0442 ;  hero_hp  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0443 ;  goto item_collide_end

 jmp .item_collide_end

.do_damage
 ; do_damage

.L0444 ;  hero_hp  =  hero_hp  -  temp1

	LDA hero_hp
	SEC
	SBC temp1
	STA hero_hp
.L0445 ;  goto item_collide_end

 jmp .item_collide_end

.item_collide
 ; item_collide

.L0446 ;  rem 'If colliding with an item, pick it up if it is not a switch.

.L0447 ;  if item_kind  <  item_switch1 then goto item_do_collide

	LDA item_kind
	CMP #item_switch1
     BCS .skipL0447
.condpart26
 jmp .item_do_collide

.skipL0447
.L0448 ;  if item_kind  >  item_switch7_on then goto item_do_collide

	LDA #item_switch7_on
	CMP item_kind
     BCS .skipL0448
.condpart27
 jmp .item_do_collide

.skipL0448
.L0449 ;  goto item_collide_end

 jmp .item_collide_end

.item_do_collide
 ; item_do_collide

.L0450 ;  gosub item_pickup

 jsr .item_pickup

.L0451 ;  gosub show_inventory

 jsr .show_inventory

.item_collide_end
 ; item_collide_end

.
 ; 

.L0452 ;  rem 'Collision between monster missile and field. 

.L0453 ;  rem 'Or between sword and monster missile.  Remove missile.

.L0454 ;  rem 'Remove misile if monster is gone.

.L0455 ;  if item_kind  >  last_monster goto missile_remove

	LDA #last_monster
	CMP item_kind
 if ((* - .missile_remove) < 127) && ((* - .missile_remove) > -128)
	bcc .missile_remove
 else
	bcs .3skipmissile_remove
	jmp .missile_remove
.3skipmissile_remove
 endif
.L0456 ;  if collision(missile0,missile1) then goto missile_remove

	BIT CXPPMM
	BVC .skipL0456
.condpart28
 jmp .missile_remove

.skipL0456
.L0457 ;  if missile1x  >  field_right then goto missile_remove

	LDA #field_right
	CMP missile1x
     BCS .skipL0457
.condpart29
 jmp .missile_remove

.skipL0457
.L0458 ;  if missile1x  <  field_left then goto missile_remove

	LDA missile1x
	CMP #field_left
     BCS .skipL0458
.condpart30
 jmp .missile_remove

.skipL0458
.L0459 ;  if missile1y  <  field_top then goto missile_remove

	LDA missile1y
	CMP #field_top
     BCS .skipL0459
.condpart31
 jmp .missile_remove

.skipL0459
.L0460 ;  if missile1y  >  field_bottom then goto missile_remove

	LDA #field_bottom
	CMP missile1y
     BCS .skipL0460
.condpart32
 jmp .missile_remove

.skipL0460
.L0461 ;  if !collision(missile1,playfield) then goto missile_remove_end

	BIT CXM1FB
	BMI .skipL0461
.condpart33
 jmp .missile_remove_end

.skipL0461
.L0462 ;  rem 'Don't remove the misile even when coliding with the playfield if the 

.L0463 ;  rem 'monster ignores the walls.

.L0464 ;  temp2  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp2
.L0465 ;  rem 'temp2 = temp2 & monster_nowalls

.L0466 ;  if temp2{5} then goto missile_remove_end

	LDA temp2
	AND #32
	BEQ .skipL0466
.condpart34
 jmp .missile_remove_end

.skipL0466
.missile_remove
 ; missile_remove

.L0467 ;  missile1y  =  nowhere

	LDA #nowhere
	STA missile1y
.L0468 ;  missile1x  =  nowhere

	LDA #nowhere
	STA missile1x
.L0469 ;  item_flags =  0

	LDA #0
	STA item_flags
.missile_remove_end
 ; missile_remove_end

.
 ; 

.L0470 ;  rem 'Collision between monster and hero's attack

.L0471 ;  if !collision(player1,missile0) then goto slash_collide_end

	BIT CXM0P
	BMI .skipL0471
.condpart35
 jmp .slash_collide_end

.skipL0471
.L0472 ;  rem 'Distinguish between item or monster collide

.L0473 ;  if item_kind{7} then goto slash_item_collide

	BIT item_kind
	BPL .skipL0473
.condpart36
 jmp .slash_item_collide

.skipL0473
.
 ; 

.L0474 ;  rem 'The curse monster cannot be harmed normally. 

.L0475 ;  rem 'It can be harmed if Signe has the three leaves, or the Numen Sword. 

.L0476 ;  rem 'Do the special handling here.

.L0477 ;  if item_kind  <>  monster_curse then goto monster_no_curse

	LDA item_kind
	CMP #monster_curse
     BEQ .skipL0477
.condpart37
 jmp .monster_no_curse

.skipL0477
.L0478 ;  rem 'Signe has the sword? The curse can be damaged.

.L0479 ;  if hero_items{5} then goto monster_no_curse

	LDA hero_items
	AND #32
	BEQ .skipL0479
.condpart38
 jmp .monster_no_curse

.skipL0479
.L0480 ;  rem 'Signe has the three leaves? The curse can be damaged. 

.L0481 ;  if hero_items{0}  &&  hero_items{1}  &&  hero_items{2} then goto monster_no_curse

	LDA hero_items
	LSR
	BCC .skipL0481
.condpart39
	LDA hero_items
	AND #2
	BEQ .skip39then
.condpart40
	LDA hero_items
	AND #4
	BEQ .skip40then
.condpart41
 jmp .monster_no_curse

.skip40then
.skip39then
.skipL0481
.L0482 ;  rem 'If we get here, Signe's unable to do any damage.

.L0483 ;  rem 'Make a deflected sound

.L0484 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0485 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0486 ;  AUDV1 = 14

	LDA #14
	STA AUDV1
.L0487 ;  AUDF1 = 1

	LDA #1
	STA AUDF1
.L0488 ;  goto slash_collide_end

 jmp .slash_collide_end

.monster_no_curse
 ; monster_no_curse

.L0489 ;  rem 'Make a hit sound once

.L0490 ;  sound_timer = 1

	LDA #1
	STA sound_timer
.L0491 ;  AUDC1 = 8

	LDA #8
	STA AUDC1
.L0492 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0493 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0494 ;  hero_flags{4}  =  0

	LDA hero_flags
	AND #239
	STA hero_flags
.L0495 ;  rem COLUP1 = red

.L0496 ;  rem 'Push back monster

.L0497 ;  if hero_flags{0} then item_y  =  item_y  -  4

	LDA hero_flags
	LSR
	BCC .skipL0497
.condpart42
	LDA item_y
	SEC
	SBC #4
	STA item_y
.skipL0497
.L0498 ;  if hero_flags{1} then item_x  =  item_x  +  4

	LDA hero_flags
	AND #2
	BEQ .skipL0498
.condpart43
	LDA item_x
	CLC
	ADC #4
	STA item_x
.skipL0498
.L0499 ;  if hero_flags{2} then item_y  =  item_y  +  4

	LDA hero_flags
	AND #4
	BEQ .skipL0499
.condpart44
	LDA item_y
	CLC
	ADC #4
	STA item_y
.skipL0499
.L0500 ;  if hero_flags{3} then item_x  =  item_x  -  4

	LDA hero_flags
	AND #8
	BEQ .skipL0500
.condpart45
	LDA item_x
	SEC
	SBC #4
	STA item_x
.skipL0500
.L0501 ;  rem 'Damage is level / 2 + 1

.L0502 ;  temp1  =   ( hero_level  /  2 ) 

; complex statement detected
	LDA hero_level
	lsr
	STA temp1
.L0503 ;  temp1  =  temp1  +  1

	INC temp1
.L0504 ;  rem 'Double damage with the Numen Sword

.L0505 ;  if hero_items{5} then temp1  =  temp1  *  2

	LDA hero_items
	AND #32
	BEQ .skipL0505
.condpart46
	LDA temp1
	asl
	STA temp1
.skipL0505
.L0506 ;  rem 'Prevent damage overflow

.L0507 ;  if temp1  <  0 then temp1  =  0  -  temp1

	LDA temp1
	CMP #0
     BCS .skipL0507
.condpart47
	LDA #0
	SEC
	SBC temp1
	STA temp1
.skipL0507
.L0508 ;  if temp1  <  item_hp then goto item_survived

	LDA temp1
	CMP item_hp
     BCS .skipL0508
.condpart48
 jmp .item_survived

.skipL0508
.L0509 ;  rem 'Item / monster killed or destroyed

.L0510 ;  if item_kind  =  monster_ikaza then quest_flags{0}  =  1

	LDA item_kind
	CMP #monster_ikaza
     BNE .skipL0510
.condpart49
	LDA quest_flags
	ORA #1
	STA quest_flags
.skipL0510
.L0511 ;  rem 'Make sure Ikaza cannot come back

.L0512 ;  rem 'Give experience and level up if needed

.L0513 ;  rem 'Experience received is danger level of the monster 

.L0514 ;  temp2  =  item_damage[item_kind]

	LDX item_kind
	LDA item_damage,x
	STA temp2
.L0515 ;  rem 'No experience for killing the curse, as it's a sitting duck.

.L0516 ;  if item_kind  =  monster_curse then goto give_experience_end

	LDA item_kind
	CMP #monster_curse
     BNE .skipL0516
.condpart50
 jmp .give_experience_end

.skipL0516
.L0517 ;  if temp2  <  hero_next then goto give_experience

	LDA temp2
	CMP hero_next
     BCS .skipL0517
.condpart51
 jmp .give_experience

.skipL0517
.L0518 ;  rem 'Level up, but not more than 99

.L0519 ;  if hero_level  >  98 then goto give_experience_end

	LDA #98
	CMP hero_level
     BCS .skipL0519
.condpart52
 jmp .give_experience_end

.skipL0519
.L0520 ;  rem 'Level up sound

.L0521 ;  COLUP0 = blue

	LDA blue
	STA COLUP0
.L0522 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0523 ;  AUDF1 = 24

	LDA #24
	STA AUDF1
.L0524 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0525 ;  sound_timer = 8

	LDA #8
	STA sound_timer
.L0526 ;  hero_level  =  hero_level  +  1

	INC hero_level
.L0527 ;  hero_hp  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L0528 ;  rem 'Actually, this is buggy for levels above 80

.L0529 ;  hero_mp  =  hero_level / 16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0530 ;  gosub set_mp_display

 jsr .set_mp_display

.L0531 ;  hero_next  =   ( hero_level  /  2 )   +  1

; complex statement detected
	LDA hero_level
	lsr
	CLC
	ADC #1
	STA hero_next
.L0532 ;  COLUP0  =  lightgreen

	LDA #lightgreen
	STA COLUP0
.L0533 ;  goto give_experience_end

 jmp .give_experience_end

.give_experience
 ; give_experience

.L0534 ;  hero_next  =  hero_next  -  temp2

	LDA hero_next
	SEC
	SBC temp2
	STA hero_next
.give_experience_end
 ; give_experience_end

.L0535 ;  rem 'Drop an item, perhaps

.L0536 ;  temp6  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp6
.L0537 ;  rem 'Bit 4: monster always drops item 

.L0538 ;  if temp6{4} then goto drop_item

	LDA temp6
	AND #16
	BEQ .skipL0538
.condpart53
 jmp .drop_item

.skipL0538
.L0539 ;  if rand  >  128 then goto drop_item

	LDA #128
	CMP rand
     BCS .skipL0539
.condpart54
 jmp .drop_item

.skipL0539
.L0540 ;  rem 'Drop occasional, not always, depeding on monster flags

.drop_no_item
 ; drop_no_item

.L0541 ;  item_kind  =  item_none

	LDA #item_none
	STA item_kind
.L0542 ;  item_hp  =  0

	LDA #0
	STA item_hp
.L0543 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L0544 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
.L0545 ;  goto drop_item_done

 jmp .drop_item_done

.drop_item
 ; drop_item

.L0546 ;  temp2  =  monster_drops[item_kind]

	LDX item_kind
	LDA monster_drops,x
	STA temp2
.L0547 ;  item_kind  =  temp2

	LDA temp2
	STA item_kind
.L0548 ;  gosub item_setup_kind bank4

 sta temp7
 lda #>(ret_point9-1)
 pha
 lda #<(ret_point9-1)
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
ret_point9
.drop_item_done
 ; drop_item_done

.L0549 ;  goto slash_collide_end

 jmp .slash_collide_end

.item_survived
 ; item_survived

.L0550 ;  item_hp  =  item_hp  -  temp1

	LDA item_hp
	SEC
	SBC temp1
	STA item_hp
.L0551 ;  rem 'Push item around

.L0552 ;  if hero_flags{0} then item_y  =  item_y  -  4

	LDA hero_flags
	LSR
	BCC .skipL0552
.condpart55
	LDA item_y
	SEC
	SBC #4
	STA item_y
.skipL0552
.L0553 ;  if hero_flags{1} then item_x  =  item_x  +  4

	LDA hero_flags
	AND #2
	BEQ .skipL0553
.condpart56
	LDA item_x
	CLC
	ADC #4
	STA item_x
.skipL0553
.L0554 ;  if hero_flags{2} then item_y  =  item_y  +  4

	LDA hero_flags
	AND #4
	BEQ .skipL0554
.condpart57
	LDA item_y
	CLC
	ADC #4
	STA item_y
.skipL0554
.L0555 ;  if hero_flags{3} then item_x  =  item_x  -  4

	LDA hero_flags
	AND #8
	BEQ .skipL0555
.condpart58
	LDA item_x
	SEC
	SBC #4
	STA item_x
.skipL0555
.L0556 ;  goto slash_collide_end

 jmp .slash_collide_end

.slash_item_collide
 ; slash_item_collide

.L0557 ;  rem 'Make a hit sound once

.L0558 ;  sound_timer = 1

	LDA #1
	STA sound_timer
.L0559 ;  AUDC1 = 8

	LDA #8
	STA AUDC1
.L0560 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0561 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0562 ;  if item_kind  <  item_switch1 then goto slash_item_pickup

	LDA item_kind
	CMP #item_switch1
     BCS .skipL0562
.condpart59
 jmp .slash_item_pickup

.skipL0562
.L0563 ;  if item_kind  >  item_switch7_on then goto slash_item_pickup

	LDA #item_switch7_on
	CMP item_kind
     BCS .skipL0563
.condpart60
 jmp .slash_item_pickup

.skipL0563
.L0564 ;  rem 'If colliding with an item that's not a switch, pick it up.

.L0565 ;  rem 'Otherwise it's a switch that's hit.  

.L0566 ;  goto hit_switch

 jmp .hit_switch

.slash_item_pickup
 ; slash_item_pickup

.L0567 ;  gosub item_pickup

 jsr .item_pickup

.L0568 ;  gosub show_inventory

 jsr .show_inventory

.L0569 ;  goto slash_collide_end

 jmp .slash_collide_end

.hit_switch
 ; hit_switch

.L0570 ;  rem 'Do nothing with the already triggered switches. 

.L0571 ;  rem '(Turn them off again, perhaps?)

.L0572 ;  if item_kind  >  item_switch7 then goto slash_collide_end

	LDA #item_switch7
	CMP item_kind
     BCS .skipL0572
.condpart61
 jmp .slash_collide_end

.skipL0572
.L0573 ;  rem 'Handle hitting of the switch: set the quest flag and change the display.

.L0574 ;  if item_kind  =  item_switch1 then quest_flags{1}  =  1

	LDA item_kind
	CMP #item_switch1
     BNE .skipL0574
.condpart62
	LDA quest_flags
	ORA #2
	STA quest_flags
.skipL0574
.L0575 ;  if item_kind  =  item_switch2 then quest_flags{2}  =  1

	LDA item_kind
	CMP #item_switch2
     BNE .skipL0575
.condpart63
	LDA quest_flags
	ORA #4
	STA quest_flags
.skipL0575
.L0576 ;  if item_kind  =  item_switch3 then quest_flags{3}  =  1

	LDA item_kind
	CMP #item_switch3
     BNE .skipL0576
.condpart64
	LDA quest_flags
	ORA #8
	STA quest_flags
.skipL0576
.L0577 ;  if item_kind  =  item_switch4 then quest_flags{4}  =  1

	LDA item_kind
	CMP #item_switch4
     BNE .skipL0577
.condpart65
	LDA quest_flags
	ORA #16
	STA quest_flags
.skipL0577
.L0578 ;  if item_kind  =  item_switch5 then quest_flags{5}  =  1

	LDA item_kind
	CMP #item_switch5
     BNE .skipL0578
.condpart66
	LDA quest_flags
	ORA #32
	STA quest_flags
.skipL0578
.L0579 ;  if item_kind  =  item_switch6 then quest_flags{6}  =  1

	LDA item_kind
	CMP #item_switch6
     BNE .skipL0579
.condpart67
	LDA quest_flags
	ORA #64
	STA quest_flags
.skipL0579
.L0580 ;  if item_kind  =  item_switch7 then quest_flags{7}  =  1

	LDA item_kind
	CMP #item_switch7
     BNE .skipL0580
.condpart68
	LDA quest_flags
	ORA #128
	STA quest_flags
.skipL0580
.L0581 ;  gosub item_setup_kind bank4

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
.slash_collide_end
 ; slash_collide_end

.
 ; 

.
 ; 

.
 ; 

.L0582 ;  rem 'Player action though button

.L0583 ;  if !joy0fire then goto hero_action_done

 lda #$80
 bit INPT4
	BEQ .skipL0583
.condpart69
 jmp .hero_action_done

.skipL0583
.L0584 ;  rem 'Skip all this if action is blocked.

.L0585 ;  if !hero_flags{4} then goto hero_action_blocked

	LDA hero_flags
	AND #16
	BNE .skipL0585
.condpart70
 jmp .hero_action_blocked

.skipL0585
.L0586 ;  if !hero_items{7} then goto hero_act_nostrike

	BIT hero_items
	BMI .skipL0586
.condpart71
 jmp .hero_act_nostrike

.skipL0586
.L0587 ;  if hero_mp  <  strike_cost then goto hero_act_nostrike

	LDA hero_mp
	CMP #strike_cost
     BCS .skipL0587
.condpart72
 jmp .hero_act_nostrike

.skipL0587
.L0588 ;  rem ' The b switch controls whether the book of striking is in use or not.

.L0589 ;  if switchleftb then goto hero_act_nostrike

 lda #$40
 bit SWCHB
	BNE .skipL0589
.condpart73
 jmp .hero_act_nostrike

.skipL0589
.L0590 ;  if item_kind  <  last_monster then goto hero_act_strike

	LDA item_kind
	CMP #last_monster
     BCS .skipL0590
.condpart74
 jmp .hero_act_strike

.skipL0590
.L0591 ;  if item_kind  >=  item_switch1  &&  item_kind  <=  item_switch7 then goto hero_act_strike

	LDA item_kind
	CMP #item_switch1
     BCC .skipL0591
.condpart75
	LDA #item_switch7
	CMP item_kind
     BCC .skip75then
.condpart76
 jmp .hero_act_strike

.skip75then
.skipL0591
.hero_act_nostrike
 ; hero_act_nostrike

.L0592 ;  temp1  =  hero_flags  &  %00001111

	LDA hero_flags
	AND #%00001111
	STA temp1
.L0593 ;  rem 'Use this to jump to the right attack action.

.L0594 ;  rem '          0     1=north 2=east 3=ne   4=south  5=n+s(emu onlu)  6=se 7=nse 

.L0595 ;  rem ' 8 = west

.L0596 ;  rem ' 9 10 11 12 13 14 15(all sides, emu only)    

.L0597 ;  on temp1 goto had hero_act_n hero_act_e hero_act_n hero_act_s hero_act_s hero_act_n hero_act_n hero_act_w hero_act_n hero_act_w hero_act_s had had had

	LDX temp1
	LDA .L0597jumptablehi,x
	PHA
	LDA .L0597jumptablelo,x
	PHA
	RTS
.L0597jumptablehi
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
.L0597jumptablelo
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

.L0598 ;  missile0x = hero_x  +  4

	LDA hero_x
	CLC
	ADC #4
	STA missile0x
.L0599 ;  missile0y = hero_y  -  14

	LDA hero_y
	SEC
	SBC #14
	STA missile0y
.L0600 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0601 ;  NUSIZ0 = $00

	LDA #$00
	STA NUSIZ0
.L0602 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_e
 ; hero_act_e

.L0603 ;  missile0x = hero_x  +  9

	LDA hero_x
	CLC
	ADC #9
	STA missile0x
.L0604 ;  missile0y = hero_y  -  4

	LDA hero_y
	SEC
	SBC #4
	STA missile0y
.L0605 ;  missile0height = 0

	LDA #0
	STA missile0height
.L0606 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0607 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_s
 ; hero_act_s

.L0608 ;  missile0x = hero_x  +  4

	LDA hero_x
	CLC
	ADC #4
	STA missile0x
.L0609 ;  missile0y = hero_y  +  8

	LDA hero_y
	CLC
	ADC #8
	STA missile0y
.L0610 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0611 ;  NUSIZ0 = $00

	LDA #$00
	STA NUSIZ0
.L0612 ;  goto hero_action_end

 jmp .hero_action_end

.hero_act_w
 ; hero_act_w

.L0613 ;  missile0x = hero_x  -  7

	LDA hero_x
	SEC
	SBC #7
	STA missile0x
.L0614 ;  missile0y = hero_y  -  4

	LDA hero_y
	SEC
	SBC #4
	STA missile0y
.L0615 ;  missile0height = 0

	LDA #0
	STA missile0height
.L0616 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0617 ;  goto hero_action_end

 jmp .hero_action_end

.L0618 ;  rem NUSIZ0=$00

.hero_act_strike
 ; hero_act_strike

.L0619 ;  hero_mp  =  hero_mp  -  strike_cost

	LDA hero_mp
	SEC
	SBC #strike_cost
	STA hero_mp
.L0620 ;  gosub set_mp_display

 jsr .set_mp_display

.L0621 ;  rem 'Strike will hit the ennemy wherever it is. A guaranteed hit I'd say ^_^

.L0622 ;  missile0x = item_x

	LDA item_x
	STA missile0x
.L0623 ;  missile0y = item_y

	LDA item_y
	STA missile0y
.L0624 ;  missile0height = 8

	LDA #8
	STA missile0height
.L0625 ;  NUSIZ0 = $30

	LDA #$30
	STA NUSIZ0
.L0626 ;  COLUP0 = rand

 sta temp7
 lda #>(ret_point11-1)
 pha
 lda #<(ret_point11-1)
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
ret_point11
	STA COLUP0
.L0627 ;  COLUP1 = red

	LDA #red
	STA COLUP1
.L0628 ;  goto hero_action_end

 jmp .hero_action_end

.had
 ; had

.hero_action_done
 ; hero_action_done

.L0629 ;  hero_flags{4}  =  1

	LDA hero_flags
	ORA #16
	STA hero_flags
.L0630 ;  rem 'Allow attacks again

.hero_action_blocked
 ; hero_action_blocked

.L0631 ;  rem 'If the action is blocked or released, hide the missile

.L0632 ;  missile0x  =  nowhere

	LDA #nowhere
	STA missile0x
.L0633 ;  missile0y  =  nowhere

	LDA #nowhere
	STA missile0y
.hero_action_end
 ; hero_action_end

.
 ; 

.L0634 ;  rem 'Deterimine hero's motion and direction from the joystick

.L0635 ;  rem 'In NESW order  

.L0636 ;  rem ' hero_flags = (hero_flags / 16) * 16 preserves the higher nibble (I hope)

.L0637 ;  rem '

.L0638 ;  rem 'For the player collision detection, we calculate the playfield position 

.L0639 ;  rem 'at any of the four points around the feet of the player, and check if 

.L0640 ;  rem 'if anything is there with pfread, and prevent motion if it is so.  

.L0641 ;  rem ' Note that we do allow Signe to turn her facing even when she cannot move.

.
 ; 

.L0642 ;  if !joy0up then goto joy0up_end

 lda #$10
 bit SWCHA
	BEQ .skipL0642
.condpart77
 jmp .joy0up_end

.skipL0642
.L0643 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

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
.L0644 ;  hero_flags{0}  =  1

	LDA hero_flags
	ORA #1
	STA hero_flags
.L0645 ;  rem 'Check top middle point 

.L0646 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0647 ;  temp2  =   ( hero_y  -  hero_high  -  1  -  1 )   /  8

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
.L0648 ;  if pfread ( temp1 ,  temp2 )  goto joy0up_end

	LDA temp1
	LDY temp2
 sta temp7
 lda #>(ret_point12-1)
 pha
 lda #<(ret_point12-1)
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
ret_point12
 if ((* - .joy0up_end) < 127) && ((* - .joy0up_end) > -128)
	BEQ .joy0up_end
 else
	bne .4skipjoy0up_end
	jmp .joy0up_end
.4skipjoy0up_end
 endif
.L0649 ;  hero_y  =  hero_y  -  1

	DEC hero_y
.joy0up_end
 ; joy0up_end

.L0650 ;  if !joy0right then goto joy0right_end

 lda #$80
 bit SWCHA
	BEQ .skipL0650
.condpart78
 jmp .joy0right_end

.skipL0650
.L0651 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

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
.L0652 ;  hero_flags{1}  =  1

	LDA hero_flags
	ORA #2
	STA hero_flags
.L0653 ;  temp1  =   ( hero_x  +  hero_wide  +  1  -  17 )   /  4

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
.L0654 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

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
.L0655 ;  rem 'Prevent motion if something is there

.L0656 ;  if pfread ( temp1 ,  temp2 )  goto joy0right_end

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
 if ((* - .joy0right_end) < 127) && ((* - .joy0right_end) > -128)
	BEQ .joy0right_end
 else
	bne .5skipjoy0right_end
	jmp .joy0right_end
.5skipjoy0right_end
 endif
.L0657 ;  hero_x  =  hero_x  +  1

	INC hero_x
.joy0right_end
 ; joy0right_end

.L0658 ;  if !joy0down then goto joy0down_end

 lda #$20
 bit SWCHA
	BEQ .skipL0658
.condpart79
 jmp .joy0down_end

.skipL0658
.L0659 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

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
.L0660 ;  hero_flags{2}  =  1

	LDA hero_flags
	ORA #4
	STA hero_flags
.L0661 ;  temp1  =   ( hero_x  +  hero_half_wide  -  17 )   /  4

; complex statement detected
	LDA hero_x
	CLC
	ADC #hero_half_wide
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0662 ;  temp2  =   ( hero_y  +  1  -  1 )   /  8

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
.L0663 ;  rem 'Check bottom middle point. don't move if it's blocked by the playfield.

.L0664 ;  if pfread ( temp1 ,  temp2 )  then goto joy0down_end

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
	BNE .skipL0664
.condpart80
 jmp .joy0down_end

.skipL0664
.L0665 ;  hero_y  =  hero_y  +  1

	INC hero_y
.joy0down_end
 ; joy0down_end

.L0666 ;  if !joy0left then goto joy0left_end

 lda #$40
 bit SWCHA
	BEQ .skipL0666
.condpart81
 jmp .joy0left_end

.skipL0666
.L0667 ;  hero_flags  =   ( hero_flags  /  16 )   *  16

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
.L0668 ;  hero_flags{3}  =  1

	LDA hero_flags
	ORA #8
	STA hero_flags
.L0669 ;  temp1  =   ( hero_x  -  1  -  17 )   /  4

; complex statement detected
	LDA hero_x
	SEC
	SBC #1
	SEC
	SBC #17
	lsr
	lsr
	STA temp1
.L0670 ;  temp2  =   ( hero_y  -  hero_half_high  -  1 )   /  8

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
.L0671 ;  rem 'Check point to the west of the feet 

.L0672 ;  if pfread ( temp1 ,  temp2 )  goto joy0left_end

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
 if ((* - .joy0left_end) < 127) && ((* - .joy0left_end) > -128)
	BEQ .joy0left_end
 else
	bne .6skipjoy0left_end
	jmp .joy0left_end
.6skipjoy0left_end
 endif
.L0673 ;  hero_x  =  hero_x  -  1

	DEC hero_x
.joy0left_end
 ; joy0left_end

.
 ; 

.L0674 ;  rem 'Monster "AI":

.L0675 ;  rem 'Animate missile if it has been launched

.L0676 ;  if missile1y  =  nowhere then goto missile_move_end

	LDA missile1y
	CMP #nowhere
     BNE .skipL0676
.condpart82
 jmp .missile_move_end

.skipL0676
.L0677 ;  if item_flags{3} then missile1x  =  missile1x  -  2  :  goto missile_move_end

	LDA item_flags
	AND #8
	BEQ .skipL0677
.condpart83
	LDA missile1x
	SEC
	SBC #2
	STA missile1x
 jmp .missile_move_end

.skipL0677
.L0678 ;  if item_flags{0} then missile1y  =  missile1y  -  2  :  goto missile_move_end

	LDA item_flags
	LSR
	BCC .skipL0678
.condpart84
	LDA missile1y
	SEC
	SBC #2
	STA missile1y
 jmp .missile_move_end

.skipL0678
.L0679 ;  if item_flags{1} then missile1x  =  missile1x  +  2  :  goto missile_move_end

	LDA item_flags
	AND #2
	BEQ .skipL0679
.condpart85
	LDA missile1x
	CLC
	ADC #2
	STA missile1x
 jmp .missile_move_end

.skipL0679
.L0680 ;  if item_flags{2} then missile1y  =  missile1y  +  2  :  goto missile_move_end

	LDA item_flags
	AND #4
	BEQ .skipL0680
.condpart86
	LDA missile1y
	CLC
	ADC #2
	STA missile1y
 jmp .missile_move_end

.skipL0680
.missile_move_end
 ; missile_move_end

.
 ; 

.L0681 ;  rem 'Move the item if it's a mobile (id lower than 128)

.L0682 ;  if item_kind{7} then goto item_move_end

	BIT item_kind
	BPL .skipL0682
.condpart87
 jmp .item_move_end

.skipL0682
.L0683 ;  temp1  =  rand

 sta temp7
 lda #>(ret_point16-1)
 pha
 lda #<(ret_point16-1)
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
ret_point16
	STA temp1
.L0684 ;  rem ' Shoot a missile sometimes...

.L0685 ;  temp6  =  monster_info[item_kind]

	LDX item_kind
	LDA monster_info,x
	STA temp6
.L0686 ;  rem 'But not if the monster cannot shoot (bit 0)

.L0687 ;  if !temp6{0} then goto activate_missile_end

	LDA temp6
	LSR
	BCS .skipL0687
.condpart88
 jmp .activate_missile_end

.skipL0687
.L0688 ;  if !game_timer{4} then goto activate_missile_end

	LDA game_timer
	AND #16
	BNE .skipL0688
.condpart89
 jmp .activate_missile_end

.skipL0688
.L0689 ;  if !game_timer{7} then goto activate_missile_end

	BIT game_timer
	BMI .skipL0689
.condpart90
 jmp .activate_missile_end

.skipL0689
.L0690 ;  if !temp1{0} then goto activate_missile_end

	LDA temp1
	LSR
	BCS .skipL0690
.condpart91
 jmp .activate_missile_end

.skipL0690
.L0691 ;  if missile1y  <>  nowhere then goto activate_missile_end

	LDA missile1y
	CMP #nowhere
     BEQ .skipL0691
.condpart92
 jmp .activate_missile_end

.skipL0691
.L0692 ;  rem 'Make a shot sound

.L0693 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0694 ;  AUDC1 = 14

	LDA #14
	STA AUDC1
.L0695 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0696 ;  AUDF1 = 20

	LDA #20
	STA AUDF1
.L0697 ;  rem 'Place the missile  

.L0698 ;  missile1x  =  item_x  +  4

	LDA item_x
	CLC
	ADC #4
	STA missile1x
.L0699 ;  missile1y  =  item_y  -  4

	LDA item_y
	SEC
	SBC #4
	STA missile1y
.L0700 ;  temp4  =  item_x  +  8

	LDA item_x
	CLC
	ADC #8
	STA temp4
.L0701 ;  if hero_x  >  temp4 then item_flags{1}  =  1

	LDA temp4
	CMP hero_x
     BCS .skipL0701
.condpart93
	LDA item_flags
	ORA #2
	STA item_flags
.skipL0701
.L0702 ;  temp4  =  item_x  -  8

	LDA item_x
	SEC
	SBC #8
	STA temp4
.L0703 ;  if hero_x  <  temp4 then item_flags{3}  =  1

	LDA hero_x
	CMP temp4
     BCS .skipL0703
.condpart94
	LDA item_flags
	ORA #8
	STA item_flags
.skipL0703
.L0704 ;  if hero_y  >  item_y then item_flags{2}  =  1

	LDA item_y
	CMP hero_y
     BCS .skipL0704
.condpart95
	LDA item_flags
	ORA #4
	STA item_flags
.skipL0704
.L0705 ;  if hero_y  <  item_y then item_flags{0}  =  1

	LDA hero_y
	CMP item_y
     BCS .skipL0705
.condpart96
	LDA item_flags
	ORA #1
	STA item_flags
.skipL0705
.L0706 ;  missile1height = 4

	LDA #4
	STA missile1height
.activate_missile_end
 ; activate_missile_end

.L0707 ;  rem 'Agressive chasing if aggressive flag is set 

.L0708 ;  if temp6{2} then goto monster_chase

	LDA temp6
	AND #4
	BEQ .skipL0708
.condpart97
 jmp .monster_chase

.skipL0708
.L0709 ;  rem 'Don't move if monster is static 

.L0710 ;  if temp6{3} then goto item_move_end

	LDA temp6
	AND #8
	BEQ .skipL0710
.condpart98
 jmp .item_move_end

.skipL0710
.L0711 ;  rem 'Only move half the time

.L0712 ;  if game_timer{2} goto item_move_end

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

.L0713 ;  rem 'Only move if the 4th and 5th bit are not set

.L0714 ;  if temp1{1} then goto item_move_end

	LDA temp1
	AND #2
	BEQ .skipL0714
.condpart99
 jmp .item_move_end

.skipL0714
.L0715 ;  rem 'Random walking monsters don't give chase

.L0716 ;  if temp6{1} then goto monster_random_walk

	LDA temp6
	AND #2
	BEQ .skipL0716
.condpart100
 jmp .monster_random_walk

.skipL0716
.monster_chase
 ; monster_chase

.L0717 ;  if hero_x  >  item_x then item_x  =  item_x  +  1 else item_x  =  item_x  -  1

	LDA item_x
	CMP hero_x
     BCS .skipL0717
.condpart101
	INC item_x
 jmp .skipelse0
.skipL0717
	DEC item_x
.skipelse0
.L0718 ;  if hero_y  >  item_y then item_y  =  item_y  +  1 else item_y  =  item_y  -  1

	LDA item_y
	CMP hero_y
     BCS .skipL0718
.condpart102
	INC item_y
 jmp .skipelse1
.skipL0718
	DEC item_y
.skipelse1
.L0719 ;  if temp6{2} then goto item_move_end

	LDA temp6
	AND #4
	BEQ .skipL0719
.condpart103
 jmp .item_move_end

.skipL0719
.L0720 ;  rem 'Aggressive ennemies don't random walk

.monster_random_walk
 ; monster_random_walk

.L0721 ;  rem 'Use bits of random number for random walk.

.L0722 ;  if temp1{0} then item_y  =  item_y  -  1

	LDA temp1
	LSR
	BCC .skipL0722
.condpart104
	DEC item_y
.skipL0722
.L0723 ;  if temp1{4} then item_x  =  item_x  -  1

	LDA temp1
	AND #16
	BEQ .skipL0723
.condpart105
	DEC item_x
.skipL0723
.L0724 ;  if temp1{0}  ||  temp1{4} then goto item_move_end

	LDA temp1
	LSR
	BCC .skipL0724
.condpart106
 jmp .condpart107
.skipL0724
	LDA temp1
	AND #16
	BEQ .skip6OR
.condpart107
 jmp .item_move_end

.skip6OR
.L0725 ;  if temp1{2} then item_y  =  item_y  +  1

	LDA temp1
	AND #4
	BEQ .skipL0725
.condpart108
	INC item_y
.skipL0725
.L0726 ;  if temp1{6} then item_x  =  item_x  +  1

	BIT temp1
	BVC .skipL0726
.condpart109
	INC item_x
.skipL0726
.item_move_end
 ; item_move_end

.
 ; 

.L0727 ;  rem 'Activate missile if needed

.
 ; 

.L0728 ;  rem 'Debug by showing flags in score field

.L0729 ;  rem sc0 = hero_flags

.L0730 ;  rem temp2 = monster_info[item_kind]

.L0731 ;  rem sc1 = temp2

.L0732 ;  rem sc2 = hero_room

.
 ; 

.L0733 ;  rem 'Music, disabled for now.

.L0734 ;  rem 'Is it time to update the note? 

.L0735 ;  rem if music_timer = 0 then gosub music_change_note

.L0736 ;  rem music_timer = music_timer - 1

.
 ; 

.
 ; 

.
 ; 

.L0737 ;  rem 'Collision op hero with edge of field, transfer to other field.

.L0738 ;  if field_left  <  hero_x then goto exit_left_done

	LDA #field_left
	CMP hero_x
     BCS .skipL0738
.condpart110
 jmp .exit_left_done

.skipL0738
.L0739 ;  hero_x  =  field_right_enter

	LDA #field_right_enter
	STA hero_x
.L0740 ;  hero_oldx  =  field_right

	LDA #field_right
	STA hero_oldx
.L0741 ;  hero_room  =  hero_room  -  1

	DEC hero_room
.L0742 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point17-1)
 pha
 lda #<(ret_point17-1)
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
ret_point17
.L0743 ;  goto exit_done

 jmp .exit_done

.exit_left_done
 ; exit_left_done

.L0744 ;  if hero_x  <  field_right then goto exit_right_done

	LDA hero_x
	CMP #field_right
     BCS .skipL0744
.condpart111
 jmp .exit_right_done

.skipL0744
.L0745 ;  hero_x  =  field_left_enter

	LDA #field_left_enter
	STA hero_x
.L0746 ;  hero_oldx  =  field_left

	LDA #field_left
	STA hero_oldx
.L0747 ;  hero_room  =  hero_room  +  1

	INC hero_room
.L0748 ;  gosub room_draw bank2

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
.L0749 ;  goto exit_done

 jmp .exit_done

.exit_right_done
 ; exit_right_done

.L0750 ;  if hero_y  >  field_top then goto exit_top_done

	LDA #field_top
	CMP hero_y
     BCS .skipL0750
.condpart112
 jmp .exit_top_done

.skipL0750
.L0751 ;  hero_y  =  field_bottom_enter

	LDA #field_bottom_enter
	STA hero_y
.L0752 ;  hero_oldy  =  field_bottom

	LDA #field_bottom
	STA hero_oldy
.L0753 ;  hero_room  =  hero_room  -  8

	LDA hero_room
	SEC
	SBC #8
	STA hero_room
.L0754 ;  gosub room_draw bank2

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
.L0755 ;  goto exit_done

 jmp .exit_done

.exit_top_done
 ; exit_top_done

.L0756 ;  if hero_y  <  field_bottom then goto exit_bottom_done

	LDA hero_y
	CMP #field_bottom
     BCS .skipL0756
.condpart113
 jmp .exit_bottom_done

.skipL0756
.L0757 ;  hero_y  =  field_top_enter

	LDA #field_top_enter
	STA hero_y
.L0758 ;  hero_oldy  =  field_top

	LDA #field_top
	STA hero_oldy
.L0759 ;  hero_room  =  hero_room  +  8

	LDA hero_room
	CLC
	ADC #8
	STA hero_room
.L0760 ;  gosub room_draw bank2

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
.L0761 ;  goto exit_done

 jmp .exit_done

.exit_bottom_done
 ; exit_bottom_done

.exit_done
 ; exit_done

.
 ; 

.L0762 ;  rem 'Collision of item with edge of field, bounce back.

.L0763 ;  if item_x  <  field_left then item_x = field_left_enter

	LDA item_x
	CMP #field_left
     BCS .skipL0763
.condpart114
	LDA #field_left_enter
	STA item_x
.skipL0763
.L0764 ;  if item_x  >  field_right then item_x = field_right_enter

	LDA #field_right
	CMP item_x
     BCS .skipL0764
.condpart115
	LDA #field_right_enter
	STA item_x
.skipL0764
.L0765 ;  if item_y  <  field_top then item_y = field_top_enter

	LDA item_y
	CMP #field_top
     BCS .skipL0765
.condpart116
	LDA #field_top_enter
	STA item_y
.skipL0765
.L0766 ;  if item_y  >  field_bottom then item_x = field_bottom_enter

	LDA #field_bottom
	CMP item_y
     BCS .skipL0766
.condpart117
	LDA #field_bottom_enter
	STA item_x
.skipL0766
.
 ; 

.
 ; 

.L0767 ;  rem 'Draw acting player in position 2

.L0768 ;  if joy0fire goto player_animate_2

 lda #$80
 bit INPT4
 if ((* - .player_animate_2) < 127) && ((* - .player_animate_2) > -128)
	BEQ .player_animate_2
 else
	bne .8skipplayer_animate_2
	jmp .player_animate_2
.8skipplayer_animate_2
 endif
.L0769 ;  rem 'Draw non moving player not animated

.L0770 ;  if hero_x  =  hero_oldx  &&  hero_y  =  hero_oldy then goto player_animate_1

	LDA hero_x
	CMP hero_oldx
     BNE .skipL0770
.condpart118
	LDA hero_y
	CMP hero_oldy
     BNE .skip118then
.condpart119
 jmp .player_animate_1

.skip118then
.skipL0770
.L0771 ;  rem 'Draw and animate player every so many ticks

.L0772 ;  if !game_timer{4} then goto player_animate_2

	LDA game_timer
	AND #16
	BNE .skipL0772
.condpart120
 jmp .player_animate_2

.skipL0772
.player_animate_1
 ; player_animate_1

.L0773 ;  if hero_flags{0} then gosub hero_draw_n

	LDA hero_flags
	LSR
	BCC .skipL0773
.condpart121
 jsr .hero_draw_n

.skipL0773
.L0774 ;  if hero_flags{1} then gosub hero_draw_e

	LDA hero_flags
	AND #2
	BEQ .skipL0774
.condpart122
 jsr .hero_draw_e

.skipL0774
.L0775 ;  if hero_flags{2} then gosub hero_draw_s

	LDA hero_flags
	AND #4
	BEQ .skipL0775
.condpart123
 jsr .hero_draw_s

.skipL0775
.L0776 ;  if hero_flags{3} then gosub hero_draw_w

	LDA hero_flags
	AND #8
	BEQ .skipL0776
.condpart124
 jsr .hero_draw_w

.skipL0776
.L0777 ;  goto player_animate_end

 jmp .player_animate_end

.player_animate_2
 ; player_animate_2

.L0778 ;  if hero_flags{0} then gosub hero_draw_n2

	LDA hero_flags
	LSR
	BCC .skipL0778
.condpart125
 jsr .hero_draw_n2

.skipL0778
.L0779 ;  if hero_flags{1} then gosub hero_draw_e2

	LDA hero_flags
	AND #2
	BEQ .skipL0779
.condpart126
 jsr .hero_draw_e2

.skipL0779
.L0780 ;  if hero_flags{2} then gosub hero_draw_s2

	LDA hero_flags
	AND #4
	BEQ .skipL0780
.condpart127
 jsr .hero_draw_s2

.skipL0780
.L0781 ;  if hero_flags{3} then gosub hero_draw_w2

	LDA hero_flags
	AND #8
	BEQ .skipL0781
.condpart128
 jsr .hero_draw_w2

.skipL0781
.player_animate_end
 ; player_animate_end

.
 ; 

.L0782 ;  rem 'Reflect sprite if moving west, otherwise, don't 

.L0783 ;  if hero_flags{3} then REFP0  =  8 else REFP0  =  0

	LDA hero_flags
	AND #8
	BEQ .skipL0783
.condpart129
	LDA #8
	STA REFP0
 jmp .skipelse2
.skipL0783
	LDA #0
	STA REFP0
.skipelse2
.
 ; 

.
 ; 

.
 ; 

.L0784 ;  rem draw screen

.L0785 ;  drawscreen

 sta temp7
 lda #>(ret_point21-1)
 pha
 lda #<(ret_point21-1)
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
ret_point21
.
 ; 

.L0786 ;  rem And continue main loop

.L0787 ;  goto main_loop

 jmp .main_loop

.
 ; 

.L0788 ;  rem 'Vblank sub. Do something useful in here...

.vblank
 ; vblank

.L0789 ;  rem 'Do something here...

.L0790 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.L0791 ;  rem 'Gosub that correctly displays MP

.set_mp_display
 ; set_mp_display

.L0792 ;  lives = hero_mp  *  32

	LDA hero_mp
	asl
	asl
	asl
	asl
	asl
	STA lives
.L0793 ;  lives:

	LDA #<lives__L0793
	STA lifepointer
	LDA lifepointer+1
	AND #$E0
	ORA #(>lives__L0793)&($1F)
	STA lifepointer+1
.L0794 ;  return

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

.L0795 ;  rem 'Display inventory ggosub

.show_inventory
 ; show_inventory

.L0796 ;  sc0  =  0

	LDA #0
	STA sc0
.L0797 ;  sc1  =  0

	LDA #0
	STA sc1
.L0798 ;  sc2  =  0

	LDA #0
	STA sc2
.L0799 ;  rem 'Add up score to display Numen Leaves

.L0800 ;  if hero_items{0} then sc0  =  sc0  +  16

	LDA hero_items
	LSR
	BCC .skipL0800
.condpart130
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0800
.L0801 ;  if hero_items{1} then sc0  =  sc0  +  16

	LDA hero_items
	AND #2
	BEQ .skipL0801
.condpart131
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0801
.L0802 ;  if hero_items{2} then sc0  =  sc0  +  16

	LDA hero_items
	AND #4
	BEQ .skipL0802
.condpart132
	LDA sc0
	CLC
	ADC #16
	STA sc0
.skipL0802
.L0803 ;  rem 'Display shield if we have it

.L0804 ;  if hero_items{3} then sc0  =  sc0  +  4

	LDA hero_items
	AND #8
	BEQ .skipL0804
.condpart133
	LDA sc0
	CLC
	ADC #4
	STA sc0
.skipL0804
.L0805 ;  rem 'Display Armor

.L0806 ;  if hero_items{4} then sc1  =  sc1  +  80

	LDA hero_items
	AND #16
	BEQ .skipL0806
.condpart134
	LDA sc1
	CLC
	ADC #80
	STA sc1
.skipL0806
.L0807 ;  rem 'Display Sword

.L0808 ;  if hero_items{5} then sc1  =  sc1  +  6

	LDA hero_items
	AND #32
	BEQ .skipL0808
.condpart135
	LDA sc1
	CLC
	ADC #6
	STA sc1
.skipL0808
.L0809 ;  if hero_items{6} then sc2  =  sc2  +  112

	BIT hero_items
	BVC .skipL0809
.condpart136
	LDA sc2
	CLC
	ADC #112
	STA sc2
.skipL0809
.L0810 ;  if hero_items{7} then sc2  =  sc2  +  8

	BIT hero_items
	BPL .skipL0810
.condpart137
	LDA sc2
	CLC
	ADC #8
	STA sc2
.skipL0810
.L0811 ;  return

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

.L0812 ;  rem 'Item Pickup gosub

.item_pickup
 ; item_pickup

.L0813 ;  rem 'All sorts of item pickup effects, should be in a gosub, I guess

.L0814 ;  rem 'Win game if finding victory item

.L0815 ;  rem 'Make a pickup sound

.L0816 ;  sound_timer = 2

	LDA #2
	STA sound_timer
.L0817 ;  AUDC1 = 12

	LDA #12
	STA AUDC1
.L0818 ;  AUDV1 = 8

	LDA #8
	STA AUDV1
.L0819 ;  AUDF1 = 2

	LDA #2
	STA AUDF1
.L0820 ;  if item_kind  =  item_victory then goto game_win bank5

	LDA item_kind
	CMP #item_victory
     BNE .skipL0820
.condpart138
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
.skipL0820
.L0821 ;  if item_kind  =  item_leaf1 then hero_items{0}  =  1

	LDA item_kind
	CMP #item_leaf1
     BNE .skipL0821
.condpart139
	LDA hero_items
	ORA #1
	STA hero_items
.skipL0821
.L0822 ;  if item_kind  =  item_leaf2 then hero_items{1}  =  1

	LDA item_kind
	CMP #item_leaf2
     BNE .skipL0822
.condpart140
	LDA hero_items
	ORA #2
	STA hero_items
.skipL0822
.L0823 ;  if item_kind  =  item_leaf3 then hero_items{2}  =  1

	LDA item_kind
	CMP #item_leaf3
     BNE .skipL0823
.condpart141
	LDA hero_items
	ORA #4
	STA hero_items
.skipL0823
.L0824 ;  if item_kind  =  item_shield then hero_items{3}  =  1

	LDA item_kind
	CMP #item_shield
     BNE .skipL0824
.condpart142
	LDA hero_items
	ORA #8
	STA hero_items
.skipL0824
.L0825 ;  if item_kind  =  item_armor then hero_items{4}  =  1

	LDA item_kind
	CMP #item_armor
     BNE .skipL0825
.condpart143
	LDA hero_items
	ORA #16
	STA hero_items
.skipL0825
.L0826 ;  if item_kind  =  item_sword then hero_items{5}  =  1

	LDA item_kind
	CMP #item_sword
     BNE .skipL0826
.condpart144
	LDA hero_items
	ORA #32
	STA hero_items
.skipL0826
.L0827 ;  if item_kind  =  item_bookheal then hero_items{6}  =  1

	LDA item_kind
	CMP #item_bookheal
     BNE .skipL0827
.condpart145
	LDA hero_items
	ORA #64
	STA hero_items
.skipL0827
.L0828 ;  if item_kind  =  item_bookstrike then hero_items{7}  =  1

	LDA item_kind
	CMP #item_bookstrike
     BNE .skipL0828
.condpart146
	LDA hero_items
	ORA #128
	STA hero_items
.skipL0828
.L0829 ;  if item_kind  <>  item_healhp then goto no_healhp

	LDA item_kind
	CMP #item_healhp
     BEQ .skipL0829
.condpart147
 jmp .no_healhp

.skipL0829
.L0830 ;  hero_hp  =  hero_hp  +  8

	LDA hero_hp
	CLC
	ADC #8
	STA hero_hp
.L0831 ;  temp2  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA temp2
.L0832 ;  if hero_hp  >  temp2 then hero_hp  =  temp2

	LDA temp2
	CMP hero_hp
     BCS .skipL0832
.condpart148
	LDA temp2
	STA hero_hp
.skipL0832
.no_healhp
 ; no_healhp

.L0833 ;  if item_kind  <>  item_healmp then goto no_healmp

	LDA item_kind
	CMP #item_healmp
     BEQ .skipL0833
.condpart149
 jmp .no_healmp

.skipL0833
.L0834 ;  hero_mp  =  hero_mp  +  1

	INC hero_mp
.L0835 ;  temp2  =  hero_level  /  16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA temp2
.L0836 ;  if hero_mp  >  temp2 then hero_mp  =  temp2

	LDA temp2
	CMP hero_mp
     BCS .skipL0836
.condpart150
	LDA temp2
	STA hero_mp
.skipL0836
.L0837 ;  gosub set_mp_display

 jsr .set_mp_display

.no_healmp
 ; no_healmp

.L0838 ;  if item_kind  <>  item_healallhp then goto no_healallhp

	LDA item_kind
	CMP #item_healallhp
     BEQ .skipL0838
.condpart151
 jmp .no_healallhp

.skipL0838
.L0839 ;  hero_hp  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.no_healallhp
 ; no_healallhp

.L0840 ;  if item_kind  <>  item_healallmp then goto no_healallmp

	LDA item_kind
	CMP #item_healallmp
     BEQ .skipL0840
.condpart152
 jmp .no_healallmp

.skipL0840
.L0841 ;  rem 'Actually, this is buggy for levels above 80

.L0842 ;  hero_mp  =  hero_level / 16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L0843 ;  gosub set_mp_display

 jsr .set_mp_display

.no_healallmp
 ; no_healallmp

.L0844 ;  rem " All done with item effects 

.
 ; 

.L0845 ;  rem ' Remove item

.L0846 ;  item_kind  =  item_none

	LDA #item_none
	STA item_kind
.L0847 ;  item_hp  =  0

	LDA #0
	STA item_hp
.L0848 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L0849 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
.L0850 ;  return

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

.L0851 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0852 ;  player0:

	LDA #<playerL0852_0

	STA player0pointerlo
	LDA #>playerL0852_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0853 ;  return

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

.L0854 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0855 ;  player0:

	LDA #<playerL0855_0

	STA player0pointerlo
	LDA #>playerL0855_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0856 ;  return

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

.L0857 ;  REFP0 = 8

	LDA #8
	STA REFP0
.L0858 ;  goto hero_draw_e_start

 jmp .hero_draw_e_start

.hero_draw_e
 ; hero_draw_e

.L0859 ;  REFP0 = 0

	LDA #0
	STA REFP0
.hero_draw_e_start
 ; hero_draw_e_start

.L0860 ;  player0:

	LDA #<playerL0860_0

	STA player0pointerlo
	LDA #>playerL0860_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0861 ;  return

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

.L0862 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0863 ;  player0:

	LDA #<playerL0863_0

	STA player0pointerlo
	LDA #>playerL0863_0

	STA player0pointerhi
	LDA #14
	STA player0height
.L0864 ;  return

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

.L0865 ;  REFP0 = 0

	LDA #0
	STA REFP0
.L0866 ;  player0:

	LDA #<playerL0866_0

	STA player0pointerlo
	LDA #>playerL0866_0

	STA player0pointerhi
	LDA #14
	STA player0height
.L0867 ;  return

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

.L0868 ;  REFP0 = 8

	LDA #8
	STA REFP0
.L0869 ;  goto hero_draw_e2_start

 jmp .hero_draw_e2_start

.hero_draw_e2
 ; hero_draw_e2

.L0870 ;  REFP0 = 0

	LDA #0
	STA REFP0
.hero_draw_e2_start
 ; hero_draw_e2_start

.L0871 ;  player0:

	LDA #<playerL0871_0

	STA player0pointerlo
	LDA #>playerL0871_0

	STA player0pointerhi
	LDA #13
	STA player0height
.L0872 ;  return

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

.
 ; 

.
 ; 

.
 ; 

.music_change_note
 ; music_change_note

.L0873 ;  AUDF0  =  music_data[music_pointer]

	LDX music_pointer
	LDA music_data,x
	STA AUDF0
.L0874 ;   AUDC0 = music_instrument

	LDA #music_instrument
	STA AUDC0
.L0875 ;   if music_data[music_pointer] = $FF then AUDV0 = 0 else AUDV0 = 8

	LDX music_pointer
	LDA music_data,x
	CMP #$FF
     BNE .skipL0875
.condpart153
	LDA #0
	STA AUDV0
 jmp .skipelse3
.skipL0875
	LDA #8
	STA AUDV0
.skipelse3
.L0876 ;   music_pointer = music_pointer + 1  

	INC music_pointer
.L0877 ;   music_timer = music_data[music_pointer]

	LDX music_pointer
	LDA music_data,x
	STA music_timer
.L0878 ;   music_pointer = music_pointer + 1

	INC music_pointer
.L0879 ;   if music_pointer > music_notes then music_pointer = 0

	LDA #music_notes
	CMP music_pointer
     BCS .skipL0879
.condpart154
	LDA #0
	STA music_pointer
.skipL0879
.L0880 ;   return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.
 ; 

.L0881 ;   bank 2

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
.
 ; 

.L0882 ;   set kernel_options pfcolors

.
 ; 

.room_draw
 ; room_draw

.L0883 ;   rem 'Reset the monster missile and item flags

.L0884 ;   missile1x = nowhere

	LDA #nowhere
	STA missile1x
.L0885 ;   missile1y = nowhere

	LDA #nowhere
	STA missile1y
.L0886 ;   item_flags = 0

	LDA #0
	STA item_flags
.L0887 ;   rem 'Set up item and draw it

.L0888 ;   goto item_setup bank4

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

.
 ; 

.L0889 ;   if hero_room > 39 goto room_draw_40_local

	LDA #39
	CMP hero_room
 if ((* - .room_draw_40_local) < 127) && ((* - .room_draw_40_local) > -128)
	bcc .room_draw_40_local
 else
	bcs .9skiproom_draw_40_local
	jmp .room_draw_40_local
.9skiproom_draw_40_local
 endif
.L0890 ;   if hero_room > 63 goto room_draw_64

	LDA #63
	CMP hero_room
 if ((* - .room_draw_64) < 127) && ((* - .room_draw_64) > -128)
	bcc .room_draw_64
 else
	bcs .10skiproom_draw_64
	jmp .room_draw_64
.10skiproom_draw_64
 endif
.
 ; 

.L0891 ;   on hero_room goto r00 r01 r02 r03 r04 r05 r06 r07 r08 r09 r10 r11 r12 r13 r14 r15 r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31 r32 r33 r34 r35 r36  r37 r38 r39 r40

	LDX hero_room
	LDA .L0891jumptablehi,x
	PHA
	LDA .L0891jumptablelo,x
	PHA
	RTS
.L0891jumptablehi
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
.L0891jumptablelo
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
.L0892 ;   rem r41 r42 r43 r44 r45 r46 r47 r48 r49 r50

.L0893 ;   rem r51 r52 r53 r54 r55 r56 r57 r58 r59 r60 r61 r62 r63 r64

.
 ; 

.room_draw_40_local
 ; room_draw_40_local

.L0894 ;   goto room_draw_40 bank3

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
.
 ; 

.room_draw_64
 ; room_draw_64

.L0895 ;   rem 'If we get here, the room number s not valid. Go to room room_start (49)

.L0896 ;   hero_room = room_start

	LDA #room_start
	STA hero_room
.L0897 ;   goto r00

 jmp .r00

.
 ; 

.L0898 ;   rem r64

.L0899 ;   rem r65 r66 r67 r68 r69 r70 r71 r72 r73 r74 r75 r76 r77 r78 r79 r80 r81 r82 r83 r84 r85 r86 r87 r88 r89 r90 r91 r92 r93 r94 r95 r96 r97 r98 r99 r100

.room_draw_end
 ; room_draw_end

.L0900 ;   rem 'Clear bottom scrolling playfield row, so collision detection works properly

.L0901 ;   var44 = 0

	LDA #0
	STA var44
.L0902 ;   var45 = 0

	LDA #0
	STA var45
.L0903 ;   var46 = 0

	LDA #0
	STA var46
.L0904 ;   var47 = 0

	LDA #0
	STA var47
.L0905 ;   return otherbank

	JMP BS_return
.
 ; 

.
 ; 

.r00
 ; r00

.L0906 ;  pfcolors:

 lda # skyblue
 sta COLUPF
 lda #>(pfcolorlabel139-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-84)
 sta pfcolortable
.L0907 ;  playfield:

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
.L0908 ;  goto room_draw_end

 jmp .room_draw_end

.r01
 ; r01

.L0909 ;  if quest_flags{1} then COLUBK = gray else COLUBK = black

	LDA quest_flags
	AND #2
	BEQ .skipL0909
.condpart155
	LDA gray
	STA COLUBK
 jmp .skipelse4
.skipL0909
	LDA black
	STA COLUBK
.skipelse4
.L0910 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel139-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-83)
 sta pfcolortable
.L0911 ;  playfield:

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
.L0912 ;  goto room_draw_end

 jmp .room_draw_end

.r02
 ; r02

.L0913 ;  if quest_flags{1} then COLUBK = gray else COLUBK = black

	LDA quest_flags
	AND #2
	BEQ .skipL0913
.condpart156
	LDA gray
	STA COLUBK
 jmp .skipelse5
.skipL0913
	LDA black
	STA COLUBK
.skipelse5
.L0914 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel139-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-82)
 sta pfcolortable
.L0915 ;  playfield:

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
.L0916 ;  goto room_draw_end

 jmp .room_draw_end

.r03
 ; r03

.L0917 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L0918 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel139-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel139-81)
 sta pfcolortable
.L0919 ;  playfield:

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
.L0920 ;  goto room_draw_end

 jmp .room_draw_end

.r04
 ; r04

.L0921 ;  COLUBK = gray

	LDA gray
	STA COLUBK
.L0922 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel156-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-84)
 sta pfcolortable
.L0923 ;  playfield:

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
.L0924 ;  goto room_draw_end

 jmp .room_draw_end

.r05
 ; r05

.L0925 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L0926 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel156-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-83)
 sta pfcolortable
.L0927 ;  playfield:

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
.L0928 ;  goto room_draw_end

 jmp .room_draw_end

.r06
 ; r06

.L0929 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L0930 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel156-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-82)
 sta pfcolortable
.L0931 ;  playfield:

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
.L0932 ;  goto room_draw_end

 jmp .room_draw_end

.r07
 ; r07

.L0933 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L0934 ;  pfcolors:

 lda # darkgreen
 sta COLUPF
 lda #>(pfcolorlabel156-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel156-81)
 sta pfcolortable
.L0935 ;  playfield:

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
.L0936 ;  goto room_draw_end

 jmp .room_draw_end

.r08
 ; r08

.L0937 ;  COLUBK = brown

	LDA brown
	STA COLUBK
.L0938 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel173-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-84)
 sta pfcolortable
.L0939 ;  if quest_flags{3} goto r08_open

	LDA quest_flags
	AND #8
 if ((* - .r08_open) < 127) && ((* - .r08_open) > -128)
	BNE .r08_open
 else
	beq .11skipr08_open
	jmp .r08_open
.11skipr08_open
 endif
.L0940 ;  playfield:

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
.L0941 ;  goto r08_end

 jmp .r08_end

.r08_open
 ; r08_open

.L0942 ;  playfield:

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

.L0943 ;  goto room_draw_end

 jmp .room_draw_end

.r09
 ; r09

.L0944 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0945 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel173-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-83)
 sta pfcolortable
.L0946 ;  playfield:

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
.L0947 ;  goto room_draw_end

 jmp .room_draw_end

.r10
 ; r10

.L0948 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0949 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel173-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-82)
 sta pfcolortable
.L0950 ;  playfield:

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
.L0951 ;  goto room_draw_end

 jmp .room_draw_end

.r11
 ; r11

.L0952 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L0953 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel173-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel173-81)
 sta pfcolortable
.L0954 ;  playfield:

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
.L0955 ;  goto room_draw_end

 jmp .room_draw_end

.r12
 ; r12

.L0956 ;  COLUBK = gray

	LDA gray
	STA COLUBK
.L0957 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel190-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-84)
 sta pfcolortable
.L0958 ;  playfield:

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
.L0959 ;  goto room_draw_end

 jmp .room_draw_end

.r13
 ; r13

.L0960 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L0961 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-83)
 sta pfcolortable
.L0962 ;  playfield:

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
.L0963 ;  goto room_draw_end

 jmp .room_draw_end

.r14
 ; r14

.L0964 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L0965 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-82)
 sta pfcolortable
.L0966 ;  playfield:

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
.L0967 ;  goto room_draw_end

 jmp .room_draw_end

.r15
 ; r15

.L0968 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel190-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel190-81)
 sta pfcolortable
.L0969 ;  playfield:

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
.L0970 ;  goto room_draw_end

 jmp .room_draw_end

.r16
 ; r16

.L0971 ;  COLUBK = yellow

	LDA yellow
	STA COLUBK
.L0972 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel207-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-84)
 sta pfcolortable
.L0973 ;  playfield:

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
.L0974 ;  goto room_draw_end

 jmp .room_draw_end

.r17
 ; r17

.L0975 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0976 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel207-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-83)
 sta pfcolortable
.L0977 ;  playfield:

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
.L0978 ;  goto room_draw_end

 jmp .room_draw_end

.r18
 ; r18

.L0979 ;  COLUBK  =  lightgreen

	LDA #lightgreen
	STA COLUBK
.L0980 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel207-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-82)
 sta pfcolortable
.L0981 ;  playfield:

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
.L0982 ;  goto room_draw_end

 jmp .room_draw_end

.r19
 ; r19

.L0983 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0984 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel207-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel207-81)
 sta pfcolortable
.L0985 ;  playfield:

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
.L0986 ;  goto room_draw_end

 jmp .room_draw_end

.r20
 ; r20

.L0987 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L0988 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-84)
 sta pfcolortable
.L0989 ;  playfield:

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
.L0990 ;  goto room_draw_end

 jmp .room_draw_end

.r21
 ; r21

.L0991 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L0992 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-83)
 sta pfcolortable
.L0993 ;  playfield:

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
.L0994 ;  goto room_draw_end

 jmp .room_draw_end

.r22
 ; r22

.L0995 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L0996 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-82)
 sta pfcolortable
.L0997 ;  playfield:

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
.L0998 ;  goto room_draw_end

 jmp .room_draw_end

.r23
 ; r23

.L0999 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01000 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel224-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel224-81)
 sta pfcolortable
.L01001 ;  playfield:

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
.L01002 ;  goto room_draw_end

 jmp .room_draw_end

.r24
 ; r24

.L01003 ;  COLUBK = yellow

	LDA yellow
	STA COLUBK
.L01004 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel241-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-84)
 sta pfcolortable
.L01005 ;  if quest_flags{5} then goto r24_open

	LDA quest_flags
	AND #32
	BEQ .skipL01005
.condpart157
 jmp .r24_open

.skipL01005
.L01006 ;  playfield:

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
.L01007 ;  goto r24_end

 jmp .r24_end

.r24_open
 ; r24_open

.L01008 ;  playfield:

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

.L01009 ;  goto room_draw_end

 jmp .room_draw_end

.r25
 ; r25

.L01010 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01011 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel241-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-83)
 sta pfcolortable
.L01012 ;  playfield:

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
.L01013 ;  goto room_draw_end

 jmp .room_draw_end

.r26
 ; r26

.L01014 ;  COLUBK  =  lightgreen

	LDA #lightgreen
	STA COLUBK
.L01015 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel241-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-82)
 sta pfcolortable
.L01016 ;  playfield:

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
.L01017 ;  goto room_draw_end

 jmp .room_draw_end

.r27
 ; r27

.L01018 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01019 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel241-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel241-81)
 sta pfcolortable
.L01020 ;  playfield:

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
.L01021 ;  goto room_draw_end

 jmp .room_draw_end

.r28
 ; r28

.L01022 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01023 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-84)
 sta pfcolortable
.L01024 ;  playfield:

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
.L01025 ;  goto room_draw_end

 jmp .room_draw_end

.r29
 ; r29

.L01026 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01027 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-83)
 sta pfcolortable
.L01028 ;  if quest_flags{6} then goto r29_open

	BIT quest_flags
	BVC .skipL01028
.condpart158
 jmp .r29_open

.skipL01028
.L01029 ;  if item_flags{3} then goto r29_open

	LDA item_flags
	AND #8
	BEQ .skipL01029
.condpart159
 jmp .r29_open

.skipL01029
.L01030 ;  playfield:

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
.L01031 ;  goto room_draw_end

 jmp .room_draw_end

.r29_open
 ; r29_open

.L01032 ;  playfield:

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
.L01033 ;  goto room_draw_end

 jmp .room_draw_end

.r30
 ; r30

.L01034 ;  pfcolors:

 lda # green 
 sta COLUPF
 lda #>(pfcolorlabel258-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-82)
 sta pfcolortable
.L01035 ;  playfield:

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
.L01036 ;  goto room_draw_end

 jmp .room_draw_end

.r31
 ; r31

.L01037 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel258-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel258-81)
 sta pfcolortable
.L01038 ;  playfield:

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
.L01039 ;  goto room_draw_end

 jmp .room_draw_end

.
 ; 

.r32
 ; r32

.L01040 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01041 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-84)
 sta pfcolortable
.L01042 ;  if quest_flags{2} then goto r32_open

	LDA quest_flags
	AND #4
	BEQ .skipL01042
.condpart160
 jmp .r32_open

.skipL01042
.L01043 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel35
PF_data35
	.byte %10000001, %11111111, %11111111, %11000001
	.byte %10000011, %11111111, %11111111, %11000011
	.byte %10000111, %11111111, %11111111, %11000111
	.byte %10001111, %11111111, %11111111, %11001111
	.byte %10011111, %11111111, %11111111, %11011111
	.byte %10111111, %11111111, %11111111, %00111111
	.byte %10001100, %00110011, %00110011, %00001100
	.byte %10001100, %00110011, %00110011, %00001100
	.byte %10011111, %11111111, %11111111, %00011111
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
pflabel35
	lda PF_data35,x
	sta playfield,x
	dex
	bpl pflabel35
.L01044 ;  goto room_draw_end bank2

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

.L01045 ;  playfield:

  ifconst pfres
    ldx #4*pfres-1
  else
	  ldx #47
  endif
	jmp pflabel36
PF_data36
	.byte %10000001, %00111111, %00111111, %11000001
	.byte %10000011, %00111111, %00111111, %11000011
	.byte %10000111, %00111111, %00111111, %11000111
	.byte %10001111, %00111111, %00111111, %11001111
	.byte %10011111, %00111111, %00111111, %11011111
	.byte %10111111, %00111111, %00111111, %00111111
	.byte %10001100, %00110011, %00110011, %00001100
	.byte %10001100, %00110011, %00110011, %00001100
	.byte %10011111, %00111111, %00111111, %00011111
	.byte %10000000, %00000000, %00000000, %00000000
	.byte %10000000, %00000000, %00000000, %00000000
pflabel36
	lda PF_data36,x
	sta playfield,x
	dex
	bpl pflabel36
.L01046 ;  goto room_draw_end bank2

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

.L01047 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-83)
 sta pfcolortable
.L01048 ;  playfield:

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
.L01049 ;  goto room_draw_end bank2

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

.L01050 ;  COLUBK  =  lightgreen

	LDA #lightgreen
	STA COLUBK
.L01051 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel275-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-82)
 sta pfcolortable
.L01052 ;  playfield:

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
.L01053 ;  goto room_draw_end bank2

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

.L01054 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01055 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel275-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel275-81)
 sta pfcolortable
.L01056 ;  if quest_flags{4} then goto r35_open

	LDA quest_flags
	AND #16
	BEQ .skipL01056
.condpart161
 jmp .r35_open

.skipL01056
.L01057 ;  playfield:

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
.L01058 ;  goto room_draw_end bank2

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

.L01059 ;  playfield:

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
.L01060 ;  goto room_draw_end bank2

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

.L01061 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01062 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-84)
 sta pfcolortable
.L01063 ;  playfield:

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
.r37
 ; r37

.L01065 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01066 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-83)
 sta pfcolortable
.L01067 ;  playfield:

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
.L01068 ;  goto room_draw_end bank2

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

.L01069 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01070 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel292-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-82)
 sta pfcolortable
.L01071 ;  playfield:

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
.r39
 ; r39

.L01073 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01074 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel292-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel292-81)
 sta pfcolortable
.L01075 ;  playfield:

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
.
 ; 

.L01077 ;  bank 3

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
.
 ; 

.room_draw_40
 ; room_draw_40

.L01078 ;  temp1 = hero_room - 40

	LDA hero_room
	SEC
	SBC #40
	STA temp1
.L01079 ;  on temp1 goto r40 r41 r42 r43 r44 r45 r46 r47 r48 r49 r50 r51 r52 r53 r54 r55 r56 r57 r58 r59 r60 r61 r62 r63

	LDX temp1
	LDA .L01079jumptablehi,x
	PHA
	LDA .L01079jumptablelo,x
	PHA
	RTS
.L01079jumptablehi
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
.L01079jumptablelo
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
.
 ; 

.r40
 ; r40

.L01081 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel309-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-84)
 sta pfcolortable
.L01082 ;  playfield:

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
.
 ; 

.
 ; 

.r41
 ; r41

.L01084 ;  COLUBK  =  lightgreen

	LDA #lightgreen
	STA COLUBK
.L01085 ;  pfcolors:

 lda # green 
 sta COLUPF
 lda #>(pfcolorlabel309-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-83)
 sta pfcolortable
.L01086 ;  playfield:

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
.r42
 ; r42

.L01088 ;  COLUBK  =  lightgreen

	LDA #lightgreen
	STA COLUBK
.L01089 ;  pfcolors:

 lda # green 
 sta COLUPF
 lda #>(pfcolorlabel309-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-82)
 sta pfcolortable
.L01090 ;  playfield:

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
	.byte %00000100, %00000000, %00010000, %00000000
	.byte %00000000, %00000000, %00000000, %11000000
pflabel47
	lda PF_data47,x
	sta playfield,x
	dex
	bpl pflabel47
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
.r43
 ; r43

.L01092 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01093 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel309-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel309-81)
 sta pfcolortable
.L01094 ;  playfield:

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
.r44
 ; r44

.L01096 ;  COLUBK = sand

	LDA #sand
	STA COLUBK
.L01097 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel326-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-84)
 sta pfcolortable
.L01098 ;  playfield:

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
.r45
 ; r45

.L01100 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01101 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel326-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-83)
 sta pfcolortable
.L01102 ;  playfield:

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
.r46
 ; r46

.L01104 ;  COLUBK = lightgreen

	LDA #lightgreen
	STA COLUBK
.L01105 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel326-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-82)
 sta pfcolortable
.L01106 ;  playfield:

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
.r47
 ; r47

.L01108 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel326-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel326-81)
 sta pfcolortable
.L01109 ;  playfield:

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
.
 ; 

.
 ; 

.
 ; 

.r48
 ; r48

.L01111 ;  COLUBK  =  lightgreen

	LDA #lightgreen
	STA COLUBK
.L01112 ;  pfcolors:

 lda # brown
 sta COLUPF
 lda #>(pfcolorlabel343-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-84)
 sta pfcolortable
.L01113 ;  playfield:

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
.
 ; 

.r49
 ; r49

.L01115 ;  COLUBK  =  lightgreen

	LDA #lightgreen
	STA COLUBK
.L01116 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel343-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-83)
 sta pfcolortable
.L01117 ;  playfield:

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
.
 ; 

.r50
 ; r50

.L01119 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01120 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel343-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-82)
 sta pfcolortable
.L01121 ;  playfield:

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
.r51
 ; r51

.L01123 ;  rem 'black ship top

.L01124 ;  COLUBK  =  seablue

	LDA #seablue
	STA COLUBK
.L01125 ;  pfcolors:

 lda # white
 sta COLUPF
 lda #>(pfcolorlabel343-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel343-81)
 sta pfcolortable
.L01126 ;  playfield:

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
.r52
 ; r52

.L01128 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01129 ;  pfcolors:

 lda # seablue
 sta COLUPF
 lda #>(pfcolorlabel360-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-84)
 sta pfcolortable
.L01130 ;  playfield:

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
.L01131 ;  goto room_draw_end bank2

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

.L01132 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01133 ;  pfcolors:

 lda # green
 sta COLUPF
 lda #>(pfcolorlabel360-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-83)
 sta pfcolortable
.L01134 ;  playfield:

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
.L01135 ;  goto room_draw_end bank2

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

.L01136 ;  COLUBK  =  lightgreen

	LDA #lightgreen
	STA COLUBK
.L01137 ;  pfcolors:

 lda # gray
 sta COLUPF
 lda #>(pfcolorlabel360-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-82)
 sta pfcolortable
.L01138 ;  if quest_flags{3} then goto r54_open

	LDA quest_flags
	AND #8
	BEQ .skipL01138
.condpart162
 jmp .r54_open

.skipL01138
.L01139 ;  playfield:

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
.L01140 ;  goto room_draw_end bank2

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

.L01141 ;  playfield:

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
.r55
 ; r55

.L01143 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01144 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel360-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel360-81)
 sta pfcolortable
.L01145 ;  if quest_flags{7} then goto r55_open

	BIT quest_flags
	BPL .skipL01145
.condpart163
 jmp .r55_open

.skipL01145
.L01146 ;  playfield:

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
.L01147 ;  goto room_draw_end bank2

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

.L01148 ;  playfield:

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
.L01149 ;  goto room_draw_end bank2

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

.L01150 ;  rem 'Shield on island

.L01151 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01152 ;  pfcolors:

 lda # seablue
 sta COLUPF
 lda #>(pfcolorlabel377-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-84)
 sta pfcolortable
.L01153 ;  if !quest_flags{6} then goto r56_closed

	BIT quest_flags
	BVS .skipL01153
.condpart164
 jmp .r56_closed

.skipL01153
.L01154 ;  playfield:

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
.r56_closed
 ; r56_closed

.L01156 ;  playfield:

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

.L01157 ;  goto room_draw_end bank2

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

.L01158 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01159 ;  pfcolors:

 lda # red
 sta COLUPF
 lda #>(pfcolorlabel377-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-83)
 sta pfcolortable
.L01160 ;  playfield:

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
.
 ; 

.r58
 ; r58

.L01162 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01163 ;  pfcolors:

 lda # seablue
 sta COLUPF
 lda #>(pfcolorlabel377-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-82)
 sta pfcolortable
.L01164 ;  if quest_flags{2} goto r58_open

	LDA quest_flags
	AND #4
 if ((* - .r58_open) < 127) && ((* - .r58_open) > -128)
	BNE .r58_open
 else
	beq .12skipr58_open
	jmp .r58_open
.12skipr58_open
 endif
.L01165 ;  playfield:

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
	.byte %00001111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
	.byte %11111111, %11111111, %11111111, %11111111
pflabel66
	lda PF_data66,x
	sta playfield,x
	dex
	bpl pflabel66
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
.r58_open
 ; r58_open

.L01167 ;  playfield:

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
.
 ; 

.
 ; 

.r59
 ; r59

.L01169 ;  COLUBK  =  seablue

	LDA #seablue
	STA COLUBK
.L01170 ;  pfcolors:

 lda # black
 sta COLUPF
 lda #>(pfcolorlabel377-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel377-81)
 sta pfcolortable
.L01171 ;  rem 'black ship bottom, when switch3 isn't pressed

.L01172 ;  if quest_flags{3} goto black_ship_open

	LDA quest_flags
	AND #8
 if ((* - .black_ship_open) < 127) && ((* - .black_ship_open) > -128)
	BNE .black_ship_open
 else
	beq .13skipblack_ship_open
	jmp .black_ship_open
.13skipblack_ship_open
 endif
.L01173 ;  playfield:

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
.L01174 ;  goto black_ship_end

 jmp .black_ship_end

.black_ship_open
 ; black_ship_open

.L01175 ;  playfield:

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
.
 ; 

.r60
 ; r60

.L01177 ;  rem 'Boss 3, Kraken room.

.L01178 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L01179 ;  pfcolors:

 lda # seablue
 sta COLUPF
 lda #>(pfcolorlabel394-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-84)
 sta pfcolortable
.L01180 ;  playfield:

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
.
 ; 

.r61
 ; r61

.L01182 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01183 ;  pfcolors:

 lda # riverblue
 sta COLUPF
 lda #>(pfcolorlabel394-83)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-83)
 sta pfcolortable
.L01184 ;  if !hero_items{5} then goto r61_closed

	LDA hero_items
	AND #32
	BNE .skipL01184
.condpart165
 jmp .r61_closed

.skipL01184
.L01185 ;  playfield:

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
.L01186 ;  goto room_draw_end bank2

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

.L01187 ;  playfield:

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
.L01188 ;  goto room_draw_end bank2

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
.
 ; 

.r62
 ; r62

.L01189 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01190 ;  pfcolors:

 lda # seablue
 sta COLUPF
 lda #>(pfcolorlabel394-82)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-82)
 sta pfcolortable
.L01191 ;  playfield:

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
.
 ; 

.
 ; 

.r63
 ; r63

.L01193 ;  COLUBK  =  sand

	LDA #sand
	STA COLUBK
.L01194 ;  pfcolors:

 lda # seablue
 sta COLUPF
 lda #>(pfcolorlabel394-81)
 sta pfcolortable+1
 lda #<(pfcolorlabel394-81)
 sta pfcolortable
.L01195 ;  playfield:

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
.L01196 ;  goto room_draw_end bank2

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
.
 ; 

.
 ; 

.
 ; 

.L01197 ;  bank 4

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
.L01198 ;  rem 'Item sprites as well as initial data for item and sprites are in bank 4

.L01199 ;  rem 'Items/monsters/mobiles in any given room

.L01200 ;  rem '8 items per line means it corresponds with map layout

.L01201 ;  data room_items

	JMP .skipL01201
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

	.byte   monster_flower, item_healhp , monster_slime, item_switch3  

	.byte   monster_scorpio, monster_crab, item_none, monster_fish

	.byte   item_shield, monster_crab, monster_scorpio, item_none 

	.byte   monster_leaf3_boss, item_switch6, monster_crab, monster_strike_boss

.skipL01201
.
 ; 

.L01202 ;  data item_hplist

	JMP .skipL01202
item_hplist
	.byte     4,  4,  8,  8,  8, 16, 16, 16, 16, 32, 32, 32, 32, 64, 32, 32

	.byte    64, 80,100,  4, 64, 32, 64, 64, 64, 80, 80, 80, 80,100,100,120

	.byte   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4

	.byte   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4

	.byte   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4

	.byte   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4

	.byte   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4

	.byte   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4

.skipL01202
.
 ; 

.L01203 ;  rem 'Shorthand for the center of screen positions.

.L01204 ;  const hc = 73

.L01205 ;  const vc = 49

.
 ; 

.L01206 ;  data item_xlist

	JMP .skipL01206
item_xlist
	.byte   hc, hc, 84, 37,109, 70, hc - 8, hc 

	.byte   hc, hc, hc, 37, hc, hc, hc, hc 

	.byte   hc,113,113, 37, hc + 12, hc, hc, hc 

	.byte   hc, hc, 63,113, hc,120, hc, hc 

	.byte   hc, 37, 90, 37, 53, 53, hc, hc 

	.byte   77, hc, hc,123, 66, hc, hc, 61 

	.byte   hc, hc, hc,113, hc, hc, hc, hc 

	.byte   37, hc, 37, 37, hc, 37, hc, hc 

.skipL01206
.
 ; 

.L01207 ;  data item_ylist

	JMP .skipL01207
item_ylist
	.byte   vc, vc, vc, 25, 70, 65, vc, vc 

	.byte   vc, vc, vc, vc, vc, vc, vc, vc + 4 

	.byte   vc, vc, vc, vc, vc + 1, vc, vc, vc + 8 

	.byte   vc, vc, 70, vc, vc + 8, vc, vc, vc - 16 

	.byte   vc, 29, 70, vc, 41, 41, vc, vc 

	.byte   33, vc, vc, vc, 70, 25, vc, vc 

	.byte   vc, 55, vc, 80, vc, vc, vc, vc 

	.byte   73, vc, vc, vc, vc, 73, vc, vc 

.skipL01207
.
 ; 

.
 ; 

.item_setup
 ; item_setup

.L01208 ;  item_kind  =  room_items[hero_room]

	LDX hero_room
	LDA room_items,x
	STA item_kind
.L01209 ;  gosub item_setup_kind

 jsr .item_setup_kind

.L01210 ;  goto item_setup_done bank2

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
.
 ; 

.L01211 ;  rem 'You must set item_kind before gosubbing to this one  

.item_setup_kind
 ; item_setup_kind

.L01212 ;  rem 'Initialize item/mobile, and it's position, HP and speed 

.L01213 ;  rem 'First, handle switches

.L01214 ;  if item_kind  <  item_switch1 then goto item_switch_end

	LDA item_kind
	CMP #item_switch1
     BCS .skipL01214
.condpart166
 jmp .item_switch_end

.skipL01214
.L01215 ;  if item_kind  >  item_switch7 then goto item_switch_end

	LDA #item_switch7
	CMP item_kind
     BCS .skipL01215
.condpart167
 jmp .item_switch_end

.skipL01215
.L01216 ;  rem 'set switch to on if needed

.L01217 ;  if quest_flags{1}  &&  item_kind  =  item_switch1 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #2
	BEQ .skipL01217
.condpart168
	LDA item_kind
	CMP #item_switch1
     BNE .skip168then
.condpart169
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip168then
.skipL01217
.L01218 ;  if quest_flags{2}  &&  item_kind  =  item_switch2 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #4
	BEQ .skipL01218
.condpart170
	LDA item_kind
	CMP #item_switch2
     BNE .skip170then
.condpart171
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip170then
.skipL01218
.L01219 ;  if quest_flags{3}  &&  item_kind  =  item_switch3 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #8
	BEQ .skipL01219
.condpart172
	LDA item_kind
	CMP #item_switch3
     BNE .skip172then
.condpart173
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip172then
.skipL01219
.L01220 ;  if quest_flags{4}  &&  item_kind  =  item_switch4 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #16
	BEQ .skipL01220
.condpart174
	LDA item_kind
	CMP #item_switch4
     BNE .skip174then
.condpart175
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip174then
.skipL01220
.L01221 ;  if quest_flags{5}  &&  item_kind  =  item_switch5 then item_kind  =  item_kind  +  item_switch_on

	LDA quest_flags
	AND #32
	BEQ .skipL01221
.condpart176
	LDA item_kind
	CMP #item_switch5
     BNE .skip176then
.condpart177
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip176then
.skipL01221
.L01222 ;  if quest_flags{6}  &&  item_kind  =  item_switch6 then item_kind  =  item_kind  +  item_switch_on

	BIT quest_flags
	BVC .skipL01222
.condpart178
	LDA item_kind
	CMP #item_switch6
     BNE .skip178then
.condpart179
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip178then
.skipL01222
.L01223 ;  if quest_flags{7}  &&  item_kind  =  item_switch7 then item_kind  =  item_kind  +  item_switch_on

	BIT quest_flags
	BPL .skipL01223
.condpart180
	LDA item_kind
	CMP #item_switch7
     BNE .skip180then
.condpart181
	LDA item_kind
	CLC
	ADC #item_switch_on
	STA item_kind
.skip180then
.skipL01223
.item_switch_end
 ; item_switch_end

.L01224 ;  rem 'Next, handle bosses that may already be dead.

.L01225 ;  rem 'Ikaza

.L01226 ;  if item_kind  <  monster_leaf1_boss then goto item_boss_end

	LDA item_kind
	CMP #monster_leaf1_boss
     BCS .skipL01226
.condpart182
 jmp .item_boss_end

.skipL01226
.L01227 ;  if item_kind  >  monster_ikaza then goto item_boss_end

	LDA #monster_ikaza
	CMP item_kind
     BCS .skipL01227
.condpart183
 jmp .item_boss_end

.skipL01227
.L01228 ;  if quest_flags{0}  &&  item_kind  =  monster_ikaza then item_kind  =  item_none

	LDA quest_flags
	LSR
	BCC .skipL01228
.condpart184
	LDA item_kind
	CMP #monster_ikaza
     BNE .skip184then
.condpart185
	LDA #item_none
	STA item_kind
.skip184then
.skipL01228
.L01229 ;  rem 'Leaf 1 dropped by boss 1

.L01230 ;  if hero_items{0}  &&  item_kind  =  monster_leaf1_boss then item_kind  =  item_none

	LDA hero_items
	LSR
	BCC .skipL01230
.condpart186
	LDA item_kind
	CMP #monster_leaf1_boss
     BNE .skip186then
.condpart187
	LDA #item_none
	STA item_kind
.skip186then
.skipL01230
.L01231 ;  rem 'Leaf 2 dropped by boss 2

.L01232 ;  if hero_items{1}  &&  item_kind  =  monster_leaf2_boss then item_kind  =  item_none

	LDA hero_items
	AND #2
	BEQ .skipL01232
.condpart188
	LDA item_kind
	CMP #monster_leaf2_boss
     BNE .skip188then
.condpart189
	LDA #item_none
	STA item_kind
.skip188then
.skipL01232
.L01233 ;  rem 'Leaf 3 dropped by boss 3

.L01234 ;  if hero_items{2}  &&  item_kind  =  monster_leaf3_boss then item_kind  =  item_none

	LDA hero_items
	AND #4
	BEQ .skipL01234
.condpart190
	LDA item_kind
	CMP #monster_leaf3_boss
     BNE .skip190then
.condpart191
	LDA #item_none
	STA item_kind
.skip190then
.skipL01234
.L01235 ;  rem 'Armor Dropped by armor boss

.L01236 ;  if hero_items{4}  &&  item_kind  =  monster_armor_boss then item_kind  =  item_none

	LDA hero_items
	AND #16
	BEQ .skipL01236
.condpart192
	LDA item_kind
	CMP #monster_armor_boss
     BNE .skip192then
.condpart193
	LDA #item_none
	STA item_kind
.skip192then
.skipL01236
.L01237 ;  rem 'Sword dropped by sword boss

.L01238 ;  if hero_items{5}  &&  item_kind  =  monster_sword_boss then item_kind  =  item_none

	LDA hero_items
	AND #32
	BEQ .skipL01238
.condpart194
	LDA item_kind
	CMP #monster_sword_boss
     BNE .skip194then
.condpart195
	LDA #item_none
	STA item_kind
.skip194then
.skipL01238
.L01239 ;  rem 'Strike book dropped by strike book boss

.L01240 ;  if hero_items{7}  &&  item_kind  =  monster_strike_boss then item_kind  =  item_none

	BIT hero_items
	BPL .skipL01240
.condpart196
	LDA item_kind
	CMP #monster_strike_boss
     BNE .skip196then
.condpart197
	LDA #item_none
	STA item_kind
.skip196then
.skipL01240
.L01241 ;  rem 'Heal book and shield are not dropped by a boss, but just found

.item_boss_end
 ; item_boss_end

.L01242 ;  rem 'Finally handle the case of the shield and the heal book that should drop only once

.L01243 ;  if hero_items{3}  &&  item_kind  =  item_shield then item_kind  =  item_none

	LDA hero_items
	AND #8
	BEQ .skipL01243
.condpart198
	LDA item_kind
	CMP #item_shield
     BNE .skip198then
.condpart199
	LDA #item_none
	STA item_kind
.skip198then
.skipL01243
.L01244 ;  if hero_items{6}  &&  item_kind  =  item_bookheal then item_kind  =  item_none

	BIT hero_items
	BVC .skipL01244
.condpart200
	LDA item_kind
	CMP #item_bookheal
     BNE .skip200then
.condpart201
	LDA #item_none
	STA item_kind
.skip200then
.skipL01244
.
 ; 

.L01245 ;  temp1  =  item_kind_mask  &  item_kind

	LDA #item_kind_mask
	AND item_kind
	STA temp1
.L01246 ;  item_hp  =  item_hplist[temp1]

	LDX temp1
	LDA item_hplist,x
	STA item_hp
.
 ; 

.L01247 ;  if temp1  >  31 goto item_draw_32

	LDA #31
	CMP temp1
 if ((* - .item_draw_32) < 127) && ((* - .item_draw_32) > -128)
	bcc .item_draw_32
 else
	bcs .14skipitem_draw_32
	jmp .item_draw_32
.14skipitem_draw_32
 endif
.L01248 ;  on temp1 goto i00 i01 i02 i03 i04 i05 i06 i07 i08 i09 i10 i11 i12 i13 i14 i15 i16 i17 i18 i19 i20 i21 i22 i23 i24 i25 i26 i27 i28 i29 i30 i31

	LDX temp1
	LDA .L01248jumptablehi,x
	PHA
	LDA .L01248jumptablelo,x
	PHA
	RTS
.L01248jumptablehi
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
.L01248jumptablelo
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
.L01249 ;  goto item_draw_done

 jmp .item_draw_done

.item_draw_32
 ; item_draw_32

.L01250 ;  temp1  =  temp1  -  32

	LDA temp1
	SEC
	SBC #32
	STA temp1
.L01251 ;  on temp1 goto i32 i33 i34 i35 i36 i37 i38 i39 i40 i41 i42 i43 i44 i45 i46 i47 i48 i49 i50 i51 i52 i53 i54 i55 i56 i57 i58 i59 i60 i61 i62 i63

	LDX temp1
	LDA .L01251jumptablehi,x
	PHA
	LDA .L01251jumptablelo,x
	PHA
	RTS
.L01251jumptablehi
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
.L01251jumptablelo
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

.
 ; 

.L01252 ;  if item_kind  <>  item_none goto item_not_none

	LDA item_kind
	CMP #item_none
 if ((* - .item_not_none) < 127) && ((* - .item_not_none) > -128)
	BNE .item_not_none
 else
	beq .15skipitem_not_none
	jmp .item_not_none
.15skipitem_not_none
 endif
.L01253 ;  item_x  =  nowhere

	LDA #nowhere
	STA item_x
.L01254 ;  item_oldx  =  nowhere

	LDA #nowhere
	STA item_oldx
.L01255 ;  item_y  =  nowhere

	LDA #nowhere
	STA item_y
.L01256 ;  item_oldy  =  nowhere

	LDA #nowhere
	STA item_oldy
.L01257 ;  return

	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.item_not_none
 ; item_not_none

.L01258 ;  rem temp1 = item_kind & item_kind_mask

.L01259 ;  rem 'look up location in table.

.L01260 ;  item_oldx  =  item_xlist[hero_room]

	LDX hero_room
	LDA item_xlist,x
	STA item_oldx
.L01261 ;  item_x  =  item_xlist[hero_room]

	LDX hero_room
	LDA item_xlist,x
	STA item_x
.L01262 ;  item_oldy  =  item_ylist[hero_room]

	LDX hero_room
	LDA item_ylist,x
	STA item_oldy
.L01263 ;  item_y  =  item_ylist[hero_room]

	LDX hero_room
	LDA item_ylist,x
	STA item_y
.L01264 ;  return

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

.
 ; 

.
 ; 

.i00
 ; i00

.L01265 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01266 ;  player1:

	LDA #<playerL01266_1

	STA player1pointerlo
	LDA #>playerL01266_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01267 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i01
 ; i01

.L01268 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01269 ;  player1:

	LDA #<playerL01269_1

	STA player1pointerlo
	LDA #>playerL01269_1

	STA player1pointerhi
	LDA #5
	STA player1height
.L01270 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i02
 ; i02

.L01271 ;  COLUP1 = red

	LDA #red
	STA COLUP1
.L01272 ;  player1:

	LDA #<playerL01272_1

	STA player1pointerlo
	LDA #>playerL01272_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01273 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.
 ; 

.i03
 ; i03

.L01274 ;  rem 'bat.xpm

.L01275 ;  player1:

	LDA #<playerL01275_1

	STA player1pointerlo
	LDA #>playerL01275_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01276 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i04
 ; i04

.L01277 ;  rem 'scorpio.xpm

.L01278 ;  player1:

	LDA #<playerL01278_1

	STA player1pointerlo
	LDA #>playerL01278_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01279 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i05
 ; i05

.L01280 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01281 ;  rem 'rabid.xpm

.L01282 ;  player1:

	LDA #<playerL01282_1

	STA player1pointerlo
	LDA #>playerL01282_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01283 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i06
 ; i06

.L01284 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01285 ;  rem 'spider.xpm

.L01286 ;  player1:

	LDA #<playerL01286_1

	STA player1pointerlo
	LDA #>playerL01286_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01287 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i07
 ; i07

.L01288 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01289 ;  rem 'snake.xpm

.L01290 ;  player1:

	LDA #<playerL01290_1

	STA player1pointerlo
	LDA #>playerL01290_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01291 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i08
 ; i08

.L01292 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01293 ;  rem 'fish.xpm

.L01294 ;  player1:

	LDA #<playerL01294_1

	STA player1pointerlo
	LDA #>playerL01294_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01295 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i09
 ; i09

.L01296 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01297 ;  rem 'lion.xpm

.L01298 ;  player1:

	LDA #<playerL01298_1

	STA player1pointerlo
	LDA #>playerL01298_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01299 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i10
 ; i10

.L01300 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01301 ;  rem 'wolf.xpm

.L01302 ;  player1:

	LDA #<playerL01302_1

	STA player1pointerlo
	LDA #>playerL01302_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01303 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i11
 ; i11

.L01304 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01305 ;  rem 'captain.xpm, used for monster_grunt. (captain and grunt were doubles)

.L01306 ;  player1:

	LDA #<playerL01306_1

	STA player1pointerlo
	LDA #>playerL01306_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01307 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i12
 ; i12

.L01308 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01309 ;  rem 'archer.xpm

.L01310 ;  player1:

	LDA #<playerL01310_1

	STA player1pointerlo
	LDA #>playerL01310_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01311 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i13
 ; i13

.L01312 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01313 ;  rem 'knight.xpm

.L01314 ;  player1:

	LDA #<playerL01314_1

	STA player1pointerlo
	LDA #>playerL01314_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01315 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i14
 ; i14

.L01316 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01317 ;  rem 'cannon.xpm

.L01318 ;  player1:

	LDA #<playerL01318_1

	STA player1pointerlo
	LDA #>playerL01318_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01319 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i15
 ; i15

.L01320 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01321 ;  rem 'zombie.xpm

.L01322 ;  player1:

	LDA #<playerL01322_1

	STA player1pointerlo
	LDA #>playerL01322_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01323 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i16
 ; i16

.L01324 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01325 ;  rem 'skeleton.xpm

.L01326 ;  player1:

	LDA #<playerL01326_1

	STA player1pointerlo
	LDA #>playerL01326_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01327 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i17
 ; i17

.L01328 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01329 ;  rem 'ghost.xpm

.L01330 ;  player1:

	LDA #<playerL01330_1

	STA player1pointerlo
	LDA #>playerL01330_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01331 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i18
 ; i18

.L01332 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01333 ;  rem 'bitmap/mage.xpm

.L01334 ;  player1:

	LDA #<playerL01334_1

	STA player1pointerlo
	LDA #>playerL01334_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01335 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i19
 ; i19

.L01336 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01337 ;  rem 'flower.xpm

.L01338 ;  player1:

	LDA #<playerL01338_1

	STA player1pointerlo
	LDA #>playerL01338_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01339 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i20
 ; i20

.L01340 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01341 ;  rem 'treant.xpm

.L01342 ;  player1:

	LDA #<playerL01342_1

	STA player1pointerlo
	LDA #>playerL01342_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01343 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i21
 ; i21

.L01344 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01345 ;  rem 'muschroom.xpm

.L01346 ;  player1:

	LDA #<playerL01346_1

	STA player1pointerlo
	LDA #>playerL01346_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01347 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i22
 ; i22

.L01348 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01349 ;  rem 'book.xpm

.L01350 ;  player1:

	LDA #<playerL01350_1

	STA player1pointerlo
	LDA #>playerL01350_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01351 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i23
 ; i23

.L01352 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01353 ;  rem 'sword.xpm. It's a fake Numen Sword that hurts the one who touches it.

.L01354 ;  player1:

	LDA #<playerL01354_1

	STA player1pointerlo
	LDA #>playerL01354_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01355 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i24
 ; i24

.L01356 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01357 ;  rem 'Curse, entrance blocking item

.L01358 ;  player1:

	LDA #<playerL01358_1

	STA player1pointerlo
	LDA #>playerL01358_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01359 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i25
 ; i25

.L01360 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01361 ;  rem 'boss1.xpm

.L01362 ;  player1:

	LDA #<playerL01362_1

	STA player1pointerlo
	LDA #>playerL01362_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01363 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i26
 ; i26

.L01364 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01365 ;  rem 'boss2.xpm

.L01366 ;  player1:

	LDA #<playerL01366_1

	STA player1pointerlo
	LDA #>playerL01366_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01367 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i27
 ; i27

.L01368 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01369 ;  rem 'boss3.xpm

.L01370 ;  player1:

	LDA #<playerL01370_1

	STA player1pointerlo
	LDA #>playerL01370_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01371 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i28
 ; i28

.L01372 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01373 ;  rem 'armorboss.xpm

.L01374 ;  player1:

	LDA #<playerL01374_1

	STA player1pointerlo
	LDA #>playerL01374_1

	STA player1pointerhi
	LDA #24
	STA player1height
.L01375 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i29
 ; i29

.L01376 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01377 ;  rem 'Sword boss is a player frame 

.L01378 ;  player1:  

	LDA #<playerL01378_1

	STA player1pointerlo
	LDA #>playerL01378_1

	STA player1pointerhi
	LDA #13
	STA player1height
.L01379 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i30
 ; i30

.L01380 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01381 ;  rem 'Strike boss is a one point sprite which is difficult to hit

.L01382 ;  player1:

	LDA #<playerL01382_1

	STA player1pointerlo
	LDA #>playerL01382_1

	STA player1pointerhi
	LDA #1
	STA player1height
.L01383 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i31
 ; i31

.L01384 ;  rem 'ikaza.xpm

.L01385 ;  rem 'ikaza.xpm

.L01386 ;  player1:

	LDA #<playerL01386_1

	STA player1pointerlo
	LDA #>playerL01386_1

	STA player1pointerhi
	LDA #16
	STA player1height
.L01387 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i32
 ; i32

.i33
 ; i33

.i34
 ; i34

.L01388 ;  rem 'leaf.xpm

.L01389 ;  player1:

	LDA #<playerL01389_1

	STA player1pointerlo
	LDA #>playerL01389_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01390 ;  goto item_draw_done

 jmp .item_draw_done

.i35
 ; i35

.L01391 ;  rem 'armor.xpm

.L01392 ;  player1:

	LDA #<playerL01392_1

	STA player1pointerlo
	LDA #>playerL01392_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01393 ;  goto item_draw_done

 jmp .item_draw_done

.i36
 ; i36

.L01394 ;  rem 'shield.xpm

.L01395 ;  player1:

	LDA #<playerL01395_1

	STA player1pointerlo
	LDA #>playerL01395_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01396 ;  goto item_draw_done

 jmp .item_draw_done

.i37
 ; i37

.L01397 ;  rem 'sword.xpm

.L01398 ;  player1:

	LDA #<playerL01398_1

	STA player1pointerlo
	LDA #>playerL01398_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01399 ;  goto item_draw_done

 jmp .item_draw_done

.i38
 ; i38

.L01400 ;  rem 'healbook.xpm

.L01401 ;  player1:

	LDA #<playerL01401_1

	STA player1pointerlo
	LDA #>playerL01401_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01402 ;  goto item_draw_done

 jmp .item_draw_done

.i39
 ; i39

.L01403 ;  rem 'strikebook.xpm

.L01404 ;  player1:

	LDA #<playerL01404_1

	STA player1pointerlo
	LDA #>playerL01404_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01405 ;  goto item_draw_done

 jmp .item_draw_done

.i40
 ; i40

.L01406 ;  rem 'healhp.xpm

.L01407 ;  player1:

	LDA #<playerL01407_1

	STA player1pointerlo
	LDA #>playerL01407_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01408 ;  goto item_draw_done

 jmp .item_draw_done

.i41
 ; i41

.L01409 ;  rem 'healmp.xpm

.L01410 ;  player1:

	LDA #<playerL01410_1

	STA player1pointerlo
	LDA #>playerL01410_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01411 ;  goto item_draw_done

 jmp .item_draw_done

.i42
 ; i42

.L01412 ;  rem 'healallhp.xpm

.L01413 ;  player1:

	LDA #<playerL01413_1

	STA player1pointerlo
	LDA #>playerL01413_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01414 ;  goto item_draw_done

 jmp .item_draw_done

.i43
 ; i43

.L01415 ;  rem 'healallmp.xpm

.L01416 ;  player1:

	LDA #<playerL01416_1

	STA player1pointerlo
	LDA #>playerL01416_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01417 ;  goto item_draw_done

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

.L01418 ;  rem 'bitmap/switch.xpm

.L01419 ;  player1:

	LDA #<playerL01419_1

	STA player1pointerlo
	LDA #>playerL01419_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01420 ;  goto item_draw_done

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

.L01421 ;  rem 'bitmap/switch_on.xpm

.L01422 ;  player1:

	LDA #<playerL01422_1

	STA player1pointerlo
	LDA #>playerL01422_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01423 ;  goto item_draw_done

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

.L01424 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01425 ;  player1:

	LDA #<playerL01425_1

	STA player1pointerlo
	LDA #>playerL01425_1

	STA player1pointerhi
	LDA #8
	STA player1height
.L01426 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.i63
 ; i63

.L01427 ;  COLUP1 = pink

	LDA #pink
	STA COLUP1
.L01428 ;  player1:

	LDA #<playerL01428_1

	STA player1pointerlo
	LDA #>playerL01428_1

	STA player1pointerhi
	LDA #13
	STA player1height
.L01429 ;  goto item_draw_done

 jmp .item_draw_done

.
 ; 

.L01430 ;  bank 5

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
.game_over
 ; game_over

.L01431 ;  rem 'Silence

.L01432 ;  AUDC1 = 0

	LDA #0
	STA AUDC1
.L01433 ;  AUDF1 = 0

	LDA #0
	STA AUDF1
.L01434 ;  AUDV1 = 0

	LDA #0
	STA AUDV1
.L01435 ;  COLUBK  =  red

	LDA #red
	STA COLUBK
.L01436 ;  hero_x = 70

	LDA #70
	STA hero_x
.L01437 ;  hero_y = 80

	LDA #80
	STA hero_y
.L01438 ;  player0:

	LDA #<playerL01438_0

	STA player0pointerlo
	LDA #>playerL01438_0

	STA player0pointerhi
	LDA #59
	STA player0height
.game_over_loop
 ; game_over_loop

.L01439 ;  rem 'Teleport to Sygne's home on reset button, with some losses

.L01440 ;  if !switchreset then goto reset_go_end

 lda #1
 bit SWCHB
	BEQ .skipL01440
.condpart202
 jmp .reset_go_end

.skipL01440
.L01441 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L01442 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L01443 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L01444 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L01445 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L01446 ;  hero_hp  =  hero_level  +  hero_base_hp

	LDA hero_level
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L01447 ;  hero_next  =  hero_level

	LDA hero_level
	STA hero_next
.L01448 ;  hero_mp  =  0

	LDA #0
	STA hero_mp
.L01449 ;  gosub set_mp_display bank1

 sta temp7
 lda #>(ret_point22-1)
 pha
 lda #<(ret_point22-1)
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
ret_point22
.L01450 ;  gosub hero_draw_s bank1

 sta temp7
 lda #>(ret_point23-1)
 pha
 lda #<(ret_point23-1)
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
ret_point23
.L01451 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point24-1)
 pha
 lda #<(ret_point24-1)
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
ret_point24
.L01452 ;  goto main_loop bank1

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

.
 ; 

.L01453 ;  COLUP0 = white

	LDA white
	STA COLUP0
.L01454 ;  COLUP1 = black

	LDA black
	STA COLUP1
.L01455 ;  rem 'Make monster look x3 size, to maitain bosss size and suggest something for non bosses.  

.L01456 ;  NUSIZ1  =  $07

	LDA #$07
	STA NUSIZ1
.special_effects_go_end
 ; special_effects_go_end

.
 ; 

.L01457 ;  drawscreen

 sta temp7
 lda #>(ret_point25-1)
 pha
 lda #<(ret_point25-1)
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
ret_point25
.L01458 ;  goto game_over_loop

 jmp .game_over_loop

.
 ; 

.
 ; 

.L01459 ;  data music_data

	JMP .skipL01459
music_data
	.byte   24,30,27,30,30,30,27,30,24,28,-1,2,24,28,-1,2,24,60

	.byte   27,28,-1,2,27,28,-1,2,27,60,24,30,20,28,-1,2,20,60

	.byte   24,30,27,30,30,30,27,30,24,28,-1,2,24,28,-1,2,24,28,-1,2

	.byte   24,30,27,28,-1,2,27,30,24,30,27,30,30,120

.skipL01459
.
 ; 

.game_win
 ; game_win

.L01460 ;  rem 'Silence

.L01461 ;  AUDC1 = 0

	LDA #0
	STA AUDC1
.L01462 ;  AUDF1 = 0

	LDA #0
	STA AUDF1
.L01463 ;  AUDV1 = 0

	LDA #0
	STA AUDV1
.L01464 ;  COLUBK  =  white

	LDA white
	STA COLUBK
.L01465 ;  COLUPF  =  yellow

	LDA yellow
	STA COLUPF
.L01466 ;  item_x = 80

	LDA #80
	STA item_x
.L01467 ;  item_y = 80

	LDA #80
	STA item_y
.L01468 ;  hero_x = 84

	LDA #84
	STA hero_x
.L01469 ;  hero_y = 80

	LDA #80
	STA hero_y
.L01470 ;  player1:

	LDA #<playerL01470_1

	STA player1pointerlo
	LDA #>playerL01470_1

	STA player1pointerhi
	LDA #73
	STA player1height
.
 ; 

.game_win_loop
 ; game_win_loop

.L01471 ;  COLUBK  =  white

	LDA white
	STA COLUBK
.L01472 ;  COLUP1  =  rand

 sta temp7
 lda #>(ret_point26-1)
 pha
 lda #<(ret_point26-1)
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
ret_point26
	STA COLUP1
.L01473 ;  COLUP0  =  rand

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
	STA COLUP0
.L01474 ;  REFP0  =  8

	LDA #8
	STA REFP0
.L01475 ;  drawscreen

 sta temp7
 lda #>(ret_point28-1)
 pha
 lda #<(ret_point28-1)
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
ret_point28
.L01476 ;  rem 'Go back to house, but level up to 90 if not so already

.L01477 ;  if !switchreset then goto reset_win_end

 lda #1
 bit SWCHB
	BEQ .skipL01477
.condpart203
 jmp .reset_win_end

.skipL01477
.L01478 ;  hero_room = room_start

	LDA #room_start
	STA hero_room
.L01479 ;  hero_x = hero_start_x

	LDA #hero_start_x
	STA hero_x
.L01480 ;  hero_oldx = hero_start_x

	LDA #hero_start_x
	STA hero_oldx
.L01481 ;  hero_y = hero_start_y

	LDA #hero_start_y
	STA hero_y
.L01482 ;  hero_oldy = hero_start_x

	LDA #hero_start_x
	STA hero_oldy
.L01483 ;  if hero_level  <  90 then hero_level  =  90

	LDA hero_level
	CMP #90
     BCS .skipL01483
.condpart204
	LDA #90
	STA hero_level
.skipL01483
.L01484 ;  hero_hp  =  hero_level  *  2  +  hero_base_hp

; complex statement detected
	LDA hero_level
	asl
	CLC
	ADC #hero_base_hp
	STA hero_hp
.L01485 ;  hero_mp  =  hero_level  /  16  +  1

; complex statement detected
	LDA hero_level
	lsr
	lsr
	lsr
	lsr
	CLC
	ADC #1
	STA hero_mp
.L01486 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point29-1)
 pha
 lda #<(ret_point29-1)
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
ret_point29
.L01487 ;  gosub hero_draw_s bank1

 sta temp7
 lda #>(ret_point30-1)
 pha
 lda #<(ret_point30-1)
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
ret_point30
.L01488 ;  gosub room_draw bank2

 sta temp7
 lda #>(ret_point31-1)
 pha
 lda #<(ret_point31-1)
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
ret_point31
.L01489 ;  goto main_loop bank1

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

.
 ; 

.
 ; 

.L01490 ;  goto game_win_loop

 jmp .game_win_loop

.
 ; 

.
 ; 

.intro_screen
 ; intro_screen

.L01491 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L01492 ;  COLUPF  =  white

	LDA white
	STA COLUPF
.L01493 ;  pfcolors:

 lda # yellow
 sta COLUPF
 lda #>(pfcolorlabel1303-84)
 sta pfcolortable+1
 lda #<(pfcolorlabel1303-84)
 sta pfcolortable
.L01494 ;  playfield:

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
.intro_loop
 ; intro_loop

.L01495 ;  COLUBK  =  black

	LDA black
	STA COLUBK
.L01496 ;  rem COLUP1 = rand

.L01497 ;  rem COLUP0 = rand

.L01498 ;  rem REFP0 = 8

.L01499 ;  drawscreen

 sta temp7
 lda #>(ret_point32-1)
 pha
 lda #<(ret_point32-1)
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
ret_point32
.L01500 ;  if switchreset  ||  joy0fire then return

 lda #1
 bit SWCHB
	BNE .skipL01500
.condpart205
 jmp .condpart206
.skipL01500
 lda #$80
 bit INPT4
	BNE .skip24OR
.condpart206
	tsx
	lda 2,x ; check return address
	eor #(>*) ; vs. current PCH
	and #$E0 ;  mask off all but top 3 bits
	beq *+5 ; if equal, do normal return
	JMP BS_return
	RTS
.skip24OR
.L01501 ;  goto intro_loop

 jmp .intro_loop

.
 ; 

.
 ; 

.L01502 ;  bank 6

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
.
 ; 

.L01503 ;  bank 7

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
.
 ; 

.L01504 ;  bank 8

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
; bB.asm file is split here
.
 ; 

.L01505 ;  inline 6lives_statusbar.asm

 include 6lives_statusbar.asm

.
 ; 

.
 ; 

.
 ; 

.
 ; 

.
 ; 

 if (<*) > (<(*+8))
	repeat ($100-<*)
	.byte 0
	repend
	endif
lives__L0293
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
lives__L0793
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
playerL0852_0

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
playerL0855_0

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
playerL0860_0

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
playerL0863_0

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
playerL0866_0

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
playerL0871_0

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
 .byte  white, green, green , green 
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
 .byte  gray , gray , green, green
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
 .byte  gray, gray, brown , green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, green
 .byte  gray, gray, green, brown
 .byte  gray, gray, green, brown
 .byte  gray, gray, brown, brown
 .byte  gray , gray , green, green
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
 .byte  brown, brown , brown , seablue + 8
 .byte  brown, green, green, seablue + 8
 .byte  brown, green, green, seablue + 4
 .byte  brown, green, green, seablue + 4
 .byte  brown, green, green, seablue + 2
 .byte  brown, brown, brown, seablue + 2
 .byte  brown, seablue, seablue, seablue
 .byte  brown, green, green, seablue
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
 .byte  white, brown , gray, green
 .byte  white, brown, gray, green
 .byte  seablue, brown, gray, green
 .byte  seablue, brown, gray, green
 .byte  red, green, gray , green
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
 .byte  brown, red + 4, seablue + 8, white
 .byte  brown, white, seablue + 8, white
 .byte  brown, white, seablue + 4, black
 .byte  brown, white, seablue + 4, black
 .byte  brown, white, seablue + 2, black
 .byte  brown, red, seablue + 2, black
 .byte  brown, red, seablue, black
 .byte  brown, red, seablue, black
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel360
 .byte  seablue, green, gray, red
 .byte  seablue, green, gray, white
 .byte  seablue, brown, gray, white
 .byte  seablue, brown , gray, white
 .byte  seablue, brown, gray, white
 .byte  seablue, brown, gray, red
 .byte  seablue, seablue, gray, red
 .byte  seablue, seablue, gray, seablue
 .byte  seablue, seablue, gray, seablue
 .byte  seablue, seablue, seablue, seablue
 .byte  seablue, green, gray , seablue
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel377
 .byte  seablue, red, seablue, black
 .byte  seablue, red, seablue, black
 .byte  seablue, red, seablue, black
 .byte  seablue, red, seablue, black
 .byte  seablue , red, seablue, black
 .byte  seablue, red, seablue, black 
 .byte  seablue, red, seablue, black
 .byte  seablue, seablue, seablue, black
 .byte  seablue, seablue, seablue, black
 .byte  seablue, seablue, seablue, seablue
 .byte  seablue, seablue, seablue, black
 if (<*) > 206
	align 256
	endif
 if (<*) < 88
	repeat (88-(<*))
	.byte 0
	repend
	endif
pfcolorlabel394
 .byte  seablue, riverblue, seablue, seablue
 .byte  seablue, riverblue, seablue, seablue
 .byte  seablue, riverblue, seablue, seablue
 .byte  seablue, seablue + 8, seablue, seablue
 .byte  seablue, seablue + 8, seablue, seablue
 .byte  seablue, seablue + 4, seablue, seablue
 .byte  seablue, seablue + 4, seablue, seablue
 .byte  seablue, seablue + 2, seablue, seablue
 .byte  seablue, seablue + 2, seablue, seablue
 .byte  seablue, seablue, seablue, seablue
 .byte  seablue, seablue, seablue, seablue
 if (<*) > (<(*+9))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01266_1

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
playerL01269_1

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
playerL01272_1

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
playerL01275_1

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
playerL01278_1

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
playerL01282_1

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
playerL01286_1

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
playerL01290_1

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
playerL01294_1

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
playerL01298_1

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
playerL01302_1

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
playerL01306_1

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
playerL01310_1

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
playerL01314_1

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
playerL01318_1

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
playerL01322_1

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
playerL01326_1

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
playerL01330_1

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
playerL01334_1

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
playerL01338_1

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
playerL01342_1

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
playerL01346_1

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
playerL01350_1

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
playerL01354_1

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
playerL01358_1

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
playerL01362_1

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
playerL01366_1

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
playerL01370_1

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
playerL01374_1

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
playerL01378_1

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
playerL01382_1

	.byte 0
	.byte   %00010000
 if (<*) > (<(*+17))
	repeat ($100-<*)
	.byte 0
	repend
	endif
playerL01386_1

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
playerL01389_1

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
playerL01392_1

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
playerL01395_1

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
playerL01398_1

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
playerL01401_1

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
playerL01404_1

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
playerL01407_1

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
playerL01410_1

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
playerL01413_1

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
playerL01416_1

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
playerL01419_1

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
playerL01422_1

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
playerL01425_1

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
playerL01428_1

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
playerL01438_0

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
playerL01470_1

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
 .byte  yellow - 4 ,0,0,0
 .byte  yellow - 8,0,0,0
 .byte  yellow - 10,0,0,0
 .byte  yellow - 12,0,0,0
 .byte  yellow - 2,0,0,0
 .byte  yellow - 4 ,0,0,0
 .byte  yellow - 8,0,0,0
 .byte  yellow - 10,0,0,0
 .byte  yellow - 12,0,0,0
 .byte  yellow,0,0,0
       echo "    ",[(scoretable - *)]d , "bytes of ROM space left in bank 8")
 
 
 
