#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Address {
    int id;
    int set;
    char *name;
    char *email;
};

struct Database {
    int max_data;
    int max_rows;
    struct Address *rows;
};

struct Connection {
    FILE *file;
    struct Database *db;
};
struct Connection *conn = NULL;

void die(const char *message)
{
    if(conn) {
        if(conn->db) {
            if(conn->db->rows) {
                for(int i = 0; i < conn->db->max_rows; i++) {
                    free(conn->db->rows[i].name);
                    free(conn->db->rows[i].email);
                }
                free(conn->db->rows);
            }
            free(conn->db);
        }
        if(conn->file) fclose(conn->file);
        free(conn);
    }
    if(errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }
    exit(1);
}

void Address_print(struct Address *addr)
{
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load()
{
    fread(&conn->db->max_data, sizeof(int), 1, conn->file);
    fread(&conn->db->max_rows, sizeof(int), 1, conn->file);

    conn->db->rows = malloc(sizeof(struct Address) * conn->db->max_rows);
    if(!conn->db->rows) die("Memory error");

    for(int i = 0; i < conn->db->max_rows; i++) {
        conn->db->rows[i].name = malloc(conn->db->max_data);
        conn->db->rows[i].email = malloc(conn->db->max_data);
        if(!conn->db->rows[i].name || !conn->db->rows[i].email) die("Memory error");

        fread(&conn->db->rows[i].id, sizeof(int), 1, conn->file);
        fread(&conn->db->rows[i].set, sizeof(int), 1, conn->file);
        fread(conn->db->rows[i].name, conn->db->max_data, 1, conn->file);
        fread(conn->db->rows[i].email, conn->db->max_data, 1, conn->file);
    }
}
void Database_find(const char *query)
{
    for (int i = 0; i < conn->db->max_rows; i++) {
        struct Address *cur = &conn->db->rows[i];

        if (cur->set && (strstr(cur->name, query) || strstr(cur->email, query))) {
            Address_print(cur);
        }
    }
}
struct Connection *Database_open(const char *filename, char mode)
{
    conn = malloc(sizeof(struct Connection));
    if(!conn) die("Memory error");

    conn->db = malloc(sizeof(struct Database));
    if(!conn->db) die("Memory error");

    if(mode == 'c') {
        conn->file = fopen(filename, "w");
    } else {
        conn->file = fopen(filename, "r+");

        if(conn->file) {
            Database_load(conn);
        }
    }

    if(!conn->file) die("Failed to open the file");

    return conn;
}

void Database_close()
{
    if(conn) {
        if(conn->file) fclose(conn->file);
        if(conn->db) {
            if(conn->db->rows) {
                for(int i = 0; i < conn->db->max_rows; i++) {
                    free(conn->db->rows[i].name);
                    free(conn->db->rows[i].email);
                }
                free(conn->db->rows);
            }
            free(conn->db);
        }
        free(conn);
    }
}

void Database_write()
{
    rewind(conn->file);

    fwrite(&conn->db->max_data, sizeof(int), 1, conn->file);
    fwrite(&conn->db->max_rows, sizeof(int), 1, conn->file);

    for(int i = 0; i < conn->db->max_rows; i++) {
        fwrite(&conn->db->rows[i].id, sizeof(int), 1, conn->file);
        fwrite(&conn->db->rows[i].set, sizeof(int), 1, conn->file);
        fwrite(conn->db->rows[i].name, conn->db->max_data, 1, conn->file);
        fwrite(conn->db->rows[i].email, conn->db->max_data, 1, conn->file);
    }

    int rc = fflush(conn->file);
    if(rc == -1) die("Cannot flush database.");
}

void Database_create(int max_data, int max_rows)
{
    conn->db->max_data = max_data;
    conn->db->max_rows = max_rows;
    conn->db->rows = malloc(sizeof(struct Address) * max_rows);
    if(!conn->db->rows) die("Memory error");

    for(int i = 0; i < max_rows; i++) {
        conn->db->rows[i].id = i;
        conn->db->rows[i].set = 0;
        conn->db->rows[i].name = malloc(max_data);
        conn->db->rows[i].email = malloc(max_data);
        if(!conn->db->rows[i].name || !conn->db->rows[i].email) die("Memory error");
    }
}

void Database_set(int id, const char *name, const char *email)
{
    struct Address *addr = &conn->db->rows[id];
    if(addr->set) die("Already set, delete it first");

    addr->set = 1;
    char *res = strncpy(addr->name, name, conn->db->max_data);
    if(!res) die("Name copy failed");

    res = strncpy(addr->email, email, conn->db->max_data);
    if(!res) die( "Email copy failed");
}

void Database_get(int id)
{
    struct Address *addr = &conn->db->rows[id];

    if(addr->set) {
        Address_print(addr);
    } else {
        die("ID is not set");
    }
}

void Database_delete(int id)
{
    struct Address addr = {.id = id, .set = 0};
    free(conn->db->rows[id].name);
    free(conn->db->rows[id].email);
    conn->db->rows[id] = addr;
    conn->db->rows[id].name = malloc(conn->db->max_data);
    conn->db->rows[id].email = malloc(conn->db->max_data);
    if(!conn->db->rows[id].name || !conn->db->rows[id].email) die("Memory error");
}

void Database_list()
{
    for(int i = 0; i < conn->db->max_rows; i++) {
        struct Address *cur = &conn->db->rows[i];

        if(cur->set) {
            Address_print(cur);
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 3) die("USAGE: ex17 <dbfile> <action> [action params]");

    char *filename = argv[1];
    char action = argv[2][0];
    conn = Database_open(filename, action);
    if(conn==NULL) die("Database_open failed");
    int id = 0;

     if(argc > 3) id = atoi(argv[3]);
     

    switch(action) {
        case 'c':
            if(argc != 5) die("Need max_data and max_rows to create");

            int max_data = atoi(argv[3]);
            int max_rows = atoi(argv[4]);
            Database_create(max_data, max_rows);
            Database_write(conn);
            break;

        case 'g':
            if(argc != 4) die("Need an id to get");
            if(id >= conn->db->max_rows) die("There's not that many records.");
            Database_get(id);
            break;

        case 's':
            if(argc != 6) die("Need id, name, email to set");
            if(id >= conn->db->max_rows) die("There's not that many records.");
            Database_set(id, argv[4], argv[5]);
            Database_write(conn);
            break;

        case 'd':
            if(argc != 4) die("Need id to delete");
            if(id >= conn->db->max_rows) die( "There's not that many records.");
            Database_delete(id);
            Database_write();
            break;
            
        case 'f':
            if (argc != 4) die("Need a query to find");
            Database_find(argv[3]);
            break;

        case 'l':
            Database_list();
            break;
        default:
            die("Invalid action, only: c=create, g=get, s=set, d=del, l=list");
    }

    Database_close();

    return 0;
}