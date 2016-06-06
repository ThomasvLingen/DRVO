#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define DRIVER_TYPE '!'

int device_node;
char const* device_node_path = "/dev/lab3_driver";

unsigned int ioctl_set_process_target = _IO(DRIVER_TYPE, 1);
unsigned int ioctl_set_signal = _IO(DRIVER_TYPE, 2);
unsigned int ioctl_send_signal = _IO(DRIVER_TYPE, 3);

int open_driver()
{
    device_node = open(device_node_path, O_RDWR);

    if (device_node < 0) {
        printf("Could not open device node.\n");
        exit(EXIT_FAILURE);
    }

    return device_node;
}

int get_process_id(int argc, char* argv[])
{
    if (argc == 1) {
        return getpid();
    } else {
        return atoi(argv[1]);
    }
}

int get_signal_to_send(int argc, char* argv[])
{
    if (argc <= 2) {
        return SIGKILL;
    } else {
        return atoi(argv[2]);
    }
}

int main(int argc, char* argv[])
{
    int ioctl_return;

    device_node = open_driver();

    // By default, we kill ourself
    int target_process_id = get_process_id(argc, argv);
    int signal_to_send = get_signal_to_send(argc, argv);

    // Set process_id
    ioctl_return = ioctl(device_node, ioctl_set_process_target, target_process_id);
    printf("[%d] Performed ioctl for process_id\n", ioctl_return);

    // Set signal to send
    ioctl_return = ioctl(device_node, ioctl_set_signal, signal_to_send);
    printf("[%d] Performed ioctl for setting signal\n", ioctl_return);

    // Send signal!
    ioctl_return = ioctl(device_node, ioctl_send_signal, NULL);
    printf("[%d] Performed ioctl for sending signal\n", ioctl_return);

    close(device_node);
}
