I can reproduce with `mongosh` connected to a serverless instance:
```
Atlas test> db.version()
8.0.0-rc17
Atlas test> db.adminCommand({bulkWrite: 1, ops: [{insert: 0, document: {foo: "bar"}}], nsInfo: [{ns: "db.coll"}]})
MongoServerError[AtlasError]: (Unauthorized) not authorized on admin to execute command { bulkWrite: 1, ops: [[{insert 0} {document [{foo bar}]}]], nsInfo: [[{ns db.coll}]], lsid: { id: {4 [149 137 108 46 117 126 74 218 185 29 250 199 40 40 23 131]} }, $clusterTime: { clusterTime: {1724242370 2}, signature: { hash: {0 [31 9 179 21 13 4 43 10 142 148 251 52 224 5 220 242 224 236 216 151]}, keyId: 7364078628537106432.000000 } }, $db: "admin" }
```



The user has the "Atlas admin" role.

Compare with a local test:

```
Enterprise replset [direct: primary] test> db.version()
8.0.0-rc7
Enterprise replset [direct: primary] test> db.adminCommand({bulkWrite: 1, ops: [{insert: 0, document: {foo: "bar"}}], nsInfo: [{ns: "db.coll"}]})
{
  cursor: {
    id: Long('0'),
    firstBatch: [ { ok: 1, idx: 0, n: 1 } ],
    ns: 'admin.$cmd.bulkWrite'
  },
  nErrors: 0,
  nInserted: 1,
  nMatched: 0,
  nModified: 0,
  nUpserted: 0,
  nDeleted: 0,
  electionId: ObjectId('7fffffff0000000000000026'),
  opTime: { ts: Timestamp({ t: 1724242327, i: 1 }), t: Long('38') },
  ok: 1,
  '$clusterTime': {
    clusterTime: Timestamp({ t: 1724242327, i: 1 }),
    signature: {
      hash: Binary.createFromBase64('AAAAAAAAAAAAAAAAAAAAAAAAAAA=', 0),
      keyId: Long('0')
    }
  },
  operationTime: Timestamp({ t: 1724242327, i: 1 })
}
```

