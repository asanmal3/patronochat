#include "server.h"
#include "database.h"
// Function to update the description of a room identified by its room_id
void edit_room_desc_by_id(sqlite3 *db, guint64 id, gchar *new_name) 
{
    sqlite3_stmt *stmt;  // Declare a pointer for the prepared SQL statement

    // Prepare the SQL statement to update the room description based on room_id
    sqlite3_prepare_v2(db, "update rooms set desc = ?1 where id = ?2", -1, &stmt, NULL);
    
    // Bind the new room description and room_id to the SQL query
    sqlite3_bind_text(stmt, 1, new_name, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, id);

    // Execute the statement to update the room description
    sqlite3_step(stmt);

    // Finalize the statement to release the allocated resources
    sqlite3_finalize(stmt);
}


// Function to update the name of a room identified by its room_id
void edit_room_name_by_id(sqlite3 *db, guint64 id, gchar *new_name) 
{
    sqlite3_stmt *stmt;  // Declare a pointer for the prepared SQL statement
    gint32 rv = SQLITE_OK;  // Return value for SQLite functions (not used here, but can be useful for error handling)

    // Prepare the SQL statement to update the room name based on room_id
    rv = sqlite3_prepare_v2(db, "update rooms set name = ?1 where id = ?2", -1, &stmt, NULL);

    // Bind the new room name and room_id to the SQL query
    sqlite3_bind_text(stmt, 1, new_name, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, id);

    // Execute the statement to update the room name
    sqlite3_step(stmt);

    // Finalize the statement to release the allocated resources
    sqlite3_finalize(stmt);
}


// Function to update the login (username) of a user identified by their user_id
void edit_user_name_by_id(sqlite3 *db, guint64 id, gchar *new_name) 
{
    sqlite3_stmt *stmt;  // Declare a pointer for the prepared SQL statement
    gint32 rv = SQLITE_OK;  // Return value for SQLite functions (not used here, but can be useful for error handling)

    // Prepare the SQL statement to update the user's login (username) based on user_id
    rv = sqlite3_prepare_v2(db, "update users set login = ?1 where id = ?2", -1, &stmt, NULL);

    // Bind the new username and user_id to the SQL query
    sqlite3_bind_text(stmt, 1, new_name, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, id);

    // Execute the statement to update the username
    sqlite3_step(stmt);

    // Finalize the statement to release the allocated resources
    sqlite3_finalize(stmt);
}


// Function to update the content of a message identified by its message_id
void edit_message_by_id(sqlite3 *db, guint64 id, gchar *new) 
{
    sqlite3_stmt *stmt;  // Declare a pointer for the prepared SQL statement
    gint32 rv = SQLITE_OK;  // Return value for SQLite functions (not used here, but can be useful for error handling)

    // Prepare the SQL statement to update the message content based on message_id
    rv = sqlite3_prepare_v2(db, "update messages set message = ?1 where message_id = ?2", -1, &stmt, NULL);

    // Bind the new message content and message_id to the SQL query
    sqlite3_bind_text(stmt, 1, new, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, id);

    // Execute the statement to update the message content
    sqlite3_step(stmt);

    // Finalize the statement to release the allocated resources
    sqlite3_finalize(stmt);
}


// Function to update the permission level (type) of a member in a room
void edit_member_type(sqlite3 *db, guint64 room_id, guint64 user_id, gint8 new_type) 
{
    sqlite3_str *sqlite_str = sqlite3_str_new(db);  // Create a new SQLite string object
    gchar *request = NULL;  // To hold the final SQL query

    // Construct the SQL query to update the member's permission based on room_id and user_id
    sqlite3_str_appendf(sqlite_str, "update members set permission = %d where room_id = %llu and user_id = %llu",
                        new_type, room_id, user_id);

    // Convert the SQLite string object into a regular string and execute the query
    request = sqlite3_str_finish(sqlite_str);
    sqlite3_exec(db, request, 0, 0, 0);  // Execute the query to update the member's permission

    // Free the allocated memory for the SQL query
    sqlite3_free(request);
}
