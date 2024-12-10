#include "api.h"


static gboolean is_valid(GFile *file, gsize size, gsize bytes) 
{
    if (file && bytes != size) 
    {
        // If the file's size does not match the expected size, delete it
        if (g_file_query_exists(file, NULL)) 
        {
            g_file_delete(file, NULL, NULL);  // Delete the invalid file
        }
        return false;  // Return false indicating the file is not valid
    }

    // Check if the file is a regular file (not a directory or symlink)
    if (file && g_file_query_file_type(file, G_FILE_QUERY_INFO_NONE, NULL)
        != G_FILE_TYPE_REGULAR) 
    {
        // If the file is not a regular file, delete it
        if (g_file_query_exists(file, NULL)) 
        {
            g_file_delete(file, NULL, NULL);  // Delete the invalid file
        }
        return false;  // Return false indicating the file is not valid
    }
    
    return true;  // Return true if the file is valid
}


static gboolean read_file(t_client *client, GFile *file,
                          gsize size, GFileOutputStream *out) 
{
    gsize bytes = 0;  // Variable to track the number of bytes written

    // If the file size is within the allowed limit (VM_MAX_FILE_SIZE)
    if (size <= VM_MAX_FILE_SIZE) 
    {
        // Attempt to splice the client's input stream into the output stream to write the file data
        bytes = g_output_stream_splice(
            G_OUTPUT_STREAM(out), client->in_s,
            G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET,
            NULL, NULL);
    }

    // Validate the file after attempting to write it
    return is_valid(file, size, bytes);
}


static void send_ready(t_client *client) 
{
    cJSON *json_request = cJSON_CreateObject();  // Create a new JSON object for the response
    gchar *json_data = NULL;  // Variable to hold the formatted JSON data

    // Add the token field to indicate the server is ready to read the file
    cJSON_AddNumberToObject(json_request, "token", RQ_READY_READ);
    
    // Calibrate the JSON message (prepare it for transmission)
    json_data = vm_message_calibration(json_request);
    
    // Send the "ready to read" response to the client
    vm_send(client->out, json_data);

    // Free the allocated memory for the response data
    g_free(json_data);
    cJSON_Delete(json_request);
}


gboolean read_file_req(t_client *client, gsize size, char *name) 
{
    // Create a GFile object for the specified file path
    GFile *file = g_file_new_for_path(name);
    // Create an output stream for the file (to write data to it)
    GFileOutputStream *out = g_file_create(file, G_FILE_CREATE_NONE, NULL, NULL);

    // Send the "ready to read" response to the client
    send_ready(client);
    
    // If the output stream is NULL or the file could not be successfully read, return false
    if (out == NULL || read_file(client, file, size, out) == false) 
    {
        g_object_unref(file);  // Unreference the GFile object to free resources
        return false;  // Return false indicating the file could not be read
    }
    
    g_object_unref(file);  // Unreference the GFile object to free resources
    return true;  // Return true indicating the file was successfully read and written
}
