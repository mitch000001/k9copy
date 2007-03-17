//
// C++ Interface: k9fifo
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9FIFO_H
#define K9FIFO_H

#include "k9common.h"
#include <qmutex.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

#define INPUT_SIZE ( 0x2000*1024)

class k9fifo {
private:
	uint32_t  head,queue;
	uchar *array;
	uint32_t m_count;
	QMutex mutex;
public:
	k9fifo () { head=queue=m_count=0;array= new uchar[INPUT_SIZE];};
	uint32_t count(); // { return  (queue-head)%INPUT_SIZE ;}
	uint32_t freespace() {return INPUT_SIZE-count();};
	void enqueue (uchar *_buffer, uint32_t _size) ;
	void dequeue(uchar *_buffer,uint32_t _size) ;
        void clear();
	~k9fifo() { delete[] array;};
};

#endif
