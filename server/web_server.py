#!/usr/bin/env python3

from flask import Flask, Response, make_response
import io
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
import matplotlib.pyplot as plt
import elastic, time_utils, plot_generation
from datetime import timedelta
from threading import Lock

app = Flask(__name__)
mutex = Lock()


@app.route("/<location>/plot.png")
def plot_png(location):
    try:
        mutex.acquire()

        es = elastic.ElasticMoistureDb()
        now = time_utils.now_utc()
        last_few_minutes = now - timedelta(days=14)
        fig, ax = plot_generation.generate_plot(es, last_few_minutes, now, location)

        output = io.BytesIO()
        FigureCanvas(fig).print_png(output)
        plt.close(fig)
        return Response(output.getvalue(), mimetype="image/png")
    except Exception as e:
        print(e)
    finally:
        mutex.release()
    return "Wtf?"


@app.route("/<location>")
def specific_location(location):
    print("location", location)
    html = f'<img src="/{location}/plot.png" alt="Moisture readings">'
    html += "<br>"*3

    es = elastic.ElasticMoistureDb()
    latest_ping = es.get_latest_ping(location)
    html += f"Latest ping: {time_utils.from_string(latest_ping['timestamp'])}, {latest_ping['rssi']}<br>"
    latest_reading = es.get_readings(max_num_readings_override=1)[0]
    html += f"Latest reading: {latest_reading.timestamp}, {latest_reading.sensor_id}, {latest_reading.raw_value}"

    response = make_response(html)
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
def all_locations():
    try:
        es = elastic.ElasticMoistureDb()
        locations = es.get_locations()
        plot_htmls = [
            f'<a href="/{location}"><img src="/{location}/plot.png" alt="Moisture readings"></a>'
            for location in locations
        ]
        html = "\n<br>\n".join(plot_htmls)
        response = make_response(html)
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Pragma"] = "no-cache"
        return response
    except Exception as e:
        return "Wtf?!"


if __name__ == "__main__":
    from waitress import serve

    print("Serving web server...")
    serve(app, host="0.0.0.0", port=80)
