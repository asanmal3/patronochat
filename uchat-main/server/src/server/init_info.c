#include "server.h"

// Function to initialize a new t_info object
// Allocates memory for the info structure and initializes the users hash table
// and the database connection.
t_info *init_info(void) 
{
    // Allocate memory for t_info object
    t_info *info = vm_malloc(sizeof(t_info));

    // Initialize the users hash table with string keys and values (for user logins)
    info->users = g_hash_table_new(g_str_hash, g_str_equal);

    // Open the database connection (specified by DB_PATH)
    info->database = open_db(DB_PATH);

    return info;  // Return the initialized t_info object
}

// Function to deinitialize and clean up a t_info object
// Closes the database connection, destroys the users hash table, and frees the memory allocated for info.
void deinit_info(t_info **info) 
{
    // Close the database connection
    close_db((*info)->database);

    // Destroy the users hash table to free its resources
    g_hash_table_destroy((*info)->users);

    // Free the memory allocated for the t_info object
    free(*info);

    // Set the pointer to NULL to avoid dangling references
    *info = NULL;
}

// Function to deinitialize a t_client object
// Cleans up resources used by the client, such as removing the user from the users hash table,
// freeing memory for the client's request, and closing the connection stream.
void deinit_client(t_client **client) 
{
    // Check if the client pointer and the client object are valid
    if (client && *client) 
    {
        // If the client has a valid users hash table, output stream, user data, and isn't a file transfer
        if ((*client)->info->users && (*client)->out && (*client)->user
            && !(*client)->is_file) 
        {
            // Remove the client user login from the users hash table
            g_hash_table_remove((*client)->info->users,
                                (*client)->user->login);
        }

        // If the client has a request string, free it
        if ((*client)->request)
        {
            g_free((*client)->request);
        }    

        // If the client has a valid connection stream, close it
        if ((*client)->conn)
        {
            g_io_stream_close(G_IO_STREAM((*client)->conn), NULL, NULL);
        }

        // Set the request pointer to NULL after freeing
        (*client)->request = NULL;

        // Free the memory allocated for the client object
        g_free(*client);

        // Set the client pointer to NULL to avoid dangling references
        *client = NULL;
    }
}
