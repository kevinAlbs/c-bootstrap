from pymongo import MongoClient
from bson.objectid import ObjectId

client = MongoClient()
cursor = client["db"]["coll"].find()

# parse_id parses an ObjectID into constituent parts.
# Assumes the ObjectID is the format described here:
# https://github.com/mongodb/specifications/blob/master/source/objectid.rst#specification
def parse_oid (oid : ObjectId):
    timestamp = oid.binary[0:4] # 4 bytes.
    rand = oid.binary[4:9] # 5 bytes.
    counter = oid.binary[9:12] # 3 bytes.
    return timestamp, rand, counter

# rand_to_oids maps the 5 byte rand to ObjectIDs that contain that 5 byte rand.
rand_to_oids = {}

# Get all ObjectIDs.
for doc in cursor:
    oid : ObjectId = doc["_id"]
    timestamp, rand, counter = parse_oid (oid)
    if rand not in rand_to_oids:
        rand_to_oids[rand] = [oid]
    else:
        print ("Duplicate rand detected: {}".format(oid))
        rand_to_oids[rand].append(oid)

# Collect all ObjectIDs with duplicate rand bytes into one table.
table = []
dups = 0
for k,v in rand_to_oids.items():
    if len(v) == 1:
        continue
    dups += 1
    for i in range(len(v)):
        timestamp, rand, counter = parse_oid (v[i])
        if i == 0:
            table.append([rand.hex(), len(v), timestamp.hex(), counter.hex()])
            continue
        table.append(["", "", timestamp.hex(), counter.hex()])

import sys
if dups == 0:
    print ("No duplicates found.")
    sys.exit(1)

import tabulate
table = tabulate.tabulate(table, headers=["random", "duplicates", "timestamp", "counter"])
print (table)