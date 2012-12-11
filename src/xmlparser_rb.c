
#line 1 "xmlparser_rb.rl"
# encoding: UTF-8
# Struct to hold the state of the ragel parser.
class BXMLRagel
  attr_accessor :buffer
  attr_accessor :root
  attr_accessor :now
end

class BXML
  NODE    = 1
  TEXT    = 2
  ATTR    = 3
  
  attr_accessor :kind
  attr_accessor :name
  
  attr_accessor :parent
  attr_accessor :child
  attr_accessor :attr
  attr_accessor :next
  
  def initialize(kind, parent)
    @kind   = kind
    @parent = parent
    @child  = @next = @attr = nil;
  end
  
  def walk(&block)
    block.call(self)
    if @attr
      @attr.walk(&block)
    end
    if @next
      @next.walk(&block)
    end
    if @child
      @child.walk(&block)
    end
  end
  
end







#line 73 "xmlparser_rb.rl"




#line 57 "xmlparser_rb.c"
static const char _xmlparser_actions[] = {
	0, 1, 0, 1, 2, 4, 4, 1, 
	3, 5
};

static const char _xmlparser_key_offsets[] = {
	0, 0, 1, 7, 13, 14
};

static const char _xmlparser_trans_keys[] = {
	60, 32, 47, 9, 13, 61, 62, 32, 
	47, 9, 13, 61, 62, 62, 60, 0
};

static const char _xmlparser_single_lengths[] = {
	0, 1, 2, 2, 1, 1
};

static const char _xmlparser_range_lengths[] = {
	0, 0, 2, 2, 0, 0
};

static const char _xmlparser_index_offsets[] = {
	0, 0, 2, 7, 12, 14
};

static const char _xmlparser_indicies[] = {
	0, 1, 1, 1, 1, 1, 2, 1, 
	3, 1, 1, 2, 4, 1, 0, 1, 
	0
};

static const char _xmlparser_trans_targs[] = {
	2, 0, 3, 4, 5
};

static const char _xmlparser_trans_actions[] = {
	3, 0, 1, 0, 5
};

static const int xmlparser_start = 1;
static const int xmlparser_first_final = 5;
static const int xmlparser_error = 0;

static const int xmlparser_en_main = 1;


#line 77 "xmlparser_rb.rl"

def ragel_parse(text)
  parser        = BXMLRagel.new
  parser.buffer = ""
  parser.root   = BXML.new(BXML::NODE, nil)
  parser.now    = parser.root
  cs      = -1 
  res     = -1
  data    = text.each_codepoint.to_a;
  p       = 0;
  pe      = text.size;
  
#line 118 "xmlparser_rb.c"
	{
	cs = xmlparser_start;
	}

#line 89 "xmlparser_rb.rl"
  
#line 125 "xmlparser_rb.c"
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
	_keys = _xmlparser_trans_keys + _xmlparser_key_offsets[cs];
	_trans = _xmlparser_index_offsets[cs];

	_klen = _xmlparser_single_lengths[cs];
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

	_klen = _xmlparser_range_lengths[cs];
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
	_trans = _xmlparser_indicies[_trans];
	cs = _xmlparser_trans_targs[_trans];

	if ( _xmlparser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _xmlparser_actions + _xmlparser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 50 "xmlparser_rb.rl"
	{ parser.buffer << (*p).chr; p "buf: #{parser.buffer}" }
	break;
	case 1:
#line 51 "xmlparser_rb.rl"
	{ parser.buffer = "" ; p "buf: #{parser.buffer}" }
	break;
	case 2:
#line 52 "xmlparser_rb.rl"
	{ 
    tag              = BXML.new(BXML::NODE, parser.now)
    parser.now.child = tag
    parser.now       = tag
  }
	break;
	case 3:
#line 58 "xmlparser_rb.rl"
	{
    parser.now= tag.parent
  }
	break;
	case 4:
#line 62 "xmlparser_rb.rl"
	{
      parser.now.name = parser.buffer;
  }
	break;
	case 5:
#line 72 "xmlparser_rb.rl"
	{ result = 1; }
	break;
#line 231 "xmlparser_rb.c"
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

#line 90 "xmlparser_rb.rl"
  
  printf("Result: %d\n%s%s\n", res, parser.buffer, parser.root.to_s);
  parser.root.walk do |tag|
    p tag, tag.kind, tag.name
  end
  return 0;  
end

ragel_parse("<hello/><world/>")




