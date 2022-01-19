from pymongo import MongoClient
import sys

client = MongoClient()
count = client["db"]["coll"].count_documents({})
if count > 0:
    resp = input ("Do you want to drop db.coll? It contains {} documents. Enter Y to confirm. ".format(count))
    if resp != "Y":
        sys.exit(0)

print ("Dropping db.coll.")
client["db"]["coll"].drop()