# Introduction #

Here you will be introduced on how to get data into oastat and how to get the data out again. Oastat can read both from file or pipe and can write output to either an SQL server or an XML file.


# Details #

## Writing to XML-files ##

The general arguments are like this:
```
oastat -f FILENAME --backend BACKEND --dbarg ARGUMENTS
```
  * FILENAME is the file to be processed. If this is blank (like "") stdin will be read instead.
  * BACKEND is the name of the backend, in this case "Xml"
  * ARGUMENTS is the argument list to the backend in case of Xml-backend the possible arguments are outputdir and possible a post process script.

Running on a file:
```
./oastat -f "~/.openarena/baseoa/games.log" --backend "Xml" --dbarg "outputdir ~/oastat"
```

Reading from stdin:
```
tail -f "~/.openarena/baseoa/games.log" | ./oastat -f "" --backend "Xml" --dbarg "outputdir ~/oastat"
```

This will generate XML-files in the choosen output dir in this case **~/oastat**

## Writing to database ##
Writing to a postgreSQL database is very simple, at least if the database has been setup on the local machine with unix socket authentication.

```
oastat -f "~/.openarena/baseoa/games.log" --backend "DbiXX" --dbarg "pgsql dbname oastat username openarena"
```

Note that BACKEND is DbiXX which is the libary oastat uses for DB-access. Instead it is the first argument of the ARGUMENT list that decides what database-driver to use. The rest of ARGUMENT is the parameters with key followed by value so in the example the parameter "dbname" will be set to "oastat" and the parameter "username" will be set to "openarena"

Writing to a mySQL database is easy too:

```
oastat -f "~/.openarena/baseoa/games.log" --backend "DbiXX" --dbarg "mysql dbname oastat username openarena"
```