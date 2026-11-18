var c = rs.conf();
c.members = [
    { _id: 0, host: "localhost:27017", priority: 1 },
    { _id: 1, host: "localhost:27018", priority: 0 },
    { _id: 2, host: "localhost:27019", priority: 0 }
];
rs.reconfig(c);