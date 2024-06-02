#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void jugar(int sock);

int main(int argc, char const *argv[]) {
	if (argc != 3) {
		std::cerr << "Uso: "<< argv[0] << "<IP_SERVIDOR> <PUERTO>" << std::endl;
		return -1;
	}

	const char *server_ip =argv[1];
	int port = std::stoi(argv[2]);

	int sock = 0;
	struct sockaddr_in serv_addr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <0){
		std::cerr << "cliente.cpp main -> Error al crear el socket" << std::endl;
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <=0){
		std::cerr << "cliente.cpp main -> Direccion ip no valida" << std::endl;
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		std::cerr << "cliente.cpp main -> Error de conexion" << std::endl;
		return -1;
	}

	jugar(sock);

	close(sock);
	return 0;
}

void jugar(int sock) {
	char buffer[1024] = {0};
	while (true){
		int valread = read(sock, buffer, 1024);
		if(valread<=0){
			break;
		}

		system("clear");
		std::cout << buffer << std::endl;
		std::string input;
		if(strstr(buffer, "G") != nullptr){
			std::cout << "[Has sido desconectado del Servidor]" << std::endl;
			break;
		}
		std::cout << "Introduzca columna(0-6): ";
		std::cin >> input;
		send(sock, input.c_str(), input.length(), 0);
	}
}
