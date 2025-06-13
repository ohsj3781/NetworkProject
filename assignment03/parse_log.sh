#!/bin/bash
# filepath: /home/ohsj3781/workspace/NetworkProject/assignment03/remove_packet_send.sh

input_file="graph_log.out"
output_file="packet_retrans.out"
parse="PacketRetrans PPS:"

grep "$parse" "$input_file" | sed "s/$parse\t//" > "$output_file"