#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#define PORT 7777
#define TABLERO_FILAS 6
#define TABLERO_COLUMNAS 7

std: :mutex mtx;

class ConnectFourServer {
public:
	ConnectFourServer(int puerto);
	void run();

private:
	int server_fd;
	struct sockaddr_in adress;
	int addrlen;

	void handle_cliente(int client_socket);
	void initialize_tablero(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]);
	bool check_ganador(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS], char jugador);
	bool tablero_lleno(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]);
	void imprimir_tablero(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]);
};

ConnectFourServer::ConnectFourServer(int puerto) {
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0) {
		perror("ConnectFourServer(int puerto) -> fallo en Socket");
		exit (EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	adress.sin_port = htons(puerto);
	addrlen = sizeof(address);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("ConnectFourServer(int puerto) -> fallo en Bind");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("ConnectFoyrServer(int puerto) -> fallo en Listen");
		exit(EXIT_FAILURE);
	}
};

void ConnectFourServer::run() {
	std::cout << "Servidor escuchando en el puerto " << PORT << std::endl;
	while (true) {
		int cliente_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
		if (cliente_socket <0) {
			perror("ConnectFourServer::run() -> Fallo Accept");
			continue;
		}
		std::thread(&ConnectFourServer::handle_cliente, this, cliente_socket).detach();
	}
}

void ConnectFourServer::handle_cliente(int cliente_socket) {
	char tablero[TABLERO_FILAS][TABLERO_COLUMNAS];
	initialize_tablero(tablero);

	char buffer[1024] = {0};
	int read_size;
}
