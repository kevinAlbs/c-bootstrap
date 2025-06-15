$CERTS="C:\code\c-bootstrap\investigations\curl-schannel-bug\certs"
# Use PKCS#12 client cert (cURL does not to support other formats on Windows?)
curl.exe "https://localhost:27777" --cert "$CERTS\client.p12" --insecure 