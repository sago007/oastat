#! /bin/bash
cat oastat_create_tables_common.sql | grep -v "RESTRICT2MYSQL" | sed -e 's/SERIAL/BIGSERIAL/g' -e 's/DATETIME/TIMESTAMP/g' -e 's/unsigned//g' > oastat_create_tables_posgrel.sql
cat oastat_create_tables_common.sql | grep -v "RESTRICT2POSTGRESQL" | sed -e 's/PRIMARY KEY//g' -e 's/unique//g'  > oastat_create_tables_mysql.sql
cat oastat_create_tables_common.sql | grep -v "ALTER TABLE" | grep -v "ADD CONSTRAINT" | grep -v "RESTRICT2MYSQL" | grep -v "RESTRICT2POSTGRESQL" | sed -e 's/SERIAL/INTEGER/g' -e "s/now()/date('now')/g" -e 's/unique/PRIMARY KEY/g' > oastat_create_tables_sqlite.sql
