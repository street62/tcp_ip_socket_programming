#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char* message);
int str_to_int(char* str);
int get_res(int operands[], int len, char* operator);

int main(int argc, char* argv[]) {
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char input[BUF_SIZE];
    char output[BUF_SIZE];
    int operands[BUF_SIZE] = { 0, };

    int str_len;
    int i, j;
    int res;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    clnt_addr_size = sizeof(clnt_addr);
    
    for (i = 0; i < 5; i++) {
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
            error_handling("accept() error!");
        } else {
            printf("Conncected Client: %d\n", i + 1);
        }

        while ((str_len = read(clnt_sock, input, BUF_SIZE)) != 0) {
            const char* delim = ",";

            char* token = strtok(input, delim); // len of operands(str)
            int num_len = str_to_int(token);

            for (j = 0; j < num_len; j++) {
                token = strtok(NULL, delim);
                operands[j] = str_to_int(token);
            }

            token = strtok(NULL, delim); // operator
            res = get_res(operands, num_len, token);
            sprintf(output, "%d", res);

            write(clnt_sock, output, strlen(output) + 1);
        }

        close(clnt_sock);
    }

    close(serv_sock);

    return 0;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int str_to_int(char* str) {
    int res = 0;
    char* cur_char = str;

    while (cur_char != NULL) {
        res *= 10;
        res += ((*cur_char) - '0');
    }

    return res;
}

int get_res(int operands[], int len, char* operator) {
    int res = operands[0];
    int i;

    switch (*operator)
    {
    case '+':
        for (i = 1; i < len; i++) {
            res += operands[i];
        }
        break;
    case '-':
        for (i = 1; i < len; i++) {
            res -= operands[i];
        }
        break;
    case '*':
        for (i = 1; i < len; i++) {
            res *= operands[i];
        }
        break;
    case '/':
        for (i = 1; i < len; i++) {
            res /= operands[i];
        }
        break;
    default:
        fprintf(stderr, "get_res(): INVALID OPERATOR %s", operator);
        break;
    }

    return res;
}