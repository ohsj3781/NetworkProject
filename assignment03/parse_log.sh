#!/bin/bash
# filepath: /home/ohsj3781/workspace/NetworkProject/assignment03/remove_packet_send.sh

input_file="packet_retrans_log.out"
output_file="clean_packet_retrans_log.out"
parse="s/Packet Retrans:\t//"

# Remove "Packet Send:" from each line
sed "$parse" "$input_file" > "$output_file"

echo "Removed 'Packet Send:' from $input_file"
echo "Output saved to: $output_file"