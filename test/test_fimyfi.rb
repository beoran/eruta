# Rests for the Fimyfi 'find my file' library
require 'test_helper.rb'
require 'fimyfi'
 
assert { Fimyfi }
assert { Fimyfi.application == 'fimyfi' }
assert { Fimyfi.application =  $0       }
assert { Fimyfi.application == $0       }
assert { Fimyfi.preference_dir }
assert { Fimyfi.preference_file('sav1')           }
assert { Fimyfi.split('foo/bar') == [:foo, :bar]  }
assert { Fimyfi.join(:foo, :bar) == File.expand_path('foo/bar')   }
assert { Fimyfi.home                              }
assert { Fimyfi.main($0)                          }
assert { Fimyfi.maindir                           }
assert { Fimyfi.application =  'eruta'            }
assert { Fimyfi.application == 'eruta'            }
assert { Fimyfi.find_settings_file                }
assert { Fimyfi.dir                               }
assert { Fimyfi.font_dir                          }
assert { Fimyfi.image_dir                         }
assert { Fimyfi.ui_dir                            }
assert { Fimyfi.ui_background_dir                 }
assert { Fimyfi.icon_dir                          }
assert { Fimyfi.cursor_dir                        }
assert { Fimyfi.map_dir                           }
 
 