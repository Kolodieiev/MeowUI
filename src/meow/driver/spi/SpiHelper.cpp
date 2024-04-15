#include "SpiHelper.h"
namespace meow
{

    volatile xSemaphoreHandle SpiHelper::_mutex;
    SpiHelper::constructor SpiHelper::_cons;

}
