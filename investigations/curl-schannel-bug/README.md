Steps to reproduce a bug found with cURL sending a client certificate with Windows Secure Channel to a TLS 1.3 server.

To reproduce, start an openssl server supporting TLS v1.3 (tested with openssl 3.0.2 and 3.5.0):

```powershell
# Update CERTS path in script.
.\start-openssl-server.ps1
```

Send curl command with client certificate:
```powershell
.\test.ps1
# Prints: curl: (35) schannel: next InitializeSecurityContext failed: SEC_E_INTERNAL_ERROR (0x80090304) - The Local Security Authority cannot be contacted
```

Compare with dotnet (also appears to use SChannel):
```powershell
cd with-dotnet
dotnet run
# Prints:
# SSL/TLS connection established using:
# Cipher: Aes256, TLS Version: Tls13
```