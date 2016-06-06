#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>

#define DRIVER_TYPE '!'

int device_node;
char const* device_node_path = "/dev/lab2_driver";

char userspace_data_send_1[5] = "abcd";
char userspace_data_send_2[10] = "abcdefghi";
char userspace_data_read_1[5];
char userspace_data_read_2[10];

int main()
{
    int ioctl_return;

    device_node = open(device_node_path, O_RDWR);

    if (device_node < 0) {
        printf("Could not open device node.\n");
        return EXIT_FAILURE;
    }

    // Perform read_1
    int read_command_1 = _IOR(DRIVER_TYPE, 1, userspace_data_read_1);
    ioctl_return = ioctl(device_node, read_command_1, &userspace_data_read_1);
    userspace_data_read_1[4] = '\0';
    printf("[%d] Performed ioctl\n", ioctl_return);
    printf("new userspace data: %s\n", userspace_data_read_1);

    // Perform read_2
    int read_command_2 = _IOR(DRIVER_TYPE, 1, userspace_data_read_2);
    ioctl_return = ioctl(device_node, read_command_2, &userspace_data_read_2);
    userspace_data_read_2[9] = '\0';
    printf("[%d] Performed ioctl\n", ioctl_return);
    printf("new userspace data: %s\n", userspace_data_read_2);

    // Perform write_1
    int write_command_1 = _IOW(DRIVER_TYPE, 1, userspace_data_send_1);
    ioctl_return = ioctl(device_node, write_command_1, &userspace_data_send_1);
    printf("[%d] Performed ioctl\n", ioctl_return);

    // Perform write_1
    int write_command_2 = _IOW(DRIVER_TYPE, 1, userspace_data_send_2);
    ioctl_return = ioctl(device_node, write_command_2, &userspace_data_send_2);
    printf("[%d] Performed ioctl\n", ioctl_return);

    close(device_node);
}
