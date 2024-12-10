#include "server.h"

// Function to close the database connection
// Takes a pointer to an open sqlite3 database as an argument
void close_db(sqlite3 *db) 
{
    gint32 rv = sqlite3_close(db);  // Attempt to close the database connection

    // Check if the closing operation was successful
    if (rv != SQLITE_OK) 
    {
        // Log an error message if the database couldn't be closed properly
        vm_logger(VM_LOG_FILE, "close database");
    }
}
