package main

import (
	"context"
	"fmt"
	"time"

	"go.mongodb.org/mongo-driver/v2/bson"
	"go.mongodb.org/mongo-driver/v2/mongo"
	"go.mongodb.org/mongo-driver/v2/mongo/options"
)

func main() {
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	client, err := mongo.Connect(options.Client().ApplyURI("mongodb://localhost:27017"))
	if err != nil {
		panic(err)
	}
	defer client.Disconnect(ctx)

	client.AppendDriverInfo(options.DriverInfo{Name: "N1", Version: "V1"})
	client.AppendDriverInfo(options.DriverInfo{Name: "N2", Version: "V2"})
	client.AppendDriverInfo(options.DriverInfo{Name: "N1", Version: "V2"}) // Is this a duplicate?
	result := client.Database("admin").RunCommand(ctx, bson.D{{Key: "ping", Value: 1}})
	var doc bson.M
	if err := result.Decode(&doc); err != nil {
		panic(err)
	}
	fmt.Println(doc)
}
