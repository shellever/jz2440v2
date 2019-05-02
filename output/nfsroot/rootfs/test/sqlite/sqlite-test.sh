#!/bin/sh

SQLITE_TEST=sqlite-test.out
SQLITE_DB=test-out.db
./$SQLITE_TEST $SQLITE_DB "create table person(name varchar(20), age int);"
./$SQLITE_TEST $SQLITE_DB "insert into person values('shellever', 18);"
./$SQLITE_TEST $SQLITE_DB "insert into person values('linuxfor', 20);"
./$SQLITE_TEST $SQLITE_DB "select * from person;"
