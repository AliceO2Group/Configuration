# Benchmark (work in progress)

## Introduction
[todo: describe process]

vars.json contains an example configuration for a benchmark  

## Preparation
Put binaries and dependency libs into directory 'deploy-client-dir'

## Deploy to servers/clients
./ansible-playbook -i example-inventory.cfg deploy-server.yaml --extra-vars @example-vars.json
./ansible-playbook -i example-inventory.cfg deploy-client.yaml --extra-vars @example-vars.json

## Run a benchmark 
./ansible-playbook -i example-inventory.cfg run-benchmark.yaml --extra-vars @example-vars.json

## Fetch results
./fetch-all-results.sh --extra-vars @example-vars.json