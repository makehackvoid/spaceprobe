#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <math.h>

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xE3 };
IPAddress server(10, 0, 100, 10);
EthernetClient ethClient;
PubSubClient client(ethClient);

const unsigned char led_r = 3;
const unsigned char led_g = 5;
const unsigned char led_b = 6;

const unsigned char enc_a = 2;
const unsigned char enc_b = 7;
const unsigned char enc_t = 8;

#define READ_ENC (PIND & 0b10000100)
const unsigned char enc_state_0 = 0b10000000;
const unsigned char enc_state_1 = 0b10000100;
const unsigned char enc_state_2 = 0b00000100;
const unsigned char enc_state_3 = 0b00000000;

unsigned char last_enc = enc_state_0;
unsigned char enc_count = 0;


const unsigned char volt = 9;
const unsigned char speak= 14;

struct led_colour {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} led_colour;

struct led_blink {
  struct led_colour colour;
  float frequency;
  unsigned long duration;
} led_blink;

struct buzz {
  unsigned int frequency;
  unsigned long duration;
};

struct volts {
  unsigned char pwm;
};

void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic,"spaceprobe/led/rgb")==0 && length == sizeof(led_colour)) {
    memcpy(&led_colour,payload,sizeof(led_colour));
  }

  if(strcmp(topic,"spaceprobe/led/rgb/fd")==0 && length == sizeof(led_blink)) {
    memcpy(&led_blink,payload,sizeof(led_blink));
    if(led_blink.duration < 5000) {
      led_blink.duration += millis();
    }
  }

  if(strcmp(topic,"spaceprobe/tone")==0 && length == sizeof(buzz)) {
    struct buzz buzz;
    memcpy(&buzz,payload,sizeof(buzz));
    if(buzz.duration < 5000) {
      tone(speak,buzz.frequency,buzz.duration);
    }
  }

  if(strcmp(topic,"spaceprobe/volts")==0 && length == sizeof(volts) ) {
    struct volts volts;
    memcpy(&volts,payload,sizeof(volts));
    analogWrite(volt,volts.pwm);
  }
}


void reconnect() {
  Serial.println("Setting up Ethernet");
  if(!Ethernet.begin(mac))
  {
    Serial.println("No network available yet");
    return;
  }

  Serial.print("Have IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println("Setting up MQTT");
  client.setServer(server, 1883);
  client.setCallback(callback);

  if (client.connect("SpaceProbe")) {
    Serial.println("connected");
    client.subscribe("spaceprobe/#");
    client.subscribe("space/status/+");
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" trying again later");
  }
}


void setup()
{
  delay(50);
  Serial.begin(57600);
  Serial.println("Starting The Space Probe");
  //reconnect();

  memset(&led_blink,0,sizeof(led_blink));

  pinMode(led_r,OUTPUT);
  pinMode(led_g,OUTPUT);
  pinMode(led_b,OUTPUT);

  pinMode(enc_a,INPUT_PULLUP);
  pinMode(enc_b,INPUT_PULLUP);
  pinMode(enc_t,INPUT_PULLUP);

  analogWrite(led_b,255);
}

void loop()
{
  if (!client.connected() || !client.loop()) {
    reconnect();
  }

  if( led_blink.duration > millis() ) {
    if( millis() % (int)(led_blink.frequency *1000) >= led_blink.frequency * 500 ) {
      analogWrite(led_r,led_blink.colour.r);
      analogWrite(led_g,led_blink.colour.g);
      analogWrite(led_b,led_blink.colour.b);
    } else {
      analogWrite(led_r,0);
      analogWrite(led_g,0);
      analogWrite(led_b,0);
    }
  } else {
    analogWrite(led_r,led_colour.r);
    analogWrite(led_g,led_colour.g);
    analogWrite(led_b,led_colour.b);
  }

  unsigned char enc_now = READ_ENC;
  char enc_delta = 0;
  if(
    (last_enc == enc_state_0 && enc_now == enc_state_1) ||
    (last_enc == enc_state_1 && enc_now == enc_state_2) ||
    (last_enc == enc_state_2 && enc_now == enc_state_3) ||
    (last_enc == enc_state_3 && enc_now == enc_state_0)
  ) {
    enc_delta = 1;
  } else if (
    (last_enc == enc_state_0 && enc_now == enc_state_3) ||
    (last_enc == enc_state_1 && enc_now == enc_state_0) ||
    (last_enc == enc_state_2 && enc_now == enc_state_1) ||
    (last_enc == enc_state_3 && enc_now == enc_state_2)
  ) {
    enc_delta = -1;
  }

  if( enc_delta ) {
    last_enc = enc_now;

    // the response curve on the panel meter roughly follows these magic
    // numbers to turn 0..85 encoder steps into 0..10 readings (with clipping)
    enc_count = constrain(int16_t(enc_count)+enc_delta,0,85);
    float qube = .030*(enc_count*enc_count) + .351*enc_count + 20;

    unsigned char volt_out = enc_count!=0 ? constrain(qube,0,255) : 0;
    analogWrite(volt,volt_out);
  }

  static uint32_t publish_debounce = 0;
  if( digitalRead(enc_t)==LOW && publish_debounce < millis() ) {

    // this turns the enc_count into 15 minute increments per detent
    float duration = round( enc_count * 0.5024)/4.0;
    Serial.print("Space is open for ");
    Serial.print(int(duration));
    Serial.print(":");
    Serial.println(int(duration*60.0)%60);

    client.publish("spaceprobe/duration",(void*)&duration,sizeof(duration));

    led_blink.duration = millis() + 1000;
    led_blink.frequency = .25;
    led_blink.colour.r = 127;
    led_blink.colour.g = 0;
    led_blink.colour.b = 0;

    publish_debounce = millis() + 1000;
    enc_count = 0;
    analogWrite(volt,0);
  }
}

