# syntax=docker/dockerfile:1

FROM python:3.8-slim-buster
WORKDIR /web_server
COPY requirements_data_receiver.txt requirements.txt
RUN pip3 install -r requirements.txt

COPY data_receiver.py \
     elastic.py \
     moisture_reading.py \
     time_utils.py \
     ./

CMD ["python3", "-u", "data_receiver.py"]
