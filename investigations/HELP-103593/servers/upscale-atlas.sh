#!/usr/bin/env bash
# Upscale the Atlas cluster $CLUSTER (M10 -> M20) to trigger a rolling
# resize, the closest analog to the maintenance event in the ticket.
# Requires CLUSTER and PROJECT_ID in the environment. Optional: TIER. Pass NOWATCH=1 to return immediately.
set -o errexit
: "${CLUSTER:?set CLUSTER (e.g. . KEVINALBS/load-secrets.sh)}"
: "${PROJECT_ID:?set PROJECT_ID (e.g. . KEVINALBS/load-secrets.sh)}"
TIER=${TIER:-M20}

echo "$(date -u +%H:%M:%S) Updating $CLUSTER to $TIER"
atlas clusters update "$CLUSTER" --projectId "$PROJECT_ID" --tier "$TIER"
if [[ -z "${NOWATCH:-}" ]]; then
    atlas clusters watch "$CLUSTER" --projectId "$PROJECT_ID"
    echo "$(date -u +%H:%M:%S) $CLUSTER is IDLE"
fi
