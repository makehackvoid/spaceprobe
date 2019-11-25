#!/usr/bin/python3

import logging
from datetime import datetime, timedelta

import paho.mqtt.client as mqtt
import struct
import facebook
import tweepy
from daemonize import Daemonize

from config import *

## set up logger
log = logging.getLogger(__name__)
log.setLevel(LOG_LEVEL)

log_fh = logging.FileHandler(LOG_FILE_LOC, 'w')
log_fh.setLevel(LOG_LEVEL)
log.addHandler(log_fh)
keep_fds = [log_fh.stream.fileno()]  # used in daemonize call


def mqtt_on_connect(client, userdata, flags, rc):
    log.debug("Connected with result code" + str(rc))
    client.subscribe(MQTT_TOPICS_SUB)


def mqtt_on_message(client, userdata, msg):
    log.debug(f'got a msg in {msg.topic}')
    if str(msg.topic) == "spaceprobe/duration":
        possibly_send_alert(msg.payload)


def possibly_send_alert(payload):
    value = struct.unpack("<f", payload)[0]
    log.debug(f'value is {value}')
    if value != 0.0:
        now = datetime.now()
        when = now + timedelta(hours=value)
        log.debug(f"we think now is {now.strftime('%Y-%m-%d %H:%M:%S')}")
        log.debug(f"we think we're closing at {when.strftime('%Y-%m-%d %H:%M:%S')}")

        message = f"The space is now open until {when.strftime('%Y-%m-%d %H:%M:%S')}"
        log.debug(f"message={message}")

        if DO_FACEBOOK:
            log.debug('sending to facebook')
            graph.put_object(
            parent_object=FB_MHV_PAGEID,
            connection_name='feed',
            message=message)

        if DO_TWITTER:
            log.debug('sending to twitter')
            tw.update_status(status=message)

def main_loop():
    log.debug('+ starting the spaceprobe server side component')
    log.debug(f'++ attempting to connect to {MQTT_BROKER_ADDRESS}:{MQTT_BROKER_PORT} with keepalive {MQTT_KEEPALIVE_SECS}')
    client = mqtt.Client(client_id=MQTT_CLIENTID)
    client.on_connect = mqtt_on_connect
    client.on_message = mqtt_on_message
    client.connect(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT, MQTT_KEEPALIVE_SECS)

    client.loop_start()

    while True:
        pass


# entry

if DO_FACEBOOK:
    graph = facebook.GraphAPI(
            access_token=FB_TOKEN,
            version="3.1")

if DO_TWITTER:
    tw_auth = tweepy.OAuthHandler(TW_CON_KEY, TW_CON_SEC)
    tw_auth.set_access_token(TW_ACC_KEY, TW_ACC_SEC)
    tw = tweepy.API(tw_auth)

# daemon
daemon = Daemonize(app='spaceprobed', pid=PID_FILE_LOC,
                   user=D_USER, group=D_GROUP,
                   action=main_loop, keep_fds=keep_fds)
daemon.start()
