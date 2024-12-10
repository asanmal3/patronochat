#include "api.h"

/*
 * Function: download
 * -------------------------------
 * This function handles the downloading of a file requested by a client. 
 * It reads the file and sends it to the client over a stream.
 * 
 * client: the client requesting the file
 * file: the file to be downloaded
 * in_d: input data stream to read from the client request
 */
static void download(t_client *client, GFile *file, GDataInputStream *in_d) 
{
    GOutputStream *out = g_io_stream_get_output_stream(G_IO_STREAM(client->conn));  // Get the output stream to send data to the client
    GFileInputStream *in = g_file_read(file, NULL, NULL);  // Open the file for reading
    gchar *ready = g_data_input_stream_read_line(in_d, NULL, NULL, NULL);  // Read a line of input from the client
    cJSON *j_request = cJSON_Parse(ready);  // Parse the client request (in JSON format)
    gint token = vm_get_token(j_request);  // Extract the token from the JSON request

    // Check if the request is ready for reading
    if (token == RQ_READY_READ) 
    {
        // Splice the input stream (file) into the output stream (client connection) to send the file
        g_output_stream_splice(out, G_INPUT_STREAM(in), G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE, NULL, NULL);
    }

    // Free the allocated memory for the request line and JSON object
    if (ready) 
    {
        g_free(ready);  // Free the memory used by the request line
    }
    cJSON_Delete(j_request);  // Free the memory used by the JSON object

    // Close the client connection after the download is complete
    g_io_stream_close(G_IO_STREAM(client->conn), NULL, NULL);
}

/*
 * Function: download_file_req
 * -------------------------------
 * This function handles a client's request to download a file. It checks the file's existence,
 * prepares the necessary metadata, and sends the file details to the client, followed by the actual file.
 * 
 * msg: the file path requested by the client
 * client: the client requesting the file
 */
void download_file_req(gchar *msg, t_client *client) 
{
    cJSON *j_response = cJSON_CreateObject();  // Create a new JSON object for the response
    GFileInfo *info = NULL;  // File information object
    gchar *j_data = NULL;  // JSON data string for sending to the client
    GFile *file = NULL;  // GFile object representing the file

    if (msg == NULL) 
    {
        return;  // If the file path is NULL, return early
    }

    // Create a GFile object from the given file path
    file = g_file_new_for_path(msg);
    
    // Query file information, including its size and name
    info = g_file_query_info(file, "standard::size,standard::name", G_FILE_QUERY_INFO_NONE, NULL, NULL);
    
    // Check if the file exists and if the path is valid
    if (msg && g_file_query_exists(file, NULL)) 
    {
        // Add response fields to indicate the file's metadata
        cJSON_AddNumberToObject(j_response, "token", RQ_DOWNLOAD_FILE);  // Add the token indicating a file download
        cJSON_AddNumberToObject(j_response, "size", g_file_info_get_size(info));  // Add the file size to the response
        cJSON_AddStringToObject(j_response, "name", msg);  // Add the file name to the response
        
        // Prepare the response message as a JSON string
        j_data = vm_message_calibration(j_response);
        
        // Send the response to the client
        vm_send(client->out, j_data);
        
        // Initiate the download process by passing the client, file, and input stream
        download(client, file, client->in);
        
        // Clean up after sending the response and completing the download
        cJSON_Delete(j_response);  // Free the JSON response object
        g_free(j_data);  // Free the allocated memory for the JSON string
    }
    else 
    {
        // If the file does not exist, close the connection with the client
        g_io_stream_close(G_IO_STREAM(client->conn), NULL, NULL);
    }

    // Unreference the GFile object and free the allocated memory for the file path
    g_object_unref(file);  // Decrement the reference count for the file object
    g_free(msg);  // Free the memory used by the file path string
}
