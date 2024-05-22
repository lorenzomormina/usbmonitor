#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include <errno.h>
#include <libusb.h>


typedef struct {
    char at[24][10];
    int size;
}device_list;


void get_devices(device_list *devs);
int device_list_contains(const device_list *devlist, const char *elem);
void device_list_copy(const device_list *from, device_list *to);
void save_log(const char *id);
void open_process(const char *id);
void get_target_path(char *path);


int main()
{

    device_list pdevs;
    device_list ndevs;
    pdevs.size = 0;

    // TODO: use a compilation flag?
    //HWND cmd = GetConsoleWindow();
    //ShowWindow(cmd, SW_HIDE);

    libusb_init(0);

    while (TRUE)
    {
        Sleep(1000);
        get_devices(&ndevs);

        // NOTE: fast but inaccurate way to check if a device is plugged in
        if (ndevs.size > pdevs.size) {
            for (int i = 0; i < ndevs.size; i++) {
                if (!device_list_contains(&pdevs, ndevs.at[i])) {
                    save_log(ndevs.at[i]);
                    open_process(ndevs.at[i]);
                }
            }
        }

        device_list_copy(&ndevs, &pdevs);
    }

    libusb_exit(0);

    return 0;
}


void get_devices(device_list *devs)
{
    libusb_device *libdev, **libdevs;
    struct libusb_device_descriptor desc;
    int i = 0;

    libusb_get_device_list(NULL, &libdevs);

    while ((libdev = libdevs[i++]) != NULL) {
        libusb_get_device_descriptor(libdev, &desc);
        sprintf(devs->at[i - 1], "%04x&%04x", desc.idVendor, desc.idProduct);
    }

    devs->size = i - 1;

    libusb_free_device_list(libdevs, 1);
}

int device_list_contains(const device_list *devlist, const char *elem)
{
    for (int i = 0; i < devlist->size; i++) {
        if (!strcmp(devlist->at[i], elem)) {
            return TRUE;
        }
    }
    return FALSE;
}

void device_list_copy(const device_list *from, device_list *to)
{
    for (int i = 0; i < from->size; i++) {
        strcpy(to->at[i], from->at[i]);
    }
    to->size = from->size;
}

void save_log(const char *id)
{
    time_t rtime;
    time(&rtime);
    struct tm *hms = localtime(&rtime);

    char buf[512];
    sprintf(buf, "USB PLUGGED at %02d:%02d:%02d. VID&PID: %s\n", hms->tm_hour, hms->tm_min, hms->tm_sec, id);
    printf(buf);
    FILE *logfile = fopen("log.txt", "a");
    fprintf(logfile, buf);
    fclose(logfile);
}

// NOTE: hopefully target.txt won't contain the path to this program
void open_process(const char *id)
{
    // XXXX&XXXX C:\path\to\exe
    FILE *inifile = fopen("target.txt", "r");
    if (inifile == NULL) {
        inifile = fopen("target.txt", "w");
        fclose(inifile);
        return;
    }

    char buf[512];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    while (fgets(buf, 512, inifile)) {
        if (strstr(buf, id)) {
            get_target_path(buf);
            CreateProcess(NULL, buf, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

        }
    }

    fclose(inifile);
}

void get_target_path(char *path)
{
    for (int i = 10; i <= 512; i++) {
        if (path[i] == EOF || path[i] == '\n') {
            path[i - 10] = 0;
            break;
        }
        path[i - 10] = path[i];
    }
}
