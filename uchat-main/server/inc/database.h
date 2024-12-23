#ifndef DATABASE_H
#define DATABASE_H

#include "server.h"

#define DB_PATH "database.db"

typedef struct s_member{
    guint64 user_id;
    guint64 room_id;
    guint64 date;
    gint8   prm;
}              t_member;

typedef struct s_db_msg {
    guint64 user_id;
    guint64 room_id;
    guint64 msg_id;
    guint64 date;
    gint8 status;
    gint8 type;
    guint64 file_size;
    gchar *msg;
    gchar *file_name;
}              t_db_msg;

typedef struct s_db_user {
    guint64 user_id;
    guint64 date;
    gchar *pass;
    gchar *auth_token;
    gchar *name;
    gchar *login;
    gchar *desc;
}             t_db_user;

typedef struct s_db_room {
    gchar *room_name;
    guint64 room_id;
    guint64 user_id;
    guint64 date;
    gint type;
    gchar *desc;
}              t_db_room;

cJSON *get_json_rooms(sqlite3 *db, guint64 date, guint64 user_id);
cJSON *get_json_members(sqlite3 *db, guint64 room_id);
void add_room_to_db(sqlite3 *db, t_db_room *room);
void add_auth_token(sqlite3 *db, char *username, char *token);
bool user_not_online(sqlite3 *db, char *username);
bool correct_credentials(sqlite3 *db, char *username, char *password);
void create_new_user(sqlite3 *db, t_db_user *user);
bool user_exists(sqlite3 *db, char *username);
sqlite3 *open_db();


t_db_msg *get_db_message(cJSON *msg_j);
t_db_user *get_db_user(cJSON *j_request);
t_db_room *get_db_room(cJSON *j_request);
void close_db(sqlite3 *db);

void free_message(t_db_msg**msg);
void free_room(t_db_room **room);
void free_user(t_db_user **user);

cJSON *get_old_messages_by_id(sqlite3 *db, guint64 room_id,
                              guint64 date, gint64 count);
gchar *create_request_message_by_id(sqlite3 *db, guint64 room_id, gint8 type);
gboolean is_msg_owner(sqlite3 *db, guint64 user_id, guint64 msg_id);
gboolean is_member(sqlite3 *db, guint64 user_id, guint64 room_id);
void edit_message_by_id(sqlite3 *db, guint64 id, gchar *new);
void edit_user_name_by_id(sqlite3 *db, guint64 id, gchar *new_name);
cJSON *search_rooms_in_db(sqlite3 *db, guint64 user_id, gchar *str_search);
cJSON *search_msgs_in_db(sqlite3 *db, guint64 user_id, gchar *str_search);
cJSON *get_object_message(sqlite3_stmt *stmt);
void add_member_to_db(sqlite3 *db, guint64 room_id, guint64 user_id, gint8 permission);
t_db_room *get_room_by_id(sqlite3 *db, guint64 id);
cJSON *get_new_messages_by_id(sqlite3 *db, guint64 room_id, guint64 date, gint64 count);
void add_message_to_db(sqlite3 *db, t_db_msg *message);
t_db_user *get_user_by_token(sqlite3 *db, gchar *token);
t_db_user *get_user_by_login(sqlite3 *db, gchar *login);
gchar *get_message_by_id(sqlite3 *db, guint64 message_id);
t_db_user *get_user_by_id(sqlite3 *db, guint64 user_id);
void edit_member_type(sqlite3 *db, guint64 room_id, guint64 user_id, gint8 new_type);
void delete_room_by_id(sqlite3 *db, guint64 room_id);
void delete_message_by_id(sqlite3 *db, guint64 id);
void edit_user_desc(sqlite3 *db, guint64 user_id, gchar *new_desc);
void edit_user_name(sqlite3 *db, guint64 user_id, gchar *new_desc);
void edit_room_name_by_id(sqlite3 *db, guint64 id, gchar *new_name);
void edit_room_desc_by_id(sqlite3 *db, guint64 id, gchar *new_name);
void delete_message_by_id(sqlite3 *db, guint64 id);
void delete_all_msgs(sqlite3 *db, guint64 room_id);

#endif