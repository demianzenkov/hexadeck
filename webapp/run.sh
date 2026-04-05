#!/bin/bash
cd "$(dirname "$0")"

if command -v python3 &>/dev/null; then
    PY=python3
elif command -v python &>/dev/null; then
    PY=python
else
    echo "Python not found. Please install Python 3."
    exit 1
fi

echo "Starting server at http://localhost:8000"
sleep 1 && open "http://localhost:8000" &
$PY -m http.server 8000
