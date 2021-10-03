#!/usr/bin/env python3
from datetime import datetime, timedelta, timezone
import elastic
import time_utils, plot_generation
import matplotlib.pyplot as plt


es = elastic.ElasticMoistureDb()

print(es.create_index())
print(es.add_reading(1100, "sensor1", time_utils.now_utc()))
print(es.add_reading(850, "sensor2", time_utils.now_utc()))
readings = es.get_readings()
print(readings)

last_few_minutes = time_utils.now_utc() - timedelta(minutes=30)
fig, ax = plot_generation.generate_plot(es, last_few_minutes, None)
plt.show()
