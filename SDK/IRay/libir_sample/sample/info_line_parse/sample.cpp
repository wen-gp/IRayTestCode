#include "sample.h"
#include "config.h"

int main(int argc, char* argv[])
{
    printf("infoline parse sample start\n");
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

    printf("set libs log level:\n");
    ircmd_log_register(IRCMD_LOG_ERROR,NULL,NULL);
    iruart_log_register(IRUART_LOG_DEBUG,NULL,NULL);
    irv4l2_log_register(IRV4L2_LOG_ERROR,NULL,NULL);

    printf("init all stream handle\n");
    int fd;
    int ret;
    StreamFrameInfo_t stream_frame_info;
    memset(&stream_frame_info, 0, sizeof(stream_frame_info) - sizeof(product_config));
    stream_frame_info.product_config = product_config;
    if (product_config.camera.v4l2_config.has_image)
    {
        printf("create image video handle\n");
        stream_frame_info.image_name = (char*)product_config.camera.v4l2_config.image_stream.device_name.data();
        ir_video_handle_create(&ir_image_video_handle);
        stream_frame_info.image_driver_handle = (Irv4l2VideoHandle_t*)irv4l2_handle_create(ir_image_video_handle);
    }
    if (product_config.camera.v4l2_config.has_temp)
    {
        printf("creat temp video handle\n");
        stream_frame_info.temp_name = (char*)product_config.camera.v4l2_config.temp_stream.device_name.data();
        ir_video_handle_create(&ir_temp_video_handle);
        stream_frame_info.temp_driver_handle = (Irv4l2VideoHandle_t*)irv4l2_handle_create(ir_temp_video_handle);
    }
    if (!product_config.camera.v4l2_config.has_image && !product_config.camera.v4l2_config.has_temp)
    {
        printf("not found image and temp stream config\n");
        return -1;
    }

    printf("init control handle\n");
    IrControlHandle_t* ir_control_handle = NULL;
    ir_control_handle_create(&ir_control_handle);
    IruartHandle_t* iruart_handle = NULL;
    iruart_handle = iruart_handle_create(ir_control_handle);

    FILE* fp;
    char buffer[80] = { 0 };
    char dev_node[20] = "/dev/";
    fp = popen("ls /dev | grep \"ttyUSB*\" ", "r");
    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);
    fp = NULL;
    printf("content of ls /dev | grep \"ttyUSB*\" is %s\n", buffer);
    strcat(dev_node, buffer);
    printf("dev_node is %s\n", dev_node);

    int i;
    for (i = sizeof(dev_node) - 1; i < (int)(sizeof(dev_node)); i--)
    {
        if (dev_node[i] == '\n')
        {
            dev_node[i] = 0;
            break;
        }
    }
    ret = ir_control_handle->ir_control_open(iruart_handle, dev_node);
    ir_control_handle->ir_control_init(iruart_handle, NULL);
    printf("open control node: %d\n", ret);
    stream_frame_info.ircmd_handle = ircmd_create_handle(ir_control_handle);
    ret = basic_video_stream_continue(stream_frame_info.ircmd_handle);
    printf("basic_video_stream_continue:%d\n", ret);    //control mipi start

    load_stream_frame_info(&stream_frame_info, true, false);
    init_pthread_sem();
    pthread_t image_thread,temp_thread,display_thread,cmd_thread,info_thread;

    if (product_config.camera.v4l2_config.has_image && product_config.camera.v4l2_config.has_temp)
    {
        pthread_create(&image_thread, NULL, v4l2_image_channel_stream_function, &stream_frame_info);
        pthread_create(&temp_thread, NULL, v4l2_temp_channel_stream_function, &stream_frame_info);
    }
    else if (product_config.camera.v4l2_config.has_image)
    {
        pthread_create(&image_thread, NULL, v4l2_stream_function, &stream_frame_info);
    }
    pthread_create(&display_thread, NULL, drm_display_function, &stream_frame_info);
    pthread_create(&info_thread, NULL, info_line_parse_function, &stream_frame_info);
    sleep(1);

    printf("in streaming\n");
    void *thread_result;
    pthread_join(image_thread, &thread_result);
    if (product_config.camera.v4l2_config.has_temp)
    {
        pthread_join(temp_thread, &thread_result);
    }
    pthread_join(display_thread, &thread_result);
    pthread_join(info_thread, &thread_result);
    printf("stop stream\n");
    destroy_pthread_sem();
    destroy_data_demo(&stream_frame_info);

    ircmd_delete_handle(stream_frame_info.ircmd_handle);
    ir_control_handle->ir_control_release(iruart_handle, NULL);
    ir_control_handle->ir_control_close(iruart_handle);
    iruart_handle_delete(ir_control_handle);
    iruart_handle = NULL;
    ir_control_handle_delete(&ir_control_handle);
    ir_control_handle = NULL;

    if (product_config.camera.v4l2_config.has_temp)
    {
        irv4l2_handle_delete((Irv4l2VideoHandle_t*)stream_frame_info.temp_driver_handle);
        ir_video_handle_delete(&ir_temp_video_handle);
    }
    if (product_config.camera.v4l2_config.has_image)
    {
        irv4l2_handle_delete((Irv4l2VideoHandle_t*)stream_frame_info.image_driver_handle);
        ir_video_handle_delete(&ir_image_video_handle);
    }

    return 0;
}
