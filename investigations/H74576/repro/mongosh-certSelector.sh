#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <certificate-subject>"
  exit 1
fi

mongosh --host localhost --port 27017 \
  --tls \
  --tlsCertificateSelector "subject=$1" \
  --tlsCAFile ./certs/ca/test-ca.pem \
  --tlsAllowInvalidHostnames
