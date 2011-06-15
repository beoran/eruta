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
*/




