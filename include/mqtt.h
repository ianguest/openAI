#ifndef MQTT_H
#define MQTT_H

#include <mosquittopp.h>

#include <queue>

#include "base64.h"




class mqtt : public mosqpp::mosquittopp
{
private:
 const char     *     host;
 const char    *     id;
 const char    *     topic;
 const char * jsonfile;
 int                port;
 int                keepalive;

 void on_connect(int rc);
 void on_disconnect(int rc);
 void on_publish(int mid);
 void on_subscribe(int mid, int qos_count, const int *granted_qos);
 void on_message(const struct mosquitto_message *message);


public:
 mqtt(const char *id, const char * _topic, const char *host, int port);
 mqtt(const char *id,  const char *host, int port);
//mqtt(const char *id, const char * _topic, const char *host, int port, const char *jsonfile);  // to update a file comming in
 ~mqtt();

 //bool receive_message(const char * _message);
 bool receive_file(const char * filename);
 bool receive_queue(int maxdepth, const char *searchterm);
 bool send_message(const char *macaddress, const char *trigger, const char *alert_name, const char *filetype, const char *videofilename);
 bool send_message(const char *macaddress, const char *trigger, const char *alert_name, unsigned frames, unsigned humans, unsigned vehicles);
 bool send_message(const char *jasonfilename);
 bool send_message(char *topic,  char *payload, int payloadlength);
 int get_next_payload_size(void);
 int get_next_topic_size(void);
 int pop_payload(void *buffer, char *topic);

private:
    unsigned char *m_fileraw;
    unsigned char *m_payload;
    unsigned char *m_message;
    char m_filename[100];
    std::queue<mosquitto_message> m_messagequeue;
    int m_maxdepth;

private:
    bool m_connected;
    bool m_published;
    bool m_validtopic;
    char m_searchterm[100];
    char *m_topic1;
    char *m_payload1;


public:


};

#endif // MQTT_H
