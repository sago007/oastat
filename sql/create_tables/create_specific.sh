cat oastat_create_tables_common.sql | grep -v "RESTRICT2MYSQL" | sed -e 's/SERIAL/BIGSERIAL/g' -e 's/DATETIME/TIMESTAMP/g' > oastat_create_tables_posgrel.sql
cat oastat_create_tables_common.sql | grep -v "RESTRICT2POSTGRESQL" | sed -e 's/PRIMARY KEY//g'  > oastat_create_tables_mysql.sql

