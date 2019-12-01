#include "mqtt.h"


#include <string.h>
#include <stdio.h>
#include <iostream>
 #include <unistd.h>

#ifndef nullptr
#define nullptr 0
#endif // nullptr

mqtt::mqtt(const char * _id,const char * _topic, const char * _host, int _port) : mosquittopp(_id)
 {
     m_connected = false;
     m_published = true;
     m_filename[0] = 0;
     mosqpp::lib_init();        // Mandatory initialization for mosquitto library
     this->keepalive = 60;    // Basic configuration setup for myMosq class
     this->id = _id;
     this->port = _port;
     this->host = _host;
     this->topic = _topic;
     m_validtopic = true;
     if (strlen(_topic) < 1)
     {
         m_validtopic = false;
         return;
     }

     jsonfile = nullptr;
     connect_async(host,     // non blocking connection to broker request
     port,
     keepalive);
     loop_start();// Start thread managing connection / publish / subscribe
     usleep(1000000);
     m_message = nullptr;

 };

 mqtt::mqtt(const char * _id, const char * _host, int _port) : mosquittopp(_id)
 {
     m_connected = false;
     m_published = true;
     m_filename[0] = 0;
     mosqpp::lib_init();        // Mandatory initialization for mosquitto library
     this->keepalive = 60;    // Basic configuration setup for myMosq class
     this->id = _id;
     this->port = _port;
     this->host = _host;
     m_topic1 = nullptr;
     m_payload1 = nullptr;
     m_validtopic = false;


     jsonfile = nullptr;
     connect_async(host,     // non blocking connection to broker request
     port,
     keepalive);
     loop_start();// Start thread managing connection / publish / subscribe
     usleep(1000000);
     m_message = nullptr;

 };

// mqtt::mqtt(const char * _id,const char * _topic, const char * _host, int _port, const char *jsonfile) : mosquittopp(_id)
// {
//     m_connected = false;
//     m_published = true;
//     this->keepalive = 60;    // Basic configuration setup for myMosq class
//     this->id = _id;
//     this->port = _port;
//     this->host = _host;
//     this->topic = _topic;
//     m_validtopic = true;
//     if (strlen(_topic) < 1)
//     {
//         m_validtopic = false;
//         return;
//     }
//     mosqpp::lib_init();        // Mandatory initialization for mosquitto library
//
//     this->jsonfile = jsonfile;
//     connect_async(host,     // non blocking connection to broker request
//     port,
//     keepalive);
//     loop_start();            // Start thread managing connection / publish / subscribe
//     usleep(1000000);
//     m_message = nullptr;
//     //subscribe(NULL,this->topic);
//
// };


mqtt::~mqtt()
{
    while (get_next_payload_size())
    {
        pop_payload(nullptr,nullptr);
    }
    if (m_validtopic)
    {
        disconnect();
        loop_stop();            // Kill the thread
        m_connected = false;
    }
    mosqpp::lib_cleanup();    // Mosquitto library cleanup   //dtor


}

int mqtt::get_next_payload_size(void)
{
    mosquitto_message message;
    if (m_messagequeue.empty())
        return 0;
    message = m_messagequeue.front();
    return message.payloadlen;

}

int mqtt::get_next_topic_size(void)
{
    mosquitto_message message;
    if (m_messagequeue.empty())
        return 0;
    message = m_messagequeue.front();
    return strlen(message.topic);

}

int mqtt::pop_payload(void *buffer, char *topic)
{
    int length;
    mosquitto_message message;
    length = get_next_payload_size();
    if (length==0)
        return 0;
    message = m_messagequeue.front();
    m_messagequeue.pop();
    if (buffer != nullptr)
        memcpy(buffer,message.payload,length);
    if (topic != nullptr)
        strcpy(topic,message.topic);
    free(message.payload);
    free(message.topic);

    return length;

}


 bool mqtt::send_message(char *topic,  char *payload, int payloadlength)
 {
     int ret = 0;
     int filesize;

     if (m_connected == false)
        return false;
     if (m_published == false)
        return false;
     if ((m_connected == true)&&(m_published == true))
     {
         m_published = false;
         std::cout << "publishing" << std::endl;
         /*
         if (m_topic1 != nullptr)
         {
            free(m_topic1);
         }
         if (m_payload1 != nullptr)
         {
            free(m_payload1);
         }
         m_topic1 = (char *)malloc(strlen(topic));
         strcpy(m_topic1,topic);
         m_payload1 = (char *)malloc(payloadlength);
         memcpy(m_payload1,payload,payloadlength); */
         ret = publish(NULL,topic,payloadlength,payload,1,false);
     }
     else
        std::cout << "failed publishing" << std::endl;
     return ( ret == MOSQ_ERR_SUCCESS );

 }


 bool mqtt::send_message(const char *jsonfilename)
 {
     int ret = 0;
     int filesize;
     if (!m_validtopic)
        return false;
     if (m_connected == false)
        return false;
     if (m_published == false)
        return false;
     FILE *fd = fopen(jsonfilename,"rt");
     if (fd != nullptr)
     {
         fseek(fd,0,SEEK_END);
         filesize = ftell(fd);
         fseek(fd,0,SEEK_SET);
         if (m_message != nullptr)
            delete m_message;
         m_message = new unsigned char[filesize+5];
         memset(m_message,0,filesize+5);
         fread(m_message,filesize,1,fd);
         fclose(fd);
         int msg_len = strlen((char *)m_message);
         if ((m_connected == true)&&(m_published == true))
         {
             m_published = false;
             std::cout << "publishing" << std::endl;
             ret = publish(NULL,this->topic,msg_len,m_message,1,false);
         }
         else
            std::cout << "failed publishing" << std::endl;

     }
     return ( ret == MOSQ_ERR_SUCCESS );
 }

 bool mqtt::send_message(const char *macaddress, const char *trigger, const char *alert_name, const char *filetype, const char *videofilename)
 {
     int newlen;
     int ret = 0;
     //size_t newlen1;

     int filesize;
     if (!m_validtopic)
        return false;
     if (m_connected == false)
        return false;
     if (m_published == false)
        return false;

     // load video file as binary
     FILE *fd = fopen(videofilename,"rb");
     if (fd != nullptr)
     {
         fseek(fd,0,SEEK_END);
         filesize = ftell(fd);
         fseek(fd,0,SEEK_SET);
         m_payload = new unsigned char[filesize*4];
         m_fileraw = new unsigned char[filesize];
         fread(m_fileraw,filesize,1,fd);
         fclose(fd);


         base64 conv;
         //newlen = conv.base64_encode(m_payload,m_fileraw,filesize);
         newlen = conv.base64_encode2(m_payload,m_fileraw,filesize);
         //newlen = newlen1;
         //sprintf((char *)m_fileraw,"%s","Hello world");
         //newlen = conv.base64_encode(m_payload,(unsigned char *)m_fileraw,11);
         delete m_fileraw;

         // make json up
         if (m_message != nullptr)
         {
             delete m_message;
             m_message = nullptr;
         }

         m_message = new unsigned char[filesize*4];
         memset((void *)&m_message[0],0,filesize*4);
         //
         sprintf((char *)m_message,"{\n\"macaddress\":\"%s\",\n\"trigger\":\"%s\",\n\"alert_name\":\"%s\",\n\"%s\":\"",macaddress,trigger,alert_name,filetype);
         int msg_len = strlen((char *)m_message);

         //if (m_payload[newlen] == '=')
         memcpy(&m_message[msg_len],m_payload,newlen);
         m_message[msg_len+newlen] = 0;
         sprintf((char *)m_message,"%s\"\n}\n",(char *)m_message);
         msg_len += newlen;
         delete m_payload;

         msg_len += 4;
         //msg_len = strlen((char *)m_message);


         if ((m_connected == true)&&(m_published == true))
         {
             m_published = false;
             std::cout << "publishing" << std::endl;
             ret = publish(NULL,this->topic,msg_len,m_message,1,false);
         }
         else
             std::cout << "failed publishing" << std::endl;
     }

    return ( ret == MOSQ_ERR_SUCCESS );


 }

 bool mqtt::send_message(const char *macaddress, const char *trigger, const char *alert_name, unsigned frames, unsigned humans, unsigned vehicles)
 {
      int newlen;
     //size_t newlen1;
     if (!m_validtopic)
        return false;
     if (m_connected == false)
        return false;
     if (m_published == false)
        return false;

     if (m_message != nullptr)
     {
         delete m_message;
         m_message = nullptr;
     }

     m_message = new unsigned char[200];

     memset((void *)&m_message[0],0,sizeof(m_message));
     //
     sprintf((char *)m_message,"{\n\"macaddress\":\"%s\",\n\"trigger\":\"%s\",\n\"alert_name\":\"%s\",\n\"framesprocessed\":%i,\n\"humandets\":%i,\n\"vehicledets\":%i\n}\n",macaddress,trigger,alert_name,frames,humans,vehicles);
     int msg_len = strlen((char *)m_message);

     int ret = 0;

     if ((m_connected == true)&&(m_published == true))
     {
         m_published = false;
         std::cout << "publishing" << std::endl;
         ret = publish(NULL,this->topic,msg_len,m_message,1,false);
     }
     else
        std::cout << "failed publishing" << std::endl;


    return ( ret == MOSQ_ERR_SUCCESS );
 }

// bool mqtt::receive_message(const char * message)
// {
//     if (!m_validtopic)
//        return false;
//    int set = subscribe(NULL, this->topic,2);
//    return set;
// }

  bool mqtt::receive_file(const char * filename)
 {
    if (!m_validtopic)
        return false;
    strcpy(m_filename,filename);
    int set = subscribe(NULL, this->topic,2);
    return set;
 }

bool mqtt::receive_queue(int maxdepth,const char *searchterm)
 {
    if (!m_validtopic)
        return false;
    m_maxdepth = maxdepth;
    strcpy(m_searchterm,searchterm);
    int set = subscribe(NULL, this->topic,2);
    return set;
 }


 void mqtt::on_disconnect(int rc) {
    std::cout << ">> myMosq - disconnection(" << rc << ")" << std::endl;
    m_connected = false;
 }

 void mqtt::on_connect(int rc)
 {
    if ( rc == 0 ) {
        std::cout << ">> myMosq - connected with server" << std::endl;
        m_connected = true;
    } else {
        std::cout << ">> myMosq - Impossible to connect with server(" << rc << ")" << std::endl;
        m_connected = false;
    }
 }

 void mqtt::on_publish(int mid)
 {
    std::cout << ">> myMosq - Message (" << mid << ") succeed to be published " << std::endl;
    m_published = true;
 }

 void mqtt::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    std::cout << ">> subscription succeeded (" << mid << ") " << std::endl;
    //printf("Subscription succeeded.\n");
}
 void mqtt::on_message(const struct mosquitto_message *message)
{
    std::cout << ">> subscription message received on topic " << message->topic << std::endl;
    if (m_filename[0] != 0)
    {
        FILE *fid = fopen(m_filename,"wt");
        if (fid)
        {
            // transfer the contents to the file
            fprintf(fid,"%s",message->payload);
            fclose(fid);
        }
    }
    else
    {
        if (strstr(message->topic,m_searchterm) != nullptr)
        {

            mosquitto_message mymessage;
            if (m_messagequeue.size() < m_maxdepth)
            {
                mymessage = *message;
                mymessage.payload = malloc(message->payloadlen);
                memcpy(mymessage.payload,message->payload,message->payloadlen);
                mymessage.topic = (char *)malloc(strlen(message->topic)+1);
                strcpy(mymessage.topic,message->topic);
                m_messagequeue.push(mymessage);
            }
        }

    }

    //printf("Subscription succeeded.\n");
}

