# Server side

## Setup

Run sudo ./setup.sh


## Some nice commands

Get shell:
sudo docker exec -it elasticsearch /bin/bash

Create backup:
sudo docker run --rm --volumes-from elasticsearch -v /${PWD}/tmp:/backup ubuntu tar cvf /backup/backup.tar /usr/share/elasticsearch

Restore from backup ish:
sudo docker run --rm --volumes-from elasticsearch -v $(pwd):/backup ubuntu bash -c "cd /dbdata && tar xvf /backup/backup.tar --strip 1"
