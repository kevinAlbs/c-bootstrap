npm install && node hello.js

# Print the most recent non-mongosh client-metadata
mongosh --eval "db.adminCommand({'getLog': 'global'}).log.forEach((x) => print(x))" | grep "client metadata" | grep -v "mongosh" | tail -1 | sed "s/^[[:space:]]*'//; s/\\\\n',$//" | jq '.attr.doc.driver'
