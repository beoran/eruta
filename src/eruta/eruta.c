/**
* C only versus Ruby + C: 
*   Only                                      Mixed
**************************************************************************
* Slower to write                     * Faster to write
* More work cloning ruby stdlib       * More reuse of Ruby stdlib.
* Have to rewrite 7k SLOC ruby in C   * Can keep and enhance most Ruby work
* Faster execution                    * Slower execution
* Easier to distribute                * More difficult to distribute
* Can use C libs immediately          * Maintain bindings to C libs
* Simplicity                          * Expressive power 
*
* Both:
* * Will probably write own scripting language anyway, based on Raku.
* * Need C anyway for libraries and bindings.
*
*
* Useful ruby already written : +- 7k SLOC
* Ruby bindings to Gari: +- 1k SLOC + +- 500 SLOC Ruby support code.
* Rchipmunk ruby bindings: +- 1k SLOC +  300 SLOC Ruby support. 
*
* I checked so many languages, but as for now, only Ruby is and C are to my 
* liking. I dislike C++, Lua, etc. I'd love to write my own compiler but that's 
* yet another time trap. Go lang was also a time sinkhole. :p
* Last C try: 640 SLOC C in +- 1 to 2 days 
*
*/




