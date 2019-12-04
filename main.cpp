#include <iostream>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <time.h>
// FFmpeg
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}
// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "../main.h"



#define nullptr 0

#define VERSION "version 1.0"

//#define MAX_PAYLOAD   600000
//#define MAX_TOPIC_STR  200


int main(int argc, char* argv[])
{
    char* homedir;

    char full[100];
    char mqttsrcip[100];
    char mqttdestip[100];
    int srcport = 1883;
    int destport = 1883;
    char nostring = 0;
    int paylength;
    char *payload = NULL;
    char *rawpayload = NULL;
    unsigned char *decodepayload = NULL;
    char *topic = NULL;
    base64 codec;



    if (argc < 5)
    {
        std::cout << "Usage: ffmpeg mqttsrcip mqttsrcport mqttdestip mqttdestport" << std::endl;
        return -1;
    }
    else
    {
        strcpy(mqttsrcip,argv[1]);
        srcport = atoi(argv[2]);
        strcpy(mqttdestip,argv[3]);
        destport = stoi(argv[4]);
    }

    std::cout
            << "tracker version: " << VERSION << "\n"
            << std::flush;

    yolodet det;

    srand(time(0));
    int rn = rand();
    char randomstring[100];
    sprintf(randomstring,"in_%i",rn);

    mqtt mqttall(randomstring,"#",mqttsrcip,srcport);
    mqttall.receive_queue(20,"Video");

    mqtt mqttvidout("outgoing",mqttdestip,destport);



    // bring the message in
    bool recording = false;

    unsigned nb_frames = 0;

    #ifdef WRITE_CONSEC_MP4_FILES
                VideoWriter writer;
#endif
    int payloadsize;
    while (1)
    {
        paylength = mqttall.get_next_payload_size();
        if (paylength > 0)
        {

            if (payload != NULL)
                free(payload);
            if (rawpayload != NULL)
                free(rawpayload);
            payloadsize = mqttall.get_next_payload_size();
            payload = ( char *)malloc(payloadsize+1);
            rawpayload = ( char *)malloc(payloadsize);
            if (decodepayload != NULL)
                free(decodepayload);
            decodepayload = (unsigned char *)malloc(payloadsize+1);
            if (topic != NULL)
                free(topic);
            topic = (char *)malloc(mqttall.get_next_topic_size()+1);
            mqttall.pop_payload(payload,topic);
            memcpy(rawpayload,payload,payloadsize);
            payload[payloadsize] = 0;
            // the payload contains video - need to load it from json
            char *start = (char *)strstr(payload,"video\":\"");
            start += 8;
            char *last = (char *)strstr(start,"\"");
            last[0] = 0;
            //Json::Reader reader;
            //Json::Value obj;
            //std::string fullstr = (const char *)payload;;
            //fullstr =
            //reader.parse(fullstr,obj);
            //std::string payloads = obj["video"].asString();
            int decodesize = codec.base64_decode2(start,decodepayload,payloadsize);
            //if (payload != NULL)
            //    free(payload);
            //payload = NULL;
            // make a tempfile in ram
            FILE *fid = fopen("temp.mp4","wb");
            if (fid != nullptr)
            {
                fwrite(decodepayload,decodesize,1,fid);
                fclose(fid);
                if (decodepayload != NULL)
                    free(decodepayload);
                decodepayload = NULL;
                // decode it
                av_register_all();
//  av_log_set_level(AV_LOG_DEBUG);
                int ret;



                // open input file context
                AVFormatContext* inctx = nullptr;
                AVDictionary *options = NULL;



                ret = avformat_open_input(&inctx, "temp.mp4", nullptr, &options);
                //ret = avformat_open_input(&inctx, "rtsp://admin:Unclejack@192.168.0.4/Streaming/Channels/2?tcp", nullptr, nullptr);
                //ret = avformat_open_input(&inctx, "garbage.mp4", nullptr, &options);
                if (ret < 0)
                {
                    std::cerr << "fail to avforamt_open_input(\"" <<  "\"): ret=" << ret;
                    return -2;
                }

                // retrive input stream information
                ret = avformat_find_stream_info(inctx, nullptr);
                if (ret < 0)
                {
                    std::cerr << "fail to avformat_find_stream_info: ret=" << ret;
                    return -4;
                }

                // find primary video stream
                AVCodec* vcodec = nullptr;
                ret = av_find_best_stream(inctx, AVMEDIA_TYPE_VIDEO, -1, -1, &vcodec, 0);
                if (ret < 0)
                {
                    std::cerr << "fail to av_find_best_stream: ret=" << ret;
                    return -5;
                }
                AVCodec * vcodec2 = nullptr;
                if (strstr(vcodec->name,"h264_v4l2m2m")!=nullptr)
                    vcodec2 = avcodec_find_decoder_by_name("h264");
                else
                    vcodec2 = avcodec_find_decoder_by_name(vcodec->name);
                const int vstrm_idx = ret;
                AVStream* vstrm = inctx->streams[vstrm_idx];

                // open video decoder context
                AVDictionary *options2 = NULL;
                //av_dict_set(&options2, "tune", "zerolatency", 0);
                //av_dict_set(&options2, "max_delay", "500000", 0);
                ret = avcodec_open2(vstrm->codec, vcodec2, &options2);
                if (ret < 0)
                {
                    std::cerr << "fail to avcodec_open2: ret=" << ret;
                    return -6;
                }
                if (av_dict_count(options2) != 0)
                {
                    std::cerr << "decoder option invalid" << ret;
                    return -7;
                }

                // print input video stream informataion
                std::cout

                        << "format: " << inctx->iformat->name << "\n"
                        << "vcodec: " << vcodec2->name << "\n"
                        << "size:   " << vstrm->codec->width << 'x' << vstrm->codec->height << "\n"
                        << "fps:    " << av_q2d(vstrm->codec->framerate) << " [fps]\n"
                        << "length: " << av_rescale_q(vstrm->duration, vstrm->time_base, {1,1000}) / 1000. << " [sec]\n"
                        << "pixfmt: " << av_get_pix_fmt_name(vstrm->codec->pix_fmt) << "\n"
                        << "frame:  " << vstrm->nb_frames << "\n"
                        << std::flush;

                // initialize sample scaler
                const int dst_width = 640; //vstrm->codec->width;
                const int dst_height = 480; //vstrm->codec->height;

                const AVPixelFormat dst_pix_fmt = AV_PIX_FMT_BGR24;
                SwsContext* swsctx = sws_getCachedContext(
                                         nullptr, vstrm->codec->width, vstrm->codec->height, vstrm->codec->pix_fmt,
                                         dst_width, dst_height, dst_pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
                if (!swsctx)
                {
                    std::cerr << "fail to sws_getCachedContext";
                    return -8;
                }
                std::cout << "output: " << dst_width << 'x' << dst_height << ',' << av_get_pix_fmt_name(dst_pix_fmt) << std::endl;

                // allocate frame buffer for output
                AVFrame* frame = av_frame_alloc();
                if (frame == nullptr)
                {
                    std::cerr << "fail to allocate frame";
                    return -9;
                }
                std::vector<uint8_t> framebuf(avpicture_get_size(dst_pix_fmt, dst_width, dst_height));
                avpicture_fill(reinterpret_cast<AVPicture*>(frame), framebuf.data(), dst_pix_fmt, dst_width, dst_height);

                // decoding loop
                AVFrame* decframe = av_frame_alloc();

                unsigned nb_frames2 = 0;
                unsigned nb_totframes = 0;
                unsigned nb_vehicledet = 0;
                unsigned nb_humandet = 0;
                bool end_of_stream = false;
                int got_pic = 0;
                AVPacket pkt;
#ifdef SHOW_GRAPHICS

                cv::namedWindow("Frame");

#endif

                double highconfidence = 0;
                int type = 0;
                while (1)
                {
                    if (!end_of_stream)
                    {
                        // read packet from input file
                        ret = av_read_frame(inctx, &pkt);
                        if (ret < 0 && ret != AVERROR_EOF)
                        {
                            std::cerr << "fail to av_read_frame: ret=" << ret;
                            return -10;
                        }
                        if (ret == 0 && pkt.stream_index != vstrm_idx)
                            break;
                        end_of_stream = (ret == AVERROR_EOF);
                        //std::cout << "!";
                    }
                    if (end_of_stream)
                    {
                        // null packet for bumping process
                        av_init_packet(&pkt);
                        pkt.data = nullptr;
                        pkt.size = 0;
                        //std::cout << "?";
                    }
                    // decode video frame
                    avcodec_decode_video2(vstrm->codec, decframe, &got_pic, &pkt);
                    if (!got_pic)
                        break;
                    // convert frame to OpenCV matrix
                    sws_scale(swsctx, decframe->data, decframe->linesize, 0, decframe->height, frame->data, frame->linesize);


                    cv::Mat image(dst_height, dst_width, CV_8UC3, framebuf.data(), frame->linesize[0]);
                    cv::Mat rawimage;
                    image.copyTo(rawimage);
                    double confidence;
                    int ret = det.step(image,&confidence);
                    if (confidence > highconfidence)
                    {
                        type = ret;
                        highconfidence = confidence;
                    }






#ifdef SHOW_GRAPHICS
                    det.draw(image);
                    cv::imshow("Frame", image);
#endif

#ifdef WRITE_CONSEC_MP4_FILES
                    if (nb_frames == 1)
                    {


                        char filename[100];
                        sprintf(filename,"test.mp4");

                        Size frameSize(dst_width, dst_height);
                        int fcc = cv::VideoWriter::fourcc('M', 'P','4','V');
                        //if (strlen(params.storedir.c_str()) > 0)
                        writer = VideoWriter(filename, fcc, DEFAULT_FPS, frameSize);
                        recording = true;
                    }
                    if (recording)
                        writer.write(image);
                    if (nb_frames  == 600)
                    {
                        writer.release();
                        recording = false;


                    }
#endif

                    ++nb_frames;

next_packet:

                    av_free_packet(&pkt);

                    {
                        if (cv::waitKey(1) == 0x1b)
                            break;
                    }

                }

                if (type == 1)
                {
                    std::cout << "HUMAN: CONFIDENCE :" << highconfidence << endl;
                    mqttvidout.send_message(topic,rawpayload,payloadsize);
                }
                if (type == 2)
                {
                    std::cout << "VEHICLE: CONFIDENCE :" << highconfidence << endl;
                    mqttvidout.send_message(topic,rawpayload,payloadsize);
                }




            }









        }

        else
            sleep(1);
    }

    while (1)
        sleep(10);


    return 0;


}


