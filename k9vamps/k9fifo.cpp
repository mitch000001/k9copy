//
// C++ Implementation: k9fifo
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9fifo.h"
#include "ac.h"

uint32_t k9fifo::count() {
	return m_count;
}

void k9fifo::enqueue (uchar *_buffer, uint32_t _size) {
   mutex.lock();
    if (_size+queue > INPUT_SIZE) {
        uint32_t s1,s2;
        s1=INPUT_SIZE-queue;
        tc_memcpy(array+queue,_buffer,s1);
        s2=_size-s1;
        tc_memcpy(array,_buffer+s1,s2);
    } else
        tc_memcpy(array+queue,_buffer,_size);
    queue=(queue+_size) %INPUT_SIZE;
    m_count+=_size;
    mutex.unlock();
}

void k9fifo::dequeue(uchar *_buffer,uint32_t _size) {
    mutex.lock();
    if ( _size+head >INPUT_SIZE) {
        uint32_t s1,s2;
        s1=INPUT_SIZE - head;
        tc_memcpy(_buffer,array+head,s1);
        s2=_size-s1;
        tc_memcpy(_buffer+s1,array,s2);
    } else
        tc_memcpy(_buffer,array+head,_size);
    head =(head+_size)%INPUT_SIZE;
    m_count -=_size;
    mutex.unlock();
}

void k9fifo::clear() {
    mutex.lock();
    head=queue;
    m_count=0;
    mutex.unlock();
}
