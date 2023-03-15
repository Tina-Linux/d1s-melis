subdir-ccflags-y += \
    -Wno-unused-but-set-parameter \
    -Wno-psabi

subdir-ccflags-y += \
    -I$(srctree)/ekernel/components/avframework/v4l2/include \
    -I$(srctree)/ekernel/components/avframework/v4l2/include/uapi \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/system/public/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/system/public/include/utils \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/system/public/include/vo \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/system/public/libion/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/include/utils \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/include/media \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/include/media/utils \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/config \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/sample/configfileparser \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/include/utils \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/include/component \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libisp/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libisp/isp_tuning \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/AudioLib/midware/encoding/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/AudioLib/midware/encoding \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/AudioLib/midware/decoding/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/AudioLib/osal \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/textEncLib/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/include_muxer \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/include_FsWriter \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/include_stream \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libISE/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libMODSoft/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libADAS/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarx/libcore/common/iniparser \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/framework/sun8iw19p1/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/framework/sun8iw19p1/include/utils \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/framework/sun8iw19p1/include/media/camera \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/framework/sun8iw19p1/include/media/recorder \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/framework/sun8iw19p1/include/media/ise \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/framework/sun8iw19p1/include/media/player \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/framework/sun8iw19p1/include/media/thumbretriever \
    -I$(srctree)/ekernel/drivers

obj-y += \
    utils/CallbackDispatcher.o \
    utils/EyeseeMessageQueue.o \
    utils/EyeseeQueue.o \
    utils/MediaStructConvert.o \
    utils/OSAL_Mutex.o \
    utils/OSAL_Queue.o \
    utils/Thread.o \
    utils/CMediaMemory.o \
    media/camera/CallbackNotifier.o \
    media/camera/PreviewWindow.o \
    media/camera/VIChannel.o \
    media/camera/VIDevice.o \
    media/camera/CameraParameters.o \
    media/camera/CameraJpegEncoder.o \
    media/camera/EyeseeCamera.o \
    media/recorder/CameraFrameManager.o \
    media/recorder/EyeseeRecorder.o \
    media/recorder/MediaCallbackDispatcher.o \
    media/recorder/DynamicBitRateControl.o \
    media/player/EyeseePlayer.o \
    media/thumbretriever/EyeseeThumbRetriever.o

obj-$(CONFIG_mpp_ise) += \
    media/ise/EyeseeISE.o \
    media/ise/ISEChannel.o \
    media/ise/ISEDevice.o

obj-$(CONFIG_mpp_eis) += \
    media/eis/EyeseeEIS.o \
    media/eis/EISChannel.o \
    media/eis/EISDevice.o

obj-$(CONFIG_mpp_uvc) += \
    media/usbcamera/EyeseeUSBCamera.o \
    media/usbcamera/UvcChannel.o \
    media/usbcamera/VdecFrameManager.o

obj-$(CONFIG_mpp_framework_videoresizer) += \
    media/videoresizer/EyeseeVideoResizer.o \
    media/videoresizer/VREncBuffer.o

obj-y += demo/sample_Camera/sample_Camera.o
obj-y += demo/sample_EncodeResolutionChange/sample_EncodeResolutionChange.o
obj-y += demo/sample_AudioEncode/sample_AudioEncode.o
obj-y += demo/sample_Player/sample_Player.o
