#ifndef UTILS_H
#define UTILS_H
t_info *init_info(void);
void deinit_info(t_info **info);
void deinit_client(t_client **client);
void free_db_user(t_db_user *user);
#endif


