#include "server.h"
#include "database.h"

// Function to delete all messages from a specific room identified by room_id
void delete_all_msgs(sqlite3 *db, guint64 room_id) 
{
    sqlite3_str *sqlite_str = sqlite3_str_new(db);  // Create a new SQLite string object
    gchar *request = NULL;  // To hold the final SQL query

    // Construct the SQL query to delete all messages where the room_id matches
    sqlite3_str_appendf(sqlite_str, "delete from messages where room_id = %llu", room_id);

    // Convert the SQLite string object into a regular string and execute the query
    request = sqlite3_str_finish(sqlite_str);
    sqlite3_exec(db, request, 0, 0, 0);  // Execute the query to delete messages
    sqlite3_free(request);  // Free the allocated memory for the SQL query
}


// Function to delete a specific message by its message_id
void delete_message_by_id(sqlite3 *db, guint64 id) 
{
    sqlite3_stmt *stmt;  // Declare a pointer for the prepared SQL statement

    // Prepare the SQL statement to delete a message based on message_id
    sqlite3_prepare_v2(db, "delete from messages where message_id = ?1", -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);  // Bind the message_id to the prepared statement
    sqlite3_step(stmt);  // Execute the statement
    sqlite3_finalize(stmt);  // Finalize the prepared statement to release resources
}


// Function to delete a room by its room_id and all associated data (members and messages)
void delete_room_by_id(sqlite3 *db, guint64 room_id) 
{
    gchar *request = NULL;  // To hold the final SQL query
    sqlite3_str *sqlite_str = sqlite3_str_new(db);  // Create a new SQLite string object

    // Construct the SQL query to delete the room, its members, and messages associated with the room
    sqlite3_str_appendf(sqlite_str, "delete from rooms where id = %llu;"
                                    "delete from members where room_id = %llu;"
                                    "delete from messages where room_id = %llu",
                        room_id, room_id, room_id);

    // Convert the SQLite string object into a regular string and execute the query
    request = sqlite3_str_finish(sqlite_str);
    sqlite3_exec(db, request, 0, 0, 0);  // Execute the query to delete room, members, and messages
    sqlite3_free(request);  // Free the allocated memory for the SQL query
}


// Function to delete a user by their user_id, and also remove their contacts
void delete_user_by_id(sqlite3 *db, guint64 id) 
{
    sqlite3_str *sqlite_str = sqlite3_str_new(db);  // Create a new SQLite string object
    gchar *request = NULL;  // To hold the final SQL query

    // Construct the SQL query to delete the user from the users table and their associated contacts
    sqlite3_str_appendf(sqlite_str, "delete from users where id = %lu; delete "
                                    "from contacts where user_id = %lu and where user_contact_id = %lu;", 
                                    id, id, id);

    // Convert the SQLite string object into a regular string and execute the query
    request = sqlite3_str_finish(sqlite_str);
    sqlite3_exec(db, request, 0, 0, 0);  // Execute the query to delete the user and contacts
    sqlite3_free(request);  // Free the allocated memory for the SQL query
}
