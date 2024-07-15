// Example: node lookup-srv.js test1.test.build.10gen.cc
const { Resolver } = require('dns');
const dns = new Resolver();
const hostname = process.argv[2];
const srvHostname = `_mongodb._tcp.${hostname}`
// Send DNS query to a non-default server.
dns.setServers(['127.0.0.1:5053'])
dns.resolveSrv(srvHostname, (err, res) => {
    console.log("#### SRV Record ####");
    if (err) {
        console.log(err);
    } else {
        console.log(res);
    }
});
