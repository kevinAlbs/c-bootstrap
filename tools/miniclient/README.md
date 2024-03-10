# MiniClient

A mini client to speak OP_MSG to a MongoDB server. Example use:

```python
import miniclient

conn = miniclient.connect ()
payload0_document = bson.encode ({"hello": "1", "$db": "db"})
reply = miniclient.send_opmsg (conn, payload0_document)
assert (reply["ok"] == 1)
conn.close ()
```

## Testing OP_MSG size limits

Run tests with:

```
python test_opmsg_limits.py
```

### Observations
Max BSON document size to insert is maxBsonObjectSize. This is currently 16MiB (16777216 == 16 * 1024 * 1024 bytes).

Max BSON document size to send is maxBsonObjectSize + allowance. The allowance is currently 16KiB (16384 == 16 * 1024 bytes) described in [SERVER-10643](https://jira.mongodb.org/browse/SERVER-10643). The server appears to sometimes add internal overhead (e.g. speaking to mongos), resulting in a reduced actual maximum to the client.

Max size of an OP_MSG is maxMessageSizeBytes. This is currently 48MB (48000000 = 48 * 1000 * 1000 bytes). Exceeding results in a connection error.
