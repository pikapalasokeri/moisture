from elasticsearch import Elasticsearch
import time_utils
from moisture_reading import MoistureReading


class ElasticMoistureDb:
    max_num_readings = 10000

    def __init__(self, local=False):
        if local:
            self._es = Elasticsearch()
        else:
            self._es = Elasticsearch(["elasticsearch"])
        self._index = "test-index7"
        self._index_ping = "ping"

    def create_index(self):
        settings = {
            "mappings": {
                "properties": {
                    "sensor_type": {"type": "keyword"},  # Or text?
                    "timestamp": {"type": "date"},
                    "sensor_id": {"type": "keyword"},
                    "location": {"type": "keyword"},
                    "raw_value": {"type": "integer"},
                }
            }
        }

        try:
            self._es.indices.create(index=self._index, body=settings)
            created = True
        except:
            created = False
        return created

    def create_index_ping(self):
        settings = {
            "mappings": {
                "properties": {
                    "timestamp": {"type": "date"},
                    "location": {"type": "keyword"},
                    "rssi": {"type": "integer"},
                }
            }
        }

        try:
            self._es.indices.create(index=self._index_ping, body=settings)
            created = True
        except:
            created = False
        return created

    def get_readings(self, datetime_start=None, datetime_end=None, max_num_readings_override=None):
        start_str = "1900-01-01"
        end_str = "2100-01-01"
        start_str = (
            time_utils.to_string(datetime_start)
            if datetime_start is not None
            else "1900-01-01"
        )
        end_str = (
            time_utils.to_string(datetime_end)
            if datetime_end is not None
            else "2100-01-01"
        )

        es = Elasticsearch()
        query = {
            "bool": {
                "must": {"term": {"sensor_type": "moisture"}},
                "filter": [
                    {
                        "range": {
                            "timestamp": {
                                "gte": start_str,
                                "lte": end_str,
                            }
                        }
                    }
                ],
            },
        }
        fields = [
            "sensor_type",
            "timestamp",
            "raw_value",
            "sensor_id",
            "location",
        ]

        max_num_readings = self.max_num_readings if max_num_readings_override is None else max_num_readings_override
        res = self._es.search(index=self._index, query=query, fields=fields, size=max_num_readings, sort={"timestamp":"desc"})
        return [MoistureReading.fromElastic(h["fields"]) for h in res["hits"]["hits"]]


    def get_sensors(self, location=None):
        es = Elasticsearch()
        agg1 = {"sensor_ids": {"terms": {"field": "sensor_id", "size": 500}}}
        if location is not None:
            filter1 = {"term": {"location": location}}
        else:
            filter1 = {"match_all": {}}
        aggs = {"sensor_ids": {"filter": filter1, "aggs": agg1}}
        res = self._es.search(index=self._index, aggs=aggs, size=0)
        return sorted(
            [
                b["key"]
                for b in res["aggregations"]["sensor_ids"]["sensor_ids"]["buckets"]
            ]
        )

    def get_locations(self):
        es = Elasticsearch()
        aggs = {"locations": {"terms": {"field": "location", "size": 500}}}
        res = self._es.search(index=self._index, aggs=aggs, size=0)
        return sorted([b["key"] for b in res["aggregations"]["locations"]["buckets"]])

    def add_reading(self, raw_value, sensor_id, location, datetime_utc):
        doc = {
            "sensor_type": "moisture",
            "sensor_id": sensor_id,
            "location": location,
            "raw_value": raw_value,
            "timestamp": datetime_utc,
        }
        res = self._es.index(self._index, body=doc, refresh=True)
        return res["result"] == "created"

    def add_ping(self, rssi, location, datetime_utc):
        doc = {
            "rssi": rssi,
            "location": location,
            "timestamp": datetime_utc,
        }
        res = self._es.index(self._index_ping, body=doc, refresh=True)
        return res["result"] == "created"

    def get_latest_ping(self, location):
        es = Elasticsearch()
        query = {
            "bool": {
                "must": {"term": {"location": location}},
            },
        }
        fields = [
            "location",
            "timestamp",
            "rssi",
        ]
        res = self._es.search(index=self._index_ping,
                              query=query,
                              fields=fields,
                              size=1,
                              sort={"timestamp":"desc"})
        return dict([(k, v[0]) for k,v in res["hits"]["hits"][0]["fields"].items()])
