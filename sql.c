#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *DB_FILE = "students.sqlite3";

sqlite3 *db_connect() {
    sqlite3 *db;
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    return db;
}

void add_student(sqlite3 *db) {
    int id, grade;
    char name[50], department[50];
    printf("Enter student id: ");
    scanf("%d", &id);
    printf("Enter student name: ");
    scanf("%s", name);
    printf("Enter grade: ");
    scanf("%d", &grade);
    printf("Enter department: ");
    scanf("%s", department);

    char *sql = sqlite3_mprintf(
        "insert into students (id, name, grade, department) "
        "values (%d, '%q', %d, '%q');",
        id, name, grade, department);

    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Student addition error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Student added successfully\n");
    }
    sqlite3_free(sql);
}

void search_student(sqlite3 *db) {
    int id;
    printf("Enter student id: ");
    scanf("%d", &id);

    char *sql = sqlite3_mprintf("SELECT * FROM students WHERE id=%d;", id);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to search student: %s\n", sqlite3_errmsg(db));
    } else {
        if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            printf("ID: %d, Name: %s, Grade: %d, Department: %s\n",
                   sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
                   sqlite3_column_int(stmt, 2), sqlite3_column_text(stmt, 3));
        } else {
            printf("Student not found\n");
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_free(sql);
}

void display_all_students(sqlite3 *db) {
    char *sql = "SELECT * FROM students;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to show all students: %s\n",
                sqlite3_errmsg(db));
    } else {
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            printf("ID: %d, Name: %s, Grade: %d, Department: %s\n",
                   sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
                   sqlite3_column_int(stmt, 2), sqlite3_column_text(stmt, 3));
        }
        sqlite3_finalize(stmt);
    }
}

int main(void) {
    sqlite3 *db = db_connect();

    int choice;
    do {
        printf("This is student database control\n");
        printf("1. Add student\n");
        printf("2. Search a student\n");
        printf("3. Display all students\n");
        printf("0. Exit\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_student(db);
                break;
            case 2:
                search_student(db);
                break;
            case 3:
                display_all_students(db);
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice\n");
        }
    } while (choice != 0);

    sqlite3_close(db);
    return 0;
}
