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

std::mutex mtx;

class ConnectFourServer {
public:
	ConnectFourServer(int puerto);
	void run();

private:
	int server_fd;
	struct sockaddr_in address;
	int addrlen;

	void handle_cliente(int cliente_socket);
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
	address.sin_port = htons(puerto);
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
		std::cout << "Nuevo cliente conectado" << std::endl;
		std::thread(&ConnectFourServer::handle_cliente, this, cliente_socket).detach();
	}
}

void ConnectFourServer::handle_cliente(int cliente_socket) {
	char tablero[TABLERO_FILAS][TABLERO_COLUMNAS];
	std::cout << "inicializando tablero" << std::endl;
	initialize_tablero(tablero);
	std::cout << "tablero inicializado" << std::endl;
	std::cout << "socket del cliente: " << cliente_socket << std::endl;
	char buffer[1024] = {0};
	int read_size;
	char jugador = 'S';
	std::string tablero_str;
                for (int f=0; f<TABLERO_FILAS; ++f) {
                        for (int c=0; c<TABLERO_COLUMNAS; ++c) {
                                tablero_str += tablero[f][c];
                                tablero_str += ' ';
                        }
                        tablero_str += '\n';
                }
                send(cliente_socket, tablero_str.c_str(), tablero_str.size(), 0);


        while((read_size = read(cliente_socket, buffer, 1024)) > 0) {
		std::cout << "en while handle cliente" << std::endl;
                mtx.lock();
		std::cout << "jugada del cliente recibida: " << buffer << std::endl;
                // inicio jugada del cliente
                int col = buffer[0] - '0'; // se asume que el cliente envía un solo caracter, revisar
                // actualizar tablero
                for (int i = TABLERO_FILAS - 1; i>=0; --i) {
                        if (tablero[i][col] == '-'){
                                tablero[i][col] = 'C';
                                break;
                        }
                }
                if (check_ganador(tablero, 'C')) {
                        send(cliente_socket, "Gana el CLIENTE\n", strlen("Gana el CLIENTE\n"), 0);
                        break;
                } else if (tablero_lleno(tablero)) {
                        send(cliente_socket, "Se declara EMPATE\n", strlen("Se declara EMPATE\n"), 0);
                        break;
                }

                // inicio jugada servidor
		int server_col = rand() % TABLERO_COLUMNAS;
		for (int i = TABLERO_FILAS-1; i>=0; --i) {
			if (tablero[i][server_col] == '-') {
				tablero[i][server_col] = 'S';
				break;
			}
		}
		if (check_ganador(tablero, 'S')) {
			send(cliente_socket, "Gana el SERVIDOR\n", strlen("Gana el SERVIDOR\n"), 0);
			break;
		}

		// enviar tablero al cliente
		std::string tablero_str;
		for (int f=0; f<TABLERO_FILAS; ++f) {
			for (int c=0; c<TABLERO_COLUMNAS; ++c) {
				tablero_str += tablero[f][c];
				tablero_str += ' ';
			}
			tablero_str += '\n';
		}
		send(cliente_socket, tablero_str.c_str(), tablero_str.size(), 0);

		mtx.unlock();
	}
	std::cout << "cliente desconectado" << std::endl;
	close(cliente_socket);
}

void ConnectFourServer::initialize_tablero(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]) {
	for (int f=0; f<TABLERO_FILAS; ++f) {
		for (int c=0; c<TABLERO_COLUMNAS; ++c) {
			tablero[f][c] = '-';
		}
	}
}

bool ConnectFourServer::check_ganador(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS], char jugador) {
	for (int f=0; f<TABLERO_FILAS;++f){
		for (int c=0; c<TABLERO_COLUMNAS; ++c){
			// horizontal
			if (c<4) {
				if (tablero[f][c] == jugador && tablero[f][c+1] == jugador && tablero[f][c+2] == jugador && tablero[f][c+3] == jugador) {
					return true;
				}
			}
			// vertical
			if (f<3) {
				if(tablero[f][c] == jugador && tablero[f+1][c] == jugador && tablero[f+2][c] == jugador && tablero[f+3][c] == jugador) {
					return true;
				}
			}
			// diagonal hacia arriba
			if (f<3 && c<4) {
				if(tablero[f][c] == jugador && tablero[f+1][c+1] == jugador && tablero[f+2][c+2] == jugador && tablero[f+3][c+3] == jugador) {
					return true;
				}
			}
			// diagonal hacia abajo
			if (f>=3 && c<4) {
				if(tablero[f][c] == jugador && tablero[f-1][c+1] == jugador && tablero[f-2][c+2] == jugador && tablero[f-3][c+3] == jugador){
					return true;
				}
			}
		}
	}
	return false;
}

bool ConnectFourServer::tablero_lleno(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]) {
	for (int f=0; f<TABLERO_FILAS; ++f) {
		for (int c=0; c<TABLERO_COLUMNAS; ++c) {
			if(tablero[f][c] == '-') return false;
		}
	}
	return true;
}

void ConnectFourServer::imprimir_tablero(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]) {
	for (int f=0; f<TABLERO_FILAS; ++f) {
		for (int c=0; c<TABLERO_COLUMNAS; ++c) {
			std::cout << tablero[f][c] << '-';
		}
		std::cout << std::endl;
	}
}

int main() {
	ConnectFourServer server(PORT);
	server.run();
	return 0;
}

