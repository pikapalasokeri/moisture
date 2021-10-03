#!/usr/bin/env python3

from flask import Flask, Response
import io
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
import elastic, time_utils, plot_generation
from datetime import timedelta

app = Flask(__name__)


@app.route("/plot.png")
def plot_png():
    try:
        es = elastic.ElasticMoistureDb()
        last_few_minutes = time_utils.now_utc() - timedelta(days=14)
        fig, ax = plot_generation.generate_plot(es, last_few_minutes, None)

        output = io.BytesIO()
        FigureCanvas(fig).print_png(output)
        return Response(output.getvalue(), mimetype="image/png")
    except e:
        return "Error"
    return "Wtf?"


@app.route("/")
def index():
    return '<img src="/plot.png" alt="Moisture readings">'


if __name__ == "__main__":
    from waitress import serve

    serve(app, host="0.0.0.0", port=8080)
