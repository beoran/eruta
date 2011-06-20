

struct NofontGlyphinfo_ {
  char glyph[12];  
  char pixels[100][7]; 
};

struct NofontGlyphinfo_ nofont_glyphs[20] = {  
{
  " !#$%&'()*",
  {
  "..... ..x.. ..... ..x.. .x..x .xxx. ..x.. ..x.. ..x.. ....."
  "..... ..x.. .x.x. .xxxx x.x.x x.... ..x.. .x... ...x. ..x.."
  "..... ..x.. xxxxx x.x.. .x.x. .x... ..... x.... ....x x.x.x"
  "..... ..x.. .x.x. .xxx. ..x.. x.x.x ..... x.... ....x .xxx."
  "..... ..... xxxxx ..x.x .x.x. x..x. ..... x.... ....x x.x.x"
  "..... ..x.. .x.x. xxxx. x.x.x .xx.x ..... .x... ...x. ..x.."
  "..... ..x.. ..... ..x.. x..x. ..... ..... ..x.. ..x.. ....."
  } 
}, 

{ 
  "+-./01234",
  { 
  "..... ..... ..... ..... ....x ..... ..... ..... ..... ....."
  "..x.. ..... ..... ..... ....x .xxx. ..x.. .xxx. xxxx. ...x."
  "..x.. ..... ..... ..... ....x x...x .xx.. x...x ....x ..xx."
  "xxxxx ..... xxxxx ..... ...x. x.x.x x.x.. ...x. xxxx. .x.x."
  "..x.. ..... ..... ..... ..x.. x...x ..x.. ..x.. ....x xxxxx"
  "..x.. ..... ..... ..... .x... .xxx. xxxxx xxxxx xxxx. ...x."
  "..... ...x. ..... ..x.. x.... ..... ..... ..... ..... ....."
  "..... ..x.. ..... ..x.. x.... ..... ..... ..... ..... ....."
  }
}, 

{
  { 0, 0, 0, 0, 0 , 0 , 0, 0, 0 },
  { "", "", "", "", "", "", ""  } 
}

} ;

/*
  
        glyphs %w{5 6 7 8 9 : ; < = >},
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        xxxxx .xxxx xxxxx .xxx. .xxx. ..x.. ...x. ...x. ..... .x...
        x.... x.... ...x. x...x x...x ..x.. ...x. ..x.. xxxxx ..x..
        xxxx. xxxx. xxxxx .xxx. .xxxx ..... ..... .x... ..... ...x.
        ....x x...x .x... x...x ....x ..... ..... x.... ..... ....x
        xxxx. .xxx. .x... .xxx. xxxx. ..... ..... .x... xxxxx ...x.
        ..... ..... ..... ..... ..... ..x.. ...x. ..x.. ..... ..x..
        ..... ..... ..... ..... ..... ..x.. ..x.. ...x. ..... .x...
        "
        
        glyphs %w{? @}, 
         "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        .xxx. .xxx. ..... ..... ..... ..... ..... ..... ..... .....
        x...x x..xx ..... ..... ..... ..... ..... ..... ..... .....
        ...x. x.x.x ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. x.x.x ..... ..... ..... ..... ..... ..... ..... .....
        ..... x..xx ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. x.... ..... ..... ..... ..... ..... ..... ..... .....
        ..... .xxxx ..... ..... ..... ..... ..... ..... ..... .....
        "


       glyphs %w{A B C D E F G H I J}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. xxxx. .xxx. xxxx. xxxxx xxxxx .xxx. x...x xxxxx xxxxx
        .x,x. x...x x...x x...x x.... x.... x.... x...x ..x.. ....x
        x,..x xxxx. x.... x...x xxxx. xxx.. x..xx xxxxx ..x.. ....x
        xxxxx x...x x...x x...x x.... x.... x...x x...x ..x.. x...x
        x,..x xxxx. .xxx. xxxx. xxxxx x.... .xxx. x...x xxxxx .xxx.
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        "
       glyphs %w{K L M N O P Q R S T}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        x...x x.... x...x x...x .xxx. xxxx. .xxx. xxxx. .xxxx xxxxx
        x..x. x.... xx.xx xx..x x...x x...x x...x x...x x.... ..x..
        xxx.. x.... x.x.x x.x.x x...x xxxx. x.,.x xxxx. .xxx. ..x..
        x..x. x.... x...x x..xx x...x x.... x..x, x...x ....x ..x..
        x...x xxxxx x...x x...x .xxx. x.... .xx,x x...x xxxx. ..x..
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        "
       glyphs %w{U V W X Y Z [ \\ ] ^}, 
        "
        ..... ..... ..... ..... ..... ..... xxxx. x.... .xxxx ..x..
        x...x x...x x...x x...x x...x xxxxx x.... x.... ....x .x.x.
        x...x x...x x...x .x.x. .x.x. ...x. x.... .x... ....x x...x
        x...x x...x x...x ..x.. ..x.. ..x.. x.... ..x.. ....x .....
        x...x .x.x. x.x.x .x.x. ..x.. .x... x.... ...x. ....x .....
        .xxx. ..x.. .x.x. x...x ..x.. xxxxx x.... ....x ....x .....
        ..... ..... ..... ..... ..... ..... x.... ....x ....x .....
        ..... ..... ..... ..... ..... ..... xxxx. ....x .xxxx .....
        "
        
        glyphs %w{a b c d e f g h i j}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... x.... ..... ....x ..... ...xx ..... x.... ..x.. ...x.
        .xxxx xxxx. .xxxx .xxxx .xxx. ..x.. .xxxx xxxx. ..... .....
        x...x x...x x.... x...x x..xx ..x.. x...x x...x ..x.. ...x.
        x...x x...x x.... x...x x,x.. xxxxx x...x x...x ..x.. ...x.
        .xxxx xxxx. .xxxx .xxxx .xxxx ..x.. .xxxx x...x ..x.. ...x.
        ..... ..... ..... ..... ..... ..x.. ....x ..... ..... x..x.
        ..... ..... ..... ..... ..... ..x.. xxxx. ..... ..... .xx..
        "
        glyphs %w{k l m n o p q r s t}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        x.... ..x.. ..... ..... ..... ..... ..... ..... ..... .x...
        x..x. ..x.. xx.x. xxxx. .xxx. xxxx. .xxxx xxxx. .xxx. xxxxx
        xxx.. ..x.. x.x.x x...x x...x x...x x...x x...x xx... .x...
        x..x. ..x.. x.x.x x...x x...x x...x x...x x.... ..xxx .x..x
        x...x ..x.. x.x.x x...x .xxx. xxxx. .xxxx x.... xxxx. ..xx.
        ..... ...x. ..... ..... ..... x.... ....x ..... ..... .....
        ..... ..... ..... ..... ..... x.... ....x ..... ..... .....
        "
        
        glyphs %w{u v w x y z { | } ~}, 
        "
        ..... ..... ..... ..... ..... ..... ...xx ..x.. xx... .....
        ..... ..... ..... ..... ..... ..... ..x.. ..x.. ..x.. .....
        x...x x...x x...x x..x. x...x xxxxx ..x.. ..x.. ..x.. .....
        x...x x...x x.x.x .xx.. x...x ...x. xx... ..x.. ...xx .x.x.
        x...x .x.x. x.x.x .xx.. x...x ..x.. xx... ..x.. ...xx x.x..
        .xxxx ..x.. .x.x. x..x. .xxxx xxxxx ..x.. ..x.. ..x.. .....
        ..... ..... ..... ..... ....x ..... ..x.. ..x.. ..x.. .....
        ..... ..... ..... ..... xxxx. ..... ...xx ..x.. xx... .....
        "
        
        # Some utf8 encoded accented letters
        glyphs %w{ä ë ï ö ü ÿ Ä Ë Ï Ö Ü Ÿ}, 
        "
        .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x.
        ..... ..... ..... ..... ..... ..... ..x.. xxxxx xxxxx .xxx. x...x x...x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
        
        glyphs %w{á é í ó ú ý Á É Í Ó Ú Ý}, 
        "
        ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x.
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. xxxxx xxxxx .xxx. x.x.x x.x.x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
        
        glyphs %w{à è ì ò ù ỳ À È Ò Ù Ỳ}, 
        "
        .x... .x... .x... .x... .x... .x... .x... .x... .x... .x... .x... .x...
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. xxxxx xxxxx .xxx. x.x.x x.x.x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "

        glyphs %w{â ê î ô û ŷ Â Ê Î Ô Û Ŷ}, 
        "
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x..
        .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. xxxxx xxxxx .xxx. xx.xx xx.xx
        .xxxx .xxx. ..... .xxx. x...x x...x .xxx. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
        
        # Some private use characters useful for games, etc.
        glyphs ["\uEF00", "\uEF01", "\uEF02", "\uEF03", "\uEF04", "\uEF05",
                "\uEF06", "\uEF07", "\uEF08", "\uEF09",],
        "
        x.... xxxxx .xx.. ..x.. ..x.. ...x. ..x.. ..x.. .xxx. ..x..
        .x... xxxxx .x.x. ..x.. x.x.x ..x.. .xxx. x.x.x xxxxx ..x..
        ..x.x ..x.. .x..x .xxx. .x.x. .x... xxxxx .xxx. x.x.x .xxx.
        ...x. ..x.. xxxxx .xxx. .x.x. xxxxx xxxxx xx.xx xx.xx x.x.x
        ..x.x ..x.. .x..x xxxxx x.x.x ...x. .xxx. .xxx. .xxx. .xxx.
        ..... ..x.. .x.x. xxxxx .xxx. ..x.. ..x.. x.x.x .x.x. ..x..
        ..... ..x.. .xx.. .xxx. ..x.. .x... .xxx. ..x.. ..... ..x..
        "
        
        
*/

