#!/usr/bin/env python3
if __name__ == "__main__":
    import elastic
    es = elastic.ElasticMoistureDb(local=True)
    es.create_index()
