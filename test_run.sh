#!/bin/bash
set -e

echo "=== Running Headless Neverball & Neverputt Smoke Tests ==="

# Clean any stale logs
mkdir -p logs
rm -f logs/neverball.log logs/neverputt.log

echo "Starting Neverball under xvfb..."
xvfb-run -a ./neverball > logs/neverball.log 2>&1 &
NB_PID=$!
sleep 3

if kill -0 $NB_PID 2>/dev/null; then
    echo "Neverball is running successfully (PID: $NB_PID)."
    kill -TERM $NB_PID 2>/dev/null || kill -9 $NB_PID 2>/dev/null || true
    wait $NB_PID 2>/dev/null || true
    echo "Neverball stopped gracefully."
else
    echo "Neverball failed to stay running! Check logs/neverball.log:"
    cat logs/neverball.log
    exit 1
fi

echo "Starting Neverputt under xvfb..."
xvfb-run -a ./neverputt > logs/neverputt.log 2>&1 &
NP_PID=$!
sleep 3

if kill -0 $NP_PID 2>/dev/null; then
    echo "Neverputt is running successfully (PID: $NP_PID)."
    kill -TERM $NP_PID 2>/dev/null || kill -9 $NP_PID 2>/dev/null || true
    wait $NP_PID 2>/dev/null || true
    echo "Neverputt stopped gracefully."
else
    echo "Neverputt failed to stay running! Check logs/neverputt.log:"
    cat logs/neverputt.log
    exit 1
fi

echo "=== All Headless Smoke Tests Passed Successfully ==="
