$CERTS="C:\code\c-bootstrap\investigations\curl-schannel-bug\certs"
openssl version
# Set `-verify 1` so server requests client certificate.
openssl s_server -port 27777 -CAfile "$CERTS\ca.pem" -cert "$CERTS\server.pem" -verify 1