#!/usr/bin/env bash
set -euo pipefail

# Directory setup
mkdir -p certs/ca certs/ia certs/server

# Generate CA key and certificate
openssl genrsa -out certs/ca/mongodb-test-ca.key 4096
openssl req -new -x509 -days 1826 \
  -key certs/ca/mongodb-test-ca.key \
  -out certs/ca/mongodb-test-ca.crt \
  -config certs-cnf/openssl-test-ca.cnf

# Generate Intermediate Authority key and CSR
openssl genrsa -out certs/ia/mongodb-test-ia.key 4096
openssl req -new \
  -key certs/ia/mongodb-test-ia.key \
  -out certs/ia/mongodb-test-ia.csr \
  -config certs-cnf/openssl-test-ia.cnf

# Sign the IA CSR with the CA to get the IA certificate
openssl x509 -sha256 -req -days 730 \
  -in certs/ia/mongodb-test-ia.csr \
  -CA certs/ca/mongodb-test-ca.crt \
  -CAkey certs/ca/mongodb-test-ca.key \
  -set_serial 01 \
  -out certs/ia/mongodb-test-ia.crt \
  -extfile certs-cnf/openssl-test-ca.cnf \
  -extensions v3_ca

# Combine the CA and IA certificates into a single PEM
cat certs/ia/mongodb-test-ia.crt certs/ca/mongodb-test-ca.crt > certs/ca/test-ca.pem

# Generate server key and CSR
openssl genrsa -out certs/server/mongodb-test-server.key 4096
openssl req -new \
  -key certs/server/mongodb-test-server.key \
  -out certs/server/mongodb-test-server.csr \
  -config certs-cnf/openssl-test-server.cnf

# Sign server CSR with IA certificate
openssl x509 -sha256 -req -days 365 \
  -in certs/server/mongodb-test-server.csr \
  -CA certs/ia/mongodb-test-ia.crt \
  -CAkey certs/ia/mongodb-test-ia.key \
  -CAcreateserial \
  -out certs/server/mongodb-test-server.crt \
  -extfile certs-cnf/openssl-test-ca.cnf \
  -extensions v3_req

# Combine server cert and key into one PEM
cat certs/server/mongodb-test-server.crt certs/server/mongodb-test-server.key > certs/server/mongodb-test-server.pem

echo "All certificates generated successfully in the ./certs directory."

