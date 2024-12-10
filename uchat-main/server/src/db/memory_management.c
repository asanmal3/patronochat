#include "server.h"
#include "database.h"
// Function to free the memory allocated for a database user struct
void free_db_user(t_db_user *user) 
{
    // Check if the user pointer is valid
    if (user) {
        // Free each of the individual fields if they are not NULL
        if (user->name) 
        {
            g_free(user->name);  // Free the allocated memory for the user's name
        }
        if (user->login) 
        {
            g_free(user->login);  // Free the allocated memory for the user's login
        }
        if (user->pass) 
        {
            g_free(user->pass);  // Free the allocated memory for the user's password
        }
        if (user->desc) 
        {
            g_free(user->desc);  // Free the allocated memory for the user's description
        }
        free((void *)user);  // Free the memory allocated for the user struct itself
        user = NULL;  // Set the user pointer to NULL to prevent dangling pointer
    }
}

// Function to free the memory allocated for a room struct
void free_room(t_db_room **room) 
{
    // Check if the room pointer is valid and if the room itself is not NULL
    if (!room || !*room)
    {
        return;  // If either is NULL, do nothing and return
    }

    // Free the individual fields of the room if they are not NULL
    if ((*room)->room_name)
    {
        g_free((*room)->room_name);  // Free the memory allocated for the room's name
    }
    if ((*room)->desc)
    {   
        g_free((*room)->desc);  // Free the memory allocated for the room's description
    }

    // Free the memory allocated for the room struct itself
    vm_free((void **)room);  // Custom memory free function (likely for freeing a double pointer)
    room = NULL;  // Set the room pointer to NULL to prevent dangling pointer
}

// Function to free the memory allocated for a message struct
void free_message(t_db_message **message) 
{
    // Check if the message pointer is valid and if the message itself is not NULL
    if (!message || !*message)
    {    
        return;  // If either is NULL, do nothing and return
    }

    // Free the individual fields of the message if they are not NULL
    if ((*message)->message)
    {    
        g_free((*message)->message);  // Free the memory allocated for the message content
    }
    if ((*message)->file_name)
    {    
        g_free((*message)->file_name);  // Free the memory allocated for the file name (if any)
    }

    // Free the memory allocated for the message struct itself
    vm_free((void **)message);  // Custom memory free function (likely for freeing a double pointer)
    message = NULL;  // Set the message pointer to NULL to prevent dangling pointer
}
