#include <mongoc/mongoc.h>

#include "../../util/hex.h"

#include "vendor_kms_message/vendor_kms_reader_writer_private.h"

int main (int argc, char** argv) {
    mongoc_init ();
    
    if (argc != 2) {
        printf ("usage: kmip_dump \"<hex encoded data\">\n");
        return EXIT_FAILURE;
    }

    size_t datalen;
    uint8_t *data = hex_to_bin (argv[1], &datalen);
    char *kmipstr = kmip_dump (data, datalen);

    printf ("%s", kmipstr);
    free (kmipstr);
    free (data);
    mongoc_cleanup ();
}
