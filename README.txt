To compile main_server.cpp (on windows), run:

g++ -o main_server.exe main_server.cpp -lws2_32 -lmswsock

To compile main_engine.cpp (on windows), run:

g++ -o main_engine.exe main_engine.cpp -lws2_32 -lmswsock

Note: This does not implement (at least as far as I'm aware) compiler optimmizations such as -O2 yet.