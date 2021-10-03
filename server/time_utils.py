from datetime import datetime, timezone
import pytz


def from_string(datetime_utc_str):
    db_format = "%Y-%m-%dT%H:%M:%S.%fZ"
    unaware = datetime.strptime(datetime_utc_str, db_format)
    aware = unaware.replace(tzinfo=timezone.utc)
    return aware


def to_string(datetime_utc):
    db_format = "%Y-%m-%dT%H:%M:%S.%f"
    return datetime_utc.strftime(db_format)


def now_utc():
    return datetime.now(tz=timezone.utc)


def local(utc_datetime):
    return utc_datetime.astimezone(pytz.timezone("Europe/Stockholm"))
