#!/bin/bash

# Compile the web version
make web

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Starting server..."
    echo "Press 'q' to quit the server"
    # Start server and save PID
    emrun --no_browser --port 8000 web/ & 
    SERVER_PID=$!
    # Wait for 'q' key
    read -n 1 key
    if [[ $key = "q" ]]; then
        kill $SERVER_PID
    fi
else
    echo "Compilation failed with error code $?"
    read -p "Press enter to continue..."
fi
