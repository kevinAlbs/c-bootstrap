using System.Net.Security;  
using System.Net.Sockets;  
using System.Security.Authentication;  
using System.Security.Cryptography.X509Certificates;


string serverName = "localhost";
int port = 27777;
string clientCertPath = "C:\\code\\c-bootstrap\\investigations\\curl-schannel-bug\\certs\\client.p12";

using TcpClient client = new TcpClient();
await client.ConnectAsync(serverName, port);

using SslStream sslStream = new SslStream(
    client.GetStream(),
    false // Do not leave the inner stream open after the SslStream is disposed  
);

X509Certificate2Collection clientCollection = X509CertificateLoader.LoadPkcs12CollectionFromFile(clientCertPath, "");

try
{
    // Specify only TLS 1.3 as the protocol  
    var sslOptions = new SslClientAuthenticationOptions
    {
        TargetHost = serverName,
        ClientCertificates = clientCollection,
        EnabledSslProtocols = SslProtocols.Tls13
    };

    // Authenticate with TLS 1.3  
    await sslStream.AuthenticateAsClientAsync(sslOptions);

    Console.WriteLine("SSL/TLS connection established using:");
    Console.WriteLine($"Cipher: {sslStream.CipherAlgorithm}, TLS Version: {sslStream.SslProtocol}");
}
catch (AuthenticationException ex)
{
    Console.WriteLine($"SSL/TLS Authentication failed: {ex.Message}");
}
catch (Exception ex)
{
    Console.WriteLine($"SSL/TLS connection failed: {ex.Message}");
}
