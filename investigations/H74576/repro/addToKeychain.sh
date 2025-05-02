#!/usr/bin/env bash
set -euo pipefail

# Check if password argument is provided
if [ $# -ne 1 ]; then
  echo "Usage: $0 <password>"
  exit 1
fi

PFX_PASSWORD="$1"  # The password for the .pfx file

# Step 1: Create PKCS#12 (.pfx) file
openssl pkcs12 -export -legacy \
  -out certs/server/test-server.pfx \
  -inkey certs/server/mongodb-test-server.key \
  -in certs/server/mongodb-test-server.crt \
  -certfile certs/ia/mongodb-test-ia.crt \
  -passout pass:"$PFX_PASSWORD"

# Step 2: Import the .pfx file into the macOS login keychain
security import certs/server/test-server.pfx \
  -k ~/Library/Keychains/login.keychain-db \
  -T /usr/bin/security \
  -P "$PFX_PASSWORD"

echo "Certificate imported to keychain successfully."
