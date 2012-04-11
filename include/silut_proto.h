#ifndef SILUT_PROTO_H
#define SILUT_PROTO_H
/*
This file was autogenerated from src/silut.c
by bin/genproto
Please do not hand edit.
*/

/** Lenght of an array of silut. NULL is the guard clause. */
int silut_len(Silut lut[]);

/** Looks up a string with an integer key integer in the lookup table by
* simple linear search. The look up table needs not be sorted in any way,
* but it's somewhat slower.
* The look up table should have NULL as it's final element.
* If not found, NULL is returned. If found the matching record is returned.
*/
Silut * silut_lsearchi(Silut lut[], int integer);

/** Looks up an integer with an integer key integer in the lookup table by
* simple linear search. The look up table needs not be sorted in any way,
* but it's somewhat slower.
* The look up table should have NULL as it's final element.
* strcmp is used to compare the strings.
* If not found, NULL is returned. If found the matching record is returned.
*/
Silut * silut_lsearchcstr(Silut lut[], const char * string);

#endif // SILUT_PROTO_H

