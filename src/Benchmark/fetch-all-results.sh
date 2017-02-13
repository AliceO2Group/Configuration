#!/bin/bash

while read timestamp;do
  ansible-playbook -i example-inventory.cfg fetch-results.yaml --extra-vars "timestamp=${timestamp}" $@
done < timestamps.txt

