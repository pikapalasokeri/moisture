#!/usr/bin/env python3

from flask import Flask, Response, request
import elastic, time_utils

app = Flask(__name__)


def compute_checksum(req):
    return 0


def is_valid(req):
    if not (
        "raw_value" in req
        and "sensor_id" in req
        and "location" in req
        and "hash" in req
    ):
        return False

    if int(req["raw_value"]) >= 4096:
        return False

    # 01:02:03:04:05:06
    if len(req["sensor_id"]) > 24:
        return False

    if len(req["location"]) > 40:
        return False

    if compute_checksum(req) != int(req["hash"]):
        return False

    return True


@app.route("/new_moisture_reading", methods=["POST"])
def index():
    request_data = request.get_json()
    print(request_data)

    if is_valid(request_data):
        print("Request is valid")
        es = elastic.ElasticMoistureDb()
        es.add_reading(
            request_data["raw_value"],
            request_data["sensor_id"],
            request_data["location"],
            time_utils.now_utc(),
        )
        print("Successfully added reading from:", request_data)

    return {"result": "success"}


@app.route("/ping", methods=["POST"])
def index():
    request_data = request.get_json()
    print(request_data)

    es = elastic.ElasticMoistureDb()
    es.add_ping(
        request_data["rssi"],
        request_data["location"],
        time_utils.now_utc(),
        )
    print("Successfully added ping from:", request_data)

    return {"result": "success"}


if __name__ == "__main__":
    from waitress import serve

    print("Serving data receiver...")
    serve(app, host="0.0.0.0", port=8080)
