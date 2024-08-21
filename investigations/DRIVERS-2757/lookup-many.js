// Test retry strategies with SRV lookup.

/*
Results on an EC2 host:

Totals (immediate):
Failures: 5357 Successes: 4643
Retry Failures: 4204 Retry Successes: 1153
Failure rate: 42.04%

Totals (wait):
Failures: 5495 Successes: 4505
Retry Failures: 604 Retry Successes: 4891
Failure rate: 6.04%

Totals (wait-with-jitter):
Failures: 4119 Successes: 5881
Retry Failures: 58 Retry Successes: 4061
Failure rate: 0.58%

*/
const { Resolver } = require('dns');

const retryStrategy = "immediate";

function makeDNSRequest(resolver, counters, is_retry) {
    const srvHostname = `_mongodb._tcp.test1.test.build.10gen.cc`
    return new Promise((resolve, reject) => {
        resolver.resolveSrv(srvHostname, (err, res) => {
            if (err) {
                if (is_retry) {
                    counters.retry_failures++;
                    reject(err);
                } else {
                    counters.failures++;
                    console.log(`retrying with strategy ${retryStrategy} ...`);
                    if (retryStrategy == "immediate") {
                        makeDNSRequest(resolver, counters, true).then((res) => {
                            resolve(res);
                        }, (err) => {
                            reject(err);
                        });
                    }
                    else if (retryStrategy == "wait") {
                        setTimeout(() => {
                            makeDNSRequest(resolver, counters, true).then((res) => {
                                resolve(res);
                            }, (err) => {
                                reject(err);
                            });
                        }, 500);
                    }
                    else if (retryStrategy == "wait-with-jitter") {
                        let sleepTime = Math.random() * 500;
                        setTimeout(() => {
                            makeDNSRequest(resolver, counters, true).then((res) => {
                                resolve(res);
                            }, (err) => {
                                reject(err);
                            });
                        }, sleepTime);
                    }
                }
            } else {
                if (is_retry) {
                    counters.retry_successes++;
                } else {
                    counters.successes++;
                }
                resolve(res);
            }
        });
    });
}

function runTrial() {
    const resolver = new Resolver();
    let promises = [];
    let start = Date.now();

    // Reset counters.
    let counters = {
        failures: 0,
        successes: 0,
        retry_successes: 0,
        retry_failures: 0
    }

    console.log("Sending DNS queries ... start");
    for (let i = 0; i < 5000; i++) {
        promises.push(makeDNSRequest(resolver, counters, false));
    }

    return Promise.allSettled(promises).then((results) => {
        console.log("Sending DNS queries ... end");
        let end = Date.now();
        console.log("Took " + (end - start) + "ms");
        console.log("Failures: " + counters.failures + " Successes: " + counters.successes);
        console.log("Retry Failures: " + counters.retry_failures + " Retry Successes: " + counters.retry_successes);

        // Print unique errors:
        let uniqueErrorStrings = new Set();
        results.forEach((res) => {
            if (res.status === "rejected") {
                uniqueErrorStrings.add(res.reason.toString());
            }
        });
        if (uniqueErrorStrings.size > 0) {
            console.log("Got %d unique errors:", uniqueErrorStrings.size);
            uniqueErrorStrings.forEach((el) => { console.log(" - " + el) });
        }
        return counters;
    });
}

async function main() {
    let totals = {
        failures: 0,
        successes: 0,
        retry_successes: 0,
        retry_failures: 0
    }
    for (let i = 0; i < 10; i++) {
        let counters = await runTrial();
        totals.failures += counters.failures;
        totals.successes += counters.successes;
        totals.retry_failures += counters.retry_failures;
        totals.retry_successes += counters.retry_successes;
        await new Promise((resolve) => { setTimeout(resolve, 5000) });
    }

    console.log(`Totals (${retryStrategy}):`);
    console.log("Failures: " + totals.failures + " Successes: " + totals.successes);
    console.log("Retry Failures: " + totals.retry_failures + " Retry Successes: " + totals.retry_successes);
    let failureRate = 100 * (totals.retry_failures) / (totals.successes + totals.failures);
    console.log(`Failure rate: ${failureRate}%`);
}

main();
