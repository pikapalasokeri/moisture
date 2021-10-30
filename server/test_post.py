#!/usr/bin/env python3
import requests
import random

locations = ["Spenaten", "RosingsPark5"]
sensor_ids = ["0", "1", "2"]


data = {
    "location": random.choice(locations),
    "sensor_id": random.choice(sensor_ids),
    "raw_value": str(random.randint(700, 1400)),
    "sensor_type": "moisture",
    "hash": 0,
}

url = "http://localhost:8080/new_moisture_reading"

headers = {"Content-Type": "application/json; charset=utf-8"}

response = requests.post(url, json=data, headers=headers)

print(response)
print(response.json())
