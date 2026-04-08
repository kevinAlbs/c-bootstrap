const { MongoClient } = require('mongodb');

async function main() {
    const client = new MongoClient('mongodb://127.0.0.1:27017');
    client.appendMetadata({ name: "N1", version: "V1" });
    client.appendMetadata({ name: "N2", version: "V2" });
    client.appendMetadata({ name: "N1", version: "V2" });
    await client.connect();
    const result = await client.db('admin').command({ ping: 1 });
    console.log(result);
    await client.close();
}

main().catch(console.error);
