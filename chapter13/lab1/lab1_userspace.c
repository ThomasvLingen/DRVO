#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>

#define DRIVER_TYPE '!'

int device_node;
char const* device_node_path = "/dev/lab1_driver";

char userspace_data = 'i';

int main()
{
    int ioctl_return;

    device_node = open(device_node_path, O_RDWR);

    if (device_node < 0) {
        printf("Could not open device node.\n");
        return EXIT_FAILURE;
    }

    // Perform read
    int read_command = _IOR(DRIVER_TYPE, 1, char);
    ioctl_return = ioctl(device_node, read_command, &userspace_data);
    printf("[%d] Performed ioctl\n", ioctl_return);
    printf("new userspace data: %c\n", userspace_data);

    // Perform write
    userspace_data = '!';
    int write_command = _IOW(DRIVER_TYPE, 1, char);
    ioctl_return = ioctl(device_node, write_command, &userspace_data);
    printf("[%d] Performed ioctl\n", ioctl_return);
    printf("new userspace data: %c\n", userspace_data);

    close(device_node);
}
