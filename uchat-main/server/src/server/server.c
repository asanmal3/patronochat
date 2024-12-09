#include "server.h"

// Callback function to handle an incoming request asynchronously
// This function is called when a line is read from the client's input stream.
static void handle_request(GObject *source_object, GAsyncResult *res, gpointer user_data) 
{
    // Cast the source object to GDataInputStream for reading data
    GDataInputStream *in = G_DATA_INPUT_STREAM(source_object);
    gsize count = 0;  // Variable to store the number of bytes read
    t_client *client = (t_client*)user_data;  // Cast the user_data to t_client

    // Check if the client is still connected; if not, deinitialize and return
    if (vm_is_connected(client->conn, client->out, client->in) == false) 
    {
        deinit_client(&client);  // Clean up resources for the client
        return;
    }

    // Read a line asynchronously from the input stream
    client->request = g_data_input_stream_read_line_finish(in, res, &count, NULL);
    if(client->request) 
    {
        // Process the request by passing it to the JSON manager
        json_manager(client);
    }

    // Continue reading the next line asynchronously
    g_data_input_stream_read_line_async(in, G_PRIORITY_DEFAULT, NULL, handle_request, client);
}


// Callback function that is triggered when a new client connection is made
// This function sets up the client's input/output streams and initiates asynchronous reading
static gboolean incoming(GSocketService *service,
                         GSocketConnection *conn,
                         GObject *source_object,
                         gpointer arg_data) 
{
    // Get the output and input streams from the connection
    GOutputStream *out_s = g_io_stream_get_output_stream(G_IO_STREAM(conn));
    GInputStream *in_s = g_io_stream_get_input_stream(G_IO_STREAM(conn));

    // Wrap the output and input streams into data streams
    GDataOutputStream *out = g_data_output_stream_new(out_s);
    GDataInputStream *in = g_data_input_stream_new(in_s);

    // Allocate memory for the client structure
    t_client *client = vm_malloc(sizeof(t_client));

    // Initialize the client's information and streams
    client->out = out;
    client->info = (t_info*)arg_data;  // Cast the argument data to t_info
    client->conn = g_object_ref(conn);  // Reference the connection object
    client->in = in;
    client->in_s = in_s;

    // Start reading the client's input stream asynchronously
    g_data_input_stream_read_line_async(in, G_PRIORITY_DEFAULT, NULL, handle_request, client);

    // Unused parameters
    (void)source_object;
    (void)service;
    return false;  // Returning false to indicate that we don't need further handling
}


// Function to validate the command-line arguments
// It checks if the arguments are correct, including the port number
static void valid_argv(gint argc, char *port) 
{
    // Check if the number of arguments is correct and the port is valid
    if (argc != 2 || strlen(port) != 4 || !atoi(port)) 
    {
        exit(1);  // Exit if the arguments are invalid
    }
}

gint main(gint argc, char **argv) 
{
    GSocketService *service = g_socket_service_new();  // Create a new socket service
    GMainLoop *loop = NULL;  // Declare the main event loop
    t_info *info = NULL;  // Declare a pointer to the server's info structure
    gint64 port = -1;  // Declare a variable for the port number

    // Validate the command-line arguments
    valid_argv(argc, argv[1]);

    // Change the working directory to the specified CACHE directory
    vm_change_working_dir(CACHE);

    // Initialize the server as a daemon
    init_uchat_daemon();

    // Initialize the server's information (user management, database)
    info = init_info();

    // Convert the port string to an integer
    port = g_ascii_strtoll(argv[1], NULL, 10);

    // Attempt to bind the server to the specified port
    if (!g_socket_listener_add_inet_port((GSocketListener*)service, port, NULL, NULL)) 
    {
        g_printerr("invalid port\n");  // Print an error if the port is invalid
        return -1;  // Return error code
    }

    // Connect the incoming signal to the incoming callback (when a new client connects)
    g_signal_connect(service, "incoming", G_CALLBACK(incoming), info);

    // Start the socket service to listen for incoming connections
    g_socket_service_start(service);

    // Log the start of the server
    vm_logger(VM_LOG_FILE, "uchat_server started");

    // Create and run the main event loop to handle incoming events
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);  // Start the event loop, blocking further code execution

    return 0;  // Return success
}
