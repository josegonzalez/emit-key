#include <fcntl.h>
#include <linux/uinput.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// Key mapping structure
struct key_mapping
{
    const char *name;
    unsigned long code;
};

// Key mapping table
static const struct key_mapping key_map[] = {
    {"F1", KEY_F1},
    {"F2", KEY_F2},
    {"F3", KEY_F3},
    {"F4", KEY_F4},
    {"F5", KEY_F5},
    {"F6", KEY_F6},
    {"F7", KEY_F7},
    {"F8", KEY_F8},
    {"F9", KEY_F9},
    {"F10", KEY_F10},
    {"F11", KEY_F11},
    {"F12", KEY_F12},
    {"p", KEY_P},
    {NULL, 0} // Sentinel
};

// Function to get key code from name
unsigned long get_key_code(const char *key_name)
{
    for (const struct key_mapping *mapping = key_map; mapping->name != NULL; mapping++)
    {
        if (strcasecmp(key_name, mapping->name) == 0)
        {
            return mapping->code;
        }
    }
    fprintf(stderr, "Error: Unknown key '%s'\n", key_name);
    exit(1);
}

void emit(int fd, int type, int code, int val)
{
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    /* timestamp values below are ignored */
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(fd, &ie, sizeof(ie));
}

int main(int argc, char *argv[])
{
    const char *key_name = "F12"; // Default key
    int opt;

    // Parse command line arguments
    while ((opt = getopt(argc, argv, "k:")) != -1)
    {
        switch (opt)
        {
        case 'k':
            key_name = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-k key_name]\n", argv[0]);
            exit(1);
        }
    }

    struct uinput_setup usetup;
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    unsigned long key_code = get_key_code(key_name);

    /*
     * The ioctls below will enable the device that is about to be
     * created, to pass key events, in this case the space key.
     */
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, key_code);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;  /* sample vendor */
    usetup.id.product = 0x5678; /* sample product */
    strcpy(usetup.name, "Example device");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    /*
     * On UI_DEV_CREATE the kernel will create the device node for this
     * device. We are inserting a pause here so that userspace has time
     * to detect, initialize the new device, and can start listening to
     * the event, otherwise it will not notice the event we are about
     * to send. This pause is only needed in our example code!
     */
    sleep(1);

    /* Key press, report the event, send key release, and report again */
    emit(fd, EV_KEY, key_code, 1);
    emit(fd, EV_SYN, SYN_REPORT, 0);
    emit(fd, EV_KEY, key_code, 0);
    emit(fd, EV_SYN, SYN_REPORT, 0);

    /*
     * Give userspace some time to read the events before we destroy the
     * device with UI_DEV_DESTOY.
     */
    sleep(1);

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

    return 0;
}