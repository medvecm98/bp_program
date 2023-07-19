#!/bin/bash

directory="." # Replace with your directory path

# Create an array to store the file names
files=()
LINE=$'\nAAA\n'

# Loop through the directory and add file names to the array
for file in "$directory"/*.md; do
    if [[ -f "$file" ]]; then
        filename=$(basename "$file")  # Get the base name of the file
        file_without_extension="${filename%.*}"  # Remove the extension
        files+=("$file_without_extension")

    fi
done

# Print the file names in the array
for file in "${files[@]}"; do
    for i in {1..25}; do
        # cp "$file.md" "$file-$i.md"
        # echo -e "$LINE $i" >> "$file-$i.md"
        rm "$file-$i.md"
    done
done
