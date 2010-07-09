  rem ' Ature Adventure
  rem ' Copyright beoran@rubyforge.org 2009, 2010.
  rem ' May be distributed under the Zlib License. No warranty, use at your own risk.
  rem ' You will need to modify Batari Basic from sources to compile this program
  rem ' You'll also need to pull it through a C preprocessor like cpp -P
  rem ' Because I'm using much more constants than Batari Basic supports normally.
  rem ' And I use the C preprocessor as a workaround to Batari Basic's 
  rem ' bugs with regards to constant definition
  rem ' It's much easier to understand what's going on if I use constants galore.
  rem ' Note: I only use integer math, for speed.
  rem ' To Do: Fix some weirdness in the color handling. 
  rem ' To Do: Fix sound effects and music. 
  rem ' To Do: Make the game bigger. Perhaps add a few more areas?
  rem ' We use playfield colors, 32K and smart branching.
  set kernel_options pfcolors
  set romsize 32k
  set smartbranching on
  rem ' Cycle debugging
  rem ' set debug cycles
  rem ' set debug cyclescore
  rem ' dim mincycles=z
  
  rem ' mincycles=255

  rem 'Constant definitions
  
  rem 'const pfres=12
  rem 'const pfrowheight=8
  
  rem 'Commonly used NTSC Colors
  const black=0
  const gray=6
  const white=14
  const yellow=30
  
  const brown=32
  const orange=32
  const blue=144
  const skyblue=174
  const green=192
  const darkgreen=191
  const riverblue=170  
  rem ' XXX: there is a bug in the ompiler (or my patches to it) 
  rem ' some of my consants like the one below are translated in stead 
  rem ' like a dim statement: eg  const turfy=218 -> dim   turfy=218
  rem ' a workaround seem to be to reorder the constants a bit
  rem ' or to remove a few of them, but this only bugs out other constants :p
  rem ' so I replaced some constants by their values, and use the cpp 
  rem ' preprocessor in stead.    
#define SEABLUE 144
  const seablue = 144
  const red   = 64
  const pink  = 78
  const skin  = 254
  const sand  = 230
  const turfy = 218
  rem ' const ochre=16
 

  rem 'Playfield dimensions
  const field_left=17
  const field_left_enter=18
  const field_right=$88
  const field_right_enter=$87
  const field_top=10
  const field_top_enter=11
  const field_bottom=90
  const field_bottom_enter=89
  const field_hcenter=61
  const field_vcenter=44
  
  rem ' Note definitions and length const music_voice=12
  rem const note_c4=30
  rem const note_d4=27
  rem const note_e4=24
  rem const note_f4=23
  rem const note_g4=20
  rem const note_a4=18
  rem const note_b4=16
  rem const note_c5=15
  rem const note_none=0 
  rem const note_full=60
  rem const note_half=30
  rem const note_quarter=15
  rem const note_fullrest=58
  rem const note_halfrest=28
  rem const note_quarterest=13
  rem const note_ndot=2

  
  rem ' An object will be positioned at 255 to hide it
  const nowhere=255
  
  rem 'Timer ticks per second, 60 for NTSC
  const ticks_per_second=60
  
  rem 'Score, split up in individual digits
  dim sc0=score
  dim sc1=score+1
  dim sc2=score+2

  rem 'Initial values
  rem 'Normal start
  const room_start=49
  rem '49
  rem 'On the east side of the river
  rem 'const room_start=4
  
  const hero_start_x=75
  const hero_start_y=75
  const item_start_x=60
  const item_start_y=40
  const item_start_hp=1
  const hero_base_hp=11
  const hero_high=8
  const hero_half_high=4
  const hero_feet_high=4
  const hero_wide=8
  const hero_half_wide=4
  rem 'Hero's position, speed and room
  dim hero_x=player0x 
  dim hero_y=player0y

  dim hero_oldx=v
  dim hero_oldy=w
  rem 'Flags:
  dim hero_flags=g  
  rem 'hero_flags{0} -> facing north
  rem 'hero_flags{1} -> facing east
  rem 'hero_flags{2} -> facing south
  rem 'hero_flags{3} -> facing west
  rem 'hero_flags{4} -> set when attacking is allowed, blocked when not
  dim hero_items=q
  rem 'hero_items{0} -> Key Leaf 1
  rem 'hero_items{1} -> Key Leaf 2
  rem 'hero_items{2} -> Key Leaf 3
  rem 'hero_items{3} -> Numen Shield
  rem 'hero_items{4} -> Numen Armor 
  rem 'hero_items{5} -> Numen Sword
  rem 'hero_items{6} -> Heal Book
  rem 'hero_items{7} -> Strike Book
  dim hero_room=r
  dim hero_hp=statusbarlength
  dim hero_mp=o
  dim hero_next=k
  dim hero_level=l
  rem 'Quest flags contain important achievments of Signe.
  dim quest_flags=h
  rem 'quest_flags{0} -> Defeated Ikaza
  rem 'quest_flags{1} -> Flipped Switch 1
  rem 'quest_flags{2} -> Flipped Switch 2
  rem 'quest_flags{3} -> Flipped Switch 3
  rem 'quest_flags{4} -> Flipped Switch 4
  rem 'quest_flags{5} -> Flipped Switch 5
  rem 'quest_flags{6} -> Flipped Switch 6
  rem 'quest_flags{7} -> Flipped Switch 7
  
  rem 'Other flags: Reserved for future extension
  rem 'quest_flags{1} -> Defeated Armor Boss
  rem 'quest_flags{2} -> Defeated Sword Boss 
  rem 'quest_flags{3} -> Defeated 
  rem 'quest_flags{0} -> Defeated Leaf Boss 1
  rem 'quest_flags{1} -> Defeated Leaf Boss 2
  rem 'quest_flags{2} -> Defeated Leaf Boss 3
  rem 'status bar color in Z, which si otherwise used for debugging
  rem ' Can't sue changes top platfield row color somehow. :p
  rem ' dim statusbarcolor=z
  
  const strike_cost=1
  const last_monster=32

  const hero_speed=1
  const hero_speed_negative=#-1

  rem item/monster position speed and kind
  dim item_x=player1x
  dim item_y=player1y
  dim item_kind=s
  dim item_hp=i
  dim item_oldx=t
  dim item_oldy=u
  rem 'Active item flags: 
  dim item_flags=f
  rem 'item_flags{0} -> Monster missile flies north
  rem 'item_flags{1} -> Monster missile flies east
  rem 'item_flags{2} -> Monster missile flies south
  rem 'item_flags{3} -> Monster missile flies west
 
  
  rem 'Game timer. Controls animation. Keeps on ticking, by overflow.
  dim game_timer=e
  const timer_loop=128
  const timer_second=60


  rem 'Item and monster kinds. 
  rem 'Monster kind is divided as follows: the 6 bits record the looks 
  rem 'and the 'index' for data tables of the monster or item
  rem 'The high 2 bits are used for 2 flag variables: 
  rem 'If item_kind{7} is set it's an immobile item, otherwise, a monster
  rem '6 is yet to be determined
  const item_kind_mask=%00111111
  const is_item_mask=%10000000
  const item_none=%10000000
  const monster_slime=1
  const monster_crab=2
  const monster_bat=3
  const monster_scorpio=4
  rem 'The famous rabid rabbit ^_^
  const monster_rabid=5
  const monster_spider=6
  const monster_snake=7
  const monster_fish=8
  const monster_lion=9
  const monster_wolf=10
  const monster_grunt=11
  const monster_archer=12
  const monster_knight=13  
  const monster_cannon=14
  const monster_zombie=15
  const monster_skeleton=16
  const monster_ghost=17
  const monster_mage=18
  const monster_flower=19
  const monster_treant=20
  const monster_mushroom=21
  const monster_book=22
  const monster_sword=23
  rem 'Monster curse blocks the way in rooms. It's invulnerable,
  rem 'but can be removed by game events.
  const monster_curse=24
  const monster_first_boss=25
  const monster_leaf1_boss=25
  const monster_leaf2_boss=26
  const monster_leaf3_boss=27
  const monster_armor_boss=28
  rem 'Boss that guards the Numen Sword
  const monster_sword_boss=29
  rem 'Boss that guards the Strike book
  const monster_strike_boss=30
  const monster_ikaza=31
  
  const item_inventory_mask=160
  const item_leaf1=160
  const item_leaf2=161
  const item_leaf3=162
  const item_armor=163
  const item_shield=164
  const item_sword=165
  const item_bookheal=166  
  const item_bookstrike=167
  const item_healhp=168
  const item_healmp=169
  const item_healallhp=170
  const item_healallmp=171
  const item_first_switch=172
  const item_switch_on=7
  const item_switch1=172
  const item_switch2=173
  const item_switch3=174
  const item_switch4=175
  const item_switch5=176
  const item_switch6=177
  const item_switch7=178
  const item_last_switch=178
  const item_first_switch_on=179
  const item_switch1_on=179
  const item_switch2_on=180
  const item_switch3_on=181
  const item_switch4_on=182
  const item_switch5_on=183
  const item_switch6_on=184
  const item_switch7_on=185
  const item_last_switch_on=182
  const item_victory=191  
  const monster_normal=0
  rem 'Bit 0 in monster flags
  const monster_shoots=1
  rem 'Bit 1 in monster flags
  const monster_randomwalk=2
  rem 'Bit 2 in monster flags
  const monster_aggressive=4
  rem 'Bit 3 in monster flags
  const monster_static=8
  rem 'Bit 4 in monster flags
  const monster_alwaysdrop=16
  rem 'Bit 5 in monster flags
  const monster_nowalls=32
  rem 'Bit 6 in monster flags
  const monster_huge=64
  rem 'Bit 7 in monster flags. Will triple if huge is also set
  rem 'Currently, double nor triple doesn't work well with the AI
  const monster_double=128
  const monster_boss_info=monster_alwaysdrop + monster_shoots + monster_huge + monster_nowalls
  const monster_boss2_info=monster_alwaysdrop + monster_shoots + monster_huge + monster_nowalls
  const monster_boss3_info=monster_alwaysdrop + monster_shoots + monster_huge + monster_nowalls
  const monster_swordboss_info=monster_alwaysdrop + monster_aggressive + monster_nowalls
  const monster_cannon_info=monster_shoots + monster_static + monster_nowalls
  
  rem ' Music wave form to use.
  const music_instrument=12
  const music_notes=69
  


  rem 'Timer and pointer for music and sound
  dim music_pointer=m
  dim music_timer=n
  dim music_which=d
  dim sound_timer=p
  rem 'Unused variables: a b c
  
  rem 'Set back to 1 when not debugging.
  const hero_start_level=20
  rem 'set back to 0 when not debugging
  const hero_start_items=
  
  rem ' Colors to use for the item display 
  data item_colors
  black, green, red,  blue,  black, green, red,  blue,
  black, green, red,  blue,  black, green, red,  blue,
  black, green, red,  blue,  black, green, red,  blue,
  black, black, red,  red,  black, orange, red,  blue,
  black, green, red,  blue,  black, green, red,  blue,
  red  , green, red, green,  black, green, red,  blue,
  white, yellow, orange, black, green, red,  blue, white, 
  yellow, orange,  blue,  black, green, red,  blue, blue, 
end

  rem 'Damage modifier for the MONSTER/item. 0 means no damage
  rem 'There is no monster nr 0, that's for item_none. 
  rem 'The curse deals massive damage to be sure it works.
  data item_damage
   0,  1,  1,  1,  1,  2,  2,  2
   2,  4,  4,  4,  4,  6,  6,  6
  10, 16, 16,  1, 16,  4, 16, 16
 128, 20, 22, 24, 48, 48, 48, 64
   0,  0,  0,  0,  0,  0,  0,  0
   0,  0,  0,  0,  0,  0,  0,  0
   0,  0,  0,  0,  0,  0,  0,  0
   0,  0,  0,  0,  0,  0,  0,  0
end
  
  rem ' 
  rem ' Monster information, bitflags (monster 0 doesn't exist, but is in this list!)
  data monster_info
  monster_normal, monster_normal, monster_nowalls+monster_shoots, monster_nowalls
  monster_normal, monster_nowalls, monster_normal, monster_aggressive
  monster_shoots + monster_nowalls, monster_normal, monster_normal, monster_normal
  monster_shoots, monster_aggressive,  monster_cannon_info, monster_randomwalk
  monster_normal, monster_nowalls, monster_nowalls + monster_shoots, monster_cannon_info
  monster_normal, monster_normal, monster_cannon_info, monster_static + monster_nowalls
  monster_static + monster_huge + monster_nowalls, monster_boss_info, monster_boss_info, monster_boss3_info
  monster_boss_info, monster_swordboss_info, monster_boss_info, monster_aggressive + monster_shoots
end

  rem ' Items the monster drops, or for items, what item flag to set
  data monster_drops
  item_healhp, item_healhp, item_healhp, item_healmp,
  item_healhp, item_healhp, item_healhp, item_healhp,
  item_healhp, item_healhp, item_healhp, item_healhp,
  item_healhp, item_healallhp, item_healhp, monster_skeleton,
  monster_ghost, item_switch5, monster_book, item_healmp,
  item_healallmp, item_healhp,  item_bookheal, item_sword,
  item_healhp, item_leaf1, item_leaf2 , item_leaf3,
  item_armor, monster_sword, item_bookstrike, item_healallhp,
end


  rem VOLUME DOWN AND SET CHANNELS
  AUDV0=0
  AUDV1=0
  AUDC0=music_instrument  

  rem INITIALIZE POINTERS AND TIMERS
  
  rem 'lives control
  lifecolor=black
  rem 'Lives are used for the MP
  lives=32
  lives:
  %00010000
  %00010000
  %01010100
  %10111010
  %01010100
  %00010000
  %00010000
  %00010000
end

  rem 'Go to intro screen, but not now while we are debugging.
  gosub intro_screen bank5
  goto main_loop_start

main_loop_start  
  rem ' Set up initial values.
  gosub music_restart bank5
  COLUBK = black 
  lifecolor=yellow  
  hero_room=room_start
  hero_x=hero_start_x
  hero_oldx=hero_start_x
  hero_y=hero_start_y 
  hero_oldy=hero_start_x
  quest_flags=0
  hero_level=hero_start_level
  hero_next=(hero_level / 2) + 1
  hero_hp=hero_level*2+hero_base_hp
  rem 'MP is between 1 and 6
  hero_mp=hero_level/16 + 1
  gosub set_mp_display
  hero_flags = 0
  item_flags = 0
  game_timer = 0
  item_kind = item_none
  hero_items = hero_start_items
  rem 'Player starts facing south, able to attack
  hero_flags{2} = 1
  hero_flags{4} = 1
  rem player0x=75
  rem player0y=75
  item_x=item_start_x
  item_oldx=item_start_x  
  item_y=item_start_y
  item_oldy=item_start_x
  missile1x=nowhere
  missile1y=nowhere
  scorecolor=64
  item_hp=item_start_hp  
  gosub show_inventory
  gosub hero_draw_s
  gosub room_draw bank2
  drawscreen
main_loop  
  COLUP0 = skin
  temp1 = item_kind & item_kind_mask
  COLUP1 = item_colors[temp1]
  rem 'Special effects for the item (double or triple size, double or triple instances)
  if item_kind{7} goto special_effects_end 
  temp2 = monster_info[item_kind]
  if temp2{6} then NUSIZ1 = $07
  if temp2{7} then NUSIZ1 = $02
  if temp2{7} && temp2{7} then NUSIZ1 =$06
special_effects_end
  PF0=$00
  

  rem 'Collisions with the walls. For now, we use push-back collision 
  rem 'for the monsters, and preventive collision detection for the player. 
  rem 'This is inconsistent, but push-back is easier to manage with regards to 
  rem 'and matters less for AI, so I'll leave it at that for now.
  if !collision(player0, playfield) then goto herofield_collide_end
  rem 'Could do something here ??? Some overlap is OK, IMO
herofield_collide_end
  
  rem 'Collisions with the walls
  rem 'For the item
  if !collision(player1, playfield) then goto itemfield_collide_end
  temp2 = monster_info[item_kind]
  rem 'Some monsters ignore walls
  if temp2{5} then goto itemfield_collide_end
  
  if item_y  = item_oldy && item_x  = item_oldx then goto itemfield_collide_still  
  item_y  = item_oldy
  item_x  = item_oldx
  goto itemfield_collide_end
itemfield_collide_still
  rem if item_y < field_vcenter then item_y  = item_oldy - 1 else item_y  = item_oldy + 1
  rem if item_x < field_hcenter then item_x  = item_oldx - 1 else item_x  = hero_oldx + 1
itemfield_collide_end


  rem 'Remember curent position which should be OK here, 
  rem 'and with that I mean not colliding with the walls. 
  hero_oldx = hero_x
  hero_oldy = hero_y
  item_oldx = item_x
  item_oldy = item_y


  rem 'Teleport to Sygne's home on reset button
  if !switchreset then goto reset_end
  hero_room=room_start
  hero_x=hero_start_x
  hero_oldx=hero_start_x
  hero_y=hero_start_y
  hero_oldy=hero_start_x
  gosub room_draw bank2
reset_end
  
  rem 'Update game timer, and let it overflow back to 0 .
  game_timer = game_timer + 1
  rem 'Update sound timer, and set to silence if sound is done
  if sound_timer < 1 then goto sound_end
  sound_timer = sound_timer - 1
  if sound_timer > 1 then goto sound_end 
    AUDV1 = 0
    AUDC1 = 0
    AUDF1 = 0
    sound_timer = 0
sound_end
  
  rem 'Collision between hero and monster/item or the monster's missile
  if !collision(player0, missile1) then goto no_missile_collide
  rem 'Ennemy missile hit player, remove it
  missile1y = nowhere
  missile1x = nowhere
  item_flags= 0  
  rem ' If the player doesn't have a shield, jump to monster collide
  rem 'Otherwise ignore the projectile and remove it  
  if !hero_items{3} then goto monster_collide
no_missile_collide  
  if !collision(player0, player1) then goto item_collide_end
  rem 'Distinguish between item or monster collide
  if item_kind{7} then goto item_collide
  rem 'Collision with monster or monster missile
monster_collide
  COLUP0 = red
  rem 'Make a hit sound
  sound_timer=2
  AUDC1=6
  AUDF1=10
  AUDV1=14  
  
  rem 'Push back the hero, but only if the playfield is free behind her.
  rem 'Also don't push back if too close to the edge of the screen.
  

  if !hero_flags{0} then goto hero_hit_north_end  
  temp1 = (hero_x + hero_half_wide - 17) / 4
  temp2 = (hero_y + 8 - 1) / 8
  if pfread(temp1, temp2) goto hero_hit_north_end
  hero_y = hero_y + 8
  rem 'Still inside the screen?  
  if hero_y < field_bottom then goto hero_hit_north_end
  rem 'if we get here, we'd be pushed out of the screen. Prevent this.
  hero_y = field_bottom_enter  
hero_hit_north_end

  if !hero_flags{1} then goto hero_hit_east_end 
  temp1 = (hero_x - 8 - 17) / 4 
  temp2 = (hero_y - hero_half_high - 1) / 8 
  if pfread(temp1, temp2) goto hero_hit_east_end
  hero_x = hero_x - 8
  rem 'Still inside the screen?  
  if field_left < hero_x then goto hero_hit_east_end
  rem 'if we get here, we'd be pushed out of the screen. Prevent this.
  hero_x = field_left_enter  
hero_hit_east_end

  if !hero_flags{2} then goto hero_hit_south_end  
  if pfread(temp1, temp2) then goto hero_hit_south_end
  temp1 = (hero_x + hero_half_wide - 17) / 4
  temp2 = (hero_y - hero_high - 8 - 1) / 8
  hero_y = hero_y - 8
  if hero_y > field_top then goto hero_hit_south_end
  rem 'if we get here, we'd be pushed out of the screen. Prevent this.
  hero_y = field_top_enter  
hero_hit_south_end

  if !hero_flags{3} then goto hero_hit_west_end  
  temp1 = (hero_x + hero_wide + 8 - 17) / 4 
  temp2 = (hero_y - hero_half_high - 1) / 8
  if pfread(temp1, temp2) then goto hero_hit_west_end
  hero_x = hero_x + 8
  rem 'Still inside the screen?  
  if hero_x < field_right then goto hero_hit_west_end
  rem 'if we get here, we'd be pushed out of the screen. Prevent this.
  hero_x = field_right_enter  
hero_hit_west_end

  temp1 = item_kind & item_kind_mask
  temp1 = item_damage[temp1]
  rem 'Halve damage if hero has the armor 
  if hero_items{4} then temp1 = temp1 / 2  
  
  rem 'Game over if damage is higher than health
  if temp1 < hero_hp then goto do_damage 
  rem 'But escape by the skin of the teeth if Signe has the healing book and 
  rem 'Numen to use it.
  if hero_items{6} && hero_mp > 0 then goto use_heal_spell
  hero_hp = 0
  goto game_over bank5
use_heal_spell
  COLUP0 = green
  hero_mp = hero_mp - 1
  gosub set_mp_display
  hero_hp = hero_level * 2 + hero_base_hp
  goto item_collide_end
do_damage  
  hero_hp = hero_hp - temp1
  goto item_collide_end
item_collide
  rem 'If colliding with an item, pick it up if it is not a switch.
  if item_kind < item_switch1 then goto item_do_collide
  if item_kind > item_switch7_on then goto item_do_collide
  goto item_collide_end
item_do_collide  
  gosub item_pickup 
  gosub show_inventory
item_collide_end

  rem 'Collision between monster missile and field. 
  rem 'Or between sword and monster missile.  Remove missile.
  rem 'Remove misile if monster is gone.
  if item_kind > last_monster goto missile_remove 
  if collision(missile0, missile1)  then goto missile_remove
  if missile1x > field_right then goto missile_remove
  if missile1x < field_left then goto missile_remove
  if missile1y < field_top then goto missile_remove
  if missile1y > field_bottom then goto missile_remove  
  if !collision(missile1, playfield) then goto missile_remove_end  
  rem 'Don't remove the misile even when coliding with the playfield if the 
  rem 'monster ignores the walls.
  temp2 = monster_info[item_kind]
  rem 'temp2 = temp2 & monster_nowalls
  if temp2{5} then goto missile_remove_end
missile_remove   
  missile1y = nowhere
  missile1x = nowhere
  item_flags= 0  
missile_remove_end

  rem 'Collision between monster and hero's attack
  if !collision(player1, missile0) then goto slash_collide_end
  rem 'Distinguish between item or monster collide
  if item_kind{7} then goto slash_item_collide  
  
  rem 'The curse monster cannot be harmed normally. 
  rem 'It can be harmed if Signe has the three leaves, or the Numen Sword. 
  rem 'Do the special handling here.
  if item_kind <> monster_curse then goto monster_no_curse
  rem 'Signe has the sword? The curse can be damaged.
  if hero_items{5} then goto monster_no_curse
  rem 'Signe has the three leaves? The curse can be damaged. 
  if hero_items{0} && hero_items{1} && hero_items{2} then goto monster_no_curse
  rem 'If we get here, Signe's unable to do any damage.
  rem 'Make a deflected sound
  sound_timer=2
  AUDC1=12
  AUDV1=14
  AUDF1=1
  goto slash_collide_end  
monster_no_curse
  rem 'Make a hit sound once
  sound_timer=1
  AUDC1=8
  AUDF1=2
  AUDV1=8 
  hero_flags{4} = 0
  rem COLUP1 = red
  rem 'Push back monster
  if hero_flags{0} then item_y = item_y - 4    
  if hero_flags{1} then item_x = item_x + 4
  if hero_flags{2} then item_y = item_y + 4
  if hero_flags{3} then item_x = item_x - 4
  rem 'Damage is level / 2 + 1
  temp1 = (hero_level / 2)
  temp1 = temp1 + 1
  rem 'Double damage with the Numen Sword
  if hero_items{5} then temp1 = temp1 * 2
  rem 'Prevent damage overflow
  if temp1 < 0 then temp1 = 0 - temp1
  if temp1 < item_hp then goto item_survived
  rem 'Item / monster killed or destroyed
  if item_kind = monster_ikaza then quest_flags{0} = 1 
  rem 'Make sure Ikaza cannot come back
  rem 'Give experience and level up if needed
  rem 'Experience received is danger level of the monster 
  temp2 = item_damage[item_kind]
  rem 'No experience for killing the curse, as it's a sitting duck.
  if item_kind = monster_curse then goto give_experience_end
  if temp2 < hero_next then goto give_experience
  rem 'Level up, but not more than 99
  if hero_level > 98 then goto give_experience_end
  rem 'Level up sound
  COLUP0=blue
  AUDV1=8
  AUDF1=24
  AUDC1=12
  sound_timer=8
  hero_level = hero_level + 1
  hero_hp = hero_level * 2 + hero_base_hp
  rem 'Actually, this is buggy for levels above 80
  hero_mp = hero_level/16 + 1
  gosub set_mp_display
  hero_next = (hero_level / 2) + 1
  COLUP0 = turfy
  goto give_experience_end
give_experience
  hero_next = hero_next - temp2
give_experience_end
  rem 'Drop an item, perhaps
  temp6 = monster_info[item_kind]
  rem 'Bit 4: monster always drops item 
  if temp6{4} then goto drop_item
  if rand > 128 then goto drop_item
  rem 'Drop occasional, not always, depeding on monster flags
drop_no_item  
  item_kind = item_none
  item_hp = 0
  item_x = nowhere
  item_y = nowhere
  goto drop_item_done
drop_item  
  temp2     = monster_drops[item_kind]
  item_kind = temp2  
  gosub item_setup_kind bank4
drop_item_done
  goto slash_collide_end
item_survived    
  item_hp = item_hp - temp1
  rem 'Push item around
  if hero_flags{0} then item_y = item_y - 4
  if hero_flags{1} then item_x = item_x + 4
  if hero_flags{2} then item_y = item_y + 4
  if hero_flags{3} then item_x = item_x - 4
  goto slash_collide_end
slash_item_collide
  rem 'Make a hit sound once
  sound_timer=1
  AUDC1=8
  AUDF1=2
  AUDV1=8 
  if item_kind < item_switch1 then goto slash_item_pickup
  if item_kind > item_switch7_on then goto slash_item_pickup
  rem 'If colliding with an item that's not a switch, pick it up.
  rem 'Otherwise it's a switch that's hit.  
  goto hit_switch
slash_item_pickup  
  gosub item_pickup 
  gosub show_inventory
  goto slash_collide_end
hit_switch
  rem 'Do nothing with the already triggered switches. 
  rem '(Turn them off again, perhaps?)
  if item_kind > item_switch7 then goto slash_collide_end
  rem 'Handle hitting of the switch: set the quest flag and change the display.
  if item_kind = item_switch1 then quest_flags{1} = 1
  if item_kind = item_switch2 then quest_flags{2} = 1
  if item_kind = item_switch3 then quest_flags{3} = 1
  if item_kind = item_switch4 then quest_flags{4} = 1
  if item_kind = item_switch5 then quest_flags{5} = 1
  if item_kind = item_switch6 then quest_flags{6} = 1
  if item_kind = item_switch7 then quest_flags{7} = 1
  gosub item_setup_kind bank4  
slash_collide_end



  rem 'Player action though button
  if !joy0fire then goto hero_action_done
  rem 'Skip all this if action is blocked.
  if !hero_flags{4} then goto hero_action_blocked
  if !hero_items{7} then goto hero_act_nostrike 
  if hero_mp < strike_cost then goto hero_act_nostrike
  rem ' The b switch controls whether the book of striking is in use or not.
  if switchleftb then goto hero_act_nostrike
  if item_kind < last_monster then goto hero_act_strike
  if item_kind >= item_switch1 &&  item_kind <= item_switch7 then goto hero_act_strike
hero_act_nostrike
  temp1 = hero_flags & %00001111
  rem 'Use this to jump to the right attack action.
  rem '          0     1=north 2=east 3=ne   4=south  5=n+s(emu onlu)  6=se 7=nse 
  rem ' 8 = west
  rem ' 9 10 11 12 13 14 15(all sides, emu only)    
  on temp1 goto had hero_act_n hero_act_e hero_act_n hero_act_s hero_act_s hero_act_n hero_act_n hero_act_w hero_act_n hero_act_w hero_act_s had had had
hero_act_n 
  missile0x=hero_x + 4
  missile0y=hero_y - 14
  missile0height=8
  NUSIZ0=$00
  goto hero_action_end
hero_act_e 
  missile0x=hero_x + 9
  missile0y=hero_y - 4
  missile0height=0
  NUSIZ0=$30
  goto hero_action_end 
hero_act_s
  missile0x=hero_x + 4
  missile0y=hero_y + 8
  missile0height=8
  NUSIZ0=$00
  goto hero_action_end 
hero_act_w
  missile0x=hero_x - 7
  missile0y=hero_y - 4
  missile0height=0
  NUSIZ0=$30
  goto hero_action_end
  rem NUSIZ0=$00
hero_act_strike
  hero_mp = hero_mp - strike_cost
  gosub set_mp_display
  rem 'Strike will hit the ennemy wherever it is. A guaranteed hit I'd say ^_^
  missile0x=item_x
  missile0y=item_y
  missile0height=8
  NUSIZ0=$30
  COLUP0=rand
  COLUP1=red
  goto hero_action_end
had
hero_action_done
  hero_flags{4} = 1
  rem 'Allow attacks again
hero_action_blocked
  rem 'If the action is blocked or released, hide the missile
  missile0x = nowhere
  missile0y = nowhere
hero_action_end

  rem 'Deterimine hero's motion and direction from the joystick
  rem 'In NESW order  
  rem ' hero_flags = (hero_flags / 16) * 16 preserves the higher nibble (I hope)
  rem '
  rem 'For the player collision detection, we calculate the playfield position 
  rem 'at any of the four points around the feet of the player, and check if 
  rem 'if anything is there with pfread, and prevent motion if it is so.  
  rem ' Note that we do allow Signe to turn her facing even when she cannot move.

  if !joy0up then goto joy0up_end
  hero_flags = (hero_flags / 16) * 16
  hero_flags{0} = 1  
  rem 'Check top middle point 
  temp1 = (hero_x + hero_half_wide - 17) / 4
  temp2 = (hero_y - hero_high - 1 - 1) / 8  
  if pfread(temp1, temp2) goto joy0up_end
  hero_y = hero_y - 1
joy0up_end  
  if !joy0right then goto joy0right_end
  hero_flags = (hero_flags / 16) * 16
  hero_flags{1} = 1
  temp1 = (hero_x + hero_wide + 1 - 17) / 4 
  temp2 = (hero_y - hero_half_high - 1) / 8
  rem 'Prevent motion if something is there
  if pfread(temp1, temp2) goto joy0right_end
  hero_x = hero_x + 1
joy0right_end
  if !joy0down then goto joy0down_end
  hero_flags = (hero_flags / 16) * 16
  hero_flags{2} = 1
  temp1 = (hero_x + hero_half_wide - 17) / 4
  temp2 = (hero_y + 1 - 1) / 8
  rem 'Check bottom middle point. don't move if it's blocked by the playfield.
  if pfread(temp1, temp2) then goto joy0down_end
  hero_y = hero_y + 1
joy0down_end
  if !joy0left then goto joy0left_end
  hero_flags = (hero_flags / 16) * 16
  hero_flags{3} = 1
  temp1 = (hero_x - 1 - 17) / 4 
  temp2 = (hero_y - hero_half_high - 1) / 8
  rem 'Check point to the west of the feet 
  if pfread(temp1, temp2) goto joy0left_end
  hero_x = hero_x - 1 
joy0left_end

  rem 'Monster "AI":
  rem 'Animate missile if it has been launched
  if missile1y = nowhere then goto missile_move_end
  if item_flags{3} then missile1x = missile1x - 2 : goto missile_move_end
  if item_flags{0} then missile1y = missile1y - 2 : goto missile_move_end
  if item_flags{1} then missile1x = missile1x + 2 : goto missile_move_end
  if item_flags{2} then missile1y = missile1y + 2 : goto missile_move_end  
missile_move_end
  
  rem 'Move the item if it's a mobile (id lower than 128)
  if item_kind{7} then goto item_move_end  
  temp1 = rand
  rem ' Shoot a missile sometimes...
  temp6 = monster_info[item_kind]
  rem 'But not if the monster cannot shoot (bit 0)
  if !temp6{0} then goto activate_missile_end
  if !game_timer{4} then goto activate_missile_end
  if !game_timer{7} then goto activate_missile_end
  if !temp1{0} then goto activate_missile_end
  if missile1y <> nowhere then goto activate_missile_end
  rem 'Make a shot sound
  sound_timer=2
  AUDC1=14
  AUDV1=8
  AUDF1=20
  rem 'Place the missile  
  missile1x = item_x + 4
  missile1y = item_y - 4
  temp4 = item_x + 8 
  if hero_x > temp4 then item_flags{1} = 1 
  temp4 = item_x - 8 
  if hero_x < temp4 then item_flags{3} = 1
  if hero_y > item_y then item_flags{2} = 1
  if hero_y < item_y then item_flags{0} = 1
  missile1height=4
activate_missile_end
  rem 'Agressive chasing if aggressive flag is set 
  if temp6{2} then goto monster_chase
  rem 'Don't move if monster is static 
  if temp6{3} then goto item_move_end
  rem 'Only move half the time
  if game_timer{2} goto item_move_end
  
  rem 'Only move if the 4th and 5th bit are not set
  if temp1{1} then goto item_move_end
  rem 'Random walking monsters don't give chase
  if temp6{1} then goto monster_random_walk
monster_chase
  if hero_x > item_x then item_x = item_x + 1 else item_x = item_x - 1
  if hero_y > item_y then item_y = item_y + 1 else item_y = item_y - 1  
  if temp6{2} then goto item_move_end
  rem 'Aggressive ennemies don't random walk
monster_random_walk
  rem 'Use bits of random number for random walk.
  if temp1{0} then item_y = item_y - 1
  if temp1{4} then item_x = item_x - 1      
  if temp1{0} || temp1{4} then goto item_move_end
  if temp1{2} then item_y = item_y + 1 
  if temp1{6} then item_x = item_x + 1  
item_move_end

  rem 'Activate missile if needed

  rem 'Debug by showing flags in score field
  rem sc0 = hero_flags
  rem temp2 = monster_info[item_kind]
  rem sc1 = temp2
  rem sc2 = hero_room
  
  rem 'Music, disabled for now.
  rem 'Is it time to update the note? 
  rem if music_timer = 0 then gosub music_change_note
  rem music_timer = music_timer - 1
  
  
  
  rem 'Collision op hero with edge of field, transfer to other field.
  if field_left < hero_x then goto exit_left_done
  hero_x = field_right_enter
  hero_oldx = field_right
  hero_room = hero_room - 1  
  gosub room_draw bank2
  goto exit_done
exit_left_done  
  if hero_x < field_right then goto exit_right_done
  hero_x = field_left_enter
  hero_oldx = field_left
  hero_room = hero_room + 1  
  gosub room_draw bank2
  goto exit_done
exit_right_done    
  if hero_y > field_top then goto exit_top_done
  hero_y = field_bottom_enter
  hero_oldy = field_bottom
  hero_room = hero_room - 8
  gosub room_draw bank2
  goto exit_done
exit_top_done  
  if hero_y < field_bottom then goto exit_bottom_done
  hero_y = field_top_enter
  hero_oldy = field_top
  hero_room = hero_room + 8
  gosub room_draw bank2
  goto exit_done
exit_bottom_done
exit_done
  
  rem 'Collision of item with edge of field, bounce back.
  if item_x < field_left then item_x=field_left_enter  
  if item_x > field_right then item_x=field_right_enter  
  if item_y < field_top then item_y=field_top_enter  
  if item_y > field_bottom then item_x=field_bottom_enter  
 
 
  rem 'Draw acting player in position 2
  if joy0fire goto player_animate_2
  rem 'Draw non moving player not animated
  if hero_x = hero_oldx && hero_y = hero_oldy then goto player_animate_1
  rem 'Draw and animate player every so many ticks
  if !game_timer{4} then goto player_animate_2
player_animate_1
  if hero_flags{0} then gosub hero_draw_n
  if hero_flags{1} then gosub hero_draw_e
  if hero_flags{2} then gosub hero_draw_s
  if hero_flags{3} then gosub hero_draw_w
  goto player_animate_end
player_animate_2
  if hero_flags{0} then gosub hero_draw_n2
  if hero_flags{1} then gosub hero_draw_e2
  if hero_flags{2} then gosub hero_draw_s2
  if hero_flags{3} then gosub hero_draw_w2
player_animate_end
  
  rem 'Reflect sprite if moving west, otherwise, don't 
  if hero_flags{3} then REFP0 = 8 else REFP0 = 0
  

  
  rem draw screen
  drawscreen
 
  rem And continue main loop
  goto main_loop
  
  rem 'Vblank sub. Do something useful in here...
vblank
  rem 'Do something here...
  return
  
  rem 'Gosub that correctly displays MP
set_mp_display
  lives=hero_mp * 32
  lives:
  %00010000
  %00010000
  %01010100
  %10111010
  %01010100
  %00010000
  %00010000
  %00010000
end
  return  

  
  rem 'Display inventory ggosub
show_inventory
  sc0 = 0
  sc1 = 0
  sc2 = 0
  rem 'Add up score to display Numen Leaves
  if hero_items{0}  then sc0 = sc0 + 16
  if hero_items{1}  then sc0 = sc0 + 16
  if hero_items{2}  then sc0 = sc0 + 16
  rem 'Display shield if we have it
  if hero_items{3}  then sc0 = sc0 + 4
  rem 'Display Armor
  if hero_items{4}  then sc1 = sc1 + 80
  rem 'Display Sword
  if hero_items{5}  then sc1 = sc1 + 6
  if hero_items{6}  then sc2 = sc2 + 112
  if hero_items{7}  then sc2 = sc2 + 8
  return
  
  
  rem 'Item Pickup gosub
item_pickup  
  rem 'All sorts of item pickup effects, should be in a gosub, I guess
  rem 'Win game if finding victory item
  rem 'Make a pickup sound
  sound_timer=2
  AUDC1=12
  AUDV1=8
  AUDF1=2
  if item_kind = item_victory then goto game_win bank5
  if item_kind = item_leaf1 then hero_items{0} = 1
  if item_kind = item_leaf2 then hero_items{1} = 1
  if item_kind = item_leaf3 then hero_items{2} = 1
  if item_kind = item_shield then hero_items{3} = 1
  if item_kind = item_armor then hero_items{4} = 1
  if item_kind = item_sword then hero_items{5} = 1
  if item_kind = item_bookheal then hero_items{6} = 1   
  if item_kind = item_bookstrike then hero_items{7} = 1  
  if item_kind <> item_healhp then goto no_healhp
    hero_hp = hero_hp + 8
    temp2   = hero_level * 2 + hero_base_hp
    if hero_hp > temp2 then hero_hp = temp2   
no_healhp
  if item_kind <> item_healmp then goto no_healmp
  hero_mp = hero_mp + 1    
  temp2   = hero_level / 16 + 1  
  if hero_mp > temp2 then hero_mp = temp2
  gosub set_mp_display
no_healmp
  if item_kind <> item_healallhp then goto no_healallhp
  hero_hp = hero_level * 2 + hero_base_hp
no_healallhp
  if item_kind <> item_healallmp then goto no_healallmp
  rem 'Actually, this is buggy for levels above 80
  hero_mp = hero_level/16 + 1  
  gosub set_mp_display
no_healallmp
  rem " All done with item effects 
  
  rem ' Remove item
  item_kind = item_none
  item_hp = 0
  item_x = nowhere
  item_y = nowhere
  return
  
  
hero_draw_n
  REFP0=0
  player0:
  %01101100
  %01101100
  %01101100
  %01111100
  %10111010
  %01111100
  %10111010
  %11111110
  %11111110
  %11111110
  %11111110
  %01111100
  %00111000
end  
  return  

hero_draw_s
  REFP0=0
  player0:
  %01101100
  %01101100
  %01111100
  %01111100
  %10111010
  %01111100
  %10111010
  %11111110
  %11010110
  %11010110
  %11111110
  %01111100
  %00111000  
end
  return

hero_draw_w
  REFP0=8  
  goto hero_draw_e_start
hero_draw_e
  REFP0=0
hero_draw_e_start  
  player0:
  %00011100
  %10011000
  %10011000
  %10111000
  %10111100
  %10011100
  %10111000
  %11111100
  %11110100
  %11110110
  %11111110
  %01111100
  %00111000
end
  return

hero_draw_n2
  REFP0=0
  player0:
  %01100000
  %01100000
  %01101100
  %01101100
  %01111101
  %00111010
  %10111100
  %11111010
  %11111110
  %11111110
  %11111110
  %11111110
  %01111100
  %00111000
end  
  return

hero_draw_s2
  REFP0=0
  player0:
  %01100000
  %01100000
  %01101100
  %01111100
  %01111101
  %00111010
  %10111100
  %11111010
  %11111110
  %11010110
  %11010110
  %11111110
  %01111100
  %00111000  
end
  return

hero_draw_w2
  REFP0=8
  goto hero_draw_e2_start
hero_draw_e2
  REFP0=0
hero_draw_e2_start
  player0:
  %11000011
  %01100110
  %00111100
  %10111001
  %11111110
  %10111100
  %10111000
  %11111100
  %11110100
  %11110110
  %11111110
  %01111100
  %00111000
end
  return




#if 0 // we don't need this one 
music_change_note 
  AUDF0 = music_data[music_pointer]
  AUDC0 = music_instrument
  if music_data[music_pointer] = $FF then AUDV0 = 0 else AUDV0 = 8
  music_pointer = music_pointer + 1  
  music_timer = music_data[music_pointer]
  music_pointer = music_pointer + 1
  if music_pointer > music_notes then music_pointer = 0
  return
#endif

  bank 2

  set kernel_options pfcolors

room_draw
  rem 'Reset the monster missile and item flags
  missile1x = nowhere
  missile1y = nowhere
  item_flags = 0
  rem 'Set up item and draw it
  goto item_setup bank4
item_setup_done

  if hero_room > 39 goto room_draw_40_local
  if hero_room > 63 goto room_draw_64

  on hero_room goto r00 r01 r02 r03 r04 r05 r06 r07 r08 r09 r10 r11 r12 r13 r14 r15 r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31 r32 r33 r34 r35 r36  r37 r38 r39 r40
  rem r41 r42 r43 r44 r45 r46 r47 r48 r49 r50
  rem r51 r52 r53 r54 r55 r56 r57 r58 r59 r60 r61 r62 r63 r64

room_draw_40_local
  goto room_draw_40 bank3

room_draw_64
  rem 'If we get here, the room number s not valid. Go to room room_start (49)
  hero_room = room_start
  goto r00

  rem r64
  rem r65 r66 r67 r68 r69 r70 r71 r72 r73 r74 r75 r76 r77 r78 r79 r80 r81 r82 r83 r84 r85 r86 r87 r88 r89 r90 r91 r92 r93 r94 r95 r96 r97 r98 r99 r100
room_draw_end
  rem 'Clear bottom scrolling playfield row, so collision detection works properly
  var44 = 0
  var45 = 0
  var46 = 0
  var47 = 0
  return otherbank
end


r00
  pfcolors:
  skyblue
  skyblue
  skyblue
  skyblue
  red
  red
  red
  red
  red
  red
  red
  skyblue
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X..............................X
  X..............................X
  X..............................X
  X..............................X
  X..............................X
  X..............................X
  XXXXXXXXXXXXXX....XXXXXXXXXXXXXX
end
  goto room_draw_end
r01
  if quest_flags{1} then COLUBK=gray else COLUBK=black
  pfcolors:
  black
  black
  black
  black
  black
  black
  black
  black
  black
  black
  black
  black
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X..............................X
  X..............................X
  X..............................X 
  X...............................
  X...............................
  X......XX..............XX.......
  X..............................X
  X..............................X
  X..............................X
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end
r02
  if quest_flags{1} then COLUBK=gray else COLUBK=black
  pfcolors:
  black
  black
  black
  black
  black
  black
  black
  black
  black
  black
  black
  black
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X........XX...............XX...X
  X........XX...............XX...X
  X...XX...XX...XXXXXXXXXX..XX...X 
  ....XX...XX...X................X
  ....XX...XX...X................X
  ....XX........X.......XXXXXXXXXX
  XXXXXXXXXXXXXXXXXXX............X
  X..............................X
  X..............................X
  XXXXXXXXXXXXXX....XXXXXXXXXXXXXX
end
  goto room_draw_end
r03
  COLUBK = sand
  pfcolors:
  brown
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  white
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
end
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X.........XXXXXXXXXXXX..........
  X.........XXXXXXXXXXXX..........
  X.........XXXXXXXXXXXX..........
  X.........XXXXXXXXXXXX..........
  X.........XXXXXXXXXXXX..........
  X........XXXXXXXXXXXXXX.........
  XX......XXXXXXXXXXXXXXXX........
  XX.....XXXXXXXXXXXXXXXXXX.......
  XX....XXXXXXXXXXXXXXXXXXXXX.....
  XX.....XXXXXXXXXXXXXXXXXX.......
end
  goto room_draw_end
r04
  COLUBK=gray
  pfcolors:
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  ...............................X
  ...............X...............X
  ..XX........XX.X.XX........XX..X 
  ..XX.......X..XXX..X.......XX..X
  ............XX.X.XX............X
  ...............X...............X
  ..XX....XX...........XX....XX..X
  ..XX....XX...........XX....XX..X  
  ...............................X
  ...............................X  
end
  goto room_draw_end
r05
  COLUBK=turfy
  pfcolors:
  green
  green
  green
  green
  green
  brown
  brown
  brown
  brown
  brown
  brown
  green
end  
  playfield:
  XXXXXXXX..XXXXXXXXXX..XXXXXXXXXX
  X.XXXX...XXXXXXXXXXXX...XXXXXXXX
  X.......XXXXXXXXXXXXXXX........X
  X......XXXXXXXXXXXXXXX.........X 
  X.......XXXXXXXXXXXXXX.........X
  X...........XXXXXX.............X
  X...........XXXXXX.............X
  X..........X......X............X
  X..............................X
  X..............................X
  X..............................X
end
  goto room_draw_end
r06
  COLUBK=turfy
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green  
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XX.......XX.....................
  XX.......XX.....................
  XX...XX..XX....XXXXXXXXXXXXXXXXX 
  XX...XX........XX...........X...
  XX...XX........XX...........X...
  XX...XXXXXX....XX......XX...XXXX
  XX...XXX...............XX.......
  XX....XX...............XX.......
  XX....XX...XXXXXXXXX...XX.......
  XX....XX...XXXXXXXXX...XX.....XX
end
  goto room_draw_end
r07
  COLUBK=turfy
  pfcolors:
  darkgreen
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green  
end
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  .............X.................X
  .............X.................X
  XXXXXXXXX....X...XXXXXXXXX.....X
  .............X...XXX...........X
  .............X...XXX...........X
  XXXXXX.....XXX...XXX...XXX.....X
  .....X.....XXX...XXX...XXX.....X
  .....X.....XXX...XXX...XXX.....X
  .....X.....XXX...XXX...XXX.....X
  XX...X.....XXX...XXX...XXX.....X
end  
  goto room_draw_end
r08
  COLUBK=brown
  pfcolors:
  red
  red
  red
  red
  red
  red
  red
  red
  red
  red
  red
  red
end  
  if quest_flags{3} goto r08_open
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXX....XXXXXXXXXXXXXX
  XXXX.....................XXXXXXX 
  XX........................XXXXXX
  XX..............................
  X...............................
  X...............................
  XXXXX..................XXXXXXXXX
  XXXXXXX..............XXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto r08_end
r08_open
  playfield:
  XXXXXXXXXXXXXX....XXXXXXXXXXXXXX
  XXXXXXXXXXXXXX....XXXXXXXXXXXXXX
  XXXXXXXXXXXXXX....XXXXXXXXXXXXXX
  XXXX.....................XXXXXXX 
  XX........................XXXXXX
  XX..............................
  X...............................
  X...............................
  XXXXX..................XXXXXXXXX
  XXXXXXX..............XXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
r08_end
  goto room_draw_end
r09
  COLUBK=sand
  pfcolors:
  black
  red
  red
  red
  red
  red
  red
  red
  red
  red
  red
  black
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXX.............
  XXXXXXXXXXX.....................
  XXXXXXXXX....................XXX 
  XXXXXXX.....XXXXXXXXXXXXXXXXXXXX
  ...XX......XXX..................
  ...........XX...................
  ......XXXXXX......XXXX....XXXXXX
  XX...XXX........XXXXXXX.........
  XXXXXXX.......XXXXXXXXXX........
  XXXXXX.......XXXXXXXXXXXX....XXX
end
  goto room_draw_end
r10
  COLUBK=sand
  pfcolors:
  black
  red
  red
  red
  red
  red
  red
  red
  red
  red
  red
  black
end
  playfield:
  XXXXXXXXXXXXXX....XXXXXXXXXXXXXX
  ............XX.................X
  .............XX................X
  XXX...........XX.....XX........X 
  XXXXXXXXXX.....XX...XX........XX
  ...................XX.......XXXX
  ..................XX......XXXX..
  XXXXXXXXXXXXXXXXXXX.....XXXX....
  .....................XXXXXX.....
  ....................XXXX........
  XXXXXXXXXXXXXXXXXXXXXXX.........
end  
  goto room_draw_end
r11
  COLUBK=turfy
  pfcolors:
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
end  
  playfield:
  XX.......XXXXXXXXXXXXXX.........
  XX.......XXXXXXXXXXXXXX.........
  XX........XXXXXXXXXXXX..........
  XX........XXXXXXXXXXXX.......... 
  XX........XXXXXXXXXXXX..........
  XX........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
end
  goto room_draw_end
r12
  COLUBK=gray
  pfcolors:
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
end  
  playfield:
  ...............................X
  ...............................X
  ...............................X
  ..XX....XX...........XX....XX..X
  ..XX....XX...........XX....XX..X
  ...............................X
  ...............................X
  ..XX....XX...........XX....XX..X
  ..XX....XX...........XX....XX..X
  ...............................X
  ...............................X
end
  goto room_draw_end
r13
  COLUBK=turfy
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
end
  playfield:
  XXXXXXXXXXXXXX...XXXXXXXXXXXXXXX
  X..............................X
  X..............................X
  X..............................X 
  X..............................X
  X..............................X
  X..............................X
  X..............................X
  X...............................
  X...............................
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end
r14
  COLUBK=turfy
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
end
  playfield:
  XXX...XX...XXXXXXXXX...XX.....XX
  XXX...XX...XXXXXXXXX...XX.......
  XXX...XX...............XX.......
  XXX...XX...............XX.......
  XXX...XXXXXXXXXXXXXX...XXXXXXXXX
  XXX.............................
  XXX.............................
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  ................................
  ................................
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end  
  goto room_draw_end
r15
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
end  
  playfield:
  XX...XXX...XXX...XXX...XXX.....X
  .....XXX...XXX...XXX...XXX.....X
  .....XXX.......................X
  .....XXX.......................X
  XXXXXXXXXXXXXXXXXXXXXXXXXXX....X
  ....XXX........................X
  ....XXX........................X
  XX..XXX....XXXXXXXXXXXXXXXXXXXXX
  ....XXX....X...................X
  ....XXX....X...................X
  XXXXXXX....XXXXXXXXXXXXXXX.....X
end
  goto room_draw_end
r16
  COLUBK=yellow
  pfcolors:
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X..............................X
  X..............................X
  X...XX....................XX...X 
  X...XX....................XX...X
  X...XX....................XX...X
  X..............................X
  X...XX....................XX...X
  X...XX....................XX...X
  X...XX....................XX...X
  X..............................X
end
  goto room_draw_end
r17
  COLUBK=sand
  pfcolors:
  brown
  brown
  brown
  brown
  red
  red
  red
  red
  red
  red
  red
  red
end  
  playfield:
  XXXXXX.......XXXXXXXXXXXX....XXX
  XXXXXXXX.......XXXXXXXX.......XX
  XXXXXXXXXX..........XX..........
  XXXXXXXXXXXXX........XX......... 
  XXXXXXXXXXXXXXXX......XX........
  XXXXXXXXXXXXXXXXXX....XX........
  XXXXXXXXXXXXXXXX....XXXX........
  XXXXXXXXXXXXXXX....XXXX.........
  XXXXXXXXXXXXXXXXX...............
  XXXXXXXXXXXXXXXXXXX.............
  XXXXXXXXXXXXXXXXXXXXXX..........
end
  goto room_draw_end
r18
  COLUBK = turfy
  pfcolors:
  red
  red
  white
  white
  red + 8
  red + 6
  red + 4
  white
  white
  white
  white
  red
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXX.........
  XXXXXXXXXXXXXXXXXXX.............
  ................................
  ................................
  ...XXXXXXXXXXXXXX...............
  ..XXXXXXXXXXXXXXXX..............
  .XXXXXXXXXXXXXXXXXX.............
  ..XXXXXXXXXXXXXXXX..............
  ..XX..XX...XXX..XX..............
  ..XXXXXX...XXXXXXX..............
  ................................
end  
  goto room_draw_end
r19
  COLUBK=sand
  pfcolors:
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
end  
  playfield:
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXXX.........
  ..........XXXXXXXXXXXXX......... 
  .........XXXXXXXXXXXXXX.........
  .........XXXXXXXXXXXXXX.........
  .........XXXXXXXXXXXXX..........
  .........XXXXXXXXXXXXX..........
  .........XXXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
end
  goto room_draw_end
r20
  COLUBK=sand
  pfcolors:
  green
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray  
  green
end  
  playfield:
  ...............................X
  ....X.X.X.....XX.XX.XX.....X.X.X
  ....XXXXX.....XXXXXXXX.....XXXXX
  ....X...X.....X......X.....X...X 
  ....X...XXXXXXX......XXXXXXX...X
  ....X...........................
  ....X...........................
  ....X...XXXXXXXXXXXXXXXXXXXXXXXX
  ....X............XX.............
  ....X............XX.............
  ....X............XX...........XX
end
  goto room_draw_end
r21
  COLUBK=turfy
  pfcolors:
  green
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray  
  green
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  ....X.X.X......X.XX.X......X.X.X
  ....XXXXX......XXXXXX......XXXXX
  ....X...XXXXXXXX....XXXXXXXX...X
  XXXXX..........................X
  ...............................X
  ...............................X
  XXXXX..........................X
  ....X..........................X
  ....X..........................X
  XX..XXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end
r22
  COLUBK=turfy
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  green
  brown
  green
  green
  green
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXX...XXX...XXXXXXXXXXXXXX
  XXXXXXX.............XXXXXXXXXXXX
  XXXXXX................XXXXXXXXXX 
  XXXX....................XXXXXXXX
  XXXX.......................XXXXX
  XXX.........................XXXX
  XX....X..............X.......XXX
  XX...XXXX...........XXXX........
  X.....X..............X..........
  X...............................
end
  goto room_draw_end
r23
  COLUBK=turfy
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
end  
  playfield:
  XXXXXXX...XXXXXXXXXXXXXXXX.....X
  XXXXXXX...XXXXXXXXXXXXXXXX.....X
  XXXXXXX....XXXXXXXXXXXXX.......X
  XXXXXXX.....XXXXXXXXXXX........X 
  XXXXXXX........................X
  XXXXXXX........................X
  XXXXXXXXXXXXXXXX..XXXXXXXXXXXXXX
  .....X......X.......X..........X
  ....XXXX...XXXX....XXXX........X
  .....X......X.......X..........X
  ...............................X
end
  goto room_draw_end
r24
  COLUBK=yellow
  pfcolors:
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
end
  if quest_flags{5} then goto r24_open  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X..............................X
  X...XX....................XX...X
  X...XX....................XX...X
  X...XX....................XX...X
  X.............XXXX.............X
  X...XX.......XXXXXX.......XX...X
  X...XX....................XX...X
  X...XX....................XX...X
  X..............................X
  X......XXXXXXX....XXXXXXX.....XX
end
  goto r24_end
r24_open
  playfield:
  X..............................X
  X..............................X
  X...XX....................XX...X
  X...XX....................XX...X
  X...XX....................XX...X
  X.............XXXX.............X
  X...XX.......XXXXXX.......XX...X
  X...XX....................XX...X
  X...XX....................XX...X
  X..............................X
  X......XXXXXXX....XXXXXXX.....XX
end
r24_end
  goto room_draw_end
r25
  COLUBK=turfy
  pfcolors:
  red
  red
  red + 8
  red + 6
  red + 4
  white
  white
  white
  white
  white
  white
  red
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXX..........
  XXXXXXXXXXXXXXXXXXXX............
  XXXXXXXXXXXXXXXXXX.XXXXXXXXXX... 
  XXXXXXXXXXXXXXX...XXXXXXXXXXXX..
  XXXXXXXXXXXXX....XXXXXXXXXXXXXX.
  XXXXXXXXXXX.......XXXXXXXXXXXX..
  XXXXXXXXX.........XXX..XX...XX..
  XXXXXX............XXXXXXX...XX..
  XXXXX...........................
  XX..............................
  XX..............................
end
  goto room_draw_end
r26
  COLUBK = turfy
  pfcolors:
  white
  white
  red + 8
  red + 6
  red + 4
  white
  white
  white
  white
  red
  red
  red
end  
  playfield:
  ................................
  ................................
  ...XXXXXXX.....XXXXXXXXXXXXX....
  ..XXXXXXXXX...XXXXXXXXXXXXXXX...
  .XXXXXXXXXXX.XXXXXXXXXXXXXXXXX..
  ..XXXXXXXXX...XXXXXXXXXXXXXXX...
  ..XX.X...XX...XX..XX...XX..XX...
  ..XXXX...XX...XXXXXX...XXXXXX...
  ................................
  ................................
  ................................
end  
  goto room_draw_end
r27
  COLUBK=sand
  pfcolors:
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
end  
  playfield:
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXXX......... 
  ..........XXXXXXXXXXXXX.........
  ..........XXXXXXXXXXXXX.........
  ..........XXXXXXXXXXXXX.........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
end
  goto room_draw_end
r28
  COLUBK=sand
  pfcolors:
  green
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray  
  green
end  
  playfield:
  ....X............XX...........XX
  ....XXXXXXXXXX...XX.....XXXXXXXX
  ....X............XX...........XX
  ....X............XX...........XX
  ....X...XXXXXXXXXXXXXXXXXXX...XX
  ....X...X.................X...XX
  ....X.........................XX
  ....XX.....XXXXXXXXXXXXXX....XXX
  ....XXXXXXXXX..........XXXXXXXXX
  ................................
  ................................
end
  goto room_draw_end
r29
  COLUBK=turfy
  pfcolors:
  green
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray  
  green
end
  if quest_flags{6} then goto r29_open
  if item_flags{3} then goto r29_open   
  playfield:
  XX..XXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XX..X.........................XX
  XX..X.........................XX
  XX..X..XXXXXXXXXXXXXXXXXX.....XX
  XX..X..XXXXXXX.XX.XXXXXXX.....XX
  XX..X..XXXXX.X.XX.X.XXXXX.....XX
  XX.....XXXXX.XXXXXX.XXXXX.....XX
  XX.....XXXXX.X.XX.X.XXXXX.....XX
  XXXXXXXXX..............XXXXXXXXX
  ..............................XX
  ..............................XX
end
  goto room_draw_end
r29_open 
  playfield:
  XX..XXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XX..X.........................XX
  XX..X.........................XX
  XX..X..XXXXXXXX..XXXXXXXX.....XX
  XX..X..XXXXXX......XXXXXX.....XX
  XX..X..XXXXX........XXXXX.....XX
  XX.....XXXXX........XXXXX.....XX
  XX.....XXXXX........XXXXX.....XX
  XXXXXXXXX..............XXXXXXXXX
  ..............................XX
  ..............................XX
end
  goto room_draw_end
r30
  pfcolors:
  green  
  green
  green
  green
  brown  
  green
  green
  green
  green
  brown
  green
  green
end  
  playfield:
  X...............................
  XX......X.....X.....X.....X.....
  XXX....XXX...XXX...XXX...XXX....
  XX....XXXXX.XXXXX.XXXXX.XXXXX...
  X.......X.....X.....X.....X.....
  XX..............................
  XX...X......X......X.....X......
  X...XXX....XXX....XXX...XXX.....
  X..XXXXX..XXXXX..XXXXX.XXXXX....
  X....X......X......X.....X......
  X.............................XX
  XX............................XX
end
  goto room_draw_end
r31
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  brown
  brown
  brown
  green
  green
end
  playfield:
  ...............................X
  ...............................X
  ...............................X
  .......XXX..............XXX....X 
  ......XXXXX.....X......XXXXX...X
  ......XXXXX....XXX.....XXXXX...X
  .......XXX....XXXXX.....XXX....X
  ........X.......X........X.....X
  ........X................X.....X
  ...............................X
  XXX...........................XX
end
  goto room_draw_end

r32  
  COLUBK=turfy
  pfcolors:
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
  white
end  
  if quest_flags{2} then goto r32_open
  playfield: 
  X.....XXXXXXXXXXXXXXXXXXX.....XX
  X....XXXXXXXXXXXXXXXXXXXXX....XX
  X...XXXXXXXXXXXXXXXXXXXXXXX...XX
  X..XXXXXXXXXXXXXXXXXXXXXXXXX..XX
  X.XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X..XX..XX..XX..XX..XX..XX..XX...
  X..XX..XX..XX..XX..XX..XX..XX...
  X..XX..XX..XX..XX..XX..XX..XX...
  X.XXXXXXXXXXXXXXXXXXXXXXXXXXXX..
  X...............................
  X...............................
end
  goto room_draw_end bank2
r32_open
  playfield:
  X.....XXXXXXXX....XXXXXXX.....XX
  X....XXXXXXXXX....XXXXXXXX....XX
  X...XXXXXXXXX......XXXXXXXX...XX
  X..XXXXXXXXXX......XXXXXXXXX..XX
  X.XXXXXXXXXXXX....XXXXXXXXXXXXXX
  X..XX..XX..XX......XX..XX..XX...
  X..XX..XX..XX......XX..XX..XX...
  X..XX..XX..XX......XX..XX..XX...
  X.XXXXXXXXXXX......XXXXXXXXXXX..
  X...............................
  X...............................
end
  goto room_draw_end bank2
r33
  pfcolors:
  white
  white
  white
  white
  red + 8
  red + 6
  white
  white
  white
  white
  white
  white
end  
  playfield:
  XX..............................
  XX.............XXX..............
  XX...........XX...XX............
  XX.............XXX.............. 
  XX..............................
  ....XXXXXXXXXXXX....XXXXXXXXXX..
  ...XXXXXXXXXXXXXX..XXXXXXXXXXXX.
  ....XXXXXXXXXXXX....XXXXXXXXXX..
  ....XX.XX...XX.X....XX.XX...XX..
  ....XXXXX...XXXX....XXXXX...XX..
  ................................
end
  goto room_draw_end bank2
r34
  COLUBK = turfy
  pfcolors:
  white
  white
  red + 8
  red + 6
  red + 4
  white
  white
  white
  white
  red
  red
  red
end  
  playfield:
  ................................
  ................................
  ...XXXXXXXXXXXXX......XXXXXXX...
  ..XXXXXXXXXXXXXXX....XXXXXXXXX..
  .XXXXXXXXXXXXXXXXX..XXXXXXXXXXX.
  ..XXXXXXXXXXXXXXX....XXXXXXXXX..
  ..XX..XX...XX..XX....XX.XX...X..
  ..XXXXXX...XXXXXX....XXXXX...X..
  ................................
  ................................
  ................................
end
  goto room_draw_end bank2
r35
  COLUBK=sand
  pfcolors:
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
  riverblue
end  
  if quest_flags{4} then goto r35_open
  playfield:
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ...........XXXXXXXXXX...........
  ............XXXXXXXX............
  ............XXXXXXXX............
  ...........XXXXXXXXXX...........
  ..........XXXXXXXXXXXX..........
  .........XXXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
end
  goto room_draw_end bank2
r35_open
  playfield:
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ...........XXXXXXXXXX...........
  ................................
  ................................
  ...........XXXXXXXXXX...........
  ..........XXXXXXXXXXXX..........
  .........XXXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXX..........
end
  goto room_draw_end bank2
r36
  COLUBK=turfy
  pfcolors:
  green
  yellow
  green
  yellow
  green
  green
  yellow
  green
  yellow
  green
  yellow
  green
end  
  playfield:
  ................................
  ..XXXXXXXX..XXXXXXXX..XXXXXXXX..
  ..XXXXXXXX..XXXXXXXX..XXXXXXXX..
  ..XXXXXXXX..XXXXXXXX..XXXXXXXX.. 
  ..XXXXXXXX..XXXXXXXX..XXXXXXXX..
  ................................
  ..XXXXXXXX..XXXXXXXX..XXXXXXXX..
  ..XXXXXXXX..XXXXXXXX..XXXXXXXX..
  ..XXXXXXXX..XXXXXXXX..XXXXXXXX..
  ..XXXXXXXX..XXXXXXXX..XXXXXXXX..
  ................................
end
  goto room_draw_end bank2  
r37
  COLUBK=turfy
  pfcolors:
  green
  yellow
  green
  yellow
  green
  green
  yellow
  green
  yellow
  green
  yellow
  green
end  
  playfield:
  ................................
  ..XXXXXXX..XXXXXXXXXX..XXXXXXX..
  ..XXXXXXX..XXXXXXXXXX..XXXXXXX..
  ..XXXXXXX..XXXXXXXXXX..XXXXXXX.. 
  ..XXXXXXX..XXXXXXXXXX..XXXXXXX..
  ................................
  ..XXXXXXX..XXXXXXXXXX..XXXXXXX..
  ..XXXXXXX..XXXXXXXXXX..XXXXXXX..
  ..XXXXXXX..XXXXXXXXXX..XXXXXXX..
  ..XXXXXXX..XXXXXXXXXX..XXXXXXX..
  ................................
end
  goto room_draw_end bank2    
r38
  COLUBK=turfy
  pfcolors:
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
end
  playfield:
  XX............................XX
  ......XXXXXXXXXXXXXXXXXXX.......
  ....XXX................XXXX.....
  ...XX.....................XX....
  ..XX.......................XX...
  ..XX........................XX..
  ..XX........................XX..
  ..XXX.....................XXXX..
  ..X.XXX.................XXX.XX..
  ..X.......XXXXXXXXXXXXXX.XX.XX.. 
  ..X.......X.XXXXXXXX.XXX.XXXXX..
end
  goto room_draw_end bank2
r39
  COLUBK=turfy
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
end  
  playfield:
  XXX...........................XX
  ..XX...X........X.......X....XXX
  ..XX..XXX......XXX.....XXX...XXX 
  ...XX..X...X....X.......X.....XX
  ..XX......XXX.................XX
  ...XX......X.......X.........XXX
  ..XX.........X....XXX.........XX
  ...XX...X...XXX....X....X....XXX
  ..XX...XXX...X.........XXX...XXX
  ...XX...X...............X.....XX
  ..XX...........................X
end
  goto room_draw_end bank2

  bank 3

room_draw_40
  temp1=hero_room-40
  on temp1 goto r40 r41 r42 r43 r44 r45 r46 r47 r48 r49 r50 r51 r52 r53 r54 r55 r56 r57 r58 r59 r60 r61 r62 r63
  goto room_draw_end bank2

r40
  pfcolors:
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
end  
  playfield:
  X...............................
  XXX.............................
  XXXXX...........................
  XXXXXXX......................... 
  XXXXXXXXXX......XXX.............
  XXXXXXXX.......XXXXX............
  XXXXXX.......XXXXXXXXX..........
  XXXXXX........XXXXXX............
  XXXXX...........XXXXXX..........
  XXX...............XXXXX.........
  X...............................
end
  goto room_draw_end bank2
  
  
r41
  COLUBK = turfy
  pfcolors:
  green  
  green
  green
  green
  brown  
  green
  green
  green
  green
  brown
  SEABLUE
  green
end  
  playfield:
  ................................
  .............X..................
  ............XXX.................
  ...........XXXXX................
  .............X..................
  .....X..........................
  ....XXX............X............
  ....XXXX..........XXX...........
  ...XXXXX.........XXXXX..........
  .....X.............X............
  ................................
end
  goto room_draw_end bank2
r42
  COLUBK = turfy
  pfcolors:
  green  
  green
  green
  green
  brown  
  green
  green
  green
  green
  brown
  SEABLUE
  green
end  
  playfield:
  ................................
  ..........................X.....
  .........................XXX....
  ........................XXXXX...
  ..........................X.....
  .....X..........................
  ....XXX............X............
  ....XXXX..........XXX...........
  ...XXXXX.........XXXXX..........
  .....X.............X..........XX
  ..............................XX
end
  goto room_draw_end bank2
r43
  COLUBK = sand
  pfcolors:
  riverblue
  riverblue
  riverblue
  riverblue
  SEABLUE + 8
  SEABLUE + 8
  SEABLUE + 4
  SEABLUE + 4
  SEABLUE + 2
  SEABLUE + 2
  SEABLUE
  SEABLUE
end  
  playfield:
  ..........XXXXXXXXXXXX..........
  ..........XXXXXXXXXXXXX.........
  ..........XXXXXXXXXXXXX.........
  ..........XXXXXXXXXXXXXX........ 
  .........XXXXXXXXXXXXXXX........
  .........XXXXXXXXXXXXXXXXX......
  .......XXXXXXXXXXXXXXXXXXXXX....
  .....XXXXXXXXXXXXXXXXXXXXXXXX...
  ..XXXXXXXXXXXXXXXXXXXXXXXXXXXXX.
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2
r44
  COLUBK=sand
  pfcolors:
  red
  red
  white
  white
  white
  white
  white
  white
  white
  SEABLUE
  SEABLUE
  red
end  
  playfield:
  ................................
  ........X.........X.............
  .......XXX.......XXX............
  ......XXXXX.....XXXXX...........
  .....XXX.XXX...XXX.XXX...X...... 
  ....XXX...XXX.XXX...XXX.XXX.....
  .......................XXXXX....
  ......................XXX.XXX...
  .....................XXX...XXX..
  XX..............................
  XX..............................
end  
  goto room_draw_end bank2
r45
  COLUBK = sand
  pfcolors:
  brown
  brown
  brown
  green
  green
  green
  brown
  brown  
  brown
  brown
  brown
  green
end  
  playfield:
  ................................
  ................................
  ................................
  ................................
  ...XXX..XXX...XXXX....XXX..XXX..
  ..X...XX...X.XXXXXX..X...XX...X.
  ......XX.....XXXXXX.....XX......
  .......XX....XX..XX.....XX......
  .......XX....XX..XX....XX.......  
  ........XX.............XX.......
  ................................
end 
 goto room_draw_end bank2
r46
  COLUBK=turfy
  pfcolors:
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray  
end  
  playfield:
  ..X.......X.XXX..XXX.XXXXXXXXX..
  ..X....XXXXXXXX..XXXXXXX.XXXXX..
  ..X.......XXXXXXXXXX.XXX.XXXXX..
  ..XXX........XXXXXXX.XXXXXX.XX.. 
  ..XX.XXX..........XXXXXXXXX.XX..
  ..XXXXX.XXX..........XXX.XXXXX..
  ..XXXXX.XXXXXX..........XXXXXX..
  ..XX.XXXXXX.XXXXXXX........XXX..
  ..XX.XXXXXXXXXX..XXXXX.......X..
  ..XXXXX.XXXXXXXXXXXXXXXXX....X..
  ..XXXXX.XXX.XXXXXXXX.X.....XXX..
  ..XX.XXXXXX.XXX..XXX.X.....XXX..
end
  goto room_draw_end bank2
r47
  pfcolors:
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
  green
end  
  playfield:
  ..XX...........................X
  ...XX..X....X.....X.....X......X
  ..XX..XXX..XXX...XXX...XXX....XX
  ...XX..X....X.....X.....X.....XX 
  ..XX.....X.....X.....X.......XXX
  ...XX...XXX...XXX...XXX......XXX
  ...XX....X.....X.....X......XXXX
  ..XX...X....X.....X.....X....XXX
  ..XX..XXX..XXX...XXX...XXX....XX
  ..XX...X....X.....X.....X......X
  ...XX..........................X
end
  goto room_draw_end bank2

  

r48
  COLUBK = turfy
  pfcolors:
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
  brown
end  
  playfield:
  X...............................
  X...........X...................
  X............................... 
  X.....X.....................X...
  X...............................
  X...............................
  X............................... 
  X...................X...........
  X...............................
  X..X............................
  X...............X...............
end
  goto room_draw_end bank2

r49
  COLUBK=turfy
  pfcolors:
  white
  white
  red + 8
  red + 6
  red + 4
  white
  white
  white
  white
  green
  green
  red
end  
  playfield:
  ................................
  ................................
  .......XXXXXXXXXXXXXXXXXX....... 
  ......XXXXXXXXXXXXXXXXXXXX......
  .....XXXXXXXXXXXXXXXXXXXXXX.....
  ......XXXXXXXXXXXXXXXXXXXX...... 
  ......XXX..XXX...XXXX..XXX......
  ......XXXXXXXX...XXXXXXXXX......
  ................................
  ................................
  ................................
end
  COLUPF=red
  goto room_draw_end bank2

r50
  COLUBK = sand + 0
  pfcolors:
  riverblue
  riverblue
  riverblue
  riverblue
  SEABLUE + 8
  SEABLUE + 7
  SEABLUE + 6
  SEABLUE + 5
  SEABLUE + 4
  SEABLUE + 3
  SEABLUE
  riverblue
end  
  playfield:  
  ..............................XX
  .............................XXX
  ...........................XXXXX
  .........................XXXXXXX 
  ........................XXXXXXXX
  ......................XXXXXXXXXX
  .....................XXXXXXXXXXX
  ...................XXXXXXXXXXXXX
  ..................XXXXXXXXXXXXXX
  ..................XXXXXXXXXXXXXX
  .................XXXXXXXXXXXXXXX
  ................XXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2
r51
  rem 'black ship top
  COLUBK = SEABLUE + 2
  pfcolors:
  white
  white
  white
  white
  white
  white
  black
  black
  black
  black
  black
  black
end  
  playfield:
  .............XXXXXXXXXXXXX......
  ...........XXXXXXXXXXXXXXX......
  ..........XXXXXXXXXXXXXXXX......
  ........XXXXXXXXXXXXXXXXXX...... 
  ......XXXXXXXXXXXXXXXXXXXX......
  .....XXXXXXXXXXXXXXXXXXXXX......
  ...............XX...............
  ....XXXXXXXXXXXXXXXXXXXXXXXXXX..
  ...XX..........XX............XX.
  ..XXX..........XX............XX.
  .XXXX..........XX............XXX
end
  goto room_draw_end bank2
r52
  COLUBK = sand
  pfcolors:
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
end  
  playfield:
  XX..............................
  XXX.............................
  XXXX............................
  XXXXX........................... 
  XXXXXXX.........................
  XXXXXXXXXXX.....................
  XXXXXXXXXXXXXX..................
  XXXXXXXXXXXXXXXXX...............
  XXXXXXXXXXXXXXXXXXXXX...........
  XXXXXXXXXXXXXXXXXXXXXXXXXX......
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2
r53
  COLUBK = sand
  pfcolors:
  green
  green
  green
  brown
  brown  
  brown
  brown
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  green
end  
  playfield:
  ................................
  ......XXX..XXX......XXX..XXX....
  .....X...XX...X....X...XX...X...
  .........XX............XX.......
  ........XX............XX........
  .......XX............XX.........
  ......XX............XX..........
  ................................
  ................................
  ................................
  XX..............................
end
  goto room_draw_end bank2 
r54
  COLUBK = turfy
  pfcolors:
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  gray
  SEABLUE
  gray  
end
  if quest_flags{3} then goto r54_open  
  playfield:
  ..XX.XXXXXX.XXXXXXXX.XXXXXX.XX..
  ..XX.XXXXXXXXXX..XXXXXXXXXX.XX..
  ..XXXXX.XXXXXXX..XXXXXXX.XXXXX..
  ..XXXXX.XXX.XXXXXXXX.XXX.XXXXX..
  ..XX.XXXXXX.XXXXXXXX.XXXXXX.XX..
  ..XX.XXXXXXXX.XX...XXXXXXXX.XX..
  ..XXXXX.XXXXXXX.X.X.XXXX.XXXX...
  ...XXXX.XXX.XX.X.X.X.XXX.XXX....
  ....XXXXXXX.XX.X.X.X.XXXXXX.....
  ......XXXXXXXXXXXXXXXXXXX.....XX
  ..............................XX
end
  goto room_draw_end bank2    
r54_open
  playfield:
  ..XX.XXXXXX.XXXXXXXXXX.....XXX..
  ..XX.XXXXXXXX...........XXXXXX..
  ..XXXXX.XXXXX........XXX.XXXXX..
  ..XXXXX.XXX.X.....XXXXXX.XXXXX..
  ..XX.XXXXXX.X...XXXX.XXXXXX.XX..
  ..XX.XXXXXXXX......XXXXXXXX.XX..
  ..XXXXX.XXXXXXX..XXXXXXX.XXXX...
  ...XXXX.XXX.XX....XX.XXX.XXX....
  ....XXXXXXX.XX....XX.XXXXXX.....
  ......XXXXXXXX....XXXXXXX.....XX
  ..............................XX
end
  goto room_draw_end bank2    
r55
  COLUBK = sand
  pfcolors:
  red
  red
  white
  white
  white
  white
  red
  red
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
end
  if quest_flags{7} then goto r55_open
  playfield:
  ...XX..........................X
  ...............................X
  .......XXXXXXX.................X
  ......XXXXXXXXX................X
  .......X.X.X.X.................X 
  ...............................X
  ...............................X
  ..................XXXXXXXXXXXXXX
  ...XXXX.........XXXX......XXXXXX
  XXXXXXXXX...XXXXXXXXXX...XXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2
r55_open  
  playfield:
  ...XX..........................X
  ...............................X
  .......XXXXXXX.................X
  ......XXXXXXXXX................X
  .......X.X.X.X.................X 
  ...............................X
  ...............................X
  ............................XXXX
  ...XXXX.........XXXX......XXXXXX
  XX..XX...........X...........XXX
  XX.............................X
end
  goto room_draw_end bank2
r56
  rem 'Shield on island
  COLUBK = sand
  pfcolors:
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE  
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
end  
  if !quest_flags{6} then goto r56_closed
  playfield:
  X...............................
  X...............................
  XXX.............................
  XXXXX...........................
  XXXXX..XX.......................
  XXXXX..XXXXXXX..................
  XXXX....XXXXXXXXXX..............
  XXX......XXXXXXXXXXXX...........
  XXX......XXXXXXXXXXXXXXXXX......
  XXXX....XXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2
r56_closed
  playfield:
  X...............................
  X...............................
  XXX.............................
  XXXXXX..........................
  XXXXXXXXX.......................
  XXXXXXXXXXXXXX..................
  XXXX....XXXXXXXXXX..............
  XXX......XXXXXXXXXXXX...........
  XXX......XXXXXXXXXXXXXXXXX......
  XXXX....XXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
r56_end
  goto room_draw_end bank2
r57
  COLUBK = sand
  pfcolors:
  red
  red
  red
  red
  red
  red
  red
  red
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
end  
  playfield:
  ................................
  ................................
  ................................
  ................................ 
  ................................
  ......................X.........
  .....................XXX........
  ................................
  .....XXXX...XXXXXX.......XXXX...
  XXXXXXXXXXXXXXXXXXXX...XXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2

r58
  COLUBK = sand
  pfcolors:
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
end  
  if quest_flags{2} goto r58_open
  playfield:
  ................XXXXXXXXXXXXXXXX
  ................XXXXXXXXXXXXXXXX
  ...............XXXXXXXXXXXXXXXXX
  ..............XXXXXXXXXXXXXXXXXX
  ..............XXXXXXXXXXXXXXXXXX
  .............XXXXXXXXXXXXXXXXXXX
  ..........XXXXXXXXXXXXXXXXXXXXXX
  ........XXXXXXXXXXXXXXXXXXXXXXXX
  ....XXXXXXXXXXXXXXXXXXXXXXXX....
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXX...
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2
r58_open
  playfield:
  ................XXXXXXXXXXXXXXXX
  ...................XXXXXXXXXXXXX
  .....................XXXXXXXXXXX
  .......................XXXXXXXXX
  .........................XXXXXXX
  ...........................XXXXX
  .............................XXX
  ................................
  ................................
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2


r59
  COLUBK = SEABLUE + 1
  pfcolors:
  black
  black
  black
  black
  black
  black
  black  
  black
  black
  black
  SEABLUE
  black
end
  rem 'black ship bottom, when switch3 isn't pressed
  if quest_flags{3} goto black_ship_open
  playfield:
  .XXXX..XX......XX...XXXXX....XXX
  XXXXX...............X...X....XXX
  .XXXX........................XXX
  ..XXX......................XXXX. 
  ...XXXXXXXXXXXX...XXXXXXXXXXXXX.
  ....XXXXXXXXXXX...XXXXXXXXXXXXX.
  XXXXXXXXXXXXXXX...XXXXXXXXXXXX..
  ..................XX............
  ..................XX............
  XXXXXXXXXXXXXXXXXXXX............
  XXXXXXXXXXXXXXXXXXXX............
end
  goto black_ship_end
black_ship_open
  playfield:
  .XXXX..XX......XX...XXXXX....XX.
  XXXXX...............X...X....XX.
  .XXXX........................XX.
  ..XXX........................XX. 
  ...XXXXXXXXXXXX...XXXXXXXXXXXX..
  ....XXXXXXXXXXX...XXXXXXXXXXXX..
  XXXXXXXXXXXXXXX...XXXXXXXXXXXXXX
  ................................
  ................................
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
black_ship_end
  goto room_draw_end bank2

r60
  rem 'Boss 3, Kraken room.
  COLUBK = black
  pfcolors:
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
end  
  playfield:
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XX............................XX
  XX............................XX
  ..............................XX
  ..............................XX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2

r61
  COLUBK = sand
  pfcolors:
  riverblue
  riverblue
  riverblue
  riverblue
  SEABLUE + 8
  SEABLUE + 8
  SEABLUE + 4
  SEABLUE + 4
  SEABLUE + 2
  SEABLUE + 2
  SEABLUE
  SEABLUE
end
  if !hero_items{5} then goto r61_closed
  playfield:
  X...............................
  X...............................
  XXX.............................
  XXXXX...........................
  XXXXX..XX.......................
  XXXXX..XXXXXXX..................
  XXXX....XXXXXXXXXX..............
  XXX......X..XXXXXXXXX...........
  XXX......XXX..XX..XXXXXXXX......
  XXXX....XXXXXX..XXXXXXXXXXXXXXXX
  XXXXXXXXXXXX..XX..XXXXXXXXXXXXXX
end
  goto room_draw_end bank2
r61_closed 
  playfield:
  XX..............................
  XXXXX...........................
  XXXXXXXX........................
  XXXXXXXXXXXX....................
  XXXXXXXXXXXXXX..................
  XXXXXXXXXXXXXXXX................
  XXXX....XXXXXXXXXX..............
  XXX......XXXXXXXXXXXX...........
  XXX......XXXXXXXXXXXXXXXXX......
  XXXX....XXXXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2

r62
  COLUBK = sand
  pfcolors:
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
end  
  playfield:
  ...............................X
  ..............................XX
  ..............................XX
  ............................XXXX 
  .............................XXX
  ............................XXXX
  ..........................XXXXXX
  .........................XXXXXXX
  ...XXX......XXXXX.......XXXXXXXX
  XXXXXXXX..XXXXXXXXXX...XXXXXXXXX
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
end
  goto room_draw_end bank2


r63
  COLUBK = sand
  pfcolors:
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
  SEABLUE
end  
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
  goto room_draw_end bank2
  


  bank 4
  rem 'Item sprites as well as initial data for item and sprites are in bank 4
  rem 'Items/monsters/mobiles in any given room
  rem '8 items per line means it corresponds with map layout
  data room_items
  monster_leaf2_boss, monster_leaf1_boss, monster_bat, item_switch4
  monster_zombie, item_victory, monster_treant, monster_wolf
  item_switch1, monster_mushroom, monster_scorpio, monster_mushroom
  monster_zombie, monster_ikaza, monster_wolf, monster_treant,
  monster_sword_boss, monster_scorpio, monster_grunt, monster_fish,
  monster_knight, monster_armor_boss, item_switch2, monster_curse
  item_switch7, monster_archer, monster_grunt, monster_rabid
  monster_archer, monster_grunt, monster_lion, monster_snake
  item_none, monster_mushroom, monster_grunt, monster_fish, 
  monster_cannon, monster_cannon, monster_mage, monster_lion,
  monster_scorpio, monster_slime, monster_bat, monster_snake  
  monster_archer, monster_knight, monster_spider, monster_mushroom
  monster_flower, item_healhp, monster_slime, item_switch3, 
  monster_scorpio, monster_crab, item_none, monster_fish
  item_shield, monster_crab, monster_scorpio, item_none,
  monster_leaf3_boss, item_switch6, monster_crab, monster_strike_boss
end

  data item_hplist
    4,  4,  8,  8,  8, 16, 16, 16, 16, 32, 32, 32, 32, 64, 32, 32
   64, 80,100,  4, 64, 32, 64, 64, 64, 80, 80, 80, 80,100,100,120
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4
end
  
  rem 'Shorthand for the center of screen positions.
  const hc=73
  const vc=49
  
  data item_xlist
  hc, hc, 84, 37,109, 70, hc - 8, hc,
  hc, hc, hc, 37, hc, hc, hc, hc,
  hc,113,113, 37, hc + 12, hc, hc, hc,
  hc, hc, 63,113, hc,120, hc, hc,
  hc, 37, 90, 37, 53, 53, hc, hc,
  77, hc, hc,123, 66, hc, hc, 61,
  hc, hc, hc,113, hc, hc, hc, hc,
  37, hc, 37, 37, hc, 37, hc, hc,
end
  
  data item_ylist
  vc, vc, vc, 25, 70, 65, vc, vc,
  vc, vc, vc, vc, vc, vc, vc, vc + 4,
  vc, vc, vc, vc, vc + 1, vc, vc, vc + 8,
  vc, vc, 70, vc, vc + 8, vc, vc, vc - 16,
  vc, 29, 70, vc, 41, 41, vc, vc,
  33, vc, vc, vc, 70, 25, vc, vc,
  vc, 55, vc, 80, vc, vc, vc, vc,
  73, vc, vc, vc, vc, 73, vc, vc,
end


item_setup
  item_kind = room_items[hero_room]
  gosub item_setup_kind
  goto item_setup_done bank2
  
  rem 'You must set item_kind before gosubbing to this one  
item_setup_kind
  rem 'Initialize item/mobile, and it's position, HP and speed 
  rem 'First, handle switches
  if item_kind < item_switch1 then goto item_switch_end
  if item_kind > item_switch7 then goto item_switch_end
  rem 'set switch to on if needed
  if quest_flags{1} && item_kind = item_switch1 then item_kind = item_kind + item_switch_on
  if quest_flags{2} && item_kind = item_switch2 then item_kind = item_kind + item_switch_on
  if quest_flags{3} && item_kind = item_switch3 then item_kind = item_kind + item_switch_on
  if quest_flags{4} && item_kind = item_switch4 then item_kind = item_kind + item_switch_on
  if quest_flags{5} && item_kind = item_switch5 then item_kind = item_kind + item_switch_on
  if quest_flags{6} && item_kind = item_switch6 then item_kind = item_kind + item_switch_on
  if quest_flags{7} && item_kind = item_switch7 then item_kind = item_kind + item_switch_on  
item_switch_end
  rem 'Next, handle bosses that may already be dead.
  rem 'Ikaza
  if item_kind < monster_leaf1_boss then goto item_boss_end
  if item_kind > monster_ikaza then goto item_boss_end
  if quest_flags{0} && item_kind = monster_ikaza then item_kind = item_none
  rem 'Leaf 1 dropped by boss 1
  if hero_items{0} && item_kind = monster_leaf1_boss then item_kind = item_none
  rem 'Leaf 2 dropped by boss 2
  if hero_items{1} && item_kind = monster_leaf2_boss then item_kind = item_none
  rem 'Leaf 3 dropped by boss 3
  if hero_items{2} && item_kind = monster_leaf3_boss then item_kind = item_none
  rem 'Armor Dropped by armor boss
  if hero_items{4} && item_kind = monster_armor_boss then item_kind = item_none  
  rem 'Sword dropped by sword boss
  if hero_items{5} && item_kind = monster_sword_boss then item_kind = item_none
  rem 'Strike book dropped by strike book boss
  if hero_items{7} && item_kind = monster_strike_boss then item_kind = item_none
  rem 'Heal book and shield are not dropped by a boss, but just found
item_boss_end
  rem 'Finally handle the case of the shield and the heal book that should drop only once
  if hero_items{3} && item_kind = item_shield then item_kind = item_none
  if hero_items{6} && item_kind = item_bookheal then item_kind = item_none
  
  temp1 = item_kind_mask & item_kind
  item_hp = item_hplist[temp1]

  if temp1 > 31 goto item_draw_32
  on temp1 goto i00 i01 i02 i03 i04 i05 i06 i07 i08 i09 i10 i11 i12 i13 i14 i15 i16 i17 i18 i19 i20 i21 i22 i23 i24 i25 i26 i27 i28 i29 i30 i31
  goto item_draw_done
item_draw_32
  temp1 = temp1 - 32
  on temp1 goto i32 i33 i34 i35 i36 i37 i38 i39 i40 i41 i42 i43 i44 i45 i46 i47 i48 i49 i50 i51 i52 i53 i54 i55 i56 i57 i58 i59 i60 i61 i62 i63
item_draw_done

  if item_kind <> item_none goto item_not_none
  item_x = nowhere
  item_oldx = nowhere
  item_y = nowhere
  item_oldy = nowhere
  return
item_not_none  
  rem temp1 = item_kind & item_kind_mask
  rem 'look up location in table.
  item_oldx = item_xlist[hero_room]
  item_x = item_xlist[hero_room]
  item_oldy = item_ylist[hero_room]  
  item_y = item_ylist[hero_room]
  return




i00
  COLUP1=black
  player1:
  %10000001
  %01000010
  %00100100
  %00011000
  %00011000
  %00100100
  %01000010
  %10000001
end
  goto item_draw_done

i01
  COLUP1=black
  player1:
  %01111100
  %11111110
  %11010110
  %01111100
  %00111000
end
  goto item_draw_done

i02
  COLUP1=red
  player1:
  %00100100
  %01011010
  %10111101
  %01111110
  %01011010
  %01000010
  %10100101
  %10100101
end
  goto item_draw_done


i03
  rem 'bat.xpm
  player1:
  %01000010
  %10000001
  %10011001
  %10111101
  %11111111
  %11100111
  %11000011
  %01000010
end
  goto item_draw_done

i04
  rem 'scorpio.xpm
  player1:
  %00101000
  %00010010
  %00011111
  %00111100
  %01100101
  %10000010
  %10010001
  %01100000
end
  goto item_draw_done

i05
  COLUP1=black
  rem 'rabid.xpm
  player1:
  %00011000
  %00111100
  %01111110
  %11011011
  %11111111
  %01111110
  %11011011
  %11000011
end
  goto item_draw_done

i06
  COLUP1=black
  rem 'spider.xpm
  player1:
  %10100101
  %10011001
  %01011010
  %00111100
  %01100110
  %10111101
  %10100101
  %00100100
end
  goto item_draw_done

i07
  COLUP1=black
  rem 'snake.xpm
  player1:
  %00111110
  %01000011
  %00111000
  %00000100
  %00111000
  %01000000
  %00110000
  %00001000
end  
  goto item_draw_done

i08
  COLUP1=black
  rem 'fish.xpm
  player1:
  %10001100
  %11011110
  %11111101
  %11111110
  %11111111
  %11111011
  %11011110
  %10001100
end  
  goto item_draw_done

i09
  COLUP1=black
  rem 'lion.xpm
  player1:
  %11011011
  %01100110
  %01000010
  %11111111
  %11011011
  %11111111
  %01011010
  %00100100
end  
  goto item_draw_done

i10
  COLUP1=black
  rem 'wolf.xpm
  player1:
  %10100101
  %10100101
  %01111110
  %01111100
  %10000111
  %10001101
  %10001110
  %00000100
end 
  goto item_draw_done

i11
  COLUP1=black
  rem 'captain.xpm, used for monster_grunt. (captain and grunt were doubles)
  player1:
  %01101100
  %01101100
  %01101100
  %01101100
  %00101000
  %10111011
  %01111111
  %01111101
  %01101101
  %01010100
  %11111110
  %00111000
  %01010100
  %10010010
  %01111100
  %00111000
end
  goto item_draw_done

i12
  COLUP1=black
  rem 'archer.xpm
  player1:
  %01101100
  %01101100
  %01101100
  %01101100
  %00101010
  %00110110
  %01101001
  %01010001
  %01111111
  %01010001
  %01101001
  %00110110
  %01111110
  %01010100
  %01111100
  %00111000
end
  goto item_draw_done

i13
  COLUP1=black
  rem 'knight.xpm
  player1:
  %01101100
  %01101100
  %01101100
  %01111100
  %10111010
  %11111110
  %11111110
  %11111110
  %00111000
  %01111100
  %11010110
  %10010010
  %11111110
  %01111100
  %00111000
  %01010100
end
  goto item_draw_done

i14
  COLUP1=black
  rem 'cannon.xpm
  player1:
  %00011100
  %00001000
  %10001000
  %11111001
  %10011111
  %00010001
  %00010000
  %00111000
end  
  goto item_draw_done

i15
  COLUP1=black
  rem 'zombie.xpm
  player1:
  %00101100
  %01101000
  %00101000
  %00101100
  %01101000
  %10111001
  %01111001
  %01011001
  %01111101
  %01101110
  %01111110
  %00111000
  %01011100
  %01110110
  %00111110
  %00011000
end
  goto item_draw_done

i16
  COLUP1=black
  rem 'skeleton.xpm
  player1:
  %01101100
  %00101000
  %00101000
  %00101000
  %10101010
  %10010010
  %10111010
  %01010100
  %00111000
  %00010000
  %00101000
  %01111100
  %01010100
  %10010010
  %01111100
  %00111000
end
  goto item_draw_done

i17
  COLUP1=black
  rem 'ghost.xpm
  player1:
  %10101010
  %11111110
  %11111110
  %11111110
  %01111100
  %01111100
  %01111100
  %11111110
  %11111110
  %10111010
  %00111000
  %01111100
  %11010110
  %10111010
  %01111100
  %00111000
end
  goto item_draw_done

i18
  COLUP1=black
  rem 'bitmap/mage.xpm
  player1:
  %00101001
  %01111101
  %01111101
  %01111101
  %00111001
  %10111011
  %10010011
  %01111101
  %00010001
  %00111001
  %01111101
  %01010101
  %01111101
  %00111001
  %00111010
  %00010001
end
  goto item_draw_done

i19
  COLUP1=black
  rem 'flower.xpm
  player1:
  %00010000
  %00111000
  %01010100
  %00010000
  %00101000
  %01000100
  %00101000
  %00010000
end  
  goto item_draw_done

i20
  COLUP1=black
  rem 'treant.xpm
  player1:
  %10101010
  %01111100
  %00111000
  %00111000
  %00111000
  %00111000
  %00111000
  %00111000
  %01111000
  %10111100
  %00111010
  %01111000
  %10111100
  %00010010
  %00110000
  %01001000
end
  goto item_draw_done

i21
  COLUP1=black
  rem 'muschroom.xpm
  player1:
  %01111110
  %00111100
  %00011000
  %00011000
  %00011000
  %11111111
  %01111110
  %00111100
end  
  goto item_draw_done

i22
  COLUP1=black
  rem 'book.xpm
  player1:
  %11111111
  %10100101
  %10100101
  %10010101
  %11001101
  %10110101
  %10000101
  %11111111
end  
  goto item_draw_done

i23
  COLUP1=black
  rem 'sword.xpm. It's a fake Numen Sword that hurts the one who touches it.
  player1:
  %10010000
  %01100000
  %01110000
  %10111000
  %00011100
  %00001110
  %00000111
  %00000011
end
  goto item_draw_done

i24
  COLUP1=black
  rem 'Curse, entrance blocking item
  player1:
  %00111100
  %01111110
  %11111111
  %11111111
  %11111111
  %11111111
  %11111111
  %11111111
  %11111111  
  %11000111
  %11000111
  %11111111
  %10010011
  %10010011
  %10010011
  %11111111
  %11111111
  %11111111
  %11111111
  %11111111
  %11111111
  %11111111
  %01111110
  %00111100
end
  goto item_draw_done

i25
  COLUP1=black
  rem 'boss1.xpm
  player1:
  %01000100
  %10101010
  %10101010
  %01000100
  %01000100
  %00000000
  %00101000
  %00111000
  %01111100
  %11111110
  %11010110
  %10111010
  %11111110
  %01111101
  %01111100
  %01111100
  %01111100
  %10111010
  %00111000
  %01111000
  %00111000
  %00010100
  %00010000
  %00010000
end
  goto item_draw_done

i26
  COLUP1=black
  rem 'boss2.xpm
  player1:
  %01011010
  %01011010
  %01011010
  %10011001
  %10111101
  %10011001
  %10111101
  %10011001
  %10011001
  %10111101
  %11011011
  %10111101
  %10111101
  %11100111
  %11000011
  %10011001
  %10100101
  %10011001
  %10111101
  %10100101
  %10000001
  %01000010
  %01000010
  %01000010
end  
  goto item_draw_done

i27
  COLUP1=black  
  rem 'boss3.xpm
  player1:
  %10100101
  %10100101
  %10100101
  %10100101
  %10100101
  %10100101
  %10111101
  %01111110
  %11011011
  %10011001
  %10011001
  %10011001
  %10011001
  %11011011
  %11111111
  %01111110
  %01111110
  %01111110
  %00111100
  %00111100
  %00111100
  %00011000
  %00011000
  %00011000
end
  goto item_draw_done

i28
  COLUP1=black
  rem 'armorboss.xpm
  player1:
  %00011000
  %00111100
  %01011010
  %01111110
  %00011000
  %00111100
  %10111101
  %11111111
  %01111110
  %01111110
  %11011011
  %11100111
  %11111111
  %01011010
  %01100110
  %11111111
  %10111101
  %00111100
  %00111100
  %00001000
  %00011000
  %00010000
  %00001000
  %00001000
end
  goto item_draw_done

i29
  COLUP1=black
  rem 'Sword boss is a player frame 
  player1:  
  %01101100
  %01101100
  %01111100
  %01111100
  %10111010
  %01111100
  %10111010
  %11111110
  %11010110
  %11010110
  %11111110
  %01111100
  %00111000  
end
  goto item_draw_done

i30
  COLUP1=black
  rem 'Strike boss is a one point sprite which is difficult to hit
  player1:
  %00010000
end
  goto item_draw_done

i31
  rem 'ikaza.xpm
  rem 'ikaza.xpm
  player1:
  %00101000
  %11111110
  %01111100
  %01111100
  %00111000
  %10111010
  %11010110
  %01111100
  %01010100
  %00111000
  %01111100
  %11010110
  %10111010
  %01111100
  %01111100
  %01010100
end  
  goto item_draw_done

i32
i33 
i34 
  rem 'leaf.xpm
  player1:
  %00000000
  %00000000
  %00111000
  %01111100
  %11111111
  %01111100
  %00111000
  %00000000
end
  goto item_draw_done
i35  
  rem 'armor.xpm
  player1:
  %01111110
  %01011010
  %10100101
  %11011011
  %11111111
  %01111110
  %00011000
  %00000000
end
  goto item_draw_done
i36  
  rem 'shield.xpm
  player1:
  %00010000
  %00111000
  %01010100
  %11010110
  %10111010
  %11010110
  %10010010
  %11111110
end
  goto item_draw_done
i37  
  rem 'sword.xpm
  player1:
  %10010000
  %01100000
  %01110000
  %10111000
  %00011100
  %00001110
  %00000111
  %00000011
end
  goto item_draw_done
i38  
  rem 'healbook.xpm
  player1:
  %11111110
  %10000010
  %10000010
  %10010010
  %10111010
  %10101010
  %10000010
  %11111110
end
  goto item_draw_done
i39
  rem 'strikebook.xpm
  player1:
  %11111110
  %10010010
  %10001010
  %11111110
  %10100010
  %10010010
  %10001010
  %11111110
end
  goto item_draw_done
i40
  rem 'healhp.xpm
  player1:
  %00000000
  %00000000
  %00010000
  %00111000
  %01111100
  %01111100
  %00101000
  %00000000
end
  goto item_draw_done
i41
  rem 'healmp.xpm
  player1:
  %00000000
  %00000000
  %00010000
  %01010100
  %10111010
  %01010100
  %00010000
  %00000000
end
  goto item_draw_done
i42
  rem 'healallhp.xpm
  player1:
  %00000000
  %00010000
  %00111000
  %01111100
  %11111110
  %11111110
  %01101100
  %00000000
end
  goto item_draw_done
i43
  rem 'healallmp.xpm
  player1:
  %00000000
  %00010000
  %01010100
  %10111010
  %10010010
  %10111010
  %01010100
  %00010000
end
  goto item_draw_done
i44 
i45 
i46 
i47 
i48 
i49 
i50
  rem 'bitmap/switch.xpm
  player1:
  %00000000
  %11111111
  %00111110
  %00001000
  %00010000
  %00100000
  %11000000
  %11000000
end
  goto item_draw_done
i51 
i52 
i53 
i54 
i55 
i56 
i57 
  rem 'bitmap/switch_on.xpm
  player1:
  %00000000
  %11111111
  %01111100
  %00010000
  %00001000
  %00000100
  %00000011
  %00000011
end
  goto item_draw_done
i58 
i59 
i60 
i61 
i62 
  COLUP1=black
  player1:
  %10000001
  %01000010
  %00100100
  %00011000
  %00011000
  %00100100
  %01000010
  %10000001
end
  goto item_draw_done

i63
  COLUP1=pink
  player1:
  %00011100
  %11111110
  %01111100
  %10111000
  %10111110
  %10011110
  %10111000
  %11111100
  %11110100
  %11110110
  %11111110
  %01111100
  %00111000
end
  goto item_draw_done
  
  bank 5
  rem ' Bank 5 contains the game over, game win and game intro, screen
  rem 'as well as a music playing routine for them.
  
  #define music_which_intro 0
  #define music_which_gameover 1
  #define music_which_victory 2

  
game_over
  gosub music_restart
  music_which=music_which_gameover
  COLUBK = red
  hero_x=70
  hero_y=80
  player0:
  %1000001
  %1000001
  %1000010
  %1111110
  %1000001
  %1111110
  %0000000
  %1111111
  %1000000
  %1111111
  %1000000
  %1111111
  %0000000
  %0001000
  %0010100
  %0100010
  %1000001
  %1000001
  %0000000
  %0011100
  %0100010
  %1000001
  %1000001
  %0100010
  %0011100
  %0000000
  %0000000
  %0000000
  %0000000
  %0000000
  %0000000
  %0000000
  %0000000
  %1111111
  %1000000
  %1111111
  %1000000
  %1111111  
  %0000000
  %1000001
  %1000001
  %1001001
  %1010101
  %1100011
  %1000001
  %0000000
  %1000001
  %1000001
  %1111111
  %0100010
  %0010100
  %0001000
  %0000000
  %0011110
  %0100010
  %1000111
  %1000000
  %0100010
  %0011100
end  
game_over_loop  
  rem 'Teleport to Sygne's home on reset button, with some losses
  if !switchreset then goto reset_go_end
  hero_room=room_start
  hero_x=hero_start_x
  hero_oldx=hero_start_x
  hero_y=hero_start_y
  hero_oldy=hero_start_x
  hero_hp = hero_level + hero_base_hp
  hero_next = hero_level
  hero_mp = 0
  rem ' Restart the game with stats somewhat reduced, 
  gosub music_restart
  gosub set_mp_display bank1 
  gosub hero_draw_s bank1
  gosub room_draw bank2
  goto main_loop bank1
reset_go_end
  
  COLUP0=white
  COLUP1=black  
  rem 'Make monster look x3 size, to maintain boss size and suggest something for non bosses.  
  NUSIZ1 = $07
special_effects_go_end
  gosub music_play  
  drawscreen
  goto game_over_loop


  data music_data
  24,30,27,30,30,30,27,30,24,28,-1,2,24,28,-1,2,24,60
  27,28,-1,2,27,28,-1,2,27,60,24,30,20,28,-1,2,20,60
  24,30,27,30,30,30,27,30,24,28,-1,2,24,28,-1,2,24,28,-1,2
  24,30,27,28,-1,2,27,30,24,30,27,30,30,120
end

game_win
  gosub music_restart
  music_which=music_which_victory
  COLUBK = white
  COLUPF = yellow
  item_x=80
  item_y=80
  hero_x=84
  hero_y=80
  player1:
  %00011100
  %11111110
  %01111100
  %00111000
  %10111110
  %10011110
  %10111000
  %11111100
  %11110100
  %11110110
  %11111110
  %01111100
  %00111000
  %00000000
  %00010000
  %00111000
  %01111100
  %11111110
  %11111110
  %01101100
  %00000000
  %00000000
  %00000000
  %11111000
  %10000100
  %10000010
  %10000010
  %10000100
  %11111000
  %00000000
  %00000000
  %10000110
  %10001010
  %10010010
  %10100010
  %11000010
  %00000000
  %00000000
  %11111110
  %10000000
  %11111110
  %10000000
  %11111110
  %00000000
  %00000000
  %00000000
  %00000000
  %11111110
  %10000000
  %11111110
  %10000000
  %11111110
  %00000000
  %10000010
  %10000010
  %11111110
  %10000010
  %10000010
  %00000000
  %00010000
  %00010000
  %00010000
  %00010000
  %11111110
  %00000000
  %00000000
  %00000000
  %00010000
  %00111000
  %01111100
  %11111110
  %11111110
  %01101100
end

game_win_loop
  COLUBK = white
  COLUP1 = rand
  COLUP0 = rand
  REFP0 = 8
  gosub music_play
  drawscreen
  rem 'Go back to house, but level up to 90 if not so already
  if !switchreset then goto reset_win_end
  
  hero_room=room_start
  hero_x=hero_start_x
  hero_oldx=hero_start_x
  hero_y=hero_start_y
  hero_oldy=hero_start_x
  if hero_level < 90 then hero_level = hero_level + 3
  hero_hp = hero_level * 2 + hero_base_hp
  hero_mp = hero_level / 16 + 1
  rem 'If the player collected all items, give them a new game plus.
  if hero_items <> 255 goto no_new_game_plus
  if hero_level < 90 then hero_level = 90
  rem 'Take away all items again, and reset the quest flags so the 
  rem 'player can play again.  
  quest_flags = 0
  hero_items  = 0  
no_new_game_plus  
  gosub music_restart
  gosub room_draw bank2
  gosub hero_draw_s bank1
  gosub room_draw bank2
  goto main_loop bank1
reset_win_end

  
  goto game_win_loop


intro_screen
  gosub music_restart
  music_which=music_which_intro
  COLUBK = black
  COLUPF = white
  pfcolors:
  yellow
  yellow - 2
  yellow - 4 
  yellow - 8
  yellow - 10
  yellow - 12
  yellow - 2
  yellow - 4 
  yellow - 8
  yellow - 10
  yellow - 12
end
  playfield:
  ..X....XXXXX..X...X..XXX...XXXX.
  .X.X.....X....X...X..X..X..X....
  X...X....X....X...X..XXX...XXXX.
  XXXXX....X....X...X..X..X..X....
  X...X....X.....XXX...X..X..XXXX.
  ................................
  XXX..XXXX..XX..XXX...XX..X..X... 
  X..X.X....X..X.X..X.X..X.XX.X...
  XXX..XXXX.X..X.XXX..XXXX.X.XX...
  X..X.X....X..X.X..X.X..X.X..X...
  XXX..XXXX..XX..X..X.X..X.X..X...
end
 COLUBK = black
intro_loop  
  rem COLUP1 = rand
  rem COLUP0 = rand
  rem REFP0 = 8
  gosub music_play
  drawscreen
  if switchreset || joy0fire then return  
  goto intro_loop



  #define music_volume  4
  #define note_rest  	0
  #define music_voice 	4
  #define music_voice_2	12
  #define c4 	30
  #define d4 	27  
  #define e4 	24
  #define f4 	23
  #define g4 	20
  #define a4 	18
  #define b4 	16
  #define c5 	15
  #define rn  note_rest
  #define n_1  240
  #define n0   120
  #define n1   60
  #define n2   30
  #define n3   20
  #define n4   15
  #define n6   10
  #define n8    8
  
  
  
  
  #define music_size_victory 48
  
  #define music_size_intro 56
music_notes_intro_p  
  data music_notes_intro
  c5, n2, a4, n2, f4, n2, d4, n2, rn, n8
  c5, n2, a4, n2, f4, n2, d4, n2, rn, n8 
  c4, n2, f4, n2, g4, n2, d4, n2, rn, n8
  c4, n2, f4, n2, g4, n2, d4, n2, rn, n8
  a4, n0, f4, n0, d4, n0, rn, n0
  f4, n0, a4, n0, c5, n0, rn, n0
end

  #define music_size_gameover 20
music_notes_gameover_p
  data music_notes_gameover  
  e4, n0, a4, n0, g4, n0, e4, n0, rn, n0  
  a4, n0, b4, n0, g4, n0, a4, n0, rn, n0
end

  #define music_size_victory 32
music_notes_victory_p
  data music_notes_victory 
  c4, n2, e4, n2, g4, n2, c5, n2 
  g4, n2, e4, n2, c4, n1, rn, n0
  c4, n2, d4, n2, e4, n4, g4, n2
  c5, n2, e4, n2, c4, n1, rn, n0
end


  data music_sizes
  music_size_intro, music_size_gameover, music_size_victory
end  


  rem 'restarts the music  
music_restart
  music_timer=1
  music_pointer=0
  AUDV0=0
  AUDV1=0
  return

music_play
  rem ' Update music timer and change note if needed
  rem ' If we get here, the timer is not 0 yet. 
  rem ' Go on to the next note, and leave it at that   
  if music_timer = 0 then goto music_do_note_change
  goto music_no_note_change
music_do_note_change  
  gosub music_change_note
music_no_note_change
  music_timer = music_timer - 1
  rem ' COLUBK = music_timer
  return
  
music_change_note  
  rem ' choose the not from the right music table
  if music_which = music_which_intro then temp1 = music_notes_intro[music_pointer]
  if music_which = music_which_gameover then temp1 = music_notes_gameover[music_pointer]
  if music_which = music_which_victory then temp1 = music_notes_victory[music_pointer]
  
  AUDF0 = temp1
  AUDF1 = temp1
  AUDC0 = music_voice
  AUDC1 = music_voice_2
  if temp1 = note_rest then AUDV0 = 0 else AUDV0 = music_volume
  if temp1 = note_rest then AUDV1 = 0 else AUDV1 = music_volume
  music_pointer = music_pointer + 1  
  rem ' Get right timig for note from right music
  if music_which = music_which_intro then music_timer = music_notes_intro[music_pointer]
  if music_which = music_which_gameover then music_timer = music_notes_gameover[music_pointer]
  if music_which = music_which_victory then music_timer = music_notes_victory[music_pointer]
  
  
  music_pointer = music_pointer + 1
  temp3 = music_sizes[music_which]
  if music_which = music_which_intro then temp3 = music_size_intro
  if music_which = music_which_gameover then temp3 = music_size_gameover
  if music_which = music_which_victory then temp3 = music_size_victory  
  if music_pointer >= temp3 then music_pointer = 0

  return


  
  bank 6
  
  bank 7
  
  bank 8
  
  inline 6lives_statusbar.asm





