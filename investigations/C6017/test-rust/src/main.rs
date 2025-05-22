use mongodb::{bson::RawDocumentBuf, options::ClientOptions, Client};  
  
#[tokio::main]  
async fn main() {  
    // Connect to the MongoDB database  
    let client_uri = "mongodb://localhost:27017";  
    let options = ClientOptions::parse(client_uri).await.unwrap();  
    let client = Client::with_options(options).unwrap();  
  
    // Select the database and collection  
    let database = client.database("db");  
    let collection = database.collection::<RawDocumentBuf>("coll");  
  
    // Raw BSON data as bytes  
    let bson_bytes: &[u8] = &[  
        0x19, 0x00, 0x00, 0x00, // total document size (25 bytes)  
        0x02,                   // BSON type for UTF-8 string  
        b'n', 0x00,             // field name "n"  
        0x06, 0x00, 0x00, 0x00, // string length (6 bytes including null terminator)  
        b'h', b'e', 0xFF, b'l', b'o', 0x00, // UTF-8 string "hello"  
        0x10,                   // BSON type for 32-bit integer  
        b'x', 0x00,             // field name "x"  
        0x2a, 0x00, 0x00, 0x00, // int32 value (42 in little-endian)  
        0x00,                   // end of object  
    ];  
  
    // Convert the bytes to a RawDocumentBuf  
    let raw_doc = RawDocumentBuf::from_bytes(bson_bytes.to_vec()).unwrap();  
  
    // Insert the raw BSON document into the collection  
    collection.insert_one(raw_doc).await.unwrap();  
  
    println!("Raw BSON document inserted successfully!");  

}  
