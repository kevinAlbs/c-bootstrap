Intended to test behavior of `PFXImportCertStore` repeatedly importing the same key name.

test.pfx is a test PFX file with a private key and certificate. The private key has a `friendlyName` of `{675812D8-12FE-47E1-8015-EC1AA02E5DD0}`:

```bash
$ openssl pkcs12 -info -in test.pfx -passin pass:secret -passout pass:secret 2>&1 | grep `friendlyName`
    friendlyName: {675812D8-12FE-47E1-8015-EC1AA02E5DD0}
```

Sample output of the test:
```
Deleting key with name ...                           {675812D8-12FE-47E1-8015-EC1AA02E5DD0}
Importing key with no flags ...                      {675812D8-12FE-47E1-8015-EC1AA02E5DD0}
Importing key with no flags ...                      {A93261EF-C064-4AEC-9F74-B6177A6E8E2D}
Importing key with PKCS12_ALLOW_OVERWRITE_KEY ...    {675812D8-12FE-47E1-8015-EC1AA02E5DD0}
```

`PFXImportCertStore` appears to do the following when importing a key with `friendlyName`:
- If no existing key has the name (i.e. [key container name](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptacquirecontexta#:~:text=The-,key%20container%20name,-.%20This%20is%20a)) of `friendlyName`, use `friendlyName`.
- If an existing key has the name `friendlyName`, generate a new GUID as the name.
- If an existing key has the name `friendlyName` and `PKCS12_ALLOW_OVERWRITE_KEY` is set, use `friendlyName` and overwrite the existing key.