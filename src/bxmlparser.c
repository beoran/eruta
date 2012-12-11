
#line 1 "bxmlparser.rl"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct BXMLNode_ BXMLNode;

enum {
  BXML_NODE_NORMAL  = 1,
  BXML_NODE_TEXT    = 2,
  BXML_NODE_ATTR    = 3,
};

struct BXMLNode_ {
  int kind;
  char * tagname;
  BXMLNode * parent, *child, *sibling, *attribute;
  char * value;
}

BXMLNode * bxmlnode_alloc() {
  return calloc(sizeof(BXMLNode));
}



/** Struct to hold the state of the ragel parser. */
struct BXMLRagel_ {
  BXNLNode * root;
  BXMLNode * now;
  char * buffer;
  int bufp; 
};


/*
a Ragel Parser for XML.


attribute = ^(space | ‘/’ | ‘>’ | ‘=’)+ >buffer %attributeName space* ‘=’ space*
((‘\” ^’\”* >buffer %attribute ‘\”) | (‘”‘ ^’”‘* >buffer %attribute ‘”‘));
element = ‘<' space* ^(space | '/' | '>‘)+ >buffer %elementStart (space+ attribute)*
:>> (space* (‘/’ %elementEndSingle)? space* ‘>’ @element);
elementBody := space* <: ((^'<'+ >buffer %text) <: space*)?
element? :>> (‘<' space* '/' ^'>‘+ ‘>’ @elementEnd);

comment  :=
header   := open '?'
main     := space* element space*;
*/


#line 77 "bxmlparser.rl"





#line 61 "bxmlparser.c"
static const char _bxmlragel_actions[] = {
	0, 1, 0, 1, 1
};

static const char _bxmlragel_key_offsets[] = {
	0, 0, 1, 7, 13, 14
};

static const char _bxmlragel_trans_keys[] = {
	60, 32, 47, 9, 13, 61, 62, 32, 
	47, 9, 13, 61, 62, 62, 0
};

static const char _bxmlragel_single_lengths[] = {
	0, 1, 2, 2, 1, 0
};

static const char _bxmlragel_range_lengths[] = {
	0, 0, 2, 2, 0, 0
};

static const char _bxmlragel_index_offsets[] = {
	0, 0, 2, 7, 12, 14
};

static const char _bxmlragel_indicies[] = {
	0, 1, 1, 1, 1, 1, 2, 1, 
	3, 1, 1, 2, 4, 1, 1, 0
};

static const char _bxmlragel_trans_targs[] = {
	2, 0, 3, 4, 5
};

static const char _bxmlragel_trans_actions[] = {
	0, 0, 1, 0, 3
};

static const int bxmlragel_start = 1;
static const int bxmlragel_first_final = 5;
static const int bxmlragel_error = 0;

static const int bxmlragel_en_main = 1;


#line 82 "bxmlparser.rl"

int bxmlragel_parse(struct BXMLRagel_ * parser, char * xmltext, int size) {
  char * buffer;
  int bufp = 0; 
  int bufs = 1024;  
  parser->buffer = calloc(bufs, 1);
  int cs, res = -1;
  {
    char * p   = xmltext;
    char * pe  = p + size + 1;
    char * eof = pe;
    /** use %access parser-> later .*/
    
#line 121 "bxmlparser.c"
	{
	cs = bxmlragel_start;
	}

#line 95 "bxmlparser.rl"
    
#line 128 "bxmlparser.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _bxmlragel_trans_keys + _bxmlragel_key_offsets[cs];
	_trans = _bxmlragel_index_offsets[cs];

	_klen = _bxmlragel_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _bxmlragel_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _bxmlragel_indicies[_trans];
	cs = _bxmlragel_trans_targs[_trans];

	if ( _bxmlragel_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _bxmlragel_actions + _bxmlragel_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 53 "bxmlparser.rl"
	{
    char * newbuf;
    parser->buffer[bufp] = (*p);    
    bufp++;
    if (bufp >= bufs) {
      int newbufs = bufs*2 + 2;
      newbuf = calloc(newbufs, 1);
      if(!newbuf) abort();
      memmove(newbuf, buffer, bufs);
      parser->buffer = newbuf;
      bufs   = newbufs;
    }
  }
	break;
	case 1:
#line 76 "bxmlparser.rl"
	{ res = 1; }
	break;
#line 222 "bxmlparser.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 96 "bxmlparser.rl"
  }
  printf("Result: %d, %s\n", res, parser->buffer);
  free(buffer);
  return 0;  
}

int main(int argc, char * argv) {
  char * str = "<hello/>";
  struct BXMLRagel_ parser;
  parser.root = NULL;
  parser.now = null;
  return bxmlragel_parse(&parser, str, strlen(str));
}




