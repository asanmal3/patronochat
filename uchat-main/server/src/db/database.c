#include "server.h"
#include "database.h"

// Function to retrieve the message content based on the message ID
gchar *get_message_by_id(sqlite3 *db, guint64 msg_id) 
{
    sqlite3_stmt *stmt;
    gchar *result = NULL;

    // Prepare SQL statement to retrieve the message based on its ID
    sqlite3_prepare_v2(db, "select message from messages where message_id = ?1", -1, &stmt, 0);
    sqlite3_bind_int64(stmt, 1, msg_id);  // Bind the message_id to the query
    sqlite3_step(stmt);  // Execute the query
    
    // If a message is found, store the result
    if (sqlite3_column_text(stmt, 0) != NULL) 
    {
        result = strdup((const char*)sqlite3_column_text(stmt, 0));
    }
    return result;
}

// Helper function to retrieve the message ID based on the room ID and date
static void get_id_msg(sqlite3 *db, t_db_msg *msg) 
{
    sqlite3_stmt *stmt;
    gint32 rv = SQLITE_OK;

    // Prepare SQL to retrieve the message ID based on room_id and date
    rv = sqlite3_prepare_v2(db, "select message_id from messages where "
                                "room_id = ?1 and date = ?2",
                            -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, msg->room_id);  // Bind room_id
    sqlite3_bind_int64(stmt, 2, msg->date);     // Bind date
    sqlite3_step(stmt);  // Execute the query
    msg->msg_id = sqlite3_column_int64(stmt, 0);  // Retrieve the message ID
    sqlite3_finalize(stmt);
}

// Helper function to bind message data to SQL statement
static void sqlite_bind_msg(sqlite3_stmt *stmt, t_db_msg *msg) 
{
    sqlite3_bind_int(stmt, 1, msg->user_id);   // Bind user_id
    sqlite3_bind_int(stmt, 2, msg->room_id);   // Bind room_id
    sqlite3_bind_int(stmt, 3, msg->date);      // Bind date
    sqlite3_bind_text(stmt, 4, msg->msg, -1, SQLITE_STATIC); // Bind message
    sqlite3_bind_int(stmt, 5, msg->type);      // Bind type
    sqlite3_bind_int(stmt, 6, msg->file_size); // Bind file size
    sqlite3_bind_text(stmt, 7, msg->file_name, -1, SQLITE_STATIC); // Bind file name
    sqlite3_bind_int(stmt, 8, msg->status);    // Bind message status
}

// Insert a new message into the database
void add_message_to_db(sqlite3 *db, t_db_msg *msg) 
{
    sqlite3_stmt *stmt;
    gint rv;

    msg->date = vm_get_time(DB_SECOND);  // Set the message date
    msg->status = DB_MSG_START;  // Set message status to 'start'
    rv = sqlite3_prepare_v2(db, "INSERT INTO MESSAGES(USER_ID, ROOM_ID, "
                           "DATE, MESSAGE, TYPE, FILE_SIZE, FILE_NAME, STATUS)"
                           "VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);",
                            -1, &stmt, NULL);
    sqlite_bind_msg(stmt, msg);  // Bind message data to SQL statement
    sqlite3_step(stmt);  // Execute the SQL
    sqlite3_finalize(stmt);
    get_id_msg(db, msg);  // Retrieve message ID
}

// Function to retrieve room details by its ID
t_db_room *get_room_by_id(sqlite3 *db, guint64 id) 
{
    t_db_room *room = NULL;
    sqlite3_stmt *stmt;

    sqlite3_prepare_v3(db, "SELECT * FROM ROOMS WHERE ID = ?1", -1, 0, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);  // Bind room_id
    if (sqlite3_step(stmt) == 100)  // Check if a room is found
    {
        room = vm_malloc(sizeof(t_db_room));  // Allocate memory for the room structure
        room->room_id = sqlite3_column_int64(stmt, 0);  // Retrieve room_id
        room->room_name = strdup((const char*)sqlite3_column_text(stmt, 1));  // Retrieve room name
        room->user_id = sqlite3_column_int64(stmt, 2);  // Retrieve user_id
        room->date = sqlite3_column_int64(stmt, 3);  // Retrieve room date
        room->desc = strdup(VM_J_STR((char*)sqlite3_column_text(stmt, 4)));  // Retrieve room description
        room->type = sqlite3_column_int(stmt, 5);  // Retrieve room type
    }
    sqlite3_finalize(stmt);
    return room;
}

// Helper function to parse special characters for SQL LIKE queries
static gchar *parse_str_search(gchar *str) 
{
    gchar *tmp = NULL;
    gint size = (gint)strlen(str);
    gint tmp_point = 0;

    // Count additional characters needed for special characters (% and _)
    for (gint i = 0; i < (gint)strlen(str); ++i) 
    {
        if (str[i] == '%' || str[i] == '_' || str[i] == '\\')
            ++size;
    }
    tmp = malloc(size + 1);
    tmp[size] = 0;
    
    // Escape special characters in the string
    for (gint i = 0; i < (gint)strlen(str); ++i, ++tmp_point) 
    {
        if (str[i] == '%' || str[i] == '_' || str[i] == '\\') 
        {
            tmp[tmp_point++] = '\\';  // Escape special characters with '\'
            tmp[tmp_point] = str[i];
        }
        else
            tmp[tmp_point] = str[i];
    }
    return tmp;
}

// Function to search for messages containing a specific string in a room
cJSON *search_msgs_in_db(sqlite3 *db, guint64 room_id, gchar *str_search) 
{
    gchar *str_search_join = NULL;
    sqlite3_stmt *stmt;
    cJSON *messages = cJSON_CreateArray();

    str_search = parse_str_search(str_search);  // Parse the search string
    str_search_join = g_strjoin("", "%%", str_search, "%%", NULL);  // Add wildcards for SQL LIKE query
    sqlite3_prepare_v2(db, "SELECT * FROM MESSAGES WHERE ROOM_ID = ?2"
                           " AND MESSAGE LIKE ?1 ESCAPE '\\' AND TYPE = ?3",
                       -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, str_search_join, -1, SQLITE_STATIC);  // Bind search string
    sqlite3_bind_int64(stmt, 2, room_id);  // Bind room_id
    sqlite3_bind_int(stmt, 3, DB_TEXT_MSG);  // Bind message type (text)
    
    // Iterate through the results and add to the JSON array
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cJSON_AddItemToArray(messages, get_object_message(stmt));  // Get message details
    }
    g_free(str_search_join);  // Free the joined search string
    sqlite3_finalize(stmt);
    return messages;
}

// Helper function to create JSON object for room data
static cJSON *get_object_room(sqlite3_stmt *stmt) 
{
    cJSON *room = cJSON_CreateObject();

    cJSON_AddNumberToObject(room, "id", sqlite3_column_int64(stmt, 0));  // Add room ID to JSON
    cJSON_AddStringToObject(room, "name", VM_J_STR((char*)sqlite3_column_text(stmt, 1)));  // Add room name
    cJSON_AddNumberToObject(room, "user_id", sqlite3_column_int64(stmt, 2));  // Add user ID
    cJSON_AddNumberToObject(room, "date", sqlite3_column_int64(stmt, 3));  // Add room creation date
    cJSON_AddStringToObject(room, "desc", VM_J_STR((char*)sqlite3_column_text(stmt, 4)));  // Add room description
    cJSON_AddNumberToObject(room, "type", sqlite3_column_int(stmt, 5));  // Add room type
    
    return room;
}

// Function to retrieve rooms for a user created after a specified date
cJSON *get_json_rooms(sqlite3 *db, guint64 date, guint64 user_id) 
{
    cJSON *json_rooms = cJSON_CreateArray();
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, "SELECT * FROM ROOMS WHERE ID IN (SELECT ROOM"
                           "_ID FROM MEMBERS WHERE USER_ID = ?2 AND "
                           "PERMISSION != ?3) AND DATE > ?1 ORDER BY "
                           "DATE DESC",
                       -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, date);  // Bind the date
    sqlite3_bind_int64(stmt, 2, user_id);  // Bind the user_id
    sqlite3_bind_int(stmt, 3, DB_BANNED);  // Exclude banned users
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cJSON_AddItemToArray(json_rooms, get_object_room(stmt));  // Add rooms to the JSON array
    }
    sqlite3_finalize(stmt);
    return json_rooms;
}

// Function to search rooms by name for a specific user
cJSON *search_rooms_in_db(sqlite3 *db, guint64 user_id, gchar *str_search) 
{
    sqlite3_stmt *stmt;
    gchar *str_search_join = NULL;
    cJSON *rooms = cJSON_CreateArray();

    str_search = parse_str_search(str_search);  // Parse the search string
    str_search_join = g_strjoin("", str_search, "%%", NULL);  // Add wildcards for SQL LIKE query
    sqlite3_prepare_v2(db, "SELECT * FROM ROOMS WHERE NAME LIKE ?1 ESCAPE"
                           " '\\' AND ID NOT IN(SELECT ROOM_ID FROM MEMBE"
                           "RS WHERE USER_ID = ?2)",
                           -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, str_search_join, -1, SQLITE_STATIC);  // Bind search string
    sqlite3_bind_int64(stmt, 2, user_id);  // Bind user_id
    
    // Iterate through the results and add to the JSON array
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cJSON_AddItemToArray(rooms, get_object_room(stmt));  // Get room details
    }
    g_free(str_search_join);  // Free the joined search string
    sqlite3_finalize(stmt);
    return rooms;
}

// Helper function to create user object from the SQL result
static t_db_user *get_user(sqlite3_stmt *stmt) 
{
    t_db_user *user = NULL;

    if (sqlite3_step(stmt) == 100)  // Check if the user is found
    {
        user = vm_malloc(sizeof(t_db_user));  // Allocate memory for user structure
        user->user_id = sqlite3_column_int64(stmt, 0);  // Retrieve user ID
        user->name = strdup((const char *)sqlite3_column_text(stmt, 1));  // Retrieve user name
        user->login = strdup((const char*)sqlite3_column_text(stmt, 2));  // Retrieve user login
        user->pass = strdup((const char*)sqlite3_column_text(stmt, 3));  // Retrieve user password
        user->auth_token = strdup((const char*)sqlite3_column_text(stmt, 4));  // Retrieve auth token
        user->date = sqlite3_column_int(stmt, 5);  // Retrieve user registration date
        user->desc = strdup(VM_J_STR((char*)sqlite3_column_text(stmt, 6)));  // Retrieve user description
    }
    sqlite3_finalize(stmt);
    return user;
}

// Function to retrieve user details by user_id
t_db_user *get_user_by_id(sqlite3 *db, guint64 user_id) 
{
    sqlite3_stmt *stmt;

    sqlite3_prepare_v3(db, "select * from users where id = ?1", -1, 0, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, user_id);  // Bind user_id
    return get_user(stmt);  // Retrieve user details
}

// Function to retrieve user details by auth token
t_db_user *get_user_by_token(sqlite3 *db, gchar *token) 
{
    sqlite3_stmt *stmt;

    sqlite3_prepare_v3(db, "select * from users where auth_token = ?1", -1, 0, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, token, -1, SQLITE_STATIC);  // Bind auth token
    return get_user(stmt);  // Retrieve user details
}


// Retrieves a user from the database by their login
t_db_user *get_user_by_login(sqlite3 *db, gchar *login) 
{
    sqlite3_stmt *stmt;

    // Prepares SQL query to fetch user by login
    sqlite3_prepare_v3(db, "SELECT * FROM USERS WHERE LOGIN = ?1", -1, 0, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);  // Bind the login parameter to the query

    return get_user(stmt);  // Fetch and return the user details
}

// Fetches user ID based on the user's date (presumably the date the user was created)
void get_user_id(sqlite3 *db, t_db_user *user) 
{
    sqlite3_stmt *stmt;

    // Prepares query to get user ID from the USERS table by date
    sqlite3_prepare_v2(db, "SELECT ID FROM USERS WHERE DATE = ?1", -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, user->date);  // Bind date parameter
    sqlite3_step(stmt);  // Execute query and step to result
    user->user_id = sqlite3_column_int(stmt, 0);  // Store the retrieved user ID
    sqlite3_finalize(stmt);  // Finalize the statement
}

// Creates a JSON object representing a user
static cJSON *get_object_member(sqlite3_stmt *stmt) 
{
    cJSON *member = cJSON_CreateObject();

    // Add user details to the JSON object
    cJSON_AddNumberToObject(member, "id", sqlite3_column_int64(stmt, 0));
    cJSON_AddStringToObject(member, "login", (char *)sqlite3_column_text(stmt, 1));
    cJSON_AddNumberToObject(member, "type", sqlite3_column_int(stmt, 2));
    return member;
}

// Retrieves all members of a room, excluding banned users
cJSON *get_json_members(sqlite3 *db, guint64 room_id) 
{
    cJSON *members = cJSON_CreateArray();
    sqlite3_stmt *stmt;

    // Prepares query to fetch users who are members of a room, excluding banned users
    sqlite3_prepare_v2(db, "SELECT ID, LOGIN, PERMISSION FROM USERS INNER "
                           "JOIN MEMBERS ON USERS.ID = MEMBERS.USER_ID "
                           "WHERE ROOM_ID = ?1 AND PERMISSION != ?2",
                           -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, room_id);  // Bind room ID
    sqlite3_bind_int(stmt, 2, DB_BANNED);  // Bind exclusion of banned users

    while (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        cJSON_AddItemToArray(members, get_object_member(stmt));  // Add each member to the JSON array
    }
    sqlite3_finalize(stmt);  // Finalize statement
    return members;
}

// Inserts a new member into the MEMBERS table
void add_member_to_db(sqlite3 *db,
                           guint64 room_id,
                           guint64 user_id,
                           gint8 permission) 
{
    gint32 rv = SQLITE_OK;
    sqlite3_str *sqlite_str = sqlite3_str_new(db);
    gchar *request = NULL;

    // Prepare the SQL query for inserting the new member
    sqlite3_str_appendf(sqlite_str, "INSERT INTO MEMBERS(USER_ID, ROOM_ID, "
                                    "DATE, PERMISSION)VALUES(%d, %lu, "
                                    "%llu, %d);",
                        user_id, room_id, vm_get_time(DB_SECOND), permission);
    request = sqlite3_str_finish(sqlite_str);  // Finish building the query
    rv = sqlite3_exec(db, request, 0, 0, 0);  // Execute the query
    sqlite3_free(request);  // Free the query string
}

// Binds room data to an SQLite statement for insertion
static void sqlite_bind_room(sqlite3_stmt *stmt, t_db_room *room) 
{
    sqlite3_bind_text(stmt, 1, room->room_name, -1, SQLITE_STATIC);  // Bind room name
    sqlite3_bind_int64(stmt, 2, room->user_id);  // Bind user ID (creator)
    sqlite3_bind_int64(stmt, 3, room->date);  // Bind room creation date
    sqlite3_bind_text(stmt, 4, room->desc, -1, SQLITE_STATIC);  // Bind room description
    sqlite3_bind_int(stmt, 5, room->type);  // Bind room type
}

// Inserts a new room into the database and adds the creator as a member
void add_room_to_db(sqlite3 *db, t_db_room *room) 
{
    sqlite3_stmt *stmt;
    gint32 rv = SQLITE_OK;

    room->date = vm_get_time(DB_SECOND);  // Set room creation date
    rv = sqlite3_prepare_v2(db, "INSERT INTO ROOMS(NAME, USER_ID, DATE, "
                                "DESC, TYPE) VALUES(?1, ?2, ?3, ?4, ?5);",
                            -1, &stmt, 0);
    sqlite_bind_room(stmt, room);  // Bind room data to the statement
    sqlite3_step(stmt);  // Execute the insert statement
    sqlite3_finalize(stmt);  // Finalize the statement

    // Retrieve the newly created room ID
    rv = sqlite3_prepare_v2(db, "SELECT ID FROM ROOMS WHERE DATE = ?1", -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, room->date);  // Bind the room creation date
    sqlite3_step(stmt);  // Execute and fetch the room ID
    room->room_id = sqlite3_column_int64(stmt, 0);  // Store the room ID
    insert_member_into_db(db, room->room_id, room->user_id, DB_ORDINARY);  // Add the creator as a member
    sqlite3_finalize(stmt);  // Finalize the statement
}

// Updates the user's auth token in the database
void add_auth_token(sqlite3 *db, gchar *login, gchar *auth_token) 
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "UPDATE USERS SET AUTH_TOKEN = ?1 WHERE LOGIN = ?2", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, auth_token, -1, SQLITE_STATIC);  // Bind auth token
    sqlite3_bind_text(stmt, 2, login, -1, SQLITE_STATIC);  // Bind user login
    sqlite3_step(stmt);  // Execute the update
    sqlite3_finalize(stmt);  // Finalize the statement
}

// Validates the user's credentials (login and password)
bool correct_credentials(sqlite3 *db, char *username, char *password) 
{
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, "SELECT ID FROM USERS WHERE LOGIN = ?1 AND PASS = ?2", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);  // Bind username
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);  // Bind password

    if (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        sqlite3_finalize(stmt);  // Finalize the statement and return true if credentials are correct
        return true;
    }

    sqlite3_finalize(stmt);  // Finalize the statement and return false if credentials are invalid
    return false;
}

// Creates a new user in the database
void create_new_user(sqlite3 *db, t_db_user *user) 
{
    sqlite3_stmt *stmt;

    user->date = vm_get_time(DB_SECOND);  // Set user creation date

    // Prepare the SQL query for inserting the new user
    sqlite3_prepare_v2(db, "INSERT INTO USERS(NAME, LOGIN, PASS, AUTH_TOKEN, DATE, DESC)"
                           "VALUES (?1, ?2, ?3, ?4, ?5, ?6);", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, user->name, -1, SQLITE_STATIC);  // Bind user details
    sqlite3_bind_text(stmt, 2, user->login, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user->pass, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, user->auth_token, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, user->date);
    sqlite3_bind_text(stmt, 6, user->desc, -1, SQLITE_STATIC);
    sqlite3_step(stmt);  // Execute the insert query
    sqlite3_finalize(stmt);  // Finalize the statement

    get_user_id(db, user);  // Retrieve the user's ID
}

// Checks if a user already exists in the database based on their username
bool user_exists(sqlite3 *db, char *username) 
{
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, "SELECT ID FROM USERS WHERE LOGIN = ?1", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);  // Bind username

    if (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        sqlite3_finalize(stmt);  // Finalize the statement and return true if user exists
        return true;
    }
    sqlite3_finalize(stmt);  // Finalize the statement and return false if user doesn't exist
    return false;
}

// Creates the MESSAGES table in the database if it doesn't already exist
void create_messages_table(sqlite3 *db) 
{
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS MESSAGES("
                     "MESSAGE_ID    INTEGER PRIMARY KEY NOT NULL,"
                     "USER_ID       INTEGER             NOT NULL,"
                     "ROOM_ID       INTEGER             NOT NULL,"  
                     "DATE          INTEGER             NOT NULL,"
                     "MESSAGE       TEXT                        ,"
                     "TYPE          INTEGER             NOT NULL,"
                     "FILE_SIZE     INTEGER                     ,"
                     "FILE_NAME     TEXT                        ,"
                     "STATUS        INTEGER             NOT NULL);",
                     0, 0, 0);  // Execute query to create the table
}

// Creates the MEMBERS table in the database if it doesn't already exist
void create_members_table(sqlite3 *db) 
{
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS MEMBERS("
                     "USER_ID        INTEGER NOT NULL,"
                     "ROOM_ID        INTEGER NOT NULL,"
                     "DATE           INTEGER NOT NULL,"
                     "PERMISSION     INTEGER NOT NULL);",
                 0, 0, 0);  // Execute query to create the table
}

// Creates the ROOMS table in the database if it doesn't already exist
void create_rooms_table(sqlite3 *db) 
{
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS ROOMS("
                     "ID                 INTEGER PRIMARY KEY NOT NULL,"
                     "NAME               TEXT                NOT NULL,"
                     "USER_ID            INTEGER             NOT NULL,"
                     "DATE               INT                 NOT NULL,"
                     "DESC               TEXT                        ,"
                     "TYPE               INT                 NOT NULL);",
                     0, 0, 0);  // Execute query to create the table
}

// Creates the USERS table in the database if it doesn't already exist
void create_users_table(sqlite3 *db) 
{
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS USERS("
                     "ID             INTEGER PRIMARY KEY NOT NULL,"
                     "NAME           TEXT                NOT NULL,"
                     "LOGIN          TEXT  UNIQUE        NOT NULL,"
                     "PASS           TEXT                NOT NULL,"
                     "AUTH_TOKEN     TEXT                NOT NULL,"
                     "DATE           INTEGER             NOT NULL,"
                     "DESC           TEXT                );",
                 0, 0, 0);  // Execute query to create the table
}

// Creates all required tables in the database
void create_tables(sqlite3 *db) 
{
    create_users_table(db);  // Create USERS table
    create_rooms_table(db);  // Create ROOMS table
    create_members_table(db);  // Create MEMBERS table
    create_messages_table(db);  // Create MESSAGES table
}

// Opens the SQLite database and creates tables if they don't exist
sqlite3 *open_db() 
{
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(DB_PATH, &db);  // Open the database
    if (rc != SQLITE_OK) 
    {
        vm_logger(VM_LOG_FILE, "error: cannot open the database");  // Log error if database can't be opened
    } 
    else 
    {
        vm_logger(VM_LOG_FILE, "database has been opened successfully");  // Log success message
    }
    create_tables(db);  // Create tables if necessary
    return db;  // Return the opened database connection
}

