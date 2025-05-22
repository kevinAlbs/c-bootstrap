from pymongo import MongoClient
from bson.raw_bson import RawBSONDocument

client = MongoClient()
coll = client["db"]["coll"]
coll.drop()
# Hex encoding of { "_id": "a\xFF\b"}
data = bytes.fromhex("12000000025f6964000400000061ff080000")
coll.insert_one (RawBSONDocument(data)) # No error.
