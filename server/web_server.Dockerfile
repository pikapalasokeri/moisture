# syntax=docker/dockerfile:1

FROM python:3.8-slim-buster
WORKDIR /web_server
COPY requirements_web_server.txt requirements.txt
RUN pip3 install -r requirements.txt

COPY elastic.py \
     moisture_reading.py \
     plot_generation.py \
     time_utils.py \
     web_server.py \
     ./

CMD ["python3", "-u", "web_server.py"]
