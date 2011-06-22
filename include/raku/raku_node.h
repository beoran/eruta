#ifndef RAKU_NODE_H
#define RAKU_NODE_H 

#include "si_wire.h"
#include "raku_token.h"

// Single character nodes are encoded as their character code. 
enum RakuNodeKind_ {
    RakuNodeFail        = 256,
};

typedef enum RakuNodeKind_ RakuNodeKind; 

struct RakuNode_;
typedef struct RakuNode_ RakuNode;




#endif
