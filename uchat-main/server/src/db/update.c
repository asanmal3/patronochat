#include "server.h"

// Function to update the permission of a user in a specific room
void edit_permission_of_user(sqlite3 *db, guint64 user_id, guint64 room_id, gint8 new) 
{
    sqlite3_str *sqlite_str = sqlite3_str_new(db);  // Initialize SQLite string builder
    gchar *request = NULL;  // Declare pointer for the final SQL query string

    // Construct SQL query to update user permission in the "members" table
    sqlite3_str_appendf(sqlite_str, "update members set permission = %d where"
                                    " id = %llu and where room_id = %llu", 
                        new, user_id, room_id);
    // Finalize the SQL query string
    request = sqlite3_str_finish(sqlite_str);
    // Execute the SQL query
    sqlite3_exec(db, request, 0, 0, 0);
}


// Function to update the token of a user
void edit_token(sqlite3 *db, guint64 user_id, gchar *new) 
{
    sqlite3_stmt *stmt;  // Declare SQLite statement handle
    gint32 rv = SQLITE_OK;  // Initialize return value for error checking

    // Prepare the SQL query to update the user's token in the "users" table
    rv = sqlite3_prepare_v2(db, "update users set token = ?1 where "
                                "id = ?2", -1, &stmt, NULL);
    // Bind the new token value to the SQL query
    sqlite3_bind_text(stmt, 1, new, -1, SQLITE_STATIC);
    // Bind the user ID to the SQL query
    sqlite3_bind_int64(stmt, 2, user_id);
    // Execute the query
    sqlite3_step(stmt);
    // Finalize the prepared statement
    sqlite3_finalize(stmt);
}


// Function to update the description of a room
void edit_desc_room_by_id(sqlite3 *db, guint64 room_id, gchar *new) 
{
    sqlite3_stmt *stmt;  // Declare SQLite statement handle
    gint32 rv = SQLITE_OK;  // Initialize return value for error checking

    // Prepare the SQL query to update the room description in the "rooms" table
    rv = sqlite3_prepare_v2(db, "update rooms set desc = ?1 "
                                "where id = ?2", -1, &stmt, NULL);
    // Bind the new description to the SQL query
    sqlite3_bind_text(stmt, 1, new, -1, SQLITE_STATIC);
    // Bind the room ID to the SQL query
    sqlite3_bind_int(stmt, 2, room_id);
    // Execute the query
    sqlite3_step(stmt);
    // Finalize the prepared statement
    sqlite3_finalize(stmt);
}


// Function to update the description of a user
void edit_user_desc(sqlite3 *db, guint64 user_id, gchar *new_desc) 
{
    sqlite3_stmt *stmt;  // Declare SQLite statement handle
    
    // Prepare the SQL query to update the user's description in the "users" table
    sqlite3_prepare_v2(db, "update users set desc = ?1 where"
                           " id = ?2", -1, &stmt, NULL);
    // Bind the new description to the SQL query
    sqlite3_bind_text(stmt, 1, new_desc, -1, SQLITE_STATIC);
    // Bind the user ID to the SQL query
    sqlite3_bind_int64(stmt, 2, user_id);
    // Execute the query
    sqlite3_step(stmt);
    // Finalize the prepared statement
    sqlite3_finalize(stmt);
}


// Function to update the name of a user
void edit_user_name(sqlite3 *db, guint64 user_id, gchar *new_desc) 
{
    sqlite3_stmt *stmt;  // Declare SQLite statement handle
    
    // Prepare the SQL query to update the user's name in the "users" table
    sqlite3_prepare_v2(db, "update users set name = ?1 where"
                           " id = ?2", -1, &stmt, NULL);
    // Bind the new name to the SQL query
    sqlite3_bind_text(stmt, 1, new_desc, -1, SQLITE_STATIC);
    // Bind the user ID to the SQL query
    sqlite3_bind_int64(stmt, 2, user_id);
    // Execute the query
    sqlite3_step(stmt);
    // Finalize the prepared statement
    sqlite3_finalize(stmt);
}
