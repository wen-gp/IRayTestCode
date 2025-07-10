#include "sample.h"
#include "config.h"

#include "libirparse.h"
#include "libircmd.h"
#include "libiruvc.h"

IruvcHandle_t* iruvc_handle = NULL;

IrlibError_e auto_image_set(StreamFrameInfo_t stream_frame_info)
{
    int ret;
    char device_name[64] = { 0 };
    IrcmdHandle_t* handle = stream_frame_info.ircmd_handle;
    VideoOutputInfo_t output_info;
    output_info.video_output_status = BASIC_ENABLE;
    ret = basic_device_info_get(handle, BASIC_DEV_NAME, device_name);
    printf("basic_device_info_get:%d\n", ret);
    //set ouput format
    if(stream_frame_info.product_config.camera.image_channel_type == "dvp")
    {
        output_info.video_output_format = ADV_DVP_FORMAT;
    }
    else if(stream_frame_info.product_config.camera.image_channel_type == "mipi")
    {
        output_info.video_output_format = ADV_MIPI_FORMAT;
    }
    else
    {
        output_info.video_output_format = ADV_USB_FORMAT;
    }

    //set output fps
    output_info.video_output_fps = stream_frame_info.product_config.camera.v4l2_config.image_stream.fps;

    if((strcmp(device_name, "WN2256") == 0) || \
            (strcmp(device_name, "WN2640") == 0) || \
            (strcmp(device_name, "WN2384") == 0))
    {
        printf("WN2 auto image\n");
        ret = adv_digital_video_output_set(handle, output_info);
        printf("adv_digital_video_output_set:%d\n", ret);
    }
    else if((strcmp(device_name, "MINI640S266-12A30009D10X") == 0) || \
            (strcmp(device_name, "G1280S") == 0))
    {
        printf("CS640 or G1280s auto image\n");
        ret = basic_video_stream_continue(handle);
        printf("basic_video_stream_continue:%d\n", ret);    //control mipi start
    }
    else
    {
        printf("not support device\n");
        return IRCMD_PARAM_ERROR;
    }
    return IRLIB_SUCCESS;
}

int main(int argc, char* argv[])
{
    printf("uart cmd and usb image sample start\n");

    config config_obj;
    char* config_path;
    if (argc != 2)
    {
        printf("usage: %s <path_of_config_file>\n", argv[0]);
        return -1;
    }

    config_path = argv[1];

    if (config_obj.parse_config(config_path) != 0)
    {
        printf("parse config failed\n");
        return -1;
    }

    single_config product_config;
    if (false == config_obj.get_config(product_config))
    {
        printf("can't find config\n");
        return -1;
    }

    printf("Depends lib version:\n");
    printf("version:%s\n", ircmd_version());
    printf("version:%s\n", ircam_version());
    printf("version:%s\n", iruart_version());
    printf("version:%s\n", iruvc_version());

    printf("set libs log level:\n");
    ircmd_log_register(IRCMD_LOG_ERROR,NULL,NULL);
    iruart_log_register(IRUART_LOG_DEBUG,NULL,NULL);
    ircam_log_register(IRCAM_LOG_ERROR,NULL, NULL);
    iruvc_log_register(IRUVC_LOG_ERROR,NULL,NULL);

    printf("init all stream handle\n");
    int fd;
    int ret;
    StreamFrameInfo_t stream_frame_info;
    memset(&stream_frame_info, 0, sizeof(stream_frame_info) - sizeof(product_config));
    stream_frame_info.product_config = product_config;

    printf("init uvc stream handle\n");
    ir_video_handle_create(&ir_image_video_handle);
    iruvc_handle = (IruvcHandle_t*)iruvc_camera_handle_create(ir_image_video_handle);//创建uvc_handle
    stream_frame_info.image_driver_handle = iruvc_handle;
    IruvcDevParam_t dev_param;
    memset(&dev_param, 0, sizeof(IruvcDevParam_t));

    dev_param.pid = product_config.camera.uvc_stream_conf.dev_info.pid;
    dev_param.vid = product_config.camera.uvc_stream_conf.dev_info.vid;
    dev_param.same_idx = product_config.camera.uvc_stream_conf.dev_info.same_id;

    stream_frame_info.image_dev_params = &dev_param;
    ret = ir_image_video_handle->ir_video_open(stream_frame_info.image_driver_handle, &dev_param);
    printf("open video : %d\n", ret);
    ret = ir_image_video_handle->ir_video_init(stream_frame_info.image_driver_handle, &dev_param);
    printf("init video : %d\n", ret);
    IruvcCamStreamParams_t params;
    memset(&params, 0, sizeof(params));
    stream_frame_info.stream_config = &params;
    
    printf("init control handle\n");
    IrControlHandle_t* ir_control_handle = NULL;
    ir_control_handle_create(&ir_control_handle);
    if (product_config.control.is_usb_control || product_config.control.is_i2c_usb_control)
    {
        if (product_config.control.is_usb_control)
        {
            iruvc_usb_handle_create_with_exist_instance(ir_control_handle, iruvc_handle);
        }
        else
        {
            iruvc_i2c_usb_handle_create_with_exist_instance(ir_control_handle, iruvc_handle);
        }

        do
        {
            if (dev_param.pid == product_config.control.usb_param.pid
                && dev_param.vid == product_config.control.usb_param.vid
                && dev_param.same_idx == product_config.control.usb_param.same_id)
            {
                break;
            }
            IruvcDevParam_t control_dev_param;
            memset(&control_dev_param, 0, sizeof(IruvcDevParam_t));
            control_dev_param.pid = product_config.control.usb_param.pid;
            control_dev_param.vid = product_config.control.usb_param.vid;
            control_dev_param.same_idx = product_config.control.usb_param.same_id;
            ret = ir_control_handle->ir_control_open(iruvc_handle, &control_dev_param);
            printf("open control node: %d\n", ret);
        } while (0);

        stream_frame_info.ircmd_handle = ircmd_create_handle(ir_control_handle);
    }
    else if (product_config.control.is_uart_control)
    {
        IruartHandle_t* iruart_handle = NULL;
        iruart_handle = iruart_handle_create(ir_control_handle);
        IrcmdHandle_t* cmd_handle = NULL;
        ret = search_com(ir_control_handle, &cmd_handle, product_config.control.uart_param.com_index);
        if (ret != 0)
        {
            printf("fail to find available com\n");
            return -1;
        }
        stream_frame_info.ircmd_handle = cmd_handle;
    }

    load_stream_frame_info(&stream_frame_info, true, true);
    init_pthread_sem();
    pthread_t stream_thread,display_thread,capture_thread,cmd_thread;
    pthread_create(&stream_thread, NULL, uvc_stream_function, &stream_frame_info);
    //pthread_create(&display_thread, NULL, opencv_display_function, &stream_frame_info);
    //pthread_create(&capture_thread, NULL, capture_function, &stream_frame_info);
    pthread_create(&cmd_thread, NULL, cmd_function, &stream_frame_info);
    sleep(1);

    printf("in streaming\n");
    void *thread_result;
    pthread_join(stream_thread, &thread_result);
    printf("stop stream\n");
    //pthread_cancel(display_thread);
    //pthread_cancel(cmd_thread);
    destroy_pthread_sem();
    destroy_data_demo(&stream_frame_info);

    if (product_config.control.is_usb_control || product_config.control.is_i2c_usb_control)
    {
        ircmd_delete_handle(stream_frame_info.ircmd_handle);
        stream_frame_info.ircmd_handle = NULL;
        ir_control_handle->ir_control_release(ir_control_handle->ir_control_handle, NULL);
        ir_control_handle->ir_control_close(ir_control_handle->ir_control_handle);
        if (product_config.control.is_usb_control)
        {
            iruvc_usb_handle_delete(iruvc_handle);
        }
        else
        {
            iruvc_i2c_usb_handle_delete(iruvc_handle);
        }
    }
    else if (product_config.control.is_uart_control)
    {
        ircmd_delete_handle(stream_frame_info.ircmd_handle);
        stream_frame_info.ircmd_handle = NULL;
        ir_control_handle->ir_control_release(ir_control_handle->ir_control_handle, NULL);
        ir_control_handle->ir_control_close(ir_control_handle->ir_control_handle);
        iruart_handle_delete(ir_control_handle);
    }

    ir_control_handle_delete(&ir_control_handle);
    ir_control_handle = NULL;
    ir_image_video_handle->ir_video_release(stream_frame_info.image_driver_handle, &dev_param);
    ir_image_video_handle->ir_video_close(stream_frame_info.image_driver_handle);
    iruvc_camera_handle_delete(iruvc_handle);
    iruvc_handle = NULL;
    ir_video_handle_delete(&ir_image_video_handle);
    ir_image_video_handle = NULL;

    return 0;
}