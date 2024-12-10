#include "server.h"
#include"responses.h"
void clear_room_response(cJSON *j_request, t_client *client) 
{
    guint64 room_id = vm_get_object(j_request, "room_id")->valueint;
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    if (get_member_type(client->info->database, client->user->user_id, room_id) != DB_ORDINARY ||
        is_member(client->info->database, client->user->user_id, room_id) == false) 
        {
        return;
    }

    delete_all_msgs(client->info->database, room_id);
    cJSON_AddNumberToObject(j_request, "token", RQ_DEL_HIST);
    cJSON_AddNumberToObject(j_request, "room_id", room_id);
    json_data = vm_message_calibration(j_request);
    vm_send_to_all(json_data, client, room_id);

    g_free(json_data);
    cJSON_Delete(json_response);
}

void edit_room_desc_response(cJSON *j_request, t_client *client) 
{
    guint64 room_id = vm_get_object(j_request, "room_id")->valueint;
    gchar *room_desc = vm_get_valuestring(j_request, "desc");
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;
    
    if (get_member_type(client->info->database, client->user->user_id, room_id)
        != DB_ORDINARY || is_valid_info(room_desc, MAX_DESC) == false) 
    {
        return;
    }

    edit_room_desc_by_id(client->info->database, room_id, room_desc);
    cJSON_AddNumberToObject(json_response, "token", RQ_UPD_ROOM_DESC);
    cJSON_AddNumberToObject(json_response, "room_id", room_id);
    cJSON_AddStringToObject(json_response, "desc", room_desc);
    json_data = vm_message_calibration(json_response);
    vm_send_to_all(json_data, client, room_id);

    g_free(json_data);
    g_free(room_desc);
    cJSON_Delete(json_response);
}

void edit_room_name_response(cJSON *j_request, t_client *client) 
{
    guint64 room_id = vm_get_object(j_request, "room_id")->valueint;
    gchar *room_name = vm_get_valuestring(j_request, "room_name");
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    if (get_member_type(client->info->database, client->user->user_id, room_id)
        != DB_ORDINARY || is_valid_room_name(room_name) == false) 
    {
        return;
    }

    edit_room_name_by_id(client->info->database, room_id, room_name);
    cJSON_AddNumberToObject(json_response, "token", RQ_UPD_ROOM_NAME);
    cJSON_AddNumberToObject(json_response, "room_id", room_id);
    cJSON_AddStringToObject(json_response, "room_name", room_name);
    json_data = vm_message_calibration(json_response);
    vm_send_to_all(json_data, client, room_id);

    g_free(json_data);
    g_free(room_name);
    cJSON_Delete(json_response);
}

void get_member_info_response(cJSON *json_request, t_client *client) 
{
    guint64 user_id = vm_get_object(json_request, "user_id")->valueint;
    cJSON *json_response = cJSON_CreateObject();
    t_db_user *user = NULL;
    gchar *json_data = NULL;

    user = get_user_by_id(client->info->database, user_id);
    cJSON_AddNumberToObject(json_response, "token", RQ_MEMBER_INFO);
    cJSON_AddStringToObject(json_response, "desc", user->desc);
    cJSON_AddStringToObject(json_response, "name", user->name);
    cJSON_AddStringToObject(json_response, "login", user->login);
    json_data = vm_message_calibration(json_response);
    vm_send(client->out, json_data);

    g_free(json_data);
    cJSON_Delete(json_response);
}

void edit_user_response(cJSON *json_request, t_client *client) 
{
    gchar *new_desc = vm_get_valuestring(json_request, "desc");
    gchar *new_name = vm_get_valuestring(json_request, "name");
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;
    
    if (is_valid_info(new_desc, MAX_DESC) == false || is_valid_info(new_name, MAX_NAME) == false) 
    {
        return;
    }

    edit_user_name(client->info->database, client->user->user_id, new_name);
    edit_user_desc(client->info->database, client->user->user_id, new_desc);
    cJSON_AddNumberToObject(json_response, "token", RQ_UPD_USER);
    cJSON_AddStringToObject(json_response, "desc", new_desc);
    cJSON_AddStringToObject(json_response, "name", new_name);
    json_data = vm_message_calibration(json_response);
    vm_send(client->out, json_data);

    g_free(json_data);
    g_free(new_desc);
    cJSON_Delete(json_response);
}

void del_msg_response(cJSON *json_request, t_client *client) 
{
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    guint64 msg_id = vm_get_object(json_request, "msg_id")->valueint;
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    if (is_member(client->info->database, client->user->user_id, room_id) &&
        get_member_type(client->info->database, client->user->user_id, room_id) == DB_ORDINARY &&
        is_msg_owner(client->info->database, client->user->user_id, msg_id)) 
    {
        
        delete_message_by_id(client->info->database, msg_id);
        cJSON_AddNumberToObject(json_response, "token", RQ_DEL_MSG);
        cJSON_AddNumberToObject(json_response, "room_id", room_id);
        cJSON_AddNumberToObject(json_response, "msg_id", msg_id);
        json_data = vm_message_calibration(json_response);
        vm_send_to_all(json_data, client, room_id);
        g_free(json_data);
    }

    cJSON_Delete(json_response);
}

void del_room_response(cJSON *json_request, t_client *client) 
{
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    if (get_member_type(client->info->database, client->user->user_id, room_id) == DB_ORDINARY) 
    {
        cJSON_AddNumberToObject(json_response, "token", RQ_DEL_ROOM);
        cJSON_AddNumberToObject(json_response, "room_id", room_id);
        json_data = vm_message_calibration(json_response);
        vm_send_to_all(json_data, client, room_id);
        
        delete_room_by_id(client->info->database, room_id);
        g_free(json_data);
    }
    cJSON_Delete(json_response);
}

void ban_member_response(cJSON *json_request, t_client *client) {
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    guint64 user_id = vm_get_object(json_request, "user_id")->valueint;
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    edit_member_type(client->info->database, room_id, user_id, DB_BANNED);
    t_db_user *member = get_user_by_id(client->info->database, user_id);
    GDataOutputStream *out = g_hash_table_lookup(client->info->users, member->login);
    
    cJSON_AddNumberToObject(json_response, "token", RQ_DEL_ROOM);
    cJSON_AddNumberToObject(json_response, "room_id", room_id);
    json_data = vm_message_calibration(json_response);
    vm_send(out, json_data);
    
    cJSON_Delete(json_response);
    g_free(json_data);

    json_response = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_response, "token", RQ_BAN_MEMBER);
    cJSON_AddNumberToObject(json_response, "room_id", room_id);
    cJSON_AddNumberToObject(json_response, "user_id", user_id);
    json_data = vm_message_calibration(json_response);
    vm_send_to_all(json_data, client, room_id);
    
    cJSON_Delete(json_response);
    g_free(json_data);
}

static bool is_valid_member(sqlite3 *db, guint64 user_id, guint64 room_id) 
{
    return get_member_type(db, user_id, room_id) != DB_BANNED && is_member(db, user_id, room_id);
}

static bool is_valid_msg_rq(gchar *msg, sqlite3 *db, guint64 user_id, guint64 room_id) 
{
    return is_valid_member(db, user_id, room_id) && is_valid_info(msg, MAX_MESSAGE);
}

void download_file_response(cJSON *json_request, t_client *client) 
{
    gchar *auth_token = vm_get_valuestring(json_request, "auth_token");
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    guint64 msg_id = vm_get_object(json_request, "msg_id")->valueint;
    gchar *msg = get_message_by_id(client->info->database, msg_id);

    client->is_file = true;
    client->user = get_user_by_token(client->info->database, auth_token);
    if (is_valid_member(client->info->database, client->user->user_id, room_id)) 
    {
        download_file_req(msg, client);
    }

    g_free(auth_token);
}

static gpointer upload_file_thread(gpointer data) 
{
    t_file_helper *file = (t_file_helper*)data;
    cJSON *json_response = cJSON_CreateObject();
    t_db_msg *msg = NULL;
    gchar *filename = NULL;
    gchar *json_data = NULL;

#if defined(__APPLE__)
    filename = g_strdup_printf("%s%llu%s%s%s",
    VM_FILES_DIR, vm_get_time(DB_MICROSECOND), file->client->user->login,
    VM_FILE_DELIM, file->name);
#elif defined(__linux__)
    filename = g_strdup_printf("%s%lu%s%s%s",
    VM_FILES_DIR, vm_get_time(DB_MICROSECOND), file->client->user->login,
    VM_FILE_DELIM, file->name);
#else
    #error "Unsupported OS"
#endif

    if (read_file_req(file->client, file->size, filename) == true) {
        cJSON_AddNumberToObject(json_response, "room_id", file->room_id);
        cJSON_AddStringToObject(json_response, "msg", filename);
        cJSON_AddNumberToObject(json_response, "msg_type", DB_FILE_MSG);
        msg = get_db_message(json_response);
        msg->user_id = file->client->user->user_id;
        insert_message_into_db(file->client->info->database, msg);
        cJSON_Delete(json_response);

        json_response = cJSON_CreateObject();
        cJSON_AddNumberToObject(json_response, "token", RQ_MSG);
        cJSON_AddStringToObject(json_response, "msg", VM_J_STR(msg->msg));
        cJSON_AddNumberToObject(json_response, "room_id", msg->room_id);
        cJSON_AddNumberToObject(json_response, "date", msg->date);
        cJSON_AddNumberToObject(json_response, "msg_id", msg->msg_id);
        cJSON_AddNumberToObject(json_response, "user_id", msg->user_id);
        cJSON_AddNumberToObject(json_response, "msg_type", msg->type);
        json_data = vm_message_calibration(json_response);
        vm_send_to_all(json_data, file->client, file->room_id);

        g_free(json_data);
        cJSON_Delete(json_response);
    }
    g_io_stream_close(G_IO_STREAM(file->client->conn), NULL, NULL);
    file->client->upload_file = FALSE;
    g_free(filename);
    g_free(file->name);
    g_free(file);
    g_thread_exit(NULL);
    return NULL;
}

void upload_file_response(cJSON *json_request, t_client *client) 
{
    gchar *name = vm_get_valuestring(json_request, "name");
    gint size = vm_get_object(json_request, "size")->valueint;
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    gchar *auth_token = vm_get_valuestring(json_request, "auth_token");
    t_file_helper *file = g_malloc0(sizeof(t_file_helper));
    
    client->is_file = true;
    client->user = get_user_by_token(client->info->database, auth_token);

    if (is_valid_member(client->info->database, client->user->user_id, room_id) == false) 
    {
        return;
    }

    file->size = size;
    file->room_id = room_id;
    file->client = client;
    file->name = g_strdup(name);
    client->upload_file = true;
    g_thread_new("upload_thread", upload_file_thread, file);
    g_free(auth_token);
    g_free(name);
}

void join_to_room_response(cJSON *json_request, t_client *client) 
{
    guint64 room_id = vm_get_object(json_request, "id")->valueint;
    cJSON *json_response = cJSON_CreateObject();
    t_db_room *db_room = NULL;
    gchar *json_data = NULL;

    if (is_member(client->info->database, client->user->user_id, room_id) == false) 
    {
        insert_member_into_db(client->info->database, room_id,
                              client->user->user_id, DB_ORDINARY);
        ///send response to the client that sent the request
        db_room = get_room_by_id(client->info->database, room_id);
        cJSON_AddNumberToObject(json_response, "token", RQ_JOIN_ROOM);
        cJSON_AddStringToObject(json_response, "name", VM_J_STR(db_room->room_name));
        cJSON_AddNumberToObject(json_response, "user_id", db_room->user_id);
        cJSON_AddNumberToObject(json_response, "id", db_room->room_id);
        cJSON_AddNumberToObject(json_response, "date", db_room->date);
        cJSON_AddStringToObject(json_response, "desc", VM_J_STR(db_room->desc));
        cJSON_AddStringToObject(json_response, "valid", "true");
        cJSON_AddStringToObject(json_response, "err_msg", "valid roomname");
        json_data = vm_message_calibration(json_response);
        vm_send(client->out, json_data);
        cJSON_Delete(json_response);
        g_free(json_data);
        ///send response to all of the clients
        json_response = cJSON_CreateObject();
        cJSON_AddNumberToObject(json_response, "token", RQ_NEW_MEMBER);
        cJSON_AddNumberToObject(json_response, "user_id", client->user->user_id);
        cJSON_AddNumberToObject(json_response, "room_id", room_id);
        cJSON_AddStringToObject(json_response, "login", client->user->login);
        json_data = vm_message_calibration(json_response);
        vm_send_to_all(json_data, client, room_id);
        g_free(json_data);
        //free room
    }
    cJSON_Delete(json_response);
}

void search_msg_response(cJSON *json_request, t_client *client) {
    gchar *search_msg = vm_get_valuestring(json_request, "search_msg");
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    cJSON *json_response = cJSON_CreateObject();
    cJSON *json_msgs = NULL;
    gchar *json_data = NULL;

    if (is_valid_msg_rq(search_msg, client->info->database, client->user->user_id, room_id)) {
        json_msgs = search_msgs_in_db(client->info->database,
                                   client->user->user_id,
                                   search_msg);
        cJSON_AddNumberToObject(json_response, "token", RQ_SEARCH_MSG);
        cJSON_AddItemReferenceToObject(json_response, "msgs", json_msgs);
        json_data = vm_message_calibration(json_response);
        vm_send(client->out, json_data);

        g_free(json_data);
    }

    g_free(search_msg);
    cJSON_Delete(json_response);
}

void search_room_response(cJSON *json_request, t_client *client) {
    cJSON *json_response = cJSON_CreateObject();
    cJSON *json_rooms = NULL;
    gchar *search_name = vm_get_valuestring(json_request, "search_name");
    gchar *json_data = NULL;

    if (strlen(search_name) > 0 && strlen(search_name) <= MAX_ROOM_LEN) {
        json_rooms = search_rooms_in_db(client->info->database,
                                     client->user->user_id,
                                     search_name);
        cJSON_AddNumberToObject(json_response, "token", RQ_SEARCH_CH);
        cJSON_AddItemReferenceToObject(json_response, "rooms", json_rooms);
        json_data = vm_message_calibration(json_response);
        vm_send(client->out, json_data);

        g_free(json_data);
    }

    g_free(search_name);
    cJSON_Delete(json_response);
}

void log_out_response(cJSON *json_request, t_client *client) 
{
    gchar *auth_token = vm_get_valuestring(json_request, "auth_token");
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    if (strcmp(client->user->auth_token, auth_token) != 0) 
    {
        return;
    }

    cJSON_AddNumberToObject(json_response, "token", RQ_LOG_OUT);
    cJSON_AddStringToObject(json_response, "auth_token", VM_J_STR(auth_token));
    json_data = vm_message_calibration(json_response);
    vm_send(client->out, json_data);

    g_hash_table_remove(client->info->users, client->user->login);
    cJSON_Delete(json_response);
    g_free(json_data);
}

void new_messages_response(cJSON *json_request, t_client *client) 
{
    cJSON *date = vm_get_object(json_request, "date");
    cJSON *count = vm_get_object(json_request, "count");
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    cJSON *json_response = NULL;
    gchar *json_data = NULL;

    if(is_member(client->info->database, client->user->user_id, room_id)) 
    {
        json_response = get_new_messages_by_id(client->info->database, room_id,
                                            date->valueint, count->valueint);
        cJSON_AddNumberToObject(json_response, "token", RQ_GET_NEW_MSGS);
        json_data = vm_message_calibration(json_response);
        vm_send(client->out, json_data);
        g_free(json_data);
    }
    
    cJSON_Delete(json_response);
}

void old_messages_response(cJSON *json_request, t_client *client) 
{
    cJSON *date = vm_get_object(json_request, "date");
    cJSON *count = vm_get_object(json_request, "count");
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    cJSON *json_response = NULL;
    gchar *json_data = NULL;

    if(is_member(client->info->database, client->user->user_id, room_id) && 
       get_member_type(client->info->database, client->user->user_id, room_id) != DB_BANNED) 
       {
        
        json_response = get_old_messages_by_id(client->info->database, room_id,
                                            date->valueint, count->valueint);
        cJSON_AddNumberToObject(json_response, "token", RQ_OLD_MSGS);
        json_data = vm_message_calibration(json_response);
        vm_send(client->out, json_data);
        g_free(json_data);
    }

    cJSON_Delete(json_response);
}

void edit_message_response(cJSON *json_request, t_client *client) 
{
    gchar *new_msg_str = vm_get_valuestring(json_request, "msg");
    guint64 room_id = vm_get_object(json_request, "room_id")->valueint;
    gdouble msg_id = vm_get_object(json_request, "msg_id")->valuedouble;
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    if (is_member(client->info->database, client->user->user_id, room_id) &&
        is_msg_owner(client->info->database, client->user->user_id, msg_id) &&
        get_member_type(client->info->database, client->user->user_id, room_id) != DB_BANNED) 
    {
        
        edit_message_by_id(client->info->database, msg_id, new_msg_str);
        cJSON_AddNumberToObject(json_response, "token", RQ_EDIT_MSG);
        cJSON_AddNumberToObject(json_response, "room_id", room_id);
        cJSON_AddNumberToObject(json_response, "msg_id", msg_id);
        cJSON_AddStringToObject(json_response, "msg", new_msg_str);

        json_data = vm_message_calibration(json_response);
        vm_send_to_all(json_data, client, room_id);
        g_free(json_data);
    }

    cJSON_Delete(json_response);
}

void send_message_response(cJSON *json_request, t_client *client) 
{
    t_db_msg *msg = get_db_message(json_request);
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    if (is_valid_msg_rq(msg->msg, client->info->database, client->user->user_id, msg->room_id)) {
        msg->user_id = client->user->user_id;
        insert_message_into_db(client->info->database, msg);

        cJSON_AddNumberToObject(json_response, "token", RQ_MSG);
        cJSON_AddStringToObject(json_response, "msg", VM_J_STR(msg->msg));
        cJSON_AddNumberToObject(json_response, "room_id", msg->room_id);
        cJSON_AddNumberToObject(json_response, "date", msg->date);
        cJSON_AddNumberToObject(json_response, "msg_id", msg->msg_id);
        cJSON_AddNumberToObject(json_response, "user_id", msg->user_id);
        cJSON_AddNumberToObject(json_response, "msg_type", msg->type);

        json_data = vm_message_calibration(json_response);
        vm_send_to_all(json_data, client, msg->room_id);
        g_free(json_data);
    }

    cJSON_Delete(json_response);
}

void get_members_response(cJSON *json_request, t_client *client) 
{
    cJSON *room_id = vm_get_object(json_request, "room_id");
    cJSON *json_members = get_json_members(client->info->database, room_id->valuedouble);
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    cJSON_AddNumberToObject(json_response, "token", RQ_GET_MEMBERS);
    cJSON_AddItemReferenceToObject(json_response, "members", json_members);
    cJSON_AddNumberToObject(json_response, "room_id", room_id->valueint);
    
    json_data = vm_message_calibration(json_response);
    vm_send(client->out, json_data);
    
    g_free(json_data);
    cJSON_Delete(json_response);
}

void new_room_response(cJSON *json_request, t_client *client) 
{
    t_db_room *room = get_db_room(json_request);
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;

    if (is_valid_room_name(room->room_name)) 
    {
        room->user_id = client->user->user_id;
        insert_room_into_db(client->info->database, room);
        ///form a cJSON response
        cJSON_AddNumberToObject(json_response, "token", RQ_NEW_ROOM);
        cJSON_AddNumberToObject(json_response, "type", RQ_JOIN_ROOM);
        cJSON_AddStringToObject(json_response, "valid", "true");
        cJSON_AddStringToObject(json_response, "name", VM_J_STR(room->room_name));
        cJSON_AddNumberToObject(json_response, "user_id", room->user_id);
        cJSON_AddNumberToObject(json_response, "id", room->room_id);
        cJSON_AddNumberToObject(json_response, "date", room->date);
        cJSON_AddStringToObject(json_response, "desc", VM_J_STR(room->desc));
        cJSON_AddStringToObject(json_response, "err_msg", "valid roomname");
    } 
    else 
    {
        cJSON_AddNumberToObject(json_response, "token", RQ_NEW_ROOM);
        cJSON_AddStringToObject(json_response, "valid", "false");
        cJSON_AddStringToObject(json_response, "err_msg", "invalid roomname");
    }
    json_data = vm_message_calibration(json_response);
    vm_send(client->out, json_data);

    g_free(json_data);
    free_room(&room);
    cJSON_Delete(json_response);
}

void get_rooms_response(cJSON *json_request, t_client *client) 
{
    guint64 date = vm_get_object(json_request, "date")->valueint;
    cJSON *json_response = cJSON_CreateObject();
    cJSON *json_rooms = NULL;
    gchar *json_data = NULL;

    json_rooms = get_json_rooms(client->info->database, date, client->user->user_id);
    cJSON_AddNumberToObject(json_response, "token", RQ_GET_ROOMS);
    cJSON_AddItemReferenceToObject(json_response, "rooms", json_rooms);
    json_data = vm_message_calibration(json_response);
    vm_send(client->out, json_data);
    
    g_free(json_data);
    cJSON_Delete(json_response);
}

void sign_in_response(cJSON *json_request, t_client *client) 
{
    cJSON *json_response = cJSON_CreateObject();
    gchar *json_data = NULL;
    gchar *valid_status = NULL;
    gchar *err_msg = NULL;

    t_db_user *user = get_db_user(json_request);
    user->auth_token = create_user_token(user->login);
    client->user = user;
    insert_auth_token(client->info->database, user->login, user->auth_token);
    t_db_user *check = get_user_by_login(client->info->database, user->login);
    
    if (check == NULL) 
    {
        valid_status = "false";
        err_msg = "user not found";
    }
    else if (g_strcmp0(check->pass, client->user->pass)) 
    {
        valid_status = "false";
        err_msg = "invalid password";
    }
    else if (g_hash_table_lookup(client->info->users, check->login)) 
    {
        valid_status = "false";
        err_msg = "user already online";
    }
    else 
    {
        client->user = check;
        valid_status = "true";
        err_msg = "correct credentials";
        g_hash_table_insert(client->info->users, client->user->login, client->out);
    }

    cJSON_AddNumberToObject(json_response, "token", RQ_SIGN_IN);
    cJSON_AddStringToObject(json_response, "auth_token", VM_J_STR(client->user->auth_token));
    cJSON_AddStringToObject(json_response, "login", VM_J_STR(client->user->login));
    cJSON_AddStringToObject(json_response, "desc", VM_J_STR(client->user->desc));
    cJSON_AddStringToObject(json_response, "name", VM_J_STR(client->user->name));
    cJSON_AddStringToObject(json_response, "valid", valid_status);
    cJSON_AddStringToObject(json_response, "err_msg", err_msg);

    json_data = vm_message_calibration(json_response);
    vm_send(client->out, json_data);

    g_free(json_data);
    cJSON_Delete(json_response);
}

void sign_up_response(cJSON *json_request, t_client *client) 
{
    t_db_user *user = get_db_user(json_request);
    cJSON *json_response = cJSON_CreateObject();

    if(user_exists(client->info->database, user->login)) 
    {
        cJSON_AddNumberToObject(json_response, "token", RQ_SIGN_UP);
        cJSON_AddStringToObject(json_response, "valid", "false");
        cJSON_AddStringToObject(json_response, "err_msg", "user already exists");
        gchar *json_data = vm_message_calibration(json_response);
        vm_send(client->out, json_data);
        g_free(json_data);
        free_db_user(user);
    }
    else 
    {
        client->user = user;
        create_new_user(client->info->database, user);
        sign_in_response(json_request, client);
    }

    cJSON_Delete(json_response);
}
