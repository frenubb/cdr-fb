#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

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
    std::vector<std::thread> client_threads;

    void handle_cliente(int cliente_socket, const char* cliente_ip, int cliente_port);
    void initialize_tablero(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]);
    bool check_ganador(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS], char jugador);
    bool tablero_lleno(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]);
    void imprimir_tablero(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]);
};

ConnectFourServer::ConnectFourServer(int puerto) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("ConnectFourServer(int puerto) -> fallo en Socket");
        exit(EXIT_FAILURE);
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
        perror("ConnectFourServer(int puerto) -> fallo en Listen");
        exit(EXIT_FAILURE);
    }
}

void ConnectFourServer::run() {
    std::cout << "Esperando conexiones..." << std::endl;
    while (true) {
        struct sockaddr_in cliente_address;
        socklen_t cliente_addrlen = sizeof(cliente_address);
        int cliente_socket = accept(server_fd, (struct sockaddr *)&cliente_address, &cliente_addrlen);
        if (cliente_socket < 0) {
            perror("ConnectFourServer::run() -> Fallo Accept");
            continue;
        }
        char cliente_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cliente_address.sin_addr, cliente_ip, INET_ADDRSTRLEN);
        int cliente_port = ntohs(cliente_address.sin_port);
        std::cout << "Juego nuevo [" << cliente_ip << ":" << cliente_port << "]" << std::endl;
        client_threads.emplace_back(&ConnectFourServer::handle_cliente, this, cliente_socket, cliente_ip, cliente_port);
    }

    // Esperar a que todos los hilos terminen antes de cerrar el servidor
    for (auto& thread : client_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ConnectFourServer::handle_cliente(int cliente_socket, const char* cliente_ip, int cliente_port) {
    char tablero[TABLERO_FILAS][TABLERO_COLUMNAS];
    initialize_tablero(tablero);

    char buffer[1024] = {0};
    int read_size;
    char jugador = 'S';
    std::string tablero_str;
    for (int f = 0; f < TABLERO_FILAS; ++f) {
        for (int c = 0; c < TABLERO_COLUMNAS; ++c) {
            tablero_str += tablero[f][c];
            tablero_str += ' ';
        }
        tablero_str += '\n';
    }
    send(cliente_socket, tablero_str.c_str(), tablero_str.size(), 0);

    while ((read_size = read(cliente_socket, buffer, 1024)) > 0) {
        mtx.lock();
        std::cout << "[" << cliente_ip << ":" << cliente_port << "] cliente juega columna " << buffer << std::endl;
        int col = buffer[0] - '0'; // se asume que el cliente envía un solo caracter, revisar

        for (int i = TABLERO_FILAS - 1; i >= 0; --i) {
            if (tablero[i][col] == '-') {
                tablero[i][col] = 'C';
                break;
            }
        }

        if (check_ganador(tablero, 'C')) {
            std::string tablero_str;
            for (int f = 0; f < TABLERO_FILAS; ++f) {
                for (int c = 0; c < TABLERO_COLUMNAS; ++c) {
                    tablero_str += tablero[f][c];
                    tablero_str += ' ';
                }
                tablero_str += '\n';
            }
            send(cliente_socket, tablero_str.c_str(), tablero_str.size(), 0);
            send(cliente_socket, "Gana el CLIENTE\n", strlen("Gana el CLIENTE\n"), 0);
            mtx.unlock();
            break;
        } else if (tablero_lleno(tablero)) {
            std::string tablero_str;
            for (int f = 0; f < TABLERO_FILAS; ++f) {
                for (int c = 0; c < TABLERO_COLUMNAS; ++c) {
                    tablero_str += tablero[f][c];
                    tablero_str += ' ';
                }
                tablero_str += '\n';
            }
            send(cliente_socket, tablero_str.c_str(), tablero_str.size(), 0);
            send(cliente_socket, "Se declara EMPATE\n", strlen("Se declara EMPATE\n"), 0);
            mtx.unlock();
            break;
        }

        int server_col = rand() % TABLERO_COLUMNAS;
        for (int i = TABLERO_FILAS - 1; i >= 0; --i) {
            if (tablero[i][server_col] == '-') {
                tablero[i][server_col] = 'S';
                break;
            }
        }

        std::string tablero_str;
        for (int f = 0; f < TABLERO_FILAS; ++f) {
            for (int c = 0; c < TABLERO_COLUMNAS; ++c) {
                tablero_str += tablero[f][c];
                tablero_str += ' ';
            }
            tablero_str += '\n';
        }
        send(cliente_socket, tablero_str.c_str(), tablero_str.size(), 0);
        if (check_ganador(tablero, 'S')) {
            send(cliente_socket, "Gana el SERVIDOR\n", strlen("Gana el SERVIDOR\n"), 0);
            mtx.unlock();
            break;
        }
        mtx.unlock();
    }
    std::cout << "cliente desconectado" << std::endl;
    close(cliente_socket);
}

void ConnectFourServer::initialize_tablero(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]) {
    for (int f = 0; f < TABLERO_FILAS; ++f) {
        for (int c = 0; c < TABLERO_COLUMNAS; ++c) {
            tablero[f][c] = '-';
        }
    }
}

bool ConnectFourServer::check_ganador(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS], char jugador) {
    for (int f = 0; f < TABLERO_FILAS; ++f) {
        for (int c = 0; c < TABLERO_COLUMNAS; ++c) {
            if (c < 4) {
                if (tablero[f][c] == jugador && tablero[f][c + 1] == jugador && tablero[f][c + 2] == jugador && tablero[f][c + 3] == jugador) {
                    return true;
                }
            }
            if (f < 3) {
                if (tablero[f][c] == jugador && tablero[f + 1][c] == jugador && tablero[f + 2][c] == jugador && tablero[f + 3][c] == jugador) {
                    return true;
                }
            }
            if (f < 3 && c < 4) {
                if (tablero[f][c] == jugador && tablero[f + 1][c + 1] == jugador && tablero[f + 2][c + 2] == jugador && tablero[f + 3][c + 3] == jugador) {
                    return true;
                }
            }
            if (f >= 3 && c < 4) {
                if (tablero[f][c] == jugador && tablero[f - 1][c + 1] == jugador && tablero[f - 2][c + 2] == jugador && tablero[f - 3][c + 3] == jugador) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ConnectFourServer::tablero_lleno(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]) {
    for (int f = 0; f < TABLERO_FILAS; ++f) {
        for (int c = 0; c < TABLERO_COLUMNAS; ++c) {
            if (tablero[f][c] == '-') return false;
        }
    }
    return true;
}

void ConnectFourServer::imprimir_tablero(char tablero[TABLERO_FILAS][TABLERO_COLUMNAS]) {
    for (int f = 0; f < TABLERO_FILAS; ++f) {
        for (int c = 0; c < TABLERO_COLUMNAS; ++c) {
            std::cout << tablero[f][c] << '-';
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <PUERTO>" << std::endl;
        return -1;
    }
    int puerto = std::stoi(argv[1]);
    ConnectFourServer server(puerto);
    server.run();
    return 0;
}
