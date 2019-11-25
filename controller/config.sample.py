LOG_FILE_LOC = '/var/log/spaceprobed/spaceprobed.log'
LOG_LEVEL    = 10 # logging.DEBUG

PID_FILE_LOC = '/var/run/spaceprobed/spaceprobed.pid'
D_USER  = 'spaceprobed'
D_GROUP = 'spaceprobed'

FB_TOKEN      = ''
FB_MHV_PAGEID = ''
DO_FACEBOOK   = False

TW_CON_KEY = ''
TW_CON_SEC = ''
TW_ACC_KEY = ''
TW_ACC_SEC = ''
DO_TWITTER = True

MQTT_BROKER_ADDRESS = ""
MQTT_BROKER_PORT    = 1883
MQTT_KEEPALIVE_SECS = 60
MQTT_TOPICS_SUB     = [("spaceprobe/duration", 0)]
MQTT_CLIENTID       = "spaceprobed"
