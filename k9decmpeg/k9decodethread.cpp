//
// C++ Implementation: k9decodethread
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9decodethread.h"

k9DecodeThread::k9DecodeThread()
{
   m_decoder=new kDecMPEG2;
   noData=FALSE;
}


k9DecodeThread::~k9DecodeThread()
{
  delete m_decoder;
}

void k9DecodeThread::clear() {
    m_fifo.clear();
    wDataRead.wakeAll();
    wDataReady.wakeAll();

}

void k9DecodeThread::addData(uchar *data,uint size) {
    while (1) {
        if (m_fifo.freespace()>=size) {
            m_fifo.enqueue(data,size);
            wDataReady.wakeAll();
            break;
        } else
            wDataRead.wait();
    }
}


int k9DecodeThread::readData(uchar * data,uint size) {
    uint size2=size;
    uint32_t readSize=0,s=0;
    
    while (1) {
	// is there data in the buffer?
	if (m_fifo.count() >0) {
		// s= size of data that we will read (maximum = size)
		s=(m_fifo.count()) <size2 ? (m_fifo.count()) : size2;
		// increments the number of readen bytes
		readSize+=s;
		// decrements the number of max bytes to read 
		size2-=s;
		//moves bytes from buffer to output
		m_fifo.dequeue(data,s);
		//moves the position of output buffer to receive next bytes
		data+=s;
		//there's now free space in input buffer, we can wake the injection thread
		wDataRead.wakeAll();
	}
	// break the loop if injection thread terminated or we got what we want (size bytes)
	// oterwise, we're waiting for datas
        if(noData || (m_fifo.count() >=size2)) {
            break;
        } else
            wDataReady.wait();
    }
    // if there's datas in input buffer and we did not get all what we wanted, we take them.
    s= (m_fifo.count()) <size2 ? (m_fifo.count()) : size2;
    readSize+=s;
    if (s>0 ) 
        m_fifo.dequeue(data,s);
   
    wDataRead.wakeAll();
    return readSize;
}

void k9DecodeThread::setNoData() {
    noData=true;
    wDataRead.wakeAll();
    wDataReady.wakeAll();
}

void k9DecodeThread::run() {
   noData=FALSE;
   m_decoder->start();
   bool wait4buffer=TRUE;
   while (1) {
	if (m_fifo.count() < (5*2048) && wait4buffer && ! noData)
	    wDataReady.wait();
 	else {
		uchar buffer[2048];
		uint32_t size=readData(buffer,2048);
		if (size==0)
		break;
		m_decoder->decode(buffer ,buffer+size,0);
	}
   }
   m_decoder->stop();

}


#include "k9decodethread.moc"
