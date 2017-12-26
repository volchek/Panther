set PATH_TO_ASIO="F:/WF/task1.1/asio-1.10.6/include"
set PATH_TO_NLOHMANN="F:/WF/task1.1"

g++ -D ASIO_STANDALONE -I %PATH_TO_ASIO% -I %PATH_TO_NLOHMANN% -D_WIN32_WINNT=0x0501 -std=gnu++11 -c Client.cpp Parser.cpp Player.cpp Train.cpp Strategy.cpp GameUnit.cpp Town.cpp Market.cpp Settlement.cpp GameState.cpp Graph.cpp Units.cpp Logger.cpp main.cpp
g++ Client.o Parser.o Player.o Train.o Strategy.o GameUnit.o Town.o Market.o Settlement.o GameState.o Graph.o Units.o Logger.o main.o -lws2_32 -o res

pause