/**
* This is a test for bxmlparser in $package$
*/
#include "si_test.h"
#include "bxmlparser.h"



int show_xml(Bxml * xml, int depth) {
  int index;
  for (index = 0; index < depth; index++) {
    fprintf(stderr, "  ");
  }
  if (xml->kind == BXML_TAG) { 
    fprintf(stderr, ">%s", xml->name);
  } else if (xml->kind == BXML_TEXT) { 
    fprintf(stderr, ">#text: %s", xml->text);
  } else {
    fprintf(stderr, ">%s: %s", xml->name, xml->text);
  }
   
  if (xml->attributes) {
    BxmlAttribute * nattr = xml->attributes;
    fprintf(stderr, "(");

    while (nattr) {
      fprintf(stderr, " %s->%s", nattr->name, nattr->value);
      nattr = nattr->next;
    }
    fprintf(stderr, " )");
  }
  fprintf(stderr, "\n");
  
  if (xml->child) {
    show_xml(xml->child, depth + 1);
  }
  if (xml->sibling) {
    show_xml(xml->sibling, depth);
  }
  return depth;
}


/*
TEST_FUNC(bxmlparser_1) {
  BxmlParser * parser = bxmlparser_new();
  Bxml * result = bxmlparser_parse_str(parser, input1);
  TEST_NOTNULL(result);
  TEST_NULL(((void*)bxmlparser_get_error(parser)));
  if (bxmlparser_get_error(parser)) {
    fprintf(stderr, "%s : %d %d \n\n" , bxmlparser_get_error(parser), 
    bxmlparser_get_line(parser), bxmlparser_get_column(parser));
  }
  bxmlparser_free(parser);  
  if (result) { 
    show_xml(result, 0);
    bxml_free(result);
  }
  TEST_DONE();
}
*/

#define TEST_XML(NAME, INPUT) \
TEST_FUNC(NAME) { \
  BxmlParser * parser = bxmlparser_new(); \
  Bxml * result = bxmlparser_parse_str(parser, INPUT); \
  TEST_NOTNULL(result); \
  TEST_NULL(((void*)bxmlparser_get_error(parser))); \
  if (bxmlparser_get_error(parser)) { \
    fprintf(stderr, "%s : %d %d \n\n" , bxmlparser_get_error(parser), \
    bxmlparser_get_line(parser), bxmlparser_get_column(parser)); \
  } \
  bxmlparser_free(parser); \
  if (result) { \
    show_xml(result, 0); \
    bxml_free(result); \
  } \
  TEST_DONE(); \
}

#define TEST_XML_FILENAME(NAME, FILENAME) \
TEST_FUNC(NAME) { \
  BxmlParser * parser = bxmlparser_new(); \
  Bxml * result = bxmlparser_parse_filename(parser, FILENAME); \
  TEST_NOTNULL(result); \
  TEST_NULL(((void*)bxmlparser_get_error(parser))); \
  if (bxmlparser_get_error(parser)) { \
    fprintf(stderr, "%s : %d %d \n\n" , bxmlparser_get_error(parser), \
    bxmlparser_get_line(parser), bxmlparser_get_column(parser)); \
  } \
  bxmlparser_free(parser); \
  if (result) { \
    show_xml(result, 0); \
    bxml_free(result); \
  } \
  TEST_DONE(); \
}


char * input_1 = "<?xml?><tag></tag>";

TEST_XML(bxmlparser_1, input_1)

char * input_2 = "<?xml version=\"1.0\"?><tag attr1=\"&amp;&#97;&amp;&#x61;&amp;val1\" attr2=\"val2\"></tag>";

TEST_XML(bxmlparser_2, input_2)

char * input_3 = "<?xml version=\"1.0\"?><tag>hello</tag>";

TEST_XML(bxmlparser_3, input_3)

char * input_4 = "<?xml version=\"1.0\"?><tag>hello &amp; world</tag>";

TEST_XML(bxmlparser_4, input_4)

char * input_5 = "<?xml version=\"1.0\"?><tag>hello <!--comment--> world</tag>";

TEST_XML(bxmlparser_5, input_5)

char * input_6 = "<?xml version=\"1.0\"?><script><![CDATA[ puts 'foo!' if (a > b) and (c < f) ]]></script>";

TEST_XML(bxmlparser_6, input_6)

char * input_7 = "<?xml version=\"1.0\"?><a><b/><c><c1>ok</c1></c><d>ok2</d></a>";

TEST_XML(bxmlparser_7, input_7)

TEST_XML_FILENAME(bxmlparser_fn_1, "data/map/map_0001.tmx");


int main(void) {
  TEST_INIT();
  TEST_RUN(bxmlparser_1);
  TEST_RUN(bxmlparser_2);
  TEST_RUN(bxmlparser_3);
  TEST_RUN(bxmlparser_4);
  TEST_RUN(bxmlparser_5);
  TEST_RUN(bxmlparser_6);
  TEST_RUN(bxmlparser_7);
  TEST_RUN(bxmlparser_fn_1);


  TEST_REPORT();
}



