#ifndef _SQLITE3_DB_HPP_
#define _SQLITE3_DB_HPP_

#include <sqlite3.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "sqliteCallback.hpp"
#define dbPath "/home/smallfish/networkProgramming/Project/nphw3_build/bbsServerDatabase.db"
//#define sqlSize 999
#define ifPrintSql true
using namespace std;
/* This .hpp is for helping you to do something about sqlite */
// If you get stmt error, the reason may be that your sql query isn't correct.

bool sqlite3_if_repeat(sqlite3 *db, string sqlStr)
/* return true means there exists "description" in the column name "attributeName" in "tableName" table */
{
        char *sql = new char[(unsigned)sqlStr.size() + 1]; // store the sqlite3 cmd
        strcpy(sql, sqlStr.c_str());
        char *errMsg = NULL;
        sqlite3_stmt *stmt;

        if (ifPrintSql) // use "ifPrintSql" button to decide whether print sql info on the server's screen
                std::cout << sql << std::endl;
        if (sqlite3_prepare(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        {
                fprintf(stderr, "Error when preparing statement: %s\a\n", errMsg);
                sqlite3_free(errMsg);
                exit(1);
        }
        int count = 0;
        while (sqlite3_step(stmt) != SQLITE_DONE)
                count++;
        sqlite3_finalize(stmt);
        delete sql;
        return (count > 0) ? true : false;
}

sqlite3 *sqlite3_openDB()
{
        sqlite3 *db;
        if (sqlite3_open(dbPath, &db))
        {
                fprintf(stderr, "Opening-database error: %s\a\n", sqlite3_errmsg(db));
                exit(1);
        }
        return db;
}

void sqlite3_insert(sqlite3 *db, string sqlStr)
{
        char *sql = new char[(unsigned)sqlStr.size() + 1]; // store the sqlite3 cmd
        strcpy(sql, sqlStr.c_str());
        char *errMsg = NULL;
        sqlite3_stmt *stmt;

        if (ifPrintSql) // use "ifPrintSql" button to decide whether print sql info on the server's screen
                std::cout << sql << std::endl;

        // int ifSuccuess = sqlite3_prepare(db, sql, -1, &stmt, NULL);
        // if (ifSuccuess != SQLITE_OK)
        // {
        //         fprintf(stderr, "Error when preparing statement: %s\a\n", errMsg);
        //         sqlite3_free(errMsg);
        //         exit(1);
        // }
        // sqlite3_finalize(stmt);

        char *data;
        bool ifSuccess;
        ifSuccess = sqlite3_exec(db, sql, insertCallback, (void *)data, &errMsg);
        // if executed successfully
        if (ifSuccess != SQLITE_OK)
        {
                fprintf(stderr, "Error on inserting into database table: %s\a\n", errMsg);
                sqlite3_free(errMsg);
                exit(1);
        }

        delete sql;
        return;
}

string sqlite3_get_oneEntity(sqlite3 *db, string sqlStr)
{
        char *sql = new char[(unsigned)sqlStr.size() + 1]; // store the sqlite3 cmd
        strcpy(sql, sqlStr.c_str());
        char *errMsg = NULL;
        sqlite3_stmt *stmt;
        if (ifPrintSql) // use "ifPrintSql" button to decide whether print sql info on the server's screen
                std::cout << sql << std::endl;
        if (sqlite3_prepare(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        {
                fprintf(stderr, "Error when preparing statement: %s\a\n", errMsg);
                sqlite3_free(errMsg);
                exit(1);
        }
        string result;
        while (sqlite3_step(stmt) != SQLITE_DONE)
                result = string((char *)sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        delete sql;
        return result;
}

string sqlite3_get_largestID(sqlite3 *db, string sqlStr)
{
        char *sql = new char[(unsigned)sqlStr.size() + 1]; // store the sqlite3 cmd
        strcpy(sql, sqlStr.c_str());
        char *errMsg = NULL;
        sqlite3_stmt *stmt;
        if (ifPrintSql) // use "ifPrintSql" button to decide whether print sql info on the server's screen
                std::cout << sql << std::endl;
        if (sqlite3_prepare(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        {
                fprintf(stderr, "Error when preparing statement: %s\a\n", errMsg);
                sqlite3_free(errMsg);
                exit(1);
        }
        string largestInteger = "-1";
        while (sqlite3_step(stmt) != SQLITE_DONE)
        {
                string temp = string((char *)sqlite3_column_text(stmt, 0));
                if (stoi(largestInteger) < stoi(temp))
                        largestInteger = temp;
        }
        sqlite3_finalize(stmt);
        delete sql;
        return largestInteger;
}

void sqlite3_NoRetrurn(sqlite3 *db, string sqlStr)
{
        char *sql = new char[(unsigned)sqlStr.size() + 1]; // store the sqlite3 cmd
        strcpy(sql, sqlStr.c_str());
        char *errMsg = NULL;
        sqlite3_stmt *stmt;
        if (ifPrintSql) // use "ifPrintSql" button to decide whether print sql info on the server's screen
                std::cout << sql << std::endl;
        if (sqlite3_prepare(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        {
                fprintf(stderr, "Error when preparing statement: %s\a\n", errMsg);
                sqlite3_free(errMsg);
                exit(1);
        }
        while (sqlite3_step(stmt) != SQLITE_DONE)
        {
        }
        sqlite3_finalize(stmt);
        delete sql;
}

void sqlite3_delete(sqlite3 *db, string sqlStr)
{
        sqlite3_NoRetrurn(db, sqlStr);
        return;
}

/* template
string sqlite3_ (sqlite3* db, string sqlStr)
{
        char *sql = new char[(unsigned)sqlStr.size() + 1]; // store the sqlite3 cmd
        strcpy(sql, sqlStr.c_str());
        char *errMsg = NULL;
        sqlite3_stmt *stmt;
        if (ifPrintSql) // use "ifPrintSql" button to decide whether print sql info on the server's screen
                std::cout << sql << std::endl;
        if (sqlite3_prepare(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        {
                fprintf(stderr, "Error when preparing statement: %s\a\n", errMsg);
                sqlite3_free(errMsg);
                exit(1);
        }
        while (sqlite3_step(stmt) != SQLITE_DONE){}
        sqlite3_finalize(stmt);
        delete sql;
}
*/

/* example code */
/*
int main(int argc, char *argv[])
{
        sqlite3 *db = sqlite3_openDB();

        bool result = sqlite3_if_repeat(db, string("select * from USERS where Username = 'wcl'"));
        cout << (result ? "yes" : "no") << endl;

        sqlite3_insert(db, string("insert into USERS (Username,Email,Password) values('0520test1','0','0' );"));

        cout << sqlite3_get_OneEntity(db, string("select Email from USERS where Username = 'adflk';")) << endl;

        cout << sqlite3_get_largestID(db, string("select UID from USERS")) << endl;

        sqlite3_close(db);
}
*/

#endif