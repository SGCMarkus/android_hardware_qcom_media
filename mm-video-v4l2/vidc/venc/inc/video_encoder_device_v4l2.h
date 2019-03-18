/*--------------------------------------------------------------------------
Copyright (c) 2012-2019, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------*/
#ifndef __OMX_VENC_DEV__
#define __OMX_VENC_DEV__

#include "OMX_Types.h"
#include "OMX_Core.h"
#include "OMX_VideoExt.h"
#include "QComOMXMetadata.h"
#include "OMX_QCOMExtns.h"
#include "qc_omx_component.h"

#include "omx_video_common.h"
#include "omx_video_base.h"
#include "omx_video_encoder.h"
#include "vidc_common.h"
#include <linux/videodev2.h>
#include "media/msm_vidc_utils.h"
#include <poll.h>
#include <list>
#include <qdMetaData.h>
#include "color_metadata.h"

#define TIMEOUT 5*60*1000
#define BIT(num) (1 << (num))
#define MAX_HYB_HIERP_LAYERS 6
#define MAX_AVC_HP_LAYERS (4)
#define MAX_V4L2_BUFS 64 //VB2_MAX_FRAME
#define ENABLE_I_QP 0x1
#define ENABLE_P_QP 0x2
#define ENABLE_B_QP 0x4

#define OMX_VIDEO_LEVEL_UNKNOWN 0

#define VENC_BFRAME_MAX_COUNT       1
#define VENC_BFRAME_MAX_FPS         60
#define VENC_BFRAME_MAX_WIDTH       1920
#define VENC_BFRAME_MAX_HEIGHT      1088
#define VENC_INFINITE_GOP 0xFFFFFFF

/* TODO: Use sanctioned vendor bits for HEIF
 * once end to end 64-bit support is available.
 */
#define GRALLOC_USAGE_PRIVATE_HEIF_VIDEO (UINT32_C(1) << 27)
#define GRALLOC_USAGE_PRIVATE_10BIT_VIDEO (UINT32_C(1) << 30)

#define REQUEST_LINEAR_COLOR_8_BIT   0x1
#define REQUEST_LINEAR_COLOR_10_BIT  0x2
#define REQUEST_LINEAR_COLOR_ALL     (REQUEST_LINEAR_COLOR_8_BIT | REQUEST_LINEAR_COLOR_10_BIT)

enum hier_type {
    HIER_NONE = 0x0,
    HIER_P = 0x1,
    HIER_B = 0x2,
};

struct msm_venc_switch {
    unsigned char    status;
};

struct msm_venc_allocatorproperty {
    unsigned long     mincount;
    unsigned long     actualcount;
    unsigned long     datasize;
    unsigned long     suffixsize;
    unsigned long     alignment;
    unsigned long     bufpoolid;
};

struct msm_venc_basecfg {
    unsigned long    input_width;
    unsigned long    input_height;
    unsigned long    dvs_width;
    unsigned long    dvs_height;
    unsigned long    codectype;
    unsigned long    fps_num;
    unsigned long    fps_den;
    unsigned long    targetbitrate;
    unsigned long    inputformat;
};

struct msm_venc_profile {
    unsigned long    profile;
};
struct msm_venc_profilelevel {
    unsigned long    level;
    unsigned long    tier;
};

struct msm_venc_sessionqp {
    unsigned long    iframeqp;
    unsigned long    pframeqp;
    unsigned long    bframeqp;
    unsigned long    enableqp;
};

struct msm_venc_ipb_qprange {
    unsigned long    max_qp_packed;
    unsigned long    min_qp_packed;
};

struct msm_venc_intraperiod {
    unsigned long    num_pframes;
    unsigned long    num_bframes;
};
struct msm_venc_seqheader {
    unsigned char *hdrbufptr;
    unsigned long    bufsize;
    unsigned long    hdrlen;
};

struct msm_venc_capability {
    unsigned long    codec_types;
    unsigned long    maxframe_width;
    unsigned long    maxframe_height;
    unsigned long    maxtarget_bitrate;
    unsigned long    maxframe_rate;
    unsigned long    input_formats;
    unsigned char    dvs;
};

struct msm_venc_entropycfg {
    unsigned longentropysel;
    unsigned long    cabacmodel;
};

struct msm_venc_dbcfg {
    unsigned long    db_mode;
    unsigned long    slicealpha_offset;
    unsigned long    slicebeta_offset;
};

struct msm_venc_intrarefresh {
    unsigned long    irmode;
    unsigned long    mbcount;
    unsigned long    framecount;
};

struct msm_venc_multiclicecfg {
    unsigned long    mslice_mode;
    unsigned long    mslice_size;
};

struct msm_venc_bufferflush {
    unsigned long    flush_mode;
};

struct msm_venc_ratectrlcfg {
    unsigned long    rcmode;
};

struct    msm_venc_voptimingcfg {
    unsigned long    voptime_resolution;
};
struct msm_venc_framerate {
    unsigned long    fps_denominator;
    unsigned long    fps_numerator;
};

struct msm_venc_targetbitrate {
    unsigned long    target_bitrate;
};


struct msm_venc_rotation {
    unsigned long    rotation;
};

struct msm_venc_timeout {
    unsigned long    millisec;
};

struct msm_venc_headerextension {
    unsigned long    header_extension;
};

struct msm_venc_video_capability {
    unsigned int min_width;
    unsigned int max_width;
    unsigned int min_height;
    unsigned int max_height;
};

struct msm_venc_idrperiod {
    unsigned long idrperiod;
};

struct msm_venc_vui_timing_info {
    unsigned int enabled;
};

struct msm_venc_peak_bitrate {
    unsigned int peakbitrate;
};

struct msm_venc_vpx_error_resilience {
    unsigned int enable;
};

struct msm_venc_priority {
    OMX_U32 priority;
};

struct msm_venc_color_space {
    OMX_U32 primaries;
    OMX_U32 range;
    OMX_U32 matrix_coeffs;
    OMX_U32 transfer_chars;
};

struct msm_venc_temporal_layers {
    enum hier_type hier_mode;
    OMX_U32 nMaxLayers;
    OMX_U32 nMaxBLayers;
    OMX_U32 nPLayers;
    OMX_U32 nBLayers;
    OMX_BOOL bIsBitrateRatioValid;
    // cumulative ratio: eg [25, 50, 75, 100] means [L0=25%, L1=25%, L2=25%, L3=25%]
    OMX_U32 nTemporalLayerBitrateRatio[OMX_VIDEO_ANDROID_MAXTEMPORALLAYERS];
    OMX_VIDEO_ANDROID_TEMPORALLAYERINGPATTERNTYPE ePattern;
};

enum v4l2_ports {
    CAPTURE_PORT,
    OUTPUT_PORT,
    MAX_PORT
};

struct extradata_buffer_info {
    unsigned long buffer_size;
    int count;
    OMX_BOOL allocated;
    enum v4l2_ports port_index;
#ifdef USE_ION
    struct venc_ion ion[VIDEO_MAX_FRAME];
#endif
};

struct statistics {
    struct timeval prev_tv;
    int prev_fbd;
    int bytes_generated;
};

enum rc_modes {
    RC_VBR_VFR = BIT(0),
    RC_VBR_CFR = BIT(1),
    RC_CBR_VFR = BIT(2),
    RC_CBR_CFR = BIT(3),
    RC_MBR_CFR = BIT(4),
    RC_MBR_VFR = BIT(5),
    RC_CQ      = BIT(6),
    RC_ALL = (RC_VBR_VFR | RC_VBR_CFR
        | RC_CBR_VFR | RC_CBR_CFR | RC_MBR_CFR | RC_MBR_VFR | RC_CQ)
};

class venc_dev
{
    public:
        venc_dev(class omx_venc *venc_class); //constructor
        ~venc_dev(); //des

        static void* async_venc_message_thread (void *);
        bool venc_open(OMX_U32);
        void venc_close();
        unsigned venc_stop(void);
        unsigned venc_pause(void);
        bool is_streamon_done(OMX_U32 port);
        unsigned venc_start(void);
        unsigned venc_flush(unsigned);
#ifdef _ANDROID_ICS_
        bool venc_set_meta_mode(bool);
#endif
        unsigned venc_resume(void);
        unsigned venc_start_done(void);
        unsigned venc_stop_done(void);
        unsigned venc_set_message_thread_id(pthread_t);
        bool allocate_extradata(unsigned);
        bool venc_free_buf(void*, unsigned);
        bool venc_empty_buf(void *, void *,unsigned,unsigned);
        bool venc_fill_buf(void *, void *,unsigned,unsigned);
        bool venc_get_buf_req(OMX_U32 *,OMX_U32 *,
                OMX_U32 *,OMX_U32);
        bool venc_set_buf_req(OMX_U32 *,OMX_U32 *,
                OMX_U32 *,OMX_U32);
        bool venc_set_param(void *,OMX_INDEXTYPE);
        bool venc_set_config(void *configData, OMX_INDEXTYPE index);
        bool venc_h264_transform_8x8(OMX_BOOL enable);
        bool venc_get_profile_level(OMX_U32 *eProfile,OMX_U32 *eLevel);
        bool venc_get_seq_hdr(void *, unsigned, unsigned *);
        bool venc_get_dimensions(OMX_U32 portIndex, OMX_U32 *w, OMX_U32 *h);
        bool venc_loaded_start(void);
        bool venc_loaded_stop(void);
        bool venc_handle_empty_eos_buffer(void);
        bool venc_loaded_start_done(void);
        bool venc_loaded_stop_done(void);
        bool venc_is_video_session_supported(unsigned long width, unsigned long height);
        bool venc_color_align(OMX_BUFFERHEADERTYPE *buffer, OMX_U32 width,
                        OMX_U32 height);
        bool venc_get_vui_timing_info(OMX_U32 *enabled);
        bool venc_get_peak_bitrate(OMX_U32 *peakbitrate);
        bool venc_get_batch_size(OMX_U32 *size);
        bool venc_get_temporal_layer_caps(OMX_U32 * /*nMaxLayers*/,
                OMX_U32 * /*nMaxBLayers*/, OMX_VIDEO_ANDROID_TEMPORALLAYERINGPATTERNTYPE */*SupportedPattern*/);
        OMX_ERRORTYPE venc_get_supported_profile_level(OMX_VIDEO_PARAM_PROFILELEVELTYPE *profileLevelType);
        bool venc_get_supported_color_format(unsigned index, OMX_U32 *colorFormat);
        bool venc_check_for_hybrid_hp(OMX_VIDEO_ANDROID_TEMPORALLAYERINGPATTERNTYPE ePattern);
        OMX_ERRORTYPE venc_set_max_hierp_layer();
        OMX_ERRORTYPE venc_set_hierp_layer();
        OMX_ERRORTYPE venc_set_bitrate_ratios();
        bool venc_validate_temporal_extn(OMX_VIDEO_PARAM_ANDROID_TEMPORALLAYERINGTYPE &temporalSettings);
        bool venc_get_output_log_flag();
        int venc_output_log_buffers(const char *buffer_addr, int buffer_len, uint64_t timestamp);
        int venc_input_log_buffers(OMX_BUFFERHEADERTYPE *buffer, int fd, int plane_offset,
                        unsigned long inputformat, bool interlaced);
        int venc_extradata_log_buffers(char *buffer_addr, int index, bool input);
        bool venc_get_hevc_profile(OMX_U32* profile);
        void venc_get_consumer_usage(OMX_U32* usage);

        struct venc_debug_cap m_debug;
        OMX_U32 m_nDriver_fd;
        int m_poll_efd;
        int num_input_planes, num_output_planes;
        int etb, ebd, ftb, fbd;

        int nPframes_cache;
        int stopped;
        int resume_in_stopped;
        pthread_t m_tid;
        bool async_thread_created;
        bool async_thread_force_stop;
        class omx_venc *venc_handle;
        OMX_ERRORTYPE allocate_extradata(struct extradata_buffer_info *extradata_info, int flags);
        void free_extradata_all();
        void free_extradata(struct extradata_buffer_info *extradata_info);
        void append_extradata_ltrinfo(OMX_OTHER_EXTRADATATYPE *, struct msm_vidc_extradata_header *);
        void append_extradata_none(OMX_OTHER_EXTRADATATYPE *);
        bool handle_output_extradata(void *, int);
        bool handle_input_extradata(struct v4l2_buffer);
        bool venc_handle_client_input_extradata(void *);
        int venc_set_format(int);
        bool hw_overload;
        bool is_gralloc_source_ubwc;
        bool is_camera_source_ubwc;
        bool is_csc_custom_matrix_enabled;
        bool csc_enable;
        OMX_U32 fd_list[64];

    private:
        OMX_U32                             m_codec;
        struct msm_venc_basecfg             m_sVenc_cfg;
        struct msm_venc_rotation            m_rotation;
        struct msm_venc_ratectrlcfg         rate_ctrl;
        struct msm_venc_targetbitrate       bitrate;
        struct msm_venc_intraperiod         intra_period;
        struct msm_venc_profile             codec_profile;
        struct msm_venc_profilelevel        profile_level;
        struct msm_venc_switch              set_param;
        struct msm_venc_voptimingcfg        time_inc;
        struct msm_venc_allocatorproperty   m_sInput_buff_property;
        struct msm_venc_allocatorproperty   m_sOutput_buff_property;
        struct msm_venc_sessionqp           session_qp;
        struct msm_venc_ipb_qprange         session_ipb_qp_values;
        struct msm_venc_multiclicecfg       multislice;
        struct msm_venc_entropycfg          entropy;
        struct msm_venc_dbcfg               dbkfilter;
        struct msm_venc_intrarefresh        intra_refresh;
        struct msm_venc_headerextension     hec;
        struct msm_venc_voptimingcfg        voptimecfg;
        struct msm_venc_video_capability    capability;
        struct msm_venc_idrperiod           idrperiod;
        struct msm_venc_vui_timing_info     vui_timing_info;
        struct msm_venc_peak_bitrate        peak_bitrate;
        struct msm_venc_vpx_error_resilience vpx_err_resilience;
        struct msm_venc_priority            sess_priority;
        OMX_U32                             operating_rate;
        struct msm_venc_color_space         color_space;
        msm_venc_temporal_layers            temporal_layers_config;
        OMX_BOOL                            downscalar_enabled;
        bool bframe_implicitly_enabled;
        bool client_req_disable_temporal_layers;

        bool venc_query_cap(struct v4l2_queryctrl &cap);
        bool venc_validate_range(OMX_S32 id, OMX_S32 val);
        bool venc_set_profile(OMX_U32 eProfile);
        bool venc_set_level(OMX_U32 eLevel);
        bool set_nB_frames(unsigned long nBframes);
        bool set_native_recoder(bool enable);
        bool set_nP_frames(unsigned long nPframes);
        bool venc_set_target_bitrate(OMX_U32 nTargetBitrate);
        bool venc_set_ratectrl_cfg(OMX_VIDEO_CONTROLRATETYPE eControlRate);
        bool venc_set_session_qp_range(OMX_QCOM_VIDEO_PARAM_IPB_QPRANGETYPE *qp_range);
        bool venc_set_encode_framerate(OMX_U32 encode_framerate);
        bool venc_set_intra_vop_refresh(OMX_BOOL intra_vop_refresh);
        bool venc_set_color_format(OMX_COLOR_FORMATTYPE color_format);
        bool venc_validate_profile_level(OMX_U32 *eProfile, OMX_U32 *eLevel);
        bool venc_set_multislice_cfg(OMX_U32 slicemode, OMX_U32 slicesize);
        bool venc_set_entropy_config(OMX_BOOL enable, OMX_U32 i_cabac_level);
        bool venc_set_inloop_filter(OMX_VIDEO_AVCLOOPFILTERTYPE loop_filter);
        bool venc_set_intra_refresh ();
        bool venc_set_error_resilience(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE* error_resilience);
        bool venc_set_voptiming_cfg(OMX_U32 nTimeIncRes);
        void venc_config_print();
        bool venc_set_extradata(OMX_U32 extra_data, OMX_BOOL enable);
        bool venc_reconfig_reqbufs();
        bool venc_set_vpe_rotation(OMX_S32 rotation_angle);
        bool venc_set_mirror(OMX_MIRRORTYPE mirror);
        bool venc_set_ltrcount(OMX_U32 count);
        bool venc_set_useltr(OMX_U32 frameIdx);
        bool venc_set_markltr(OMX_U32 frameIdx);
        bool venc_set_inband_video_header(OMX_BOOL enable);
        bool venc_set_au_delimiter(OMX_BOOL enable);
        bool venc_set_hier_layers(QOMX_VIDEO_HIERARCHICALCODINGTYPE type, OMX_U32 num_layers);
        bool venc_set_vui_timing_info(OMX_BOOL enable);
        bool venc_set_peak_bitrate(OMX_U32 nPeakBitrate);
        bool venc_set_vpx_error_resilience(OMX_BOOL enable);
        bool venc_set_batch_size(OMX_U32 size);
        bool venc_calibrate_gop();
        bool venc_get_index_from_fd(OMX_U32 buffer_fd, OMX_U32 *index);
        bool venc_set_hierp_layers(OMX_U32 hierp_layers);
        bool venc_set_baselayerid(OMX_U32 baseid);
        bool venc_set_qp(OMX_U32 i_frame_qp, OMX_U32 p_frame_qp,OMX_U32 b_frame_qp, OMX_U32 enable);
        bool venc_set_aspectratio(void *nSar);
        bool venc_set_priority(OMX_U32 priority);
        bool venc_set_session_priority(OMX_U32 priority);
        bool venc_set_operatingrate(OMX_U32 rate);
        bool venc_set_lowlatency_mode(OMX_BOOL enable);
        bool venc_set_roi_qp_info(OMX_QTI_VIDEO_CONFIG_ROIINFO *roiInfo);
        bool venc_set_blur_resolution(OMX_QTI_VIDEO_CONFIG_BLURINFO *blurInfo);
        bool venc_set_colorspace(OMX_U32 primaries, OMX_U32 range, OMX_U32 transfer_chars, OMX_U32 matrix_coeffs);
        bool venc_set_iframesize_type(QOMX_VIDEO_IFRAMESIZE_TYPE type);
        unsigned long venc_get_color_format(OMX_COLOR_FORMATTYPE eColorFormat);
        unsigned long venc_get_codectype(OMX_VIDEO_CODINGTYPE eCompressionFormat);
        bool venc_set_nal_size (OMX_VIDEO_CONFIG_NALSIZE *nalSizeInfo);
        bool venc_set_grid_enable();
        bool venc_set_extradata_hdr10metadata();
        bool venc_store_dynamic_config(OMX_INDEXTYPE type, OMX_PTR config);
        bool venc_cvp_enable(private_handle_t *handle);
        bool venc_get_cvp_metadata(private_handle_t *handle);

        OMX_U32 pmem_free();
        OMX_U32 pmem_allocate(OMX_U32 size, OMX_U32 alignment, OMX_U32 count);
        OMX_U32 venc_allocate_recon_buffers();

        int metadatamode;
        bool streaming[MAX_PORT];
        bool extradata;
        struct extradata_buffer_info input_extradata_info;
        struct extradata_buffer_info output_extradata_info;
        bool m_hdr10meta_enabled;
        ColorMetaData colorData= {};

        bool m_cvp_meta_enabled;
        CVPMetadata cvpMetadata;
        pthread_mutex_t pause_resume_mlock;
        pthread_cond_t pause_resume_cond;
        bool paused;
        int color_format;
        bool camera_mode_enabled;
        bool low_latency_mode;
        struct roidata {
            bool dirty;
            OMX_QTI_VIDEO_CONFIG_ROIINFO info;
        };
        bool m_roi_enabled;
        pthread_mutex_t m_roilock;
        std::list<roidata> m_roilist;
        void get_roi_for_timestamp(struct roidata &roi, OMX_TICKS timestamp);
        bool venc_empty_batch (OMX_BUFFERHEADERTYPE *buf, unsigned index);
        static const int kMaxBuffersInBatch = 16;
        unsigned int mBatchSize;
        struct BatchInfo {
            BatchInfo();
            /* register a buffer and obtain its unique id (v4l2-buf-id)
             */
            int registerBuffer(int bufferId);
            /* retrieve the buffer given its v4l2-buf-id
             */
            int retrieveBufferAt(int v4l2Id);
            bool isPending(int bufferId);

          private:
            static const int kMaxBufs = 64;
            static const int kBufIDFree = -1;
            pthread_mutex_t mLock;
            int mBufMap[64];  // Map with slots for each buffer
            size_t mNumPending;

        };
        BatchInfo mBatchInfo;
        bool mUseAVTimerTimestamps;
        bool venc_set_hdr_info(const MasteringDisplay&, const ContentLightLevel&);
        bool mIsGridset;
        OMX_U32 mUseLinearColorFormat;

        union dynamicConfigData {
            OMX_VIDEO_CONFIG_BITRATETYPE bitrate;
            OMX_CONFIG_FRAMERATETYPE framerate;
            QOMX_VIDEO_INTRAPERIODTYPE intraperiod;
            OMX_CONFIG_INTRAREFRESHVOPTYPE intravoprefresh;
            OMX_CONFIG_ROTATIONTYPE rotation;
            OMX_VIDEO_VP8REFERENCEFRAMETYPE vp8refframe;
            OMX_QCOM_VIDEO_CONFIG_LTRMARK_TYPE markltr;
            OMX_QCOM_VIDEO_CONFIG_LTRUSE_TYPE useltr;
            OMX_SKYPE_VIDEO_CONFIG_QP configqp;
            OMX_VIDEO_CONFIG_ANDROID_TEMPORALLAYERINGTYPE temporallayer;
            OMX_SKYPE_VIDEO_CONFIG_BASELAYERPID configbaselayerid;
        };
        struct dynamicConfig {
            OMX_TICKS timestamp;
            unsigned int type;
            union dynamicConfigData config_data;
        };
        pthread_mutex_t m_configlock;
        std::list<dynamicConfig> m_configlist;
        bool handle_dynamic_config(OMX_BUFFERHEADERTYPE *bufferHdr);
        bool venc_config_bitrate(OMX_VIDEO_CONFIG_BITRATETYPE *bit_rate);
        bool venc_config_framerate(OMX_CONFIG_FRAMERATETYPE *frame_rate);
        bool venc_config_intravoprefresh(OMX_CONFIG_INTRAREFRESHVOPTYPE *intra_vop_refresh);
        bool venc_config_vp8refframe(OMX_VIDEO_VP8REFERENCEFRAMETYPE *vp8refframe);
        bool venc_config_markLTR(OMX_QCOM_VIDEO_CONFIG_LTRMARK_TYPE *markltr);
        bool venc_config_useLTR(OMX_QCOM_VIDEO_CONFIG_LTRUSE_TYPE *useltr);
        bool venc_config_qp(OMX_SKYPE_VIDEO_CONFIG_QP *configqp);
};

enum instance_state {
    MSM_VIDC_CORE_UNINIT_DONE = 0x0001,
    MSM_VIDC_CORE_INIT,
    MSM_VIDC_CORE_INIT_DONE,
    MSM_VIDC_OPEN,
    MSM_VIDC_OPEN_DONE,
    MSM_VIDC_LOAD_RESOURCES,
    MSM_VIDC_LOAD_RESOURCES_DONE,
    MSM_VIDC_START,
    MSM_VIDC_START_DONE,
    MSM_VIDC_STOP,
    MSM_VIDC_STOP_DONE,
    MSM_VIDC_RELEASE_RESOURCES,
    MSM_VIDC_RELEASE_RESOURCES_DONE,
    MSM_VIDC_CLOSE,
    MSM_VIDC_CLOSE_DONE,
    MSM_VIDC_CORE_UNINIT,
};
#endif

