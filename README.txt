To compile main_server.cpp (on windows), run:

g++ -o main_server.exe main_server.cpp -lws2_32 -lmswsock

Mac:

g++ -std=c++17 -o main_server main_server.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lboost_system


To compile main_engine.cpp (on windows), run:

g++ -o main_engine.exe main_engine.cpp -lws2_32 -lmswsock

Mac:

g++ -std=c++17 -o main_engine main_engine.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lboost_system


Note: This does not implement (at least as far as I'm aware) compiler optimizations such as -O2 yet.
