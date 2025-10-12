#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include <stdbool.h>
#include <string.h>

#define FAILF(fmt, ...)                                                            \
    if (1)                                                                         \
    {                                                                              \
        fprintf(stderr, "%s:%d FAIL: " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                                   \
    }                                                                              \
    else                                                                           \
        ((void)0)

#define FAIL(msg) FAILF("%s", msg);

#define ASSERT(stmt)        \
    if (!(stmt))            \
    {                       \
        FAILF("%s", #stmt); \
    }                       \
    else                    \
        ((void)0)

#define ASSERTF(stmt, fmt, ...)  \
    if ((!stmt))                 \
    {                            \
        FAILF(fmt, __VA_ARGS__); \
    }                            \
    else                         \
        ((void)0)

char *
winerr_to_string(DWORD err_code)
{
    LPSTR msg = NULL;
    if (0 == FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY |
                                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL,
                            err_code,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPSTR)&msg,
                            0,
                            NULL))
    {
        LocalFree(msg);
        return _strdup("Failed to get error");
    }

    // Remove trailing newline.
    size_t msglen = strlen(msg);
    if (msglen >= 1 && msg[msglen - 1] == '\n')
    {
        if (msglen >= 2 && msg[msglen - 2] == '\r')
        {
            // Remove trailing \r\n.
            msg[msglen - 2] = '\0';
        }
        else
        {
            // Just remove trailing \n.
            msg[msglen - 1] = '\0';
        }
    }

    char *ret = _strdup(msg);
    LocalFree(msg);
    return ret;
}

static char *
read_file(const char *filename, size_t *out_len)
{
    FILE *file = fopen(filename, "rb");
    ASSERT(file);
    ASSERT(out_len);

    // Get file length:
    long file_len;
    {
        ASSERT(0 == fseek(file, 0, SEEK_END));
        file_len = ftell(file);
        ASSERT(file_len >= 0);
        ASSERT(0 == fseek(file, 0, SEEK_SET));
    }

    // Read file into NUL-terminated string:
    char *contents;
    {
        contents = (char *)malloc((size_t)file_len + 1u);
        contents[file_len] = '\0';
        ASSERT((size_t)file_len == fread(contents, 1, file_len, file));
        *out_len = (size_t)file_len;
    }

    (void)fclose(file); // Ignore error.
    return contents;
}

// delete_capi_key deletes a persisted key imported into the CryptoAPI (not CNG)
static void delete_capi_key(const char *container_name)
{
    /* Key stored in legacy CAPI (CryptoAPI). */
    DWORD flags = CRYPT_DELETEKEYSET;
    HCRYPTPROV prov = 0;
    LPSTR prov_name = "Microsoft Enhanced Cryptographic Provider v1.0";
    bool ok = CryptAcquireContextA(
        &prov,
        container_name,
        prov_name,
        PROV_RSA_FULL,
        flags);
    ASSERTF(ok, "CryptAcquireContextA failed: %s", winerr_to_string(GetLastError()));
    ASSERT(prov == 0);
}

static void print_imported_key_name(HCERTSTORE store)
{
    PCCERT_CONTEXT cert = NULL;
    bool found_private_key = false;

    while ((cert = CertEnumCertificatesInStore(store, cert)) != NULL)
    {
        CRYPT_KEY_PROV_INFO *key_prov_info = NULL;
        DWORD key_prov_info_len = 0;

        if (CertGetCertificateContextProperty(cert, CERT_KEY_PROV_INFO_PROP_ID, NULL, &key_prov_info_len))
        {
            ASSERT(!found_private_key); // Expect only one cert has a private key.
            found_private_key = true;
            key_prov_info = (CRYPT_KEY_PROV_INFO *)malloc(key_prov_info_len);
            if (CertGetCertificateContextProperty(cert, CERT_KEY_PROV_INFO_PROP_ID, key_prov_info, &key_prov_info_len))
            {
                wprintf(L"%s\n", key_prov_info->pwszContainerName ? key_prov_info->pwszContainerName : L"(none)");
            }
            free(key_prov_info);
        }
    }
}

int main(void)
{
    size_t len;
    char *data = read_file("test.pfx", &len);
    const char *key_name = "{675812D8-12FE-47E1-8015-EC1AA02E5DD0}";
    CRYPT_DATA_BLOB blob = {.cbData = (DWORD)len, .pbData = data};

    printf("Deleting key with name ...                           %s\n", key_name);
    delete_capi_key(key_name);

    printf("Importing key with no flags ...                      ");
    {
        HCERTSTORE store = PFXImportCertStore(&blob, L"secret", 0 /* dwFlags */);
        ASSERTF(store, "PFXImportCertStore failed: %s", winerr_to_string(GetLastError()));
        print_imported_key_name(store);
    }

    printf("Importing key with no flags ...                      ");
    {
        HCERTSTORE store = PFXImportCertStore(&blob, L"secret", 0 /* dwFlags */);
        ASSERTF(store, "PFXImportCertStore failed: %s", winerr_to_string(GetLastError()));
        print_imported_key_name(store);
    }

    printf("Importing key with PKCS12_ALLOW_OVERWRITE_KEY ...    ");
    {
        HCERTSTORE store = PFXImportCertStore(&blob, L"secret", PKCS12_ALLOW_OVERWRITE_KEY /* dwFlags */);
        ASSERTF(store, "PFXImportCertStore failed: %s", winerr_to_string(GetLastError()));
        print_imported_key_name(store);
    }

    free(data);
}