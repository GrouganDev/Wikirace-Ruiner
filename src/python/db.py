import sqlite3
import os
import sys

if __name__ == "__main__":
    conn = sqlite3.connect("src/python/my_database.db")
    cursor = conn.cursor()

    name = sys.argv[1]

    conn.executescript('''
    PRAGMA journal_mode = WAL
    ''')

    cursor.execute("SELECT link FROM articles WHERE name = ?", (name,))

    for row in cursor:
        name2 = row[0]
        print(name2)

    conn.commit()
    conn.close()