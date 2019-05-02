#include <stdio.h>
#include <sqlite3.h>


static int callback(void *notUsed, int argc, char **argv, char **azColName)
{
    int i;

    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");

    return 0;
}


int main(int argc, char *argv[])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    const char *dbfile;
    const char *sql;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <database> <sql-statement>\n", argv[0]);
        exit(1);
    }

    dbfile = argv[1];
    sql = argv[2];

    rc = sqlite3_open(dbfile, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);

    return 0;
}


