import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pytz
import time_utils


def generate_plot(es, start_datetime, end_datetime, location):
    try:
        fig, ax = plt.subplots()

        ax.set_title(f"{time_utils.local(time_utils.now_utc())}")

        readings = es.get_readings(start_datetime, end_datetime)

        has_plotted = False

        print("Location:", location)
        sensors = es.get_sensors(location)
        print("sensors:", sensors)
        series = {sensor_id: ([], []) for sensor_id in sensors}
        print("series:", series)
        for r in readings:
            if r.sensor_id in sensors and r.location == location:
                print("Got reading:", r)
                series[r.sensor_id][0].append(time_utils.local(r.timestamp))
                series[r.sensor_id][1].append(r.raw_value)

        for sensor_id, serie in series.items():
            print(sensor_id, serie)
            if serie[0]:
                timestamps, values = zip(*sorted(zip(serie[0], serie[1])))
                ax.plot(timestamps, values, ".-", label=f"{location}/{sensor_id}")
                print("Plotted serie:", location, sensor_id)
                has_plotted = True

        if not has_plotted:
            print("No data, plotting empty...")
            ax.plot(
                [time_utils.local(start_datetime), time_utils.local(end_datetime)],
                [0, 0],
                ".",
                label=f"{location}/no data",
            )
            print("Empty was plotted.")

        formatter = mdates.DateFormatter(
            "%Y-%m-%d %H:%M", tz=pytz.timezone("Europe/Stockholm")
        )
        ax.xaxis.set_major_formatter(formatter)

        ax.tick_params(axis="x", rotation=90)
        ax.legend()
        ax.grid()
        fig.tight_layout()
        return fig, ax
    except:
        return None, None
