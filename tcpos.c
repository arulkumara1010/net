#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>

#define SERVER_PORT 8080
#define MAX_LINE 256
#define MAX_PENDING 5

int main()
{
    struct sockaddr_in sin;
    struct sockaddr_in client_addr;
    char buf[MAX_LINE];
    int sock_id, new_sock_id, len;

    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    if ((sock_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        exit(1);
    }

    if (bind(sock_id, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        close(sock_id);
        exit(1);
    }

    if (listen(sock_id, MAX_PENDING) < 0)
    {
        close(sock_id);
        exit(1);
    }

    while (1)
    {
        len = sizeof(client_addr);
        if ((new_sock_id = accept(sock_id, (struct sockaddr *)&client_addr, &len)) < 0)
        {
            continue;
        }

        while ((len = recv(new_sock_id, buf, sizeof(buf), 0)) > 0)
        {
            buf[len] = '\0';
            fputs(buf, stdout);
        }

        close(new_sock_id);
    }

    close(sock_id);
    return 0;
}
