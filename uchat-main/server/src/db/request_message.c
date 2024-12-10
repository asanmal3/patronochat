#include "server.h"
#include "database.h"
// Function to append the condition for fetching older messages to the query string
static void message_old(sqlite3_str **str) 
{
    // Append condition to fetch messages older than the provided date
    sqlite3_str_appendall(*str, " and date < ?1");
}

// Function to append the condition for fetching newer messages to the query string
static void message_new(sqlite3_str **str) 
{
    // Append condition to fetch messages newer than the provided date
    sqlite3_str_appendall(*str, " and date > ?1");
}

// Function to create a SQL query to fetch messages by room_id and message type (new or old)
gchar *create_request_message_by_id(sqlite3 *db, guint64 room_id, gint8 type) 
{
    sqlite3_str *str = sqlite3_str_new(db); // Initialize a new SQLite string builder
    gchar *request = NULL;  // Declare a pointer for the final SQL query string

    // Define an array of function pointers for message filters (new or old)
    void (*func[])(sqlite3_str **) = {message_new, message_old}; // `message_new` for newer messages, `message_old` for older messages

    // Start the SQL query with the basic selection based on room_id
    sqlite3_str_appendf(str, "select * from messages where room_id = %llu", room_id);

    // Call the appropriate function to append the date filter (new or old messages)
    func[type](&str); // `type` determines whether the filter for new or old messages is applied

    // Order the results by message_id in descending order (most recent first)
    sqlite3_str_appendall(str, " order by message_id desc");

    // Convert the SQLite string object to a regular C string (final SQL query)
    request = sqlite3_str_finish(str);

    // Return the final SQL query string
    return request;
}
