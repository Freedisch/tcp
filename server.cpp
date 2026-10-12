#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

bool send_all(int fd, const char* data, size_t len){
    while(len > 0){
        ssize_t sent = send(fd, data, len, 0);
        if (sent <= 0) return false;
        data += sent;
        len -= sent;
        return true;
    }
}
int main(){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0){
            perror("socket");
            return 1;
        }

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(8080);
        if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); return 1; }

        if (listen(server_fd, 5) < 0) {
            perror("listen");
            return 1;
        }
        std::cout << "Server listening on port 8080...\n";

        while(true){
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
            if (client_fd < 0){
                perror("accept");
                continue;
            }

            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
            std::cout << "Client connected from " << ip << ":" << ntohs(client_addr.sin_port) << "\n";

            char buf[1024];
            while(true){
                ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
                if(n == 0){
                    std::cout << "client hang up. \n"; break;
                };
                if (n < 0)  { perror("recv"); break; }
                std::cout << "Got " << n << " bytes: " << std::string(buf, n);

                if(!send_all(client_fd, buf, n)) break;
            }

            close(client_fd);
        }
        close(server_fd);

        std::cout << "Server listening on port 8080...\n";


}
