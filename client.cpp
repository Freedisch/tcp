#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>



bool send_all(int fd, const char* data, size_t len){
    while(len > 0){
        ssize_t sent = send(fd, data, len, 0);
        if (sent <= 0) return false;
        data += sent;
        len -= sent;
    }
}

int main(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1;}

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if(connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {perror("connect"); return 1;}
    std::cout << "Connected! Type messages (Ctrl+D to quit):\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        line += '\n';
        if(!send_all(sock, line.data(), line.size())) break;

        std::string reply;
        char buf[1024];

        while (reply.find('\n') == std::string::npos) {
            ssize_t n = recv(sock, buf, sizeof(buf), 0);
            if (n <= 0){
                std::cout << "Server closed the connection.\n"; close(sock); return 0;
                reply.append(buf, n);
            }
            std::cout << "server echoed: " << reply;
        }
        close(sock);
    }

}
