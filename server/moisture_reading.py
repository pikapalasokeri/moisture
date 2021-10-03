from datetime import datetime, timezone
import time_utils


class MoistureReading:
    @staticmethod
    def fromElastic(doc):
        assert doc["sensor_type"][0] == "moisture"
        return MoistureReading(
            doc["timestamp"][0],
            doc["sensor_id"][0],
            doc["raw_value"][0],
        )

    def __init__(self, timestamp, sensor_id, raw_value):
        self.timestamp = time_utils.from_string(timestamp)
        self.sensor_id = sensor_id
        self.raw_value = raw_value

    def __repr__(self):
        return f"({self.sensor_id} {self.timestamp} : {self.raw_value})"
