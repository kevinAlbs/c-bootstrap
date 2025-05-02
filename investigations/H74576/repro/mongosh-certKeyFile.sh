#!/usr/bin/env bash
set -euo pipefail

mongosh --host localhost --port 27017 \
  --tls \
  --tlsCertificateKeyFile ./certs/server/mongodb-test-server.pem \
  --tlsCAFile ./certs/ca/test-ca.pem \
  --tlsAllowInvalidHostnames
