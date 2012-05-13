
// #define	DMX_OUT_DECODER0 3
// #define	DMX_OUT_DECODER1 4
// #define DMX_IN_FRONTEND0 2
// #define	DMX_IN_FRONTEND1 3

/* extensions for program stream and mpeg 1 */
#define DMX_SET_PS_MODE_FILTER   _IOW('o',80,struct dmx_pes_filter_params *)

struct dvrEvent
{
    unsigned int packet_count;
    unsigned int source;
    unsigned int trigger;
    unsigned int timestamp;
};
#define HW_EVENT_SEQUENCE_HEADER 0x01000000
#define HW_EVENT_IFRAME          0x02000000
#define HW_EVENT_PFRAME          0x04000000
#define HW_EVENT_BFRAME          0x08000000

#define DVR_MPEG1  1
#define DVR_MPEG2_PS 2
#define DVR_MPEG2_TS 3
#define DVR_SET_STREAM_TYPE    _IOW('o',81,int)
#define DVR_GET_EVENT	       _IOR('o',82,struct dvrEvent *)
#define DVR_GET_BYTESLOST	_IOR('o',83,int *)

// This one are taken from DV/Siemens backend garbage-srcs

// dvr-device can be mmap with a fixed size of 1536 KBs.
// The mmaped-area is organized as ringbuffer.
// In this structure the state of the ringbuffer is delivered

struct dvr_ring_buffer_state {
    int offset;		// offset to read from
    int readable;	// continuously readable bytes (seems to be
                        // always 128 KB)
    int fill;		// Bufferfill
};

#define DVR_GET_RING_BUFFER_STATE 3	// struct dvr_ringbuffer_state
                                        // has to be passed as pointer.
                                        // Returns 0 if some bytes are
                                        // readable

#define DMX_GET_AUDIO_SYNC_DIFF 7

struct dvr_write_buffer_state {
  int fill; // Values are obscure. Units are unknown
            // (while replaying in vdr I get 8 and 12 most of the time)
  int size;
};

#define DVR_GET_WRITE_BUFFER_STATE 7	// struct dvr_write_buffer_state
                                        // has to be passed as pointer.

