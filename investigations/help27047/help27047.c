// A C translation of the following Java example:
//
// @Test
// public void testMongoEncryptionPerformance() {
//   byte[] localMasterKey = new byte[] {
//       -99, -108, 75, 13, -109, -48, -59, 68, -91, 114, -3, 50, 27, -108, 48, -112, 35, 53,
//       115, 124, -16, -10, -62, -12, -38, 35, 86, -25, -113, 4, -52, -6, -34, 117, -76, 81,
//       -121, -13, -117, -105, -41, 75, 68, 59, -84, 57, -94, -58, 77, -111, 0, 62, -47, -6, 74,
//       48, -63, -46, -58, 94, -5, -84, 65, -14, 72, 19, 60, -101, 80, -4, -89, 36, 122, 46, 2,
//       99, -93, -58, 22, 37, 81, 80, 120, 62, 15, -40, 110, -124, -90, -20, -115, 45, 36, 71,
//       -27, -80
//   };
//   final String dataKeyId = "local";
//   Map<String, Map<String, Object>> kmsProviders = Map
//       .of(dataKeyId, Map.of("key", localMasterKey));
 
//   // Enforce TLS 1.2
//   System.setProperty("jdk.tls.client.protocols", "TLSv1.2");
 
//   // Setup encryption.
//   ConnectionString target = new ConnectionString("mongodb://localhost");
//   MongoNamespace vaultNs = new MongoNamespace("csfle", "vault");
//   ClientEncryptionSettings clientEncryptionSettings = ClientEncryptionSettings.builder()
//       .keyVaultMongoClientSettings(
//           MongoClientSettings.builder().applyConnectionString(target).build())
//       .keyVaultNamespace(vaultNs.getFullName()).kmsProviders(kmsProviders).build();
//   ClientEncryption encryptor = ClientEncryptions.create(clientEncryptionSettings);
//   BsonBinary bsonDataKeyId = encryptor.createDataKey(dataKeyId, new DataKeyOptions());
//   EncryptOptions encryptOptions;
//   encryptOptions = new EncryptOptions("AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic")
//       .keyId(bsonDataKeyId);
 
//   for (int j = 0; j < 3; ++j) {
//     int l = new int[]{16, 16*8, 16*100}[j];
//     byte[] b = new byte[l];
//     Random r = new Random();
//     long[] nanos = new long[100000];
//     for (int i = 0; i < nanos.length; ++i) {
//       r.nextBytes(b);
//       String s = new String(b, StandardCharsets.UTF_8);
 
//       long t = System.nanoTime();
//       encryptor.encrypt(new BsonString(s), encryptOptions);
//       nanos[i] = System.nanoTime() - t;
//     }
//     nanos = Arrays.stream(nanos).sorted().toArray();
//     System.out.println("Median " + b.length + " bytes encryption time (ns): " + nanos[nanos.length / 2]);
//     // Optimal performance without overhead: ~1.3 cycles/byte.
//     // https://software.intel.com/content/dam/develop/external/us/en/documents/10tb24-breakthrough-aes-performance-with-intel-aes-new-instructions-final-secure.pdf
//     // This machine is an Intel(R) Core(TM) i7-9850H CPU @ 2.60GHz.
//     long optimalNanos = (long) (1.3 * b.length * 1000 * 1000 * 1000 / (2.6 * 1000 * 1000 * 1000));
//     System.out.println("Optimal " + b.length + " bytes encryption time (ns): " + optimalNanos);
//   }
// }

// Compile with:
// export PKG_CONFIG_PATH=/Users/kevin.albertson/install/mongo-c-driver-1.18.0/lib/pkgconfig
// export DYLD_LIBRARY_PATH=/Users/kevin.albertson/install/mongo-c-driver-1.18.0/lib
// clang -o help27047.out help27047.c -O0 $(pkg-config --libs --cflags libmongoc-1.0)
// 
// Sample results taken on macOS 11.4:
// (.venv) kevin.albertson@Kevins-MacBook-Pro help27047 % ./help27047.out                                                                   
// 2021/08/26 18:10:44.0834: [8160589]:    DEBUG:       mongoc: Median 16 bytes encryption time (ns): 9627
// 2021/08/26 18:10:45.0999: [8160589]:    DEBUG:       mongoc: Median 128 bytes encryption time (ns): 10417
// 2021/08/26 18:10:47.0159: [8160589]:    DEBUG:       mongoc: Median 160 bytes encryption time (ns): 10541

#include <stdio.h>

#include <mongoc/mongoc.h>

void nextBytes (uint8_t *dst, int length) {
   int i;
   for (i = 0;i < length; i++) {
      dst[i] = (uint8_t) rand();
   }
}

int64_t nanoTime (void) {
   struct timespec ts;
   clock_gettime (CLOCK_MONOTONIC_RAW, &ts);
   return ((int64_t) ts.tv_sec) * 1000000000 + ts.tv_nsec;
}

int comparator (const void* a, const void* b) {
   int64_t* ai = (int64_t*)a;
   int64_t* bi = (int64_t*)b;
   return *ai - *bi;
}

int main () {
   mongoc_init ();
   
   uint8_t localMasterKey[] = {
      -99, -108, 75, 13, -109, -48, -59, 68, -91, 114, -3, 50, 27, -108, 48, -112, 35, 53,
      115, 124, -16, -10, -62, -12, -38, 35, 86, -25, -113, 4, -52, -6, -34, 117, -76, 81,
      -121, -13, -117, -105, -41, 75, 68, 59, -84, 57, -94, -58, 77, -111, 0, 62, -47, -6, 74,
      48, -63, -46, -58, 94, -5, -84, 65, -14, 72, 19, 60, -101, 80, -4, -89, 36, 122, 46, 2,
      99, -93, -58, 22, 37, 81, 80, 120, 62, 15, -40, 110, -124, -90, -20, -115, 45, 36, 71,
      -27, -80
   };

   char* dataKeyId = "local";
   bson_t* kmsProviders = BCON_NEW (dataKeyId, "{", "key", BCON_BIN (BSON_SUBTYPE_BINARY, localMasterKey, sizeof (localMasterKey) / sizeof (uint8_t)), "}");
 
   mongoc_uri_t *target = mongoc_uri_new ("mongodb://localhost:27017");
   mongoc_client_t *vaultClient = mongoc_client_new_from_uri (target);
   mongoc_client_encryption_opts_t *clientEncryptionSettings = mongoc_client_encryption_opts_new ();
   mongoc_client_encryption_opts_set_keyvault_client (clientEncryptionSettings, vaultClient);
   mongoc_client_encryption_opts_set_keyvault_namespace (clientEncryptionSettings, "csfle", "vault");
   mongoc_client_encryption_opts_set_kms_providers (clientEncryptionSettings, kmsProviders);
   bson_error_t error;
   mongoc_client_encryption_t *encryptor = mongoc_client_encryption_new (clientEncryptionSettings, &error);
   if (!encryptor) {
      MONGOC_ERROR ("Failed to create encryptor: %s", error.message);
      return EXIT_FAILURE;
   }

   mongoc_client_encryption_datakey_opts_t *datakey_opts = mongoc_client_encryption_datakey_opts_new ();
   bson_value_t bsonDataKeyId;
   if (!mongoc_client_encryption_create_datakey (encryptor, "local", datakey_opts, &bsonDataKeyId, &error)) {
      MONGOC_ERROR ("Failed to create data key: %s", error.message);
      return EXIT_FAILURE;
   }

   mongoc_client_encryption_encrypt_opts_t *encryptOptions = mongoc_client_encryption_encrypt_opts_new();
   mongoc_client_encryption_encrypt_opts_set_algorithm (encryptOptions, "AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic");
   mongoc_client_encryption_encrypt_opts_set_keyid (encryptOptions, &bsonDataKeyId);

#define NSAMPLES 100000
   int j, i;
   for (j = 0; j < 3; ++j) {
      int lengths[] = {16, 16*8, 16*10};
      int l = lengths[j];
      uint8_t* b = bson_malloc (l * sizeof (uint8_t));
      int64_t* nanos = bson_malloc (sizeof (int64_t) * NSAMPLES);
      for (i = 0; i < NSAMPLES; i++) {
         nextBytes (b, l);
         bson_value_t s;
         s.value_type = BSON_TYPE_UTF8;
         s.value.v_utf8.len = l;
         s.value.v_utf8.str = (char*)b;

         bson_value_t ciphertext;
         int64_t t = nanoTime();
         if (!mongoc_client_encryption_encrypt (encryptor, &s, encryptOptions, &ciphertext, &error)) {
            MONGOC_ERROR ("Failed to encrypt value: %s", error.message);
            return EXIT_FAILURE;
         }
         nanos[i] = nanoTime() - t; 
         bson_value_destroy (&ciphertext);
      }

      qsort (nanos, NSAMPLES, sizeof (int64_t), comparator);
      MONGOC_DEBUG("Median %d bytes encryption time (ns): %" PRId64, l, nanos[NSAMPLES / 2]);

      bson_free (nanos);
      bson_free (b);
   }

   mongoc_client_encryption_encrypt_opts_destroy (encryptOptions);
   bson_value_destroy (&bsonDataKeyId);
   mongoc_client_encryption_datakey_opts_destroy (datakey_opts);
   mongoc_client_encryption_opts_destroy (clientEncryptionSettings);
   mongoc_client_destroy (vaultClient);
   mongoc_uri_destroy (target);
   bson_destroy (kmsProviders);
   mongoc_cleanup ();
}