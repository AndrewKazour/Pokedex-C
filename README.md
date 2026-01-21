# Pokedex-C
My Pokedex project written in C

Dependencies: 

1. To get all 151 pokemon there is a python script written that is run once. It needs the requests dependency:
pip install requests

2. Make or MinGW installed

**Build and Run**

To build and run the program:

mingw32-make run

To Load Web UI: http://localhost:8080/


**For resetting all or specific pokemon:**

All: .\pokedex_server.exe --reset
Specific: .\pokedex_server.exe --reset-id <id>

Help (Shows these commands): .\pokedex_server.exe --help 