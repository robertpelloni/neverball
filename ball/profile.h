#ifndef PROFILE_H
#define PROFILE_H

void profile_init(void);
void profile_save(void);

int  profile_get_currency(void);
void profile_add_currency(int amount);

#endif
