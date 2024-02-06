```bash
brew deps --tree --installed mongo-c-driver
```

Produces:
```
brew deps --tree --installed mongo-cxx-driver
mongo-cxx-driver
└── mongo-c-driver
    └── openssl@3
        └── ca-certificates
```
