#include <stdio.h>
#include "async_event.h"

int asyncConnEvent::onReadable()
{
    printf("onReadable");
    return 0;
}

int asyncConnEvent::onWriteable()
{
    printf("onWriteable");
    return 0;
}
