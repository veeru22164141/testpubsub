#include <stdio.h>
#include <mosquitto.h>
#include <string.h>
#include <stdlib.h>

#define MQTT_HOST "mqtt-broker"
#define MQTT_PORT 1883
#define MQTT_TOPIC "test/topic0"
#define MQTT_PAYLOAD "Hello, World!"

void on_connect(struct mosquitto *mosq, void *userdata, int rc)
{
    if(rc == 0)
    {
        printf("Connected to MQTT broker\n");
    }
    else
    {
        printf("Failed to connect %d\n",rc);
    }
}

void log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    if(level <= MOSQ_LOG_DEBUG)
    {
        printf("LOG level %d : %s\n",level,str);
    }
}

int main()
{
    struct mosquitto *mosq = NULL;
    int rc;
    int attempts = 0;
    const int max_attempts = 3;
    char *MQTT_TOPIC = getenv("MQTT_TOPIC_ENV");
    printf("MQTT_TOPIC: %s\n", MQTT_TOPIC);
    if(MQTT_TOPIC == NULL)
    {
        MQTT_TOPIC = "test/topic0";
    }


    mosquitto_lib_init();
    mosq = mosquitto_new("publisher-test", true, NULL);
    if(!mosq)
    {
        fprintf(stderr,"Failed to create new mosquitto instance\n");
        return 1;
    }
    mosquitto_log_callback_set(mosq, log_callback);
    mosquitto_connect_callback_set(mosq, on_connect);
    rc = mosquitto_username_pw_set(mosq, "viru", "Veeru@123");
    if(rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr,"Failed to set username and password\n");
        return 1;
    }
    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if(rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr,"Failed to connect to broker\n");
        return 1;
    }
    do{

        attempts++;
        rc = mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(MQTT_PAYLOAD), MQTT_PAYLOAD, 0, false);
        if(rc != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr,"Failed to publish message\n");
            return 1;
        }
        else
        {
            printf("attempt %d: Published message\n", attempts);
            printf("Published message\n");
        }
        //wait for 5 seconds
        mosquitto_loop(mosq, 5, 1);
    }while ( attempts < max_attempts);
    
    while(1);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}