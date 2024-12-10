#pragma once

#if defined(__APPLE__)
#include <malloc/malloc.h>
#define VM_MALLOC_SIZE(x) malloc_size(x)
#elif defined(_WIN64) || defined(_WIN32)
#include <malloc.h>
#define VM_MALLOC_SIZE(x) _msize(x)
#elif defined(__linux__)
#include <malloc.h>
#define VM_MALLOC_SIZE(x) malloc_usable_size(x)
#define SO_REUSEPORT 15
#endif

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <strings.h>
#include <sys/stat.h>
#include <gio/gio.h>
#include <glib.h>
#include "cJSON.h"
#include "sqlite3.h"
#include "database.h"
#include "vmchat.h"
#include "responses.h"
#include"api.h"
#include"utils.h"
#define MAX_CLIENTS 10
#define BUF_SIZE 2048
#define BACKLOG 10
#define NAME_LEN 32
#define IP_LEN 15
#define MAX_ROOM_LEN 100

#define CACHE "server/cache"
#define VM_REQUEST_PER_SECOND 20
#define VM_DELAY (1000000 / VM_REQUEST_PER_SECOND)

typedef struct s_send_helper {
    GHashTable *table;
    gchar *j_data;
}              t_send_helper;



typedef struct s_file_helper {
    t_client *client;
    guint64 size;
    guint64 room_id;
    gchar *name;
}              t_file_helper;


void init_uchat_daemon(void);
void run_the_server(char *port);
void vm_send_to_all(gchar *j_data, t_client *client, guint64 room_id);
void json_manager(t_client *client);





