#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define SERVER_PORT 8080
#define MAX_LINE 256

int main(int argc, char *argv[])
{
    struct hostent *hp;
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int sock_id;

    if (argc != 2)
    {
        exit(1);
    }

    char *host = argv[1];

    if ((hp = gethostbyname(host)) == NULL)
    {
        exit(1);
    }

    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    if ((sock_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        exit(1);
    }

    if (connect(sock_id, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        close(sock_id);
        exit(1);
    }

    while (fgets(buf, sizeof(buf), stdin))
    {
        buf[MAX_LINE - 1] = '\0';
        int len = strlen(buf) + 1;
        if (send(sock_id, buf, len, 0) < 0)
        {
            close(sock_id);
            exit(1);
        }
    }

    close(sock_id);
    return 0;
}
