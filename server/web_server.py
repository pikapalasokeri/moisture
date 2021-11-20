#!/usr/bin/env python3

from flask import Flask, Response, make_response
import io
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
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
