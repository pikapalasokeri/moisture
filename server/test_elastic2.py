#!/usr/bin/env python3
from datetime import datetime, timedelta, timezone
import elastic
import time_utils, plot_generation
import matplotlib.pyplot as plt


es = elastic.ElasticMoistureDb(True)

print(es.create_index())
print(es.add_reading(801, "sensor1", "location1", time_utils.now_utc()))
print(es.add_reading(802, "sensor2", "location1", time_utils.now_utc()))
print(es.add_reading(812, "sensor12", "location2", time_utils.now_utc()))
readings = es.get_readings()
print(readings)
print(es.get_sensors("location1"))
print(es.get_sensors())
print(es.get_locations())

last_few_minutes = time_utils.now_utc() - timedelta(minutes=30)
fig, ax = plot_generation.generate_plot(es, last_few_minutes, None)
plt.show()
