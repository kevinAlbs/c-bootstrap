# Repro attempt on ubuntu1804-large

Install Genny. See https://github.com/mongodb/genny/blob/master/docs/setup.md

Start a single node replica set:
```
mlaunch init --replicaset --nodes=1 --name=rs0 --priority --binarypath $(m bin 5.0.6-ent) --hostname localhost --enableMajorityReadConcern --setParameter enableTestCommands=1  --dir rs1
```

I modified the ConnectionsBuildup.yml workload to run on a single node replica set.

```
./run-genny workload /home/ubuntu/code/c-bootstrap/investigations/cdriver4300/ConnectionsBuildup.yml
```

Capture gdb stack samples:
```
genny_pid=$(pgrep genny_core)
for i in $(seq 5); do gdb -p $genny_pid -batch -ex 'thread apply all bt'; done >gdb.txt
```

Process the gdb stacks using scripts from https://github.com/10gen/t2

Use python 2 for the scripts.
```
pip install python-dateutil
pip install isodate
pip install pytz
```

See [this comment](https://jira.mongodb.org/browse/PERF-2203?focusedCommentId=4358940&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-4358940) in PERF-2203 for a set of steps to generate a reverse call tree.

```
python ../t2/scripts/gdb2stacks.py <gdb.txt >gdb.stacks
grep CrudActor gdb.stacks >CrudActor.stacks
python ../t2/scripts/calltree.py --reverse <CrudActor.stacks >CrudActor.html
```

My first attempt was run with a local one node replica set on an ubuntu1804-large host. 2356 of 2500 samples are in mongoc_socket_recv. I interpret that to mean the server is the bottleneck. Results are in CrudActor-1.html.