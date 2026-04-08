use mongodb::{bson::doc, options::{ClientOptions,DriverInfo}, Client};

#[tokio::main]
async fn main() -> mongodb::error::Result<()> {
    let options = ClientOptions::parse("mongodb://127.0.0.1:27017").await?;
    let client = Client::with_options(options)?;
    client.append_metadata(DriverInfo::builder().name("N1").version(Some("V1".to_string())).build()).unwrap();
    client.append_metadata(DriverInfo::builder().name("N2").version(Some("V2".to_string())).build()).unwrap();
    client.append_metadata(DriverInfo::builder().name("N1").version(Some("V2".to_string())).build()).unwrap();
    let result = client.database("admin").run_command(doc! { "ping": 1 }).await?;
    println!("{:?}", result);
    Ok(())
}
