#include <fcntl.h>
#include <getopt.h>
#include <linux/uinput.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

// Key mapping structure
struct key_mapping
{
    const char *name;
    unsigned long code;
};

int fd;
unsigned long key_code;

// Key mapping table
static const struct key_mapping key_map[] = {
    {"f1", KEY_F1},
    {"f2", KEY_F2},
    {"f3", KEY_F3},
    {"f4", KEY_F4},
    {"f5", KEY_F5},
    {"f6", KEY_F6},
    {"f7", KEY_F7},
    {"f8", KEY_F8},
    {"f9", KEY_F9},
    {"f10", KEY_F10},
    {"f11", KEY_F11},
    {"f12", KEY_F12},
    {"a", KEY_A},
    {"b", KEY_B},
    {"c", KEY_C},
    {"d", KEY_D},
    {"e", KEY_E},
    {"f", KEY_F},
    {"g", KEY_G},
    {"h", KEY_H},
    {"i", KEY_I},
    {"j", KEY_J},
    {"k", KEY_K},
    {"l", KEY_L},
    {"m", KEY_M},
    {"n", KEY_N},
    {"o", KEY_O},
    {"p", KEY_P},
    {"q", KEY_Q},
    {"r", KEY_R},
    {"s", KEY_S},
    {"t", KEY_T},
    {"u", KEY_U},
    {"v", KEY_V},
    {"w", KEY_W},
    {"x", KEY_X},
    {"y", KEY_Y},
    {"z", KEY_Z},
    {"0", KEY_0},
    {"1", KEY_1},
    {"2", KEY_2},
    {"3", KEY_3},
    {"4", KEY_4},
    {"5", KEY_5},
    {"6", KEY_6},
    {"7", KEY_7},
    {"8", KEY_8},
    {"9", KEY_9},
    {NULL, 0} // Sentinel
};

// Function to get key code from name
unsigned long get_key_code(const char *key_name)
{
    char *lower_key = strdup(key_name);
    if (!lower_key)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }

    // Convert to lowercase
    for (char *p = lower_key; *p; p++)
    {
        *p = tolower(*p);
    }

    unsigned long code = 0;
    for (const struct key_mapping *mapping = key_map; mapping->name != NULL; mapping++)
    {
        if (strcmp(lower_key, mapping->name) == 0)
        {
            code = mapping->code;
            break;
        }
    }

    free(lower_key);

    if (code == 0)
    {
        fprintf(stderr, "Error: Unknown key '%s'\n", key_name);
        exit(1);
    }

    return code;
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

void press_key(int fd, int key_code)
{
    emit(fd, EV_KEY, key_code, 1);
    emit(fd, EV_SYN, SYN_REPORT, 0);
    emit(fd, EV_KEY, key_code, 0);
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        exit(130);
    }
    else if (signal == SIGTERM)
    {
        exit(143);
    }
    else if (signal == SIGUSR1)
    {
        press_key(fd, key_code);
    }
    else
    {
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    const char *key_name = "F12";
    int opt;
    int wait_for_signal = 0;
    while ((opt = getopt(argc, argv, "k:s")) != -1)
    {
        switch (opt)
        {
        case 'k':
            key_name = optarg;
            break;
        case 's':
            wait_for_signal = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-k key_name]\n", argv[0]);
            exit(1);
        }
    }

    struct uinput_setup usetup;
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    key_code = get_key_code(key_name);

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, key_code);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "Emit key");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    struct sigaction sa = {
        .sa_handler = signal_handler,
        .sa_flags = SA_RESTART};
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    if (wait_for_signal)
    {
        while (1)
        {
            sleep(1);
        }
    }
    else
    {
        /*
         * On UI_DEV_CREATE the kernel will create the device node for this
         * device. We are inserting a pause here so that userspace has time
         * to detect, initialize the new device, and can start listening to
         * the event, otherwise it will not notice the event we are about
         * to send. This pause is only needed in our example code!
         */
        sleep(1);

        press_key(fd, key_code);
    }

    /*
     * Give userspace some time to read the events before we destroy the
     * device with UI_DEV_DESTOY.
     */
    sleep(1);

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

    return 0;
}