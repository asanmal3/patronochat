#include "server.h"
#include "database.h"
// Helper function to create a JSON object representing a single message from the database result
cJSON *get_object_message(sqlite3_stmt *stmt) 
{
    cJSON *msg = cJSON_CreateObject();  // Create a new JSON object for the message

    // Add each field of the message to the JSON object by extracting the data from the statement
    cJSON_AddNumberToObject(msg, "msg_id", sqlite3_column_int64(stmt, 0));  // Message ID
    cJSON_AddNumberToObject(msg, "user_id", sqlite3_column_int64(stmt, 1));  // User ID
    cJSON_AddNumberToObject(msg, "room_id", sqlite3_column_int64(stmt, 2));  // Room ID
    cJSON_AddNumberToObject(msg, "date", sqlite3_column_int64(stmt, 3));  // Message date (timestamp)
    cJSON_AddStringToObject(msg, "msg", VM_J_STR((char*)sqlite3_column_text(stmt, 4)));  // Message content
    cJSON_AddNumberToObject(msg, "msg_type", sqlite3_column_int(stmt, 5));  // Message type (text, file, etc.)
    cJSON_AddNumberToObject(msg, "file_size", sqlite3_column_int(stmt, 6));  // File size (if applicable)
    cJSON_AddStringToObject(msg, "file_name", VM_J_STR((char*)sqlite3_column_text(stmt, 7)));  // File name (if applicable)
    cJSON_AddNumberToObject(msg, "status", sqlite3_column_int(stmt, 8));  // Status (e.g., read/unread)
    
    return msg;  // Return the created JSON object representing the message
}

// Helper function to fetch messages by room ID and date, and return them as a JSON object
static cJSON *get_messages_by_id(sqlite3_stmt *stmt, guint64 room_id, 
                                 gint64 count, guint64 date) 
{
    cJSON *room = cJSON_CreateObject();  // Create a new JSON object for the room
    cJSON *message = cJSON_CreateArray();  // Create an array to hold the messages

    sqlite3_bind_int64(stmt, 1, date);  // Bind the date to the SQL query

    // Loop through the result set and add each message to the JSON array
    for (gint64 i = 0; i < count && sqlite3_step(stmt) == SQLITE_ROW; ++i) 
    {
        cJSON_AddItemToArray(message, get_object_message(stmt));  // Add the message as a JSON object
    }

    // Add room information and messages to the JSON object
    cJSON_AddItemToObject(room, "room_id", cJSON_CreateNumber(room_id));  // Room ID
    cJSON_AddItemToObject(room, "messages", message);  // Messages array
    
    sqlite3_finalize(stmt);  // Finalize the statement to free the resources
    return room;  // Return the room object containing the messages
}

// Function to fetch new messages for a room after a specific date
cJSON *get_new_messages_by_id(sqlite3 *db, guint64 room_id,
                                 guint64 date, gint64 count) 
{
    sqlite3_stmt *stmt;  // Declare the prepared statement pointer
    gint32 rv = SQLITE_OK;  // Return value for SQLite functions (not used here, but can be useful for error handling)
    
    // Create the SQL request for fetching new messages based on room_id and date
    gchar *request = create_request_message_by_id(db, room_id, DB_NEW_MESSAGE);

    // Prepare the SQL statement for execution
    rv = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);

    // Free the request string as it's no longer needed
    sqlite3_free(request);

    // Get the messages by executing the prepared statement
    return get_messages_by_id(stmt, room_id, count, date);  
}

// Function to fetch old messages for a room before a specific date
cJSON *get_old_messages_by_id(sqlite3 *db, guint64 room_id,
                              guint64 date, gint64 count) 
{
    sqlite3_stmt *stmt;  // Declare the prepared statement pointer
    
    // Create the SQL request for fetching old messages based on room_id and date
    gchar *request = create_request_message_by_id(db, room_id, DB_OLD_MESSAGE);

    // Prepare the SQL statement for execution
    sqlite3_prepare_v2(db, request, -1, &stmt, NULL);

    // Free the request string as it's no longer needed
    sqlite3_free(request);

    // Get the messages by executing the prepared statement
    return get_messages_by_id(stmt, room_id, count, date);
}
