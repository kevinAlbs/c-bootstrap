#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <unistd.h>

void handle_error(const char *message)
{
    fprintf(stderr, "Error: %s\n", message);
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

int create_socket(const char *hostname, const char *port)
{

    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    struct addrinfo *result;
    int s;

    if (getaddrinfo(hostname, port, &hints, &result) != 0)
    {
        handle_error("getaddrinfo failed");
    }

    // Try connecting to any result returned by DNS lookup.
    int sockfd = -1;
    for (struct addrinfo *rp = result; rp; rp = rp->ai_next)
    {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1)
        {
            handle_error("socket failed");
        }

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == -1)
        {
            close(sockfd);
            sockfd = -1;
            continue;
        }

        break;
    }

    if (sockfd == -1)
    {
        handle_error("unable to connect\n");
    }

    freeaddrinfo(result);
    return sockfd;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *hostname = argv[1];
    const char *port = "443";
    SSL_CTX *ctx;
    SSL *ssl;
    int sockfd; // Socket file descriptor.

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == NULL)
    {
        handle_error("SSL_CTX_new failed");
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); // Abort handshake if certificate verification fails.
    SSL_CTX_set_default_verify_paths(ctx);

    sockfd = create_socket(hostname, port);

    ssl = SSL_new(ctx);
    if (ssl == NULL)
    {
        handle_error("SSL_new failed");
    }

    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) <= 0)
    {
        handle_error("SSL_connect failed");
    }

    char request[1024];
    snprintf(request, sizeof(request), "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", hostname);

    SSL_write(ssl, request, strlen(request));

    char response[4096];
    int bytes;
    while ((bytes = SSL_read(ssl, response, sizeof(response) - 1)) > 0)
    {
        response[bytes] = '\0';
        printf("%s", response);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);

    return 0;
}
