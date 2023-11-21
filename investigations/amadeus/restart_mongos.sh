MONGOS_CMD=$(cat .menv/.mlaunch_startup | jq -r '.startup_info["27017"]')
eval $MONGOS_CMD
