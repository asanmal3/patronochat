#include "server.h"

// Function to create a new message struct from a JSON request
t_db_message *get_db_message(cJSON *j_request) 
{
    // Allocate memory for the message struct
    t_db_message *message = vm_malloc(sizeof(t_db_message));

    // Extract values from the JSON request and assign them to the message fields
    message->room_id = vm_get_object(j_request, "room_id")->valueint;  // Get room_id from the JSON request
    message->type = vm_get_object(j_request, "msg_type")->valueint;     // Get message type from the JSON request
    message->file_size = 0;  // Initialize file_size to 0 (as no file size is provided in the request)
    message->message = g_strstrip(vm_get_valuestring(j_request, "msg")); // Get the message content and strip any extra spaces
    message->file_name = g_strdup(""); // Initialize file_name as an empty string (no file name provided)

    // Return the populated message struct
    return message;
}

// Function to create a new user struct from a JSON request
t_db_user *get_db_user(cJSON *j_request) 
{
    // Allocate memory for the user struct
    t_db_user *user = vm_malloc(sizeof(t_db_user));

    // Extract values from the JSON request and assign them to the user fields
    user->name = vm_get_valuestring(j_request, "name");    // Get the user's name from the JSON request
    user->login = vm_get_valuestring(j_request, "login");  // Get the user's login from the JSON request
    user->pass = vm_get_valuestring(j_request, "pass");    // Get the user's password from the JSON request
    user->desc = vm_get_valuestring(j_request, "desc");    // Get the user's description from the JSON request
    user->auth_token = "";  // Initialize auth_token as an empty string (not provided in the request)

    // Return the populated user struct
    return user;
}

// Function to create a new room struct from a JSON request
t_db_room *get_db_room(cJSON *j_request) 
{
    // Allocate memory for the room struct
    t_db_room *room = vm_malloc(sizeof(t_db_room));

    // Extract values from the JSON request and assign them to the room fields
    room->room_name = vm_get_valuestring(j_request, "name"); // Get the room's name from the JSON request
    room->desc = vm_get_valuestring(j_request, "desc");      // Get the room's description from the JSON request
    room->type = vm_get_object(j_request, "type")->valueint; // Get the room type from the JSON request (e.g., private, public)

    // Return the populated room struct
    return room;
}
