import miniclient
from miniclient import maxBsonObjectSize, maxMessageSizeBytes, allowance
import bson
import unittest

"""
Test OP_MSG limits.
"""


class TestOpMsg(unittest.TestCase):
    def setUp(self) -> None:
        self.conn = miniclient.connect()
        # Drop collection to clean-up data from prior test runs.
        reply = miniclient.send_opmsg(
            self.conn, bson.encode({"drop": "coll", "$db": "db"})
        )
        self.assertEqual(reply["ok"], 1)

        # Check if speaking to mongos.
        reply = miniclient.send_opmsg(
            self.conn, bson.encode({"hello": 1, "$db": "admin"})
        )
        self.assertEqual(reply["ok"], 1)
        self.ismongos = "msg" in reply and reply["msg"] == "isdbgrid"

        return super().setUp()

    def test_payload0_max_insert(self):
        # Test payload 0
        # ... with insert document size == maxBsonObjSize
        overhead = 22  # Data from rest of document.
        to_insert = {"_id": 0, "x": "a" * (maxBsonObjectSize - overhead)}
        self.assertEqual(len(bson.encode(to_insert)), maxBsonObjectSize)
        payload0_document = bson.encode(
            {"insert": "coll", "$db": "db", "documents": [to_insert]}
        )
        reply = miniclient.send_opmsg(self.conn, payload0_document)
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 1)

        # ... with insert document size == maxBsonObjSize + 1
        overhead = 22  # Data from rest of document.
        to_insert = {"_id": 0, "x": "a" * (maxBsonObjectSize - overhead + 1)}
        self.assertEqual(len(bson.encode(to_insert)), maxBsonObjectSize + 1)
        payload0_document = bson.encode(
            {"insert": "coll", "$db": "db", "documents": [to_insert]}
        )
        reply = miniclient.send_opmsg(self.conn, payload0_document)
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 0)
        self.assertIn("object to insert too large", reply["writeErrors"][0]["errmsg"])

    def test_payload1_max_insert(self):
        # Test payload 1
        # ... with insert document size == maxBsonObjSize
        payload0_document = bson.encode(
            {
                "insert": "coll",
                "$db": "db",
            }
        )
        payload1_document = bson.encode({"_id": 0, "x": "a" * (maxBsonObjectSize - 22)})
        self.assertEqual(len(payload1_document), maxBsonObjectSize)
        reply = miniclient.send_opmsg(
            self.conn, payload0_document, "documents", [payload1_document]
        )
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 1)
        # Drop collection to clean-up.
        reply = miniclient.send_opmsg(
            self.conn, bson.encode({"drop": "coll", "$db": "db"})
        )
        self.assertEqual(reply["ok"], 1)
        # Test payload 1
        # ... with insert document size , maxBsonObjSize + 1
        payload0_document = bson.encode(
            {
                "insert": "coll",
                "$db": "db",
            }
        )
        payload1_document = bson.encode(
            {"_id": 0, "x": "a" * (maxBsonObjectSize + 1 - 22)}
        )
        self.assertEqual(len(payload1_document), maxBsonObjectSize + 1)
        reply = miniclient.send_opmsg(
            self.conn, payload0_document, "documents", [payload1_document]
        )
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 0)
        self.assertIn("object to insert too large", reply["writeErrors"][0]["errmsg"])
        # Drop collection to clean-up.
        reply = miniclient.send_opmsg(
            self.conn, bson.encode({"drop": "coll", "$db": "db"})
        )
        self.assertEqual(reply["ok"], 1)

    def test_payload0_max_update(self):
        # Insert a document to receive update
        reply = miniclient.send_opmsg(
            self.conn,
            bson.encode({"insert": "coll", "$db": "db", "documents": [{"_id": 0}]}),
        )
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 1)

        # Test payload 0
        # ... with update statement size > maxBsonObjSize, such that the payload document size == maxBsonObjSize + allowance
        update_statement = {
            "q": {
                # Put large payload in query to avoid storing a large document. This may avoid the "object to large to insert error"
                "x": {"$ne": "b" * (maxBsonObjectSize + allowance - 110)}
            },
            "u": {"$set": {"x": "a"}},
        }
        payload0_document = bson.encode(
            {"update": "coll", "$db": "db", "updates": [update_statement]}
        )
        self.assertEqual(len(payload0_document), maxBsonObjectSize + allowance)
        reply = miniclient.send_opmsg(self.conn, payload0_document)
        if self.ismongos:
            # mongos appears to add size to update statement, resulting in an error.
            self.assertEqual(reply["ok"], 0)
            self.assertIn(
                "BSONObj size: 16793922 (0x1004142) is invalid. Size must be between 0 and 16793600(16MB)",
                reply["errmsg"],
            )
        else:
            self.assertEqual(reply["ok"], 1)
            self.assertEqual(reply["n"], 1)

        # Drop collection to clean-up.
        reply = miniclient.send_opmsg(
            self.conn, bson.encode({"drop": "coll", "$db": "db"})
        )
        self.assertEqual(reply["ok"], 1)

        # Insert a document to receive update
        reply = miniclient.send_opmsg(
            self.conn,
            bson.encode({"insert": "coll", "$db": "db", "documents": [{"_id": 0}]}),
        )
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 1)

        # Test payload 0
        # ... with update statement size > maxBsonObjSize, such that the payload document size == maxBsonObjSize + allowance + 1
        update_statement = {
            "q": {
                # Put large payload in query to avoid storing a large document. This may avoid the "object to large to insert error"
                "x": {"$ne": "b" * (maxBsonObjectSize + allowance + 1 - 110)}
            },
            "u": {"$set": {"x": "a"}},
        }
        payload0_document = bson.encode(
            {"update": "coll", "$db": "db", "updates": [update_statement]}
        )
        self.assertEqual(len(payload0_document), maxBsonObjectSize + allowance + 1)
        reply = miniclient.send_opmsg(self.conn, payload0_document)
        self.assertEqual(reply["ok"], 0)
        self.assertIn(
            "BSONObj size: 16793601 (0x1004001) is invalid. Size must be between 0 and 16793600(16MB)",
            reply["errmsg"],
        )

    def test_payload1_max_update(self):
        # Test payload 1
        # ... with update statement size == maxBsonObjSize + allowance.

        # Insert a document to receive update
        reply = miniclient.send_opmsg(
            self.conn,
            bson.encode({"insert": "coll", "$db": "db", "documents": [{"_id": 0}]}),
        )
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 1)

        overhead = 59
        update_statement = bson.encode(
            {
                "q": {
                    # Put large payload in query to avoid storing a large document. This may avoid the "object to large to insert error"
                    "x": {"$ne": "b" * (maxBsonObjectSize + allowance - overhead)}
                },
                "u": {"$set": {"x": "a"}},
            }
        )
        self.assertEqual(len(update_statement), maxBsonObjectSize + allowance)

        payload0_document = bson.encode({"update": "coll", "$db": "db"})
        reply = miniclient.send_opmsg(
            self.conn, payload0_document, "updates", [update_statement]
        )

        # Both mongos and mongod appear to interally add size to update statement, resulting in an error.
        if self.ismongos:
            # mongos reports command error.
            self.assertEqual(reply["ok"], 0)
            self.assertIn(
                "BSONObj size: 16793973 (0x1004175) is invalid. Size must be between 0 and 16793600(16MB)",
                reply["errmsg"],
            )
        else:
            self.assertEqual(reply["ok"], 1)
            self.assertEqual(reply["n"], 0)
            self.assertIn(
                "BSONObj size: 16793617 (0x1004011) is invalid. Size must be between 0 and 16793600(16MB)",
                reply["writeErrors"][0]["errmsg"],
            )

    def test_payload0_max_opmsg(self):
        # Test payload 0
        # ... with payload document size == maxBsonObjSize + allowance
        overhead = 22  # Data from rest of document.
        to_insert = {"_id": 0, "x": "a" * (maxBsonObjectSize - overhead)}
        self.assertLessEqual(len(bson.encode(to_insert)), maxBsonObjectSize)
        to_insert_extra = {"_id": 1, "x": "a" * 16306}
        self.assertLessEqual(len(bson.encode(to_insert_extra)), maxBsonObjectSize)
        payload0_document = bson.encode(
            {"insert": "coll", "$db": "db", "documents": [to_insert, to_insert_extra]}
        )
        self.assertEqual(len(payload0_document), maxBsonObjectSize + allowance)
        reply = miniclient.send_opmsg(self.conn, payload0_document)
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 2)

        # Test payload 0
        # ... with payload document size == maxBsonObjSize + allowance + 1
        overhead = 22  # Data from rest of document.
        to_insert = {"_id": 0, "x": "a" * (maxBsonObjectSize - overhead)}
        self.assertLessEqual(len(bson.encode(to_insert)), maxBsonObjectSize)
        to_insert_extra = {"_id": 1, "x": "a" * (16306 + 1)}
        self.assertLessEqual(len(bson.encode(to_insert_extra)), maxBsonObjectSize)
        payload0_document = bson.encode(
            {"insert": "coll", "$db": "db", "documents": [to_insert, to_insert_extra]}
        )
        self.assertEqual(len(payload0_document), maxBsonObjectSize + allowance + 1)
        reply = miniclient.send_opmsg(self.conn, payload0_document)
        self.assertEqual(reply["ok"], 0)
        self.assertIn(
            "BSONObj size: 16793601 (0x1004001) is invalid. Size must be between 0 and 16793600(16MB)",
            reply["errmsg"],
        )

    def test_payload1_max_opmsg(self):
        # Test payload 1
        # ... with message size == maxMessageSizeBytes
        payload0_document = bson.encode(
            {
                "insert": "coll",
                "$db": "db",
            }
        )
        payload1_documents = []
        id = 0
        longstr = "a" * 1024
        for _ in range(45889):
            payload1_documents.append(bson.encode({"_id": id, "x": longstr}))
            id += 1
        # Add one more to get exact length.
        payload1_documents.append(bson.encode({"_id": id, "x": "a" * 14}))
        opmsg = miniclient.build_opmsg(
            payload0_document, "documents", payload1_documents
        )
        self.assertEqual(len(opmsg), maxMessageSizeBytes)

        reply = miniclient.send_opmsg(
            self.conn, payload0_document, "documents", payload1_documents
        )
        self.assertEqual(reply["ok"], 1)
        self.assertEqual(reply["n"], 45890)

        # Test payload 1
        # ... with message size == maxMessageSizeBytes + 1
        payload0_document = bson.encode(
            {
                "insert": "coll",
                "$db": "db",
            }
        )

        payload1_documents = []
        id = 0
        longstr = "a" * 1024
        for _ in range(45889):
            payload1_documents.append(bson.encode({"_id": id, "x": longstr}))
            id += 1
        # Add one more to get exact length.
        payload1_documents.append(bson.encode({"_id": id, "x": "a" * (14 + 1)}))
        opmsg = miniclient.build_opmsg(
            payload0_document, "documents", payload1_documents
        )
        assert len(opmsg) == maxMessageSizeBytes + 1
        # Expect network error.
        with self.assertRaises(ConnectionError):
            reply = miniclient.send_opmsg(
                self.conn, payload0_document, "documents", payload1_documents
            )

    def tearDown(self) -> None:
        self.conn.close()
        return super().tearDown()


if __name__ == "__main__":
    unittest.main()
