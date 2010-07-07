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

.L090 ;  rem 'On the east side of the river

.L091 ;  rem 'const room_start=4

.
 ; 

.L092 ;  const hero_start_x = 75

.L093 ;  const hero_start_y = 75

.L094 ;  const item_start_x = 60

.L095 ;  const item_start_y = 40

.L096 ;  const item_start_hp = 1

.L097 ;  const hero_base_hp = 11

.L098 ;  const hero_high = 8

.L099 ;  const hero_half_high = 4

.L0100 ;  const hero_feet_high = 4

.L0101 ;  const hero_wide = 8

.L0102 ;  const hero_half_wide = 4

.L0103 ;  rem 'Hero's position, speed and room

.L0104 ;  dim hero_x = player0x

.L0105 ;  dim hero_y = player0y

.
 ; 

.L0106 ;  dim hero_oldx = v

.L0107 ;  dim hero_oldy = w

.L0108 ;  rem 'Flags:

.L0109 ;  dim hero_flags = g

.L0110 ;  rem 'hero_flags{0} -> facing north

.L0111 ;  rem 'hero_flags{1} -> facing east

.L0112 ;  rem 'hero_flags{2} -> facing south

.L0113 ;  rem 'hero_flags{3} -> facing west

.L0114 ;  rem 'hero_flags{4} -> set when attacking is allowed, blocked when not

.L0115 ;  dim hero_items = q

.L0116 ;  rem 'hero_items{0} -> Key Leaf 1

.L0117 ;  rem 'hero_items{1} -> Key Leaf 2

.L0118 ;  rem 'hero_items{2} -> Key Leaf 3

.L0119 ;  rem 'hero_items{3} -> Numen Shield

.L0120 ;  rem 'hero_items{4} -> Numen Armor 

.L0121 ;  rem 'hero_items{5} -> Numen Sword

.L0122 ;  rem 'hero_items{6} -> Heal Book

.L0123 ;  rem 'hero_items{7} -> Strike Book

.L0124 ;  dim hero_room = r

.L0125 ;  dim hero_hp = statusbarlength

.L0126 ;  dim hero_mp = o

.L0127 ;  dim hero_next = k

.L0128 ;  dim hero_level = l

.L0129 ;  rem 'Quest flags contain important achievments of Signe.

.L0130 ;  dim quest_flags = h

.L0131 ;  rem 'quest_flags{0} -> Defeated Ikaza

.L0132 ;  rem 'quest_flags{1} -> Flipped Switch 1

.L0133 ;  rem 'quest_flags{2} -> Flipped Switch 2

.L0134 ;  rem 'quest_flags{3} -> Flipped Switch 3

.L0135 ;  rem 'quest_flags{4} -> Flipped Switch 4

.L0136 ;  rem 'quest_flags{5} -> Flipped Switch 5

.L0137 ;  rem 'quest_flags{6} -> Flipped Switch 6

.L0138 ;  rem 'quest_flags{7} -> Flipped Switch 7

.
 ; 

.L0139 ;  rem 'Other flags: Reserved for future extension

.L0140 ;  rem 'quest_flags{1} -> Defeated Armor Boss

.L0141 ;  rem 'quest_flags{2} -> Defeated Sword Boss 

.L0142 ;  rem 'quest_flags{3} -> Defeated 

.L0143 ;  rem 'quest_flags{0} -> Defeated Leaf Boss 1

.L0144 ;  rem 'quest_flags{1} -> Defeated Leaf Boss 2

.L0145 ;  rem 'quest_flags{2} -> Defeated Leaf Boss 3

.L0146 ;  rem 'status bar color in Z, which si otherwise used for debugging

.L0147 ;  rem ' Can't sue changes top platfield row color somehow. :p

.L0148 ;  rem ' dim statusbarcolor=z

.
 ; 

.L0149 ;  const strike_cost = 1

.L0150 ;  const last_monster = 32

.
 ; 

.L0151 ;  const hero_speed = 1

.L0152 ;  const hero_speed_negative = # - 1

.
 ; 

.L0153 ;  rem item/monster position speed and kind

.L0154 ;  dim item_x = player1x

.L0155 ;  dim item_y = player1y

.L0156 ;  dim item_kind = s

.L0157 ;  dim item_hp = i

.L0158 ;  dim item_oldx = t

.L0159 ;  dim item_oldy = u

.L0160 ;  rem 'Active item flags: 

.L0161 ;  dim item_flags = f

.L0162 ;  rem 'item_flags{0} -> Monster missile flies north

.L0163 ;  rem 'item_flags{1} -> Monster missile flies east

.L0164 ;  rem 'item_flags{2} -> Monster missile flies south

.L0165 ;  rem 'item_flags{3} -> Monster missile flies west

.
 ; 

.
 ; 

.L0166 ;  rem 'Game timer. Controls animation. Keeps on ticking, by overflow.

.L0167 ;  dim game_timer = e

.L0168 ;  const timer_loop = 128

.L0169 ;  const timer_second = 60

.
 ; 

.
 ; 

.L0170 ;  rem 'Item and monster kinds. 

.L0171 ;  rem 'Monster kind is divided as follows: the 6 bits record the looks 

.L0172 ;  rem 'and the 'index' for data tables of the monster or item

.L0173 ;  rem 'The high 2 bits are used for 2 flag variables: 

.L0174 ;  rem 'If item_kind{7} is set it's an immobile item, otherwise, a monster

.L0175 ;  rem '6 is yet to be determined

.L0176 ;  const item_kind_mask = %00111111

.L0177 ;  const is_item_mask = %10000000

.L0178 ;  const item_none = %10000000

.L0179 ;  const monster_slime = 1

.L0180 ;  const monster_crab = 2

.L0181 ;  const monster_bat = 3

.L0182 ;  const monster_scorpio = 4

.L0183 ;  rem 'The famous rabid rabbit ^_^

.L0184 ;  const monster_rabid = 5

.L0185 ;  const monster_spider = 6

.L0186 ;  const monster_snak