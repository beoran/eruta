require 'test_helper'
require 'bump'


def vec_is(resx, resy, res)
  return res.x == resx && res.y == resy 
end

assert { Bump }
assert { Bump::Vector }
v1, v2, v3 = nil, nil, nil 
assert { v1 = Bump::Vector.new(2, 3)      }
assert { v2 = v1.vec(3, 2)                }
assert { vec_is    5,   5,  v1 + v2       }
assert { vec_is   -1,   1,  v1 - v2       }
assert { vec_is   -6,  -9,  v1 * -3.0     }
assert { vec_is    1, 1.5,  v1 / 2.0      }
assert { vec_is    1, 1.5,  v1 / 2.0      }
assert { vec_is    1, 1.5,  v1 / 2.0      }
assert { v1.dot(v2) == 12.0               }
assert { v1.cross(v2) == -5.0             }
assert { v3 = v2.vec(3, 4)                }
assert { v3.length == 5.0                 }
assert { v3.lensq  == 25.0                }
assert { Bump::Vector.zero                }
assert { Bump::Vector.zero.zero?          }
# assert { Bump::Vector.zero.angle == 0.0 }
assert { Bump::Vector.zero.perp           }
v4, v5, v6  = nil, nil, nil
ang         = Math::PI / 4.0
assert { v4 = Bump::Vector.for_angle(ang) }
p v1.angle, ang 
# Something wrong with the angles!!!
assert { v1.angle ==  ang                 }















