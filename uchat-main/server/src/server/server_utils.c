#include "server.h"

// Function to get a list of users' logins who are members of a specific room, 
// excluding users with a "banned" permission (permission != DB_BANNED)
GList *get_members_list(sqlite3 *db, guint64 room_id) 
{
    sqlite3_stmt *stmt;  // SQLite statement object for prepared queries
    GList *list = NULL;   // List to hold the logins of the users

    // Prepare the SQL query to select distinct logins of users from the 'users' table 
    // who are members of the specified room and are not banned
    sqlite3_prepare_v2(db, "select distinct login from users inner join "
                           "members on users.id = members.user_id where "
                           "room_id = ?1 and permission != ?2", -1, &stmt, 0);

    // Bind the room_id and the banned permission to the query parameters
    sqlite3_bind_int64(stmt, 1, room_id);  // Bind room_id parameter
    sqlite3_bind_int(stmt, 2, DB_BANNED);  // Bind banned permission

    // Iterate over the result set, retrieving each login and adding it to the list
    while (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        list = g_list_append(list, strdup((char*)sqlite3_column_text(stmt,0)));  // Append login to the list
    }

    sqlite3_finalize(stmt);  // Finalize the prepared statement to free resources
    return list;  // Return the list of logins
}

// Helper function to send a message to each user in the list
static void send_to_all(gpointer list, gpointer user_data) 
{
    t_send_helper *send_helper = (t_send_helper*)user_data;  // Extract the send helper
    GHashTable *table = send_helper->table;  // The table of user connections
    GDataOutputStream *out = g_hash_table_lookup(table, list);  // Find the connection stream for the user

    // If a valid output stream is found, send the message to the user
    if (out) 
    {
        vm_send(out, send_helper->j_data);  // Send the message to the user using vm_send
    }
    (void)list;  // To avoid unused variable warning
}

// Function to send a message to all members of a specific room
void vm_send_to_all(gchar *j_data, t_client *client, guint64 room_id) 
{
    // Get the list of users in the specified room (excluding banned users)
    GList *list = get_members_list(client->info->database, room_id);

    // Allocate memory for the send helper object
    t_send_helper *send_helper = g_malloc0(sizeof(t_send_helper));

    // Set the data to be sent and the user connection table
    send_helper->j_data = j_data;
    send_helper->table = client->info->users;

    // Iterate over the list of members and send the message to each one
    g_list_foreach(list, send_to_all, send_helper);

    // Free the allocated memory for the send helper object
    g_free(send_helper);
}
