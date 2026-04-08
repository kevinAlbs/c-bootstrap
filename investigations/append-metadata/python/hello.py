from pymongo import MongoClient
from pymongo.driver_info import DriverInfo

client = MongoClient("mongodb://localhost:27017")
client.append_metadata(DriverInfo(name="N1", version="V1"))
client.append_metadata(DriverInfo(name="N2", version="V2"))
client.append_metadata(DriverInfo(name="N1", version="V2"))
result = client.admin.command("ping")

