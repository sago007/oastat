# Introduction

Here you will be introduced on how to get data into oastat and how to get the data out again. Oastat can read both from file or pipe and can write output to either an SQL server or an XML file.

Homepage: <https://github.com/sago007/oastat/>

## Building

### Dependencies

* CppDb - <http://cppcms.com/sql/cppdb/>
* libgcrypt
* boost_program_options

On Ubuntu (15.04 or later) all requirements can be installed with: sudo apt-get install build-essential libboost-program-options-dev libcppdb-dev libgcrypt20-dev
On earlier versions CppDb must be installed manually.

CppDb can be disabled from the build by adding the line:
USECPPDB=0 to src/Makefile.local (file must be created)

Go to ./src and type "make"
The executable will be in the same folder.

To create the database scripts:
cd sql/create_tables/
./create_specific.sh

This will create the scripts for PostgreSQL, MySQL and SQLite

## Details

### Writing to XML-files

The general arguments are like this:

`oastat -f FILENAME --backend BACKEND --dbarg ARGUMENTS`

* FILENAME is the file to be processed. If this is blank (like "") stdin will be read instead.
* BACKEND is the name of the backend, in this case "Xml"
* ARGUMENTS is the argument list to the backend in case of Xml-backend the possible arguments are outputdir and possible a post process script.

Running on a file:

`./oastat -f "~/.openarena/baseoa/games.log" --backend "Xml" --dbarg "outputdir ~/oastat"`

Reading from stdin:

`tail -f "~/.openarena/baseoa/games.log" | ./oastat -f "" --backend "Xml" --dbarg "outputdir ~/oastat"`

This will generate XML-files in the choosen output dir in this case *~/oastat*

### Writing to database
The database backend is called CppDb. Writing to a MySQL server is done like this:

`oastat --backend "CppDb" --dbarg "mysql:database=oastat;user=<USERNAME>;password=<PASSWORD>"`

See <http://cppcms.com/sql/cppdb/connstr.html> for details on how to create the connection string.

## Windows version
A precompiled version for Windows can be found at: <https://files.poulsander.com/~poul19/public_files/oastat/>
This version is compiled on a Debian system using MXE (<http://mxe.cc/>).

The windows version only has support for XMl output and sqlite. The reason it does not support mySQL and PostgreSQL is because I was not able to compile a version of cppdb that supported it.

The Windows version includes two scripts:
* oastat_create_db.bat - Creates a sqlite3-database
* oastat_fill_db.bat - Fills the database with data from "%APPDATA%/OpenArena/baseoa/games.log"

The windows version cannot read from stdin or use the "--tail" command but it does demonstrate the duplicate check.  

For looking at the sqlite-file generated I recommend Sqliteman (<http://sqliteman.com/>)

## License
The program is licences under GNU GPL v2 or any later version. See COPYING for details.
The Windows version may include files from CPPDB (<http://cppcms.com/sql/cppdb/>) that are licensed under the Boost Software License 1.0 or The MIT license at your opinion.
The windows version may also be packed with sqlite3.exe that are in the public domain.
