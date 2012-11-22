/* This file was generated with:
'cfunctions -aoff -n -m -w tile_proto src/tile.c' */
#ifndef CFH_TILE_PROTO
#define CFH_TILE_PROTO

/* From 'src/tile.c': */
/** Cleans up a tileset, and empties it.  */

#line 69 "src/tile.c"
void tileset_done (Tileset * set );
/** Deallocates a tileset */

#line 79 "src/tile.c"
void tileset_free (Tileset * set );
/** Retuns the amount of tiles in the tile set. */

#line 85 "src/tile.c"
int tileset_size (Tileset * set );
/** Initializes a given tileset with a given bitmap tile sheet */

#line 90 "src/tile.c"
Tileset * tileset_init (Tileset * set , Image * sheet );
/** Creates a new tileset with the given tile sheet image. */

#line 123 "src/tile.c"
Tileset * tileset_new (Image * sheet );
/** Recalculates the tile's position (now) in it's tile set. */

#line 144 "src/tile.c"
Tile * tile_recalculate (Tile * tile );
/** Initializes a tile to belong to a given tile set. */

#line 155 "src/tile.c"
Tile * tile_init (Tile * tile , Tileset * set , int index );
/** Gets a tile from a tile set by it's tile id. */

#line 171 "src/tile.c"
Tile * tileset_get (Tileset * set , int index );
/** Sets the animation parameter of this tile */

#line 178 "src/tile.c"
Tile * tile_anim_ (Tile * tile , int anim );
/** Gets the animation parameter of this tile, or 0 if NULL */

#line 185 "src/tile.c"
int tile_anim (Tile * tile );
/** Sets the wait parameter of this tile in ms */

#line 191 "src/tile.c"
Tile * tile_wait_ (Tile * tile , int wait );
/** Gets the wait parameter of this tile in ms, or -1 if NULL */

#line 198 "src/tile.c"
int tile_wait (Tile * tile );
/** Gets the value of the flags of a tile. */

#line 219 "src/tile.c"
int tile_flags (Tile * tile );
/** Sets the flags on a tile. */

#line 225 "src/tile.c"
Tile * tile_flags_ (Tile * tile , int flags );
/** Sets a single flag on a tile. */

#line 232 "src/tile.c"
Tile * tile_setflag (Tile * tile , int flag );
/** Removes a single flag on a tile. */

#line 239 "src/tile.c"
Tile * tile_unflag (Tile * tile , int flag );
/** Checks a single flag on a tile. */

#line 246 "src/tile.c"
int tile_isflag (Tile * tile , int flag );
/** Sets a tile's flags from a property string.
* This uses an internal lookup table.
*/

#line 254 "src/tile.c"
Tile * tile_property_ (Tile * tile , char * property );
/** Rewinds a tile's animations. */

#line 262 "src/tile.c"
void tile_rewindanime (Tile * tile );
/** Updates a tile to animate it. Ignores dt for now. */

#line 270 "src/tile.c"
void tile_update (Tile * tile , double dt );
/** Updates all tiles in a tile set so they all get animated. */

#line 294 "src/tile.c"
void tileset_update (Tileset * set , double dt );
/** Draw a tile to the current active drawing target at the
given coordinates */

#line 307 "src/tile.c"
void tile_draw (Tile * tile , int x , int y );
/** Tile's index. Returns -1 if tile is NULL; */

#line 322 "src/tile.c"
int tile_index (Tile * tile );
/**  Information about the tile's properties. Return -1 if tile is NULL. */

#line 328 "src/tile.c"
int tile_kind (Tile * tile );

#endif /* CFH_TILE_PROTO */
