#include "api.h"


gint8 get_member_type(sqlite3 *db, guint64 user_id, guint64 room_id) 
{
    sqlite3_stmt *stmt;  // SQLite statement object
    gint32 rv = 0;  // Return value for error checking
    gint8 perm = -1;  // Default permission value (no permission)

    // Prepare SQL query to fetch the user's permission for the given room
    rv = sqlite3_prepare_v2(db, "select permission from members where "
                                "room_id = ?1 and user_id = ?2 ", 
                            -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, room_id);  // Bind room ID to the query
    sqlite3_bind_int64(stmt, 2, user_id);  // Bind user ID to the query

    // Execute the query and check if a row is returned
    if ((rv = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        perm = sqlite3_column_int(stmt, 0);  // Get the permission value from the result
    }
    sqlite3_finalize(stmt);  // Finalize the statement to release resources
    return perm;  // Return the permission level
}

gboolean is_msg_owner(sqlite3 *db, guint64 user_id, guint64 msg_id) 
{
    sqlite3_stmt *stmt;  // SQLite statement object
    gint32 rv = 0;  // Return value for error checking
    gboolean is_owner = false;  // Default is not the owner

    // Prepare SQL query to check if the user is the owner of the message
    rv = sqlite3_prepare_v2(db, "select * from messages where message_id = ?1 "
                                "and user_id = ?2 ", -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, msg_id);  // Bind message ID to the query
    sqlite3_bind_int64(stmt, 2, user_id);  // Bind user ID to the query

    // Execute the query and check if a row is returned
    if ((rv = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        is_owner = true;  // If a row is found, user is the owner
    }
    sqlite3_finalize(stmt);  // Finalize the statement to release resources
    return is_owner;  // Return TRUE if user is the owner
}


gboolean is_member(sqlite3 *db, guint64 user_id, guint64 room_id) 
{
    sqlite3_stmt *stmt;  // SQLite statement object
    gint32 rv = SQLITE_OK;  // Return value for error checking

    // Prepare SQL query to check if the user is a member of the room
    rv = sqlite3_prepare_v2(db, "select * from members where user_id = ?1 "
                                "and room_id = ?2", -1, &stmt, 0);
    sqlite3_bind_int64(stmt, 1, user_id);  // Bind user ID to the query
    sqlite3_bind_int64(stmt, 2, room_id);  // Bind room ID to the query

    // Execute the query and check if a row is returned
    if ((rv = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        sqlite3_finalize(stmt);  // Finalize the statement to release resources
        return true;  // Return TRUE if the user is a member
    }
    sqlite3_finalize(stmt);  // Finalize the statement to release resources
    return false;  // Return FALSE if the user is not a member
}


gboolean is_valid_info(gchar *msg, gsize max_value) 
{
    gsize len = 0;  // Length of the message

    // Check if the message is NULL or empty
    if (!msg)
    {    
        return false;
    }
    len = strlen(msg);  // Get the length of the message
    // Check if the message length is within the allowed range
    if (len <= 0 || len > max_value)
    {    
        return false;
    }
    return true;  // Return TRUE if the message length is valid
}


static void randomize_str(gchar *str, size_t count) 
{
    // Randomly modify the characters of the input string
    for (size_t i = 0; i < count; ++i) 
    {
        str[i] = (gchar)g_random_int_range(1, CHAR_MAX);  // Assign random values to each character
    }
}
gchar *create_user_token(char *login) 
{
    gchar *seed = g_compute_checksum_for_string(G_CHECKSUM_MD5, login, strlen(login));  // MD5 checksum of login
    gchar *token = NULL;  // Token to be returned
    gchar *hash = NULL;  // SHA256 hash of the random string

    // Randomize the seed string and generate a new hash based on the randomized seed
    randomize_str(seed, strlen(login) / 2);
    hash = g_compute_checksum_for_string(G_CHECKSUM_SHA256, seed, strlen(seed));  // SHA256 checksum of the randomized seed
    // Create the final token by concatenating the hash and login
    token = g_strjoin("", hash, login, NULL);
    
    g_free(hash);  // Free memory for the hash
    g_free(seed);  // Free memory for the seed
    return strdup(token);  // Return a copy of the generated token
}


gboolean is_valid_room_name(gchar *roomname) 
{
    size_t len = strlen(roomname);  // Get the length of the room name
    // Check if the room name is NULL, empty, or exceeds the maximum allowed length
    if (!roomname) 
    {
        return false;
    }
    if (len <= 0 || len > MAX_ROOM_LEN) 
    {
        return false;
    }
    return true;  // Return TRUE if the room name is valid
}
