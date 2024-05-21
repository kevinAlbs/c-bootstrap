from pymongo import MongoClient

client = MongoClient()
client["db"].command({"ping": 1})
# client.close()
# Observe server logs: {"t":{"$date":"2024-05-21T13:19:34.984-04:00"},"s":"I",  "c":"-",        "id":20883,   "ctx":"conn1388","msg":"Interrupted operation as its client disconnected","attr":{"opId":11864065}}
