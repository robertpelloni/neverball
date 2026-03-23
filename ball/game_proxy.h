#ifndef GAME_PROXY_H
#define GAME_PROXY_H

#include "cmd.h"

<<<<<<< HEAD
void       game_proxy_filter(int (*fn)(const union cmd *));
=======
>>>>>>> origin/csy-extras
void       game_proxy_enq(const union cmd *);
union cmd *game_proxy_deq(void);
void       game_proxy_clr(void);

#endif
