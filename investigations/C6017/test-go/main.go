package main

import (
	"context"
	"fmt"
	"log"
	"time"

	"go.mongodb.org/mongo-driver/v2/bson"
	"go.mongodb.org/mongo-driver/v2/mongo"
	"go.mongodb.org/mongo-driver/v2/mongo/options"
)

func main() {
	// Create a client and connect to MongoDB
	ctx, cancel := context.WithTimeout(context.Background(), 20*time.Second)
	defer cancel()
	client, err := mongo.Connect(options.Client().ApplyURI("mongodb://localhost:27017"))
	if err != nil {
		log.Fatalf("Failed to connect: %v", err)
	}

	defer func() {
		if err := client.Disconnect(ctx); err != nil {
			log.Fatalf("Failed to disconnect from MongoDB: %v", err)
		}
	}()

	fmt.Println("Successfully connected to MongoDB")

	// Get a handle for the desired database and collection
	database := client.Database("testdb")        // Replace with your database name
	collection := database.Collection("testcol") // Replace with your collection name

	// Insert the document into the collection
	result, err := collection.InsertOne(ctx, bson.D{{"bad", "\xFF"}})
	if err != nil {
		log.Fatalf("Failed to insert document: %v", err)
	}

	fmt.Printf("Document inserted with ID: %v\n", result.InsertedID)
}
