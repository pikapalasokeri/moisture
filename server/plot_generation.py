import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pytz


def generate_plot(es, start_datetime, end_datetime):
    readings = es.get_readings(start_datetime, end_datetime)
    sensors = es.get_sensors()

    series = {sensor_id: ([], []) for sensor_id in sensors}
    series_timestamps = {sensor_id: [] for sensor_id in sensors}
    for r in readings:
        series[r.sensor_id][0].append(r.timestamp)
        series[r.sensor_id][1].append(r.raw_value)

    fig, ax = plt.subplots()
    for sensor_id, serie in series.items():
        timestamps, values = zip(*sorted(zip(serie[0], serie[1])))
        plt.plot(timestamps, values, ".-", label=sensor_id)

    formatter = mdates.DateFormatter(
        "%Y-%m-%d %H:%M", tz=pytz.timezone("Europe/Stockholm")
    )
    ax.xaxis.set_major_formatter(formatter)

    ax.tick_params(axis="x", rotation=90)
    ax.legend()
    ax.grid()
    fig.tight_layout()
    return fig, ax
