# Create encrypted PKCS#8 private key with password "foobar".
openssl pkcs8 -topk8 \
    -in certs/server/mongodb-test-server.key \
    -out certs/server/mongodb-test-server-encrypted-pkcs8.key \
    -v2 des3 \
    -v2prf hmacWithSHA1 \
    -passout pass:foobar

# Create .PEM file with both cert and private key.
cat certs/server/mongodb-test-server.crt \
    certs/server/mongodb-test-server-encrypted-pkcs8.key \
    > certs/server/mongodb-test-server-encrypted-pkcs8.pem
