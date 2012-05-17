#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <sys/event.h>

static void noteProcDeath(CFFileDescriptorRef fdref, CFOptionFlags callBackTypes, void *info) {
    struct kevent kev;
    int fd = CFFileDescriptorGetNativeDescriptor(fdref);
    kevent(fd, NULL, 0, &kev, 1, NULL);
    // take action on death of process here
    printf("process with pid '%u' died\n", (unsigned int)kev.ident);
    CFFileDescriptorInvalidate(fdref);
    CFRelease(fdref); // the CFFileDescriptorRef is no longer of any use in this example
}

// one argument, an integer pid to watch, required
int main(int argc, char *argv[]) {
    if (argc < 2) exit(1);
    int fd = kqueue();
    struct kevent kev;
    EV_SET(&kev, atoi(argv[1]), EVFILT_PROC, EV_ADD|EV_ENABLE, NOTE_EXIT, 0, NULL);
    kevent(fd, &kev, 1, NULL, 0, NULL);
    CFFileDescriptorRef fdref = CFFileDescriptorCreate(kCFAllocatorDefault, fd, true, noteProcDeath, NULL);
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    CFRunLoopSourceRef source = CFFileDescriptorCreateRunLoopSource(kCFAllocatorDefault, fdref, 0);
    CFRunLoopAddSource(CFRunLoopGetMain(), source, kCFRunLoopDefaultMode);
    CFRelease(source);
    // run the run loop for 20 seconds
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 20.0, false);
    return 0;
}
