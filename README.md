# Introduction
A light im server.
# Usage
## Dependency
### MySQL
Create a database called lightim and configure database in DB.h. Execute the sql "createTable.sql" in data directory.
### REDIS
Configure REDIS connection in REDIS.h.
## Build
```
mkdir build
cd build
cmake ..
make server
```
## Run
```
./server
```
