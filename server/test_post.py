#!/usr/bin/env python3
import requests

data = {"location": "spenat",
        "sensor_id": "12:12:42:42",
        "raw_value": "1000",
        "sensor_type": "moisture",
        "hash": 0}

url = "http://127.0.0.1:8080/new_moisture_reading"

headers = {"Content-Type": "application/json; charset=utf-8"}

response = requests.post(url, json=data, headers=headers)

print(response)
print(response.json())
