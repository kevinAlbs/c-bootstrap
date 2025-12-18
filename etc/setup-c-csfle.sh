#!/bin/bash

# Set environment variables for testing the C driver with CSFLE.

DRIVERS_TOOLS="$HOME/code/drivers-evergreen-tools"
if [ ! -d "$DRIVERS_TOOLS" ]; then
    echo "Could not find drivers-evergreen-tools at $DRIVERS_TOOLS."
    echo "Set DRIVERS_TOOLS to a local checkout of: https://github.com/mongodb-labs/drivers-evergreen-tools"
    exit 1
fi

# Get secrets from AWS Secrets Manager.
if [ -z "$AWS_PROFILE" ]; then
    echo "Set AWS_PROFILE to a profile that can access the secrets in AWS Secrets Manager."
    echo "See https://wiki.corp.mongodb.com/display/DRIVERS/Using+AWS+Secrets+Manager+to+Store+Testing+Secrets"
    exit 1
fi
export AWS_PROFILE

"$DRIVERS_TOOLS"/.evergreen/csfle/setup-secrets.sh drivers/csfle # Writes to secrets-export.sh
. secrets-export.sh

# Export variables for testing the C driver.
export MONGOC_TEST_AWS_ACCESS_KEY_ID=$FLE_AWS_KEY
export MONGOC_TEST_AWS_SECRET_ACCESS_KEY=$FLE_AWS_SECRET
export MONGOC_TEST_AZURE_TENANT_ID=$FLE_AZURE_CLIENTID
export MONGOC_TEST_AZURE_CLIENT_ID=$FLE_AZURE_TENANTID
export MONGOC_TEST_AZURE_CLIENT_SECRET=$FLE_AZURE_CLIENTSECRET
export MONGOC_TEST_GCP_EMAIL=$FLE_GCP_EMAIL
export MONGOC_TEST_GCP_PRIVATEKEY=$FLE_GCP_PRIVATEKEY
export MONGOC_TEST_CSFLE_TLS_CA_FILE=$CSFLE_TLS_CA_FILE
export MONGOC_TEST_CSFLE_TLS_CERTIFICATE_KEY_FILE=$CSFLE_TLS_CLIENT_CERT_FILE
export MONGOC_TEST_AWS_TEMP_SECRET_ACCESS_KEY=$CSFLE_AWS_TEMP_SECRET_ACCESS_KEY
export MONGOC_TEST_AWS_TEMP_ACCESS_KEY_ID=$CSFLE_AWS_TEMP_ACCESS_KEY_ID
export MONGOC_TEST_AWS_TEMP_SESSION_TOKEN=$CSFLE_AWS_TEMP_SESSION_TOKEN

