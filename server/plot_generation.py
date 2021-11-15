import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pytz
import time_utils


def generate_plot(es, start_datetime, end_datetime):
    # TODO: One subplot per location.

    fig, ax = plt.subplots()

    ax.set_title(f"{time_utils.local(time_utils.now_utc())}")

    readings = es.get_readings(start_datetime, end_datetime)

    locations = es.get_locations()
    print("Got locations:", locations)
    for location in locations:
        print("Location:", location)
        sensors = es.get_sensors(location)
        print("sensors:", sensors)
        series = {sensor_id: ([], []) for sensor_id in sensors}
        print("series:", series)
        series_timestamps = {sensor_id: [] for sensor_id in sensors}
        print("series timestamps:", series_timestamps)
        for r in readings:
            if r.sensor_id in sensors and r.location == location:
                print("Got reading:", r)
                series[r.sensor_id][0].append(r.timestamp)
                series[r.sensor_id][1].append(r.raw_value)

        for sensor_id, serie in series.items():
            print(sensor_id, serie)
            if serie[0]:
                timestamps, values = zip(*sorted(zip(serie[0], serie[1])))
                plt.plot(timestamps, values, ".-", label=f"{location}/{sensor_id}")
                print("Plotted serie:", location, sensor_id)

    formatter = mdates.DateFormatter(
        "%Y-%m-%d %H:%M", tz=pytz.timezone("Europe/Stockholm")
    )
    ax.xaxis.set_major_formatter(formatter)

    ax.tick_params(axis="x", rotation=90)
    ax.legend()
    ax.grid()
    fig.tight_layout()
    return fig, ax
