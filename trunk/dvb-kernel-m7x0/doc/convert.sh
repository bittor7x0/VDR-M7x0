#!/bin/sh
# this script will help you to convert sources written for an
# ancient version of the DVB API to the current DVB API (version 3)
# (mostly namespace cleanups)

if test -z $1 || ! test -d $1 ; then
	echo usage: $0 \<original directory\>
	exit -1
fi

echo clone $1 to $1.new

TOPDIR=`pwd`

cd $TOPDIR/$1

for x in `find . -type d` ; do
	echo mkdir -p $TOPDIR/$1.new/$x
	mkdir -p $TOPDIR/$1.new/$x
done

for x in `find . -type f` ; do 
	echo patch $TOPDIR/$1/$x
	echo \ \ \ to $TOPDIR/$1.new/$x...
	cat $TOPDIR/$1/$x | sed \
		    -e "s/_OST_OSD_H_/_DVBOSD_H_/g" \
		    -e "s/_OST_CA_H_/_CA_H_/g" \
		    -e "s/ca_slot_info_s/ca_slot_info/g" \
		    -e "s/ca_descr_info_s/ca_descr_info/g" \
		    -e "s/ca_cap_t/ca_caps_t/g" \
		    -e "s/ca_cap_s/ca_caps/g" \
		    -e "s/ca_msg_s/ca_msg/g" \
		    -e "s/ca_descr_s/ca_descr/g" \
		    -e "s/_OST_NET_H_/_DVBNET_H_/g" \
		    -e "s/_OST_AUDIO_H_/_DVBAUDIO_H_/g" \
		    -e "s/audioStreamSource_t/audio_stream_source_t/g" \
		    -e "s/audioPlayState_t/audio_play_state_t/g" \
		    -e "s/audioChannelSelect_t/audio_channel_select_t/g" \
		    -e "s/audioStatus/audio_status/g" \
		    -e "s/AVSyncState/AV_sync_state/g" \
		    -e "s/muteState/mute_state/g" \
		    -e "s/playState/play_state/g" \
		    -e "s/channelSelect/channel_select/g" \
		    -e "s/bypassMode/bypass_mode/g" \
		    -e "s/audioMixer/audio_mixer/g" \
		    -e "s/audioKaraoke/audio_karaoke/g" \
		    -e "s/audioAttributes_t/audio_attributes_t/g" \
		    -e "s/_OST_VIDEO_H_/_DVBVIDEO_H_/g" \
		    -e "s/videoFormat/video_format/g" \
		    -e "s/videoSystem_t/video_system_t/g" \
		    -e "s/videoDisplayFormat_t/video_displayformat_t/g" \
		    -e "s/videoStreamSource_t/video_stream_source_t/g" \
		    -e "s/videoPlayState_t/video_play_state_t/g" \
		    -e "s/videoEvent/video_event/g" \
		    -e "s/videoStatus/video_status/g" \
		    -e "s/videoBlank/video_blank/g" \
		    -e "s/playState/play_state/g" \
		    -e "s/streamSource/stream_source/g" \
		    -e "s/displayFormat/display_format/g" \
		    -e "s/videoDisplayStillPicture/video_still_picture/g" \
		    -e "s/videoHighlight/video_highlight/g" \
		    -e "s/videoSPUPalette/video_spu_palette/g" \
		    -e "s/videoSPU/video_spu/g" \
		    -e "s/streamID/stream_id/g" \
		    -e "s/videoNaviPack/video_navi_pack/g" \
		    -e "s/videoAttributes_t/video_attributes_t/g" \
		    -e "s/_OST_DMX_H_/_DMX_H_/g" \
		    -e "s/dmxOutput_t/dmx_output_t/g" \
		    -e "s/dmxInput_t/dmx_input_t/g" \
		    -e "s/dmxPesType_t/dmx_pes_type_t/g" \
		    -e "s/dmxScramblingStatus_t/dmx_scrambling_status_t/g" \
		    -e "s/dmxFilter/dmx_filter/g" \
		    -e "s/dmxSctFilterParams/dmx_sct_filter_params/g" \
		    -e "s/dmxPesFilterParams/dmx_pes_filter_params/g" \
		    -e "s/dmxEvent/dmx_event/g" \
		    -e "s/dmxCaps_t/dmx_caps_t/g" \
		    -e "s/dmxCaps_s/dmx_caps/g" \
		    -e "s/dmxSource_t/dmx_source_t/g" \
		    -e "s/EBUFFEROVERFLOW/EOVERFLOW/g" \
		    -e "s/dvb_pid_t/uint16_t/g" \
		    -e "s/pesType/pes_type/g" \
		    -e "s/_FRONTEND_H_/_FRONTEND_H_/g" \
		    -e "s/FrontendType/fe_type_t/g" \
		    -e "s/FrontendCapabilities/fe_caps_t/g" \
		    -e "s/SecVoltage/fe_sec_voltage_t/g" \
		    -e "s/SecToneMode/fe_sec_tone_mode_t/g" \
		    -e "s/SecMiniCmd/fe_sec_mini_cmd_t/g" \
		    -e "s/FrontendStatus/fe_status_t/g" \
		    -e "s/SpectralInversion/fe_spectral_inversion_t/g" \
		    -e "s/CodeRate/fe_code_rate_t/g" \
		    -e "s/Modulation/fe_modulation_t/g" \
		    -e "s/TransmitMode/fe_transmit_mode_t/g" \
		    -e "s/BandWidth/fe_bandwidth_t/g" \
		    -e "s/GuardInterval/fe_guard_interval_t/g" \
		    -e "s/Hierarchy/fe_hierarchy_t/g" \
		    -e "s/DmxDevInit/dvb_dmxdev_init/g" \
		    -e "s/DmxDevRelease/dvb_dmxdev_release/g" \
		    -e "s/DmxDevFile2Filter/dvb_dmxdev_file_to_filter/g" \
		    -e "s/DmxDevFile2DVR/dvb_dmxdev_file_to_dvr/g" \
		    -e "s/DmxDevBufferInit/dvb_dmxdev_buffer_init/g" \
		    -e "s/DmxDevBufferWrite/dvb_dmxdev_buffer_write/g" \
		    -e "s/DmxDevBufferRead/dvb_dmxdev_buffer_read/g" \
		    -e "s/DmxDevFilterStop/dvb_dmxdev_filter_stop/g" \
		    -e "s/DmxDevSetBufferSize/dvb_dmxdev_set_buffer_size/g" \
		    -e "s/DmxDevFilterTimeout/dvb_dmxdev_filter_timeout/g" \
		    -e "s/DmxDevFilterTimer/dvb_dmxdev_filter_timer/g" \
		    -e "s/DmxDevSectionCallback/dvb_dmxdev_section_callback/g" \
		    -e "s/DmxDevTSCallback/dvb_dmxdev_ts_callback/g" \
		    -e "s/DmxDevFeedStop/dvb_dmxdev_feed_stop/g" \
		    -e "s/DmxDevFeedStart/dvb_dmxdev_feed_start/g" \
		    -e "s/DmxDevFeedRestart/dvb_dmxdev_feed_restart/g" \
		    -e "s/DmxDevFilterStateSet/dvb_dmxdev_filter_state_set/g" \
		    -e "s/DmxDevPesFilterSet/dvb_dmxdev_pes_filter_set/g" \
		    -e "s/DmxDevFilterSet/dvb_dmxdev_filter_set/g" \
		    -e "s/DmxDevFilterReset/dvb_dmxdev_filter_reset/g" \
		    -e "s/DmxDevFilterAlloc/dvb_dmxdev_filter_alloc/g" \
		    -e "s/DmxDevFilterFree/dvb_dmxdev_filter_free/g" \
		    -e "s/DmxDevFilterStart/dvb_dmxdev_filter_start/g" \
		    -e "s/DmxDevDVRStateSet/dvb_dmxdev_dvr_state_set/g" \
		    -e "s/DmxDevReadSec/dvb_dmxdev_read_sec/g" \
		    -e "s/DvbDmxInit/dvb_dmx_init/g" \
		    -e "s/DvbDmxRelease/dvb_dmx_release/g" \
		    -e "s/DvbDmxSWFilterPacket/dvb_dmx_swfilter_packet/g"  \
		    -e "s/DvbDmxSWFilter/dvb_dmx_swfilter/g" \
		    -e "s/dvb_dmx_swfilterPayload/dvb_dmx_swfilter_payload/g" \
		    -e "s/dvb_dmx_swfilterSectionFilter/dvb_dmx_swfilter_sectionfilter/g" \
		    -e "s/dvb_dmx_swfilterSectionFeed/dvb_dmx_swfilter_section_feed/g" \
		    -e "s/dvb_dmx_swfilterSectionPacket/dvb_dmx_swfilter_section_packet/g" \
		    -e "s/dvb_dmx_swfilter_packetType/dvb_dmx_swfilter_packet_type/g" \
		    -e "s/DvbDmxFilterAlloc/dvb_dmx_filter_alloc/g" \
		    -e "s/DvbDmxFeedAlloc/dvb_dmx_feed_alloc/g" \
		    -e "s/\<init_ipack\>/dvb_filter_ipack_init/g" \
		    -e "s/\<reset_ipack\>/dvb_filter_ipack_reset/g" \
		    -e "s/\<free_ipack\>/dvb_filter_ipack_free/g" \
		    -e "s/\<send_ipack_rest\>/dvb_filter_ipack_flush/g" \
		    -e "s/\<instant_repack\>/dvb_filter_instant_repack/g" \
		    -e "s/\<pes2ts\>/dvb_filter_pes2ts/g" \
		    -e "s/\<generic_usercopy\>/video_usercopy/g" \
		    -e "s/\<dvb_device_t\>/struct dvb_device/g" \
		    -e "s/\<dvb_adapter_t\>/struct dvb_adapter/g" \
		    -e "s/\<dvb_adapter_s\>/dvb_adapter/g" \
		    -e "s/\<dvb_demux_t\>/struct dvb_demux/g" \
		    -e "s/struct dvb_demux_s/struct dvb_demux/g" \
		    -e "s/dvb_demux_filter_t/struct dvb_demux_filter/g" \
		    -e "s/struct dvb_demux_filter_s/struct dvb_demux_filter/g" \
		    -e "s/dvb_demux_feed_t/struct dvb_demux_feed/g" \
		    -e "s/struct dvb_demux_feed_s/struct dvb_demux_feed/g" \
			> $TOPDIR/$1.new/$x
	chmod --reference=$TOPDIR/$1/$x $TOPDIR/$1.new/$x
done

