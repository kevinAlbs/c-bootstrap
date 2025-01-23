# Create the URI for the many mongoses
port_start=27017
port_end=27056 # Inclusive.
uristr = "mongodb://"
for port in range(port_start, port_end + 1):
    uristr += f"localhost:{port},"
print(uristr[:-1]) # Remove trailing comma
