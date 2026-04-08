import com.mongodb.client.MongoClients;
import org.bson.Document;
import com.mongodb.MongoDriverInformation;

public class Hello {
    public static void main(String[] args) {

        try (var client = MongoClients.create("mongodb://localhost:27017")) {
            {
                MongoDriverInformation.Builder builder = MongoDriverInformation.builder();
                builder.driverName("N1");
                builder.driverVersion("V1");
                client.appendMetadata(builder.build());
            }

            {
                MongoDriverInformation.Builder builder = MongoDriverInformation.builder();
                builder.driverName("N2");
                builder.driverVersion("V2");
                client.appendMetadata(builder.build());
            }

            {
                MongoDriverInformation.Builder builder = MongoDriverInformation.builder();
                builder.driverName("N1");
                builder.driverVersion("V2");
                client.appendMetadata(builder.build());
            }

            var result = client.getDatabase("admin").runCommand(new Document("ping", 1));
            System.out.println(result.toJson());
        }
    }
}
