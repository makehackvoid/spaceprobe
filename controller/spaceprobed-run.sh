#!/sbin/openrc-run

depend() {
    after mosquitto
}

configfile="/opt/spaceprobed/controllers/config.py"
user=$(grep 'D_USER' $configfile | awk -F ' +' '{print $3}' | tr -d \')
group=$(grep 'D_GROUP' $configfile | awk -F ' +' '{print $3}' | tr -d \')

command=/usr/bin/python3
command_args=/opt/spaceprobed/controller/spaceprobed.py
command_user="$user:$group"
pidfile=$(grep 'PID_FILE_LOC' $configfile | awk -F ' +' '{print $3}' | tr -d \')
