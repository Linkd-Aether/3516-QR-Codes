all: QRServer QRClient
QRServer: QRServer.cpp
	g++ -o QRServer QRServer.cpp
QRClient: QRClient.cpp
	g++ -o QRClient QRClient.cpp
