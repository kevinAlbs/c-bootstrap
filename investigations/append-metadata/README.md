Tests driver handling of duplicate DriverInfo fields. Tests a language-equivalent of:

```python
client = MongoClient("mongodb://localhost:27017")
client.append_metadata(DriverInfo(name="N1", version="V1"))
client.append_metadata(DriverInfo(name="N2", version="V2"))
client.append_metadata(DriverInfo(name="N1", version="V2")) # Are these duplicates?
result = client.admin.command("ping")
```

Results:
```yaml
{
  "name": "mongo-go-driver|N1|N2|N1",
  "version": "2.5.0|V1|V2|V2"
}
{
  "name": "PyMongo|c|N1|N2",
  "version": "4.15.4|V1|V2"
}
{
  "name": "mongo-java-driver|sync|N1|N2|N1",
  "version": "5.6.4|V1|V2|V2"
}
{
  "name": "nodejs|N1|N2|N1",
  "version": "7.1.1|V1|V2|V2"
}
{
  "name": "mongo-rust-driver|N1|N2|N1",
  "version": "3.5.2|V1|V2|V2"
}
```
