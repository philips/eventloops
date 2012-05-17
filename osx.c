#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <fcntl.h>
#include "uv.h"
#include <sys/event.h>

static int64_t start_time;
static uv_fs_t req2;

static int count = 0;
static int idle_counter = 0;

static void idle_cb(uv_idle_t* handle, int status) {
  printf("idle_counter: %i\n", idle_counter++);
}

static void open_cb(uv_fs_t* req) {
  printf("open_cb path: %s count: %i\n", req->path, count++);
}

static void open_setup(uv_fs_t *req, const char *path) {
  int r;
  uv_fs_open(uv_default_loop(), req, path, O_RDONLY, 0, open_cb);
}

static void evEvent(CFFileDescriptorRef fdref, CFOptionFlags callBackTypes, void *info) {
    struct kevent kev;
    int fd = CFFileDescriptorGetNativeDescriptor(fdref);

    printf("event loop picked up ev kqueue event, running uv once\n");
    uv_run_once(uv_default_loop());

    CFFileDescriptorInvalidate(fdref);
    CFRelease(fdref); // the CFFileDescriptorRef is no longer of any use in this example
}

int main(int argc, char *argv[]) {
    int fd;
    uv_fs_t req;
    uv_idle_t h;

    uv_idle_init(uv_default_loop(), &h);
    uv_idle_start(&h, idle_cb);

    fd = uv_backend_fd();
    printf("ev_backend_fd: %i\n", fd);

    open_setup(&req, "osx.c");

    CFFileDescriptorRef fdref = CFFileDescriptorCreate(kCFAllocatorDefault, fd, true, evEvent, NULL);
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    CFRunLoopSourceRef source = CFFileDescriptorCreateRunLoopSource(kCFAllocatorDefault, fdref, 0);
    CFRunLoopAddSource(CFRunLoopGetMain(), source, kCFRunLoopDefaultMode);
    CFRelease(source);

    uv_run_once(uv_default_loop());
    usleep(1000000);
    // run the run loop for 20 seconds
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 20.0, false);
    return 0;
}
