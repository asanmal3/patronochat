#ifndef API_H
#define API_H
#include"server.h"
void get_user_id(sqlite3 *db, t_db_user *user);
gchar *create_user_token(char *login);
gboolean is_valid_room_name(gchar *roomname);
gboolean is_valid_info(gchar *msg, gsize max_value);
gint8 get_member_type(sqlite3 *db, guint64 user_id, guint64 room_id);
GList *get_members_list(sqlite3 *db, guint64 room_id);
gboolean read_file_req(t_client *client, gsize size, char *name);
void download_file_req(gchar *msg, t_client *client);
#endif





