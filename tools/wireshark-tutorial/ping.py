from pymongo import MongoClient
import os

# Get URI specified by MONGODB_URI environment variable, or default to mongodb://localhost:27017.
uri = os.environ.get("MONGODB_URI", "mongodb://localhost:27017")
client = MongoClient(uri)
client["db"].command({"ping": 1})
print ("sent ping to '{}' on database 'db'.".format(uri))