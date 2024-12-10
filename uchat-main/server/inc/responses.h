#ifndef RESPONSES_H
#define RESPONSES_H
#include "server.h"
#include"database.h"
typedef struct s_info {
    GHashTable *users;
    sqlite3* database;
}              t_info;

typedef struct s_client {
    GSocketConnection *conn;
    GDataOutputStream *out;
    GDataInputStream *in;
    GInputStream *in_s;
    gchar *request;
    t_db_user *user;
    t_info *info;
    gboolean upload_file;
    gboolean is_file;
}        t_client; 
void sign_up(cJSON *json_request, t_client *client);
void sign_in(cJSON *json_request, t_client *client);
void new_room_response(cJSON *json_request, t_client *client);
void get_rooms_response(cJSON *json_request, t_client *client);
void get_members_response(cJSON *json_request, t_client *client);
void send_message_response(cJSON *json_request, t_client *client);
void edit_message_response(cJSON *json_request, t_client *client);
void old_messages_response(cJSON *json_request, t_client *client);
void log_out_response(cJSON *json_request, t_client *client);
void search_room_response(cJSON *json_request, t_client *client);
void search_msg_response(cJSON *json_request, t_client *client);
void join_to_room_response(cJSON *json_request, t_client *client);
void new_messages_response(cJSON *json_request, t_client *client);
void new_member_response(cJSON *json_request, t_client *client);
void upload_file_response(cJSON *json_request, t_client *client);
void download_file_response(cJSON *json_request, t_client *client);
void ban_member_response(cJSON *json_request, t_client *client);
void del_room_response(cJSON *json_request, t_client *client);
void del_msg_response(cJSON *json_request, t_client *client);
void edit_user_response(cJSON *json_request, t_client *client);
void get_member_info_response(cJSON *json_request, t_client *client);
void edit_room_name_response(cJSON *json_request, t_client *client);
void edit_room_desc_response(cJSON *json_request, t_client *client);
void clear_room_response(cJSON *json_request, t_client *client);

#endif




