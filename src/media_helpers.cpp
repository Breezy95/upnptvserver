extern "C" {
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#include <string>
 
#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */
 
#define SCALE_FLAGS SWS_BICUBIC
 
// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;
 
    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;
 
    AVFrame *frame;
    AVFrame *tmp_frame;
 
    AVPacket *tmp_pkt;
 
    float t, tincr, tincr2;
 
    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;


// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;
 
    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;
 
    AVFrame *frame;
    AVFrame *tmp_frame;
 
    AVPacket *tmp_pkt;
 
    float t, tincr, tincr2;
 
    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;



static inline char* ts_make_string(char *buf, int64_t ts)
{
    if (ts == AV_NOPTS_VALUE) snprintf(buf, AV_TS_MAX_STRING_SIZE, "NOPTS");
    else                      snprintf(buf, AV_TS_MAX_STRING_SIZE, "%" PRId64, ts);
    return buf;
}

static inline char *ts_make_time_string(char *buf, int64_t ts, AVRational *tb)
{
    if (ts == AV_NOPTS_VALUE) snprintf(buf, AV_TS_MAX_STRING_SIZE, "NOPTS");
    else                      snprintf(buf, AV_TS_MAX_STRING_SIZE, "%.6g", av_q2d(*tb) * ts);
    return buf;
} 
static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{


    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    std::string s;
 
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           ts_make_string(s.data(), pkt->pts), ts_make_time_string(s.data(), pkt->pts, time_base),
           ts_make_string(s.data(), pkt->dts), ts_make_time_string(s.data(), pkt->dts, time_base),
           ts_make_string(s.data(), pkt->duration), ts_make_time_string(s.data(), pkt->duration, time_base),
           pkt->stream_index);
}