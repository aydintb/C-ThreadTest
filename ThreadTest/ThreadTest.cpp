// ThreadTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>


#define datasize	512*512*512
int *data = NULL;
int *tdata = NULL;


//-----------------

HANDLE hWaitEvent;
HANDLE hWaitEventMain;
//HANDLE hWaitMutex;
//
void PauseThreadProc(void *vPauseTime )
{
	int *iPause = (int*) vPauseTime;
	WaitForSingleObject( hWaitEvent, *iPause );
	//ReleaseMutex(hWaitMutex);
	SetEvent(hWaitEventMain);
	// exlicitly close the thread handle
	_endthread();
}
// 		SetEvent(hWaitEvent); gelirse waitten vaz gecer
class WaitSome
{
public :
	HANDLE hThread;

	WaitSome(int iPauseTime)
	{
		//hWaitMutex = CreateMutex (NULL, FALSE, NULL);
		hWaitEventMain = CreateEvent(NULL, FALSE, FALSE, L"PauseThreadMain");
		hWaitEvent = CreateEvent(NULL, FALSE, FALSE, L"PauseThread");
		ResetEvent(hWaitEvent);
		hThread = (HANDLE) _beginthread( PauseThreadProc, 0, &iPauseTime );
		ResetEvent(hWaitEventMain);
		WaitForSingleObject( hWaitEventMain, iPauseTime );
	};

	~WaitSome()
	{
		CloseHandle(hWaitEvent);
		CloseHandle(hWaitEventMain);
		//CloseHandle(hWaitMutex);
		//CloseHandle(hThread); // bu gerekli degil
	};
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void DataSetIslemi()
{
	for (int i = 0; i<datasize; i++)
	{
		double result = 0;
		for (int j = 0; j < 20; j++)
		{
			double toplam = (double)i / (double)(j+1);
			result += toplam;
		}
		data[i] = (int)result;
	}
}

struct ThreadParamStruct
{
	int start;
	int end;
};

void DataSetIslemiThread(int start, int end)
{
	for (int i = start; i<end; i++)
	{
		double result = 0;
		for (int j = 0; j < 20; j++)
		{
			double toplam = (double)i / (double)(j+1);
			result += toplam;
		}
		tdata[i] = (int)result;
	}
}

void ThreadProc( void *vTP )
{
	ThreadParamStruct *tp = (ThreadParamStruct *) vTP;
	int start = tp->start;
	int end = tp->end;
	delete tp;

	DataSetIslemiThread(start, end);



	// exlicitly close the thread handle
	_endthread();

}

#define MAX_THREADS 12
/*
			x86			x64
Normal 		34 saniye	17
8 thread	13 saniye	8
10 thread	10 saniye	9	
12 thread	10 saniye	8
16 thread	11 saniye	8
20 thread	11 saniye	8sn
32 thread	11 saniye	10
42 thread	11 saniye	10
64 thread	12 saniye	9
sonuc : 6 thread per core ideal gibi
*/

int _tmain(int argc, _TCHAR* argv[])
{

	WaitSome(5000);

	data = new int[datasize];
	tdata = new int[datasize];

	long start = GetTickCount();

	// Threadsiz yapilan islem
	//DataSetIslemi();

	long end = GetTickCount();
	float fark = (float)(end - start) / 1000;

	long tstart = GetTickCount();

	int index = 0;
	int step = datasize / MAX_THREADS;

	HANDLE *hThread = new HANDLE[MAX_THREADS];
	for (int i = 0; i< MAX_THREADS; i++)
	{
		ThreadParamStruct *tp = new ThreadParamStruct();
		tp->start = index;
		index += step;
		tp->end = index;

		if (i == MAX_THREADS - 1)
			tp->end = datasize;

		hThread[i] = (HANDLE) _beginthread( ThreadProc, 0, tp );
	} 
	WaitForMultipleObjects(MAX_THREADS, hThread, true, INFINITE );

	//for (int i = 0; i<MAX_THREADS; i++)
	//	WaitForSingleObject(hThread[i], INFINITE);

	delete [] hThread;

	// thread handles closed by endthread() !
	//for (int i = 0; i < MAX_THREADS; i++)
	//{
	//	if (hThread[i] != NULL)
	//		CloseHandle(hThread[i]);
	//}


	//while (ThreadNr)
	//	WaitForSingleObject(hRunMutex, 100L);

	long tend = GetTickCount();
	int tfark = (tend - tstart) / 1000;


	delete[] data;
	delete[] tdata;

	return 0;
}

/*
global olarak bunu olustur
HANDLE hIOMutex= CreateMutex (NULL, FALSE, NULL);

ayni isi yapan, ornegin ayni filea yazan threadler icin alttaki yapiyi kullan
WaitForSingleObject( hIOMutex, INFINITE );
fseek( fp, desired_position, 0L );
fwrite( data, sizeof( data ), 1, fp );
ReleaseMutex( hIOMutex);


----------------------------

//HANDLE hRunMutex;
//HANDLE hScreenMutex; 
//int ThreadNr; 

	//hScreenMutex = CreateMutex( NULL, FALSE, NULL ); 
	//hRunMutex = CreateMutex( NULL, TRUE, NULL );
	//ThreadNr = 0;


	// All threads done. Clean up handles. 
	//CloseHandle( hScreenMutex );
	//CloseHandle( hRunMutex );

her bir thread icerisinde ayni resource a erisiyorsan
	//WaitForSingleObject( hScreenMutex, INFINITE );
	//ThreadNr--;
	//ReleaseMutex( hScreenMutex );

*/