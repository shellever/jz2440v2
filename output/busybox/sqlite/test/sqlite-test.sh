#!/bin/sh


SQLITE_DB=test-out.db

rm -rf $SQLITE_DB

./sqlite-test.out $SQLITE_DB "create table person(name varchar(20), age int);"
./sqlite-test.out $SQLITE_DB "insert into person values('shellever', 18);"
./sqlite-test.out $SQLITE_DB "insert into person values('linuxfor', 20);"
./sqlite-test.out $SQLITE_DB "select * from person;"
