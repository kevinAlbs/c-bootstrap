# Manually insert two documents with duplicate ObjectID 5 byte rand.
# This is used to self-test after_test.py.

from pymongo import MongoClient
from bson.objectid import ObjectId

client = MongoClient()

client["db"]["coll"].delete_many({"manually_constructed": True})

client["db"]["coll"].insert_one({
        "_id": ObjectId("AAAAAAA1" + "BBBBBBBBBB" + "CCCCC1"),
        "manually_constructed": True
    })

client["db"]["coll"].insert_one({
        "_id": ObjectId("AAAAAAA2" + "BBBBBBBBBB" + "CCCCC2"),
        "manually_constructed": True
    })