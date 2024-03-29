/* 
 * Copyright (C) 2012-2014 Chris McClelland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <makestuff.h>
#include <libfpgalink.h>
#include <libbuffer.h>
#include <liberror.h>
#include <libdump.h>
#include <argtable2.h>
#include <readline/readline.h>
#include <readline/history.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
 _Bool K[32]= {1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,1};
 _Bool Ack0[32]= {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
 _Bool Ack1[32]= {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};



_Bool info[8][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,1,0,0,0},
	{0,0,0,1,0,0,0,0},
	{0,0,0,1,1,0,0,0},	
	{0,0,1,0,0,0,0,0},
	{0,0,1,0,1,0,0,0},
	{0,0,1,1,0,0,0,0},
	{0,0,1,1,1,0,0,0}};


  void readCsv(int pos_X,int pos_Y){
  	const char delimiter[2] = ",";
				FILE* stream = fopen("track_data.csv","r");

					char myline[1024];

					int totalentries = 0;

					while (fgets(myline,1024,stream))	{
						totalentries++;
						//printf("%s\n", myline);
					}

					stream = fopen("track_data.csv","r");
					int signal_info[totalentries][5];
					int ith_entry = 0;
					while(fgets(myline,1024,stream)){

						char* str = strdup(myline);
						char* token = NULL;
						

						token = strtok(myline,delimiter);

						int jth_filling = 0;
						while(token != NULL){
							
							int to_int = atoi(token);
							token = strtok(NULL,delimiter);
							signal_info[ith_entry][jth_filling] = to_int;
							jth_filling++;
							//printf("%d ",to_int);
						}
						ith_entry++;
						//printf("\n");

					}

					//printf("%s", myline);
					


			
					for (int i = 0; i < totalentries; ++i)
					{
						if(signal_info[i][0] == pos_X && signal_info[i][1] == pos_Y){
							info[signal_info[i][2]][0] = 1;
							info[signal_info[i][2]][1] = signal_info[i][3];

							
							info[signal_info[i][2]][5] = info[signal_info[i][4]][2];
							info[signal_info[i][2]][6] = info[signal_info[i][4]][3];
 							info[signal_info[i][2]][7] = info[signal_info[i][4]][4];
							
						}
					}
					return;
  }	


void encrypter(_Bool P[], _Bool K[], _Bool C[]) {
  _Bool T[4];
  int N1=0;
  
  for(int i=0; i<32; i++)
    {
        C[i] = P[i];
    }
    
  for(int i=0; i<32; i++)
    {
        if (K[i]==1) N1++;
    }
    
  T[3] = K[31]^K[27]^K[23]^K[19]^K[15]^K[11]^K[7]^K[3];
  T[2] = K[30]^K[26]^K[22]^K[18]^K[14]^K[10]^K[6]^K[2];
  T[1] = K[29]^K[25]^K[21]^K[17]^K[13]^K[9]^K[5]^K[1];
  T[0] = K[28]^K[24]^K[20]^K[16]^K[12]^K[8]^K[4]^K[0];
  
  for(int i=0; i<N1; i++)
  {
    for(int j=0; j<32;j++){
      C[31-j] = C[31-j]^T[(31-j)%4];
    }
    
    
    if(T[3]==1){
		T[3]=0;
          if(T[2]==1){
		T[2]=0;
              	if(T[1]==1){
		    T[1]=0;
                	if(T[0]==1){
		            T[0]=0;
			}
			else{T[0]=1;}
		}
		else{T[1]=1;}
	} 
	  else{T[2]=1;} 
    }
	else{T[3]=1;}
}
}
void decrypter(_Bool C[], _Bool K[], _Bool P[]) {
  _Bool T[4];
  int N1=0;
  
  for(int i=0; i<32; i++)
    {
        P[i] = C[i];
    }
    
  for(int i=0; i<32; i++)
    {
        if (K[i]==0) N1++;
    }
    
  T[3] = K[31]^K[27]^K[23]^K[19]^K[15]^K[11]^K[7]^K[3];
  T[2] = K[30]^K[26]^K[22]^K[18]^K[14]^K[10]^K[6]^K[2];
  T[1] = K[29]^K[25]^K[21]^K[17]^K[13]^K[9]^K[5]^K[1];
  T[0] = K[28]^K[24]^K[20]^K[16]^K[12]^K[8]^K[4]^K[0];
    if(T[3]==0){
		T[3]=1;
          if(T[2]==0){
		T[2]=1;
              	if(T[1]==0){
		    T[1]=1;
                	if(T[0]==0){
		            T[0]=1;
			}
			else{T[0]=0;}
		}
		else{T[1]=0;}
	} 
	  else{T[2]=0;} 
    }
	else{T[3]=0;}
  
  for(int i=0; i<N1; i++)
  {
    for(int j=0; j<32;j++){
      P[31-j] = P[31-j]^T[(31-j)%4];
    }
    
    if(T[3]==0){
		T[3]=1;
          if(T[2]==0){
		T[2]=1;
              	if(T[1]==0){
		    T[1]=1;
                	if(T[0]==0){
		            T[0]=1;
			}
			else{T[0]=0;}
		}
		else{T[1]=0;}
	} 
	  else{T[2]=0;} 
    }
	else{T[3]=0;}

}
}
typedef _Bool bit;
bool sigIsRaised(void);
void sigRegisterHandler(void);



void uint8_tobit(uint8 myinp,_Bool out[]){
	

	uint8 temp = myinp;

	for (int i = 0; i < 8; ++i)
	{
		if (temp%2 == 0)
		{
			out[7-i] = 0;
		}
		else {
			out[7-i] = 1;
		}
		temp = temp/2;

	}
	return;
}

int powerof2(int n){
	int out = 1;
	for (int i = 0; i < n; ++i)
	{
		out = out*2;
	}
	return out;
}

uint8 bit_touint8(bit * mybit){
	uint8 out=0;

	for (int i = 0; i < 8; ++i)
	{
		out = out + (*(mybit+7-i))*powerof2(i);
	}

	return out;
}

static const char *ptr;
static bool enableBenchmarking = false;

static bool isHexDigit(char ch) {
	return
		(ch >= '0' && ch <= '9') ||
		(ch >= 'a' && ch <= 'f') ||
		(ch >= 'A' && ch <= 'F');
}



static uint16 calcChecksum(const uint8 *data, size_t length) {
	uint16 cksum = 0x0000;
	while ( length-- ) {
		cksum = (uint16)(cksum + *data++);
	}
	return cksum;
}

static bool getHexNibble(char hexDigit, uint8 *nibble) {
	if ( hexDigit >= '0' && hexDigit <= '9' ) {
		*nibble = (uint8)(hexDigit - '0');
		return false;
	} else if ( hexDigit >= 'a' && hexDigit <= 'f' ) {
		*nibble = (uint8)(hexDigit - 'a' + 10);
		return false;
	} else if ( hexDigit >= 'A' && hexDigit <= 'F' ) {
		*nibble = (uint8)(hexDigit - 'A' + 10);
		return false;
	} else {
		return true;
	}
}

static int getHexByte(uint8 *byte) {
	uint8 upperNibble;
	uint8 lowerNibble;
	if ( !getHexNibble(ptr[0], &upperNibble) && !getHexNibble(ptr[1], &lowerNibble) ) {
		*byte = (uint8)((upperNibble << 4) | lowerNibble);
		byte += 2;
		return 0;
	} else {
		return 1;
	}
}

static const char *const errMessages[] = {
	NULL,
	NULL,
	"Unparseable hex number",
	"Channel out of range",
	"Conduit out of range",
	"Illegal character",
	"Unterminated string",
	"No memory",
	"Empty string",
	"Odd number of digits",
	"Cannot load file",
	"Cannot save file",
	"Bad arguments"
};

typedef enum {
	FLP_SUCCESS,
	FLP_LIBERR,
	FLP_BAD_HEX,
	FLP_CHAN_RANGE,
	FLP_CONDUIT_RANGE,
	FLP_ILL_CHAR,
	FLP_UNTERM_STRING,
	FLP_NO_MEMORY,
	FLP_EMPTY_STRING,
	FLP_ODD_DIGITS,
	FLP_CANNOT_LOAD,
	FLP_CANNOT_SAVE,
	FLP_ARGS
} ReturnCode;

static ReturnCode doRead(
	struct FLContext *handle, uint8 chan, uint32 length, FILE *destFile, uint16 *checksum,
	const char **error)
{
	ReturnCode retVal = FLP_SUCCESS;
	uint32 bytesWritten;
	FLStatus fStatus;
	uint32 chunkSize;
	const uint8 *recvData;
	uint32 actualLength;
	const uint8 *ptr;
	uint16 csVal = 0x0000;
	#define READ_MAX 65536

	// Read first chunk
	chunkSize = length >= READ_MAX ? READ_MAX : length;
	fStatus = flReadChannelAsyncSubmit(handle, chan, chunkSize, NULL, error);
	CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");
	length = length - chunkSize;

	while ( length ) {
		// Read chunk N
		chunkSize = length >= READ_MAX ? READ_MAX : length;
		fStatus = flReadChannelAsyncSubmit(handle, chan, chunkSize, NULL, error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");
		length = length - chunkSize;
		
		// Await chunk N-1
		fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");

		// Write chunk N-1 to file
		bytesWritten = (uint32)fwrite(recvData, 1, actualLength, destFile);
		CHECK_STATUS(bytesWritten != actualLength, FLP_CANNOT_SAVE, cleanup, "doRead()");

		// Checksum chunk N-1
		chunkSize = actualLength;
		ptr = recvData;
		while ( chunkSize-- ) {
			csVal = (uint16)(csVal + *ptr++);
		}
	}

	// Await last chunk
	fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, error);
	CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");
	
	// Write last chunk to file
	bytesWritten = (uint32)fwrite(recvData, 1, actualLength, destFile);
	CHECK_STATUS(bytesWritten != actualLength, FLP_CANNOT_SAVE, cleanup, "doRead()");

	// Checksum last chunk
	chunkSize = actualLength;
	ptr = recvData;
	while ( chunkSize-- ) {
		csVal = (uint16)(csVal + *ptr++);
	}
	
	// Return checksum to caller
	*checksum = csVal;
cleanup:
	return retVal;
}

static ReturnCode doWrite(
	struct FLContext *handle, uint8 chan, FILE *srcFile, size_t *length, uint16 *checksum,
	const char **error)
{
	ReturnCode retVal = FLP_SUCCESS;
	size_t bytesRead, i;
	FLStatus fStatus;
	const uint8 *ptr;
	uint16 csVal = 0x0000;
	size_t lenVal = 0;
	#define WRITE_MAX (65536 - 5)
	uint8 buffer[WRITE_MAX];

	do {
		// Read Nth chunk
		bytesRead = fread(buffer, 1, WRITE_MAX, srcFile);
		if ( bytesRead ) {
			// Update running total
			lenVal = lenVal + bytesRead;

			// Submit Nth chunk
			fStatus = flWriteChannelAsync(handle, chan, bytesRead, buffer, error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doWrite()");

			// Checksum Nth chunk
			i = bytesRead;
			ptr = buffer;
			while ( i-- ) {
				csVal = (uint16)(csVal + *ptr++);
			}
		}
	} while ( bytesRead == WRITE_MAX );

	// Wait for writes to be received. This is optional, but it's only fair if we're benchmarking to
	// actually wait for the work to be completed.
	fStatus = flAwaitAsyncWrites(handle, error);
	CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doWrite()");

	// Return checksum & length to caller
	*checksum = csVal;
	*length = lenVal;
cleanup:
	return retVal;
}

static int parseLine(struct FLContext *handle, const char *line, const char **error) {
	ReturnCode retVal = FLP_SUCCESS, status;
	FLStatus fStatus;
	struct Buffer dataFromFPGA = {0,};
	BufferStatus bStatus;
	uint8 *data = NULL;
	char *fileName = NULL;
	FILE *file = NULL;
	double totalTime, speed;
	#ifdef WIN32
		LARGE_INTEGER tvStart, tvEnd, freq;
		DWORD_PTR mask = 1;
		SetThreadAffinityMask(GetCurrentThread(), mask);
		QueryPerformanceFrequency(&freq);
	#else
		struct timeval tvStart, tvEnd;
		long long startTime, endTime;
	#endif
	bStatus = bufInitialise(&dataFromFPGA, 1024, 0x00, error);
	CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
	ptr = line;
	do {
		while ( *ptr == ';' ) {
			ptr++;
		}
		switch ( *ptr ) {
		case 'r':{
			uint32 chan;
			uint32 length = 1;
			char *end;
			ptr++;
			
			// Get the channel to be read:
			errno = 0;
			chan = (uint32)strtoul(ptr, &end, 16);
			CHECK_STATUS(errno, FLP_BAD_HEX, cleanup);

			// Ensure that it's 0-127
			CHECK_STATUS(chan > 127, FLP_CHAN_RANGE, cleanup);
			ptr = end;

			// Only three valid chars at this point:
			CHECK_STATUS(*ptr != '\0' && *ptr != ';' && *ptr != ' ', FLP_ILL_CHAR, cleanup);

			if ( *ptr == ' ' ) {
				ptr++;

				// Get the read count:
				errno = 0;
				length = (uint32)strtoul(ptr, &end, 16);
				CHECK_STATUS(errno, FLP_BAD_HEX, cleanup);
				ptr = end;
				
				// Only three valid chars at this point:
				CHECK_STATUS(*ptr != '\0' && *ptr != ';' && *ptr != ' ', FLP_ILL_CHAR, cleanup);
				if ( *ptr == ' ' ) {
					const char *p;
					const char quoteChar = *++ptr;
					CHECK_STATUS(
						(quoteChar != '"' && quoteChar != '\''),
						FLP_ILL_CHAR, cleanup);
					
					// Get the file to write bytes to:
					ptr++;
					p = ptr;
					while ( *p != quoteChar && *p != '\0' ) {
						p++;
					}
					CHECK_STATUS(*p == '\0', FLP_UNTERM_STRING, cleanup);
					fileName = malloc((size_t)(p - ptr + 1));
					CHECK_STATUS(!fileName, FLP_NO_MEMORY, cleanup);
					CHECK_STATUS(p - ptr == 0, FLP_EMPTY_STRING, cleanup);
					strncpy(fileName, ptr, (size_t)(p - ptr));
					fileName[p - ptr] = '\0';
					ptr = p + 1;
				}
			}
			if ( fileName ) {
				uint16 checksum = 0x0000;

				// Open file for writing
				file = fopen(fileName, "wb");
				CHECK_STATUS(!file, FLP_CANNOT_SAVE, cleanup);
				free(fileName);
				fileName = NULL;

				#ifdef WIN32
					QueryPerformanceCounter(&tvStart);
					status = doRead(handle, (uint8)chan, length, file, &checksum, error);
					QueryPerformanceCounter(&tvEnd);
					totalTime = (double)(tvEnd.QuadPart - tvStart.QuadPart);
					totalTime /= freq.QuadPart;
					speed = (double)length / (1024*1024*totalTime);
				#else
					gettimeofday(&tvStart, NULL);
					status = doRead(handle, (uint8)chan, length, file, &checksum, error);
					gettimeofday(&tvEnd, NULL);
					startTime = tvStart.tv_sec;
					startTime *= 1000000;
					startTime += tvStart.tv_usec;
					endTime = tvEnd.tv_sec;
					endTime *= 1000000;
					endTime += tvEnd.tv_usec;
					totalTime = (double)(endTime - startTime);
					totalTime /= 1000000;  // convert from uS to S.
					speed = (double)length / (1024*1024*totalTime);
				#endif
				if ( enableBenchmarking ) {
					printf(
						"Read %d bytes (checksum 0x%04X) from channel %d at %f MiB/s\n",
						length, checksum, chan, speed);
				}
				CHECK_STATUS(status, status, cleanup);

				// Close the file
				fclose(file);
				file = NULL;
			} else {
				size_t oldLength = dataFromFPGA.length;
				bStatus = bufAppendConst(&dataFromFPGA, 0x00, length, error);
				CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
				#ifdef WIN32
					QueryPerformanceCounter(&tvStart);
					fStatus = flReadChannel(handle, (uint8)chan, length, dataFromFPGA.data + oldLength, error);
					QueryPerformanceCounter(&tvEnd);
					totalTime = (double)(tvEnd.QuadPart - tvStart.QuadPart);
					totalTime /= freq.QuadPart;
					speed = (double)length / (1024*1024*totalTime);
				#else
					gettimeofday(&tvStart, NULL);
					fStatus = flReadChannel(handle, (uint8)chan, length, dataFromFPGA.data + oldLength, error);
					gettimeofday(&tvEnd, NULL);
					startTime = tvStart.tv_sec;
					startTime *= 1000000;
					startTime += tvStart.tv_usec;
					endTime = tvEnd.tv_sec;
					endTime *= 1000000;
					endTime += tvEnd.tv_usec;
					totalTime = (double)(endTime - startTime);
					totalTime /= 1000000;  // convert from uS to S.
					speed = (double)length / (1024*1024*totalTime);
				#endif
				if ( enableBenchmarking ) {
					printf(
						"Read %d bytes (checksum 0x%04X) from channel %d at %f MiB/s\n",
						length, calcChecksum(dataFromFPGA.data + oldLength, length), chan, speed);
				}
				CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			}
			break;
		}
		case 'w':{
			unsigned long int chan;
			size_t length = 1, i;
			char *end, ch;
			const char *p;
			ptr++;
			
			// Get the channel to be written:
			errno = 0;
			chan = strtoul(ptr, &end, 16);
			CHECK_STATUS(errno, FLP_BAD_HEX, cleanup);

			// Ensure that it's 0-127
			CHECK_STATUS(chan > 127, FLP_CHAN_RANGE, cleanup);
			ptr = end;

			// There must be a space now:
			CHECK_STATUS(*ptr != ' ', FLP_ILL_CHAR, cleanup);

			// Now either a quote or a hex digit
		   ch = *++ptr;
			if ( ch == '"' || ch == '\'' ) {
				uint16 checksum = 0x0000;

				// Get the file to read bytes from:
				ptr++;
				p = ptr;
				while ( *p != ch && *p != '\0' ) {
					p++;
				}
				CHECK_STATUS(*p == '\0', FLP_UNTERM_STRING, cleanup);
				fileName = malloc((size_t)(p - ptr + 1));
				CHECK_STATUS(!fileName, FLP_NO_MEMORY, cleanup);
				CHECK_STATUS(p - ptr == 0, FLP_EMPTY_STRING, cleanup);
				strncpy(fileName, ptr, (size_t)(p - ptr));
				fileName[p - ptr] = '\0';
				ptr = p + 1;  // skip over closing quote

				// Open file for reading
				file = fopen(fileName, "rb");
				CHECK_STATUS(!file, FLP_CANNOT_LOAD, cleanup);
				free(fileName);
				fileName = NULL;
				
				#ifdef WIN32
					QueryPerformanceCounter(&tvStart);
					status = doWrite(handle, (uint8)chan, file, &length, &checksum, error);
					QueryPerformanceCounter(&tvEnd);
					totalTime = (double)(tvEnd.QuadPart - tvStart.QuadPart);
					totalTime /= freq.QuadPart;
					speed = (double)length / (1024*1024*totalTime);
				#else
					gettimeofday(&tvStart, NULL);
					status = doWrite(handle, (uint8)chan, file, &length, &checksum, error);
					gettimeofday(&tvEnd, NULL);
					startTime = tvStart.tv_sec;
					startTime *= 1000000;
					startTime += tvStart.tv_usec;
					endTime = tvEnd.tv_sec;
					endTime *= 1000000;
					endTime += tvEnd.tv_usec;
					totalTime = (double)(endTime - startTime);
					totalTime /= 1000000;  // convert from uS to S.
					speed = (double)length / (1024*1024*totalTime);
				#endif
				if ( enableBenchmarking ) {
					printf(
						"Wrote "PFSZD" bytes (checksum 0x%04X) to channel %lu at %f MiB/s\n",
						length, checksum, chan, speed);
				}
				CHECK_STATUS(status, status, cleanup);

				// Close the file
				fclose(file);
				file = NULL;
			} else if ( isHexDigit(ch) ) {
				// Read a sequence of hex bytes to write
				uint8 *dataPtr;
				p = ptr + 1;
				while ( isHexDigit(*p) ) {
					p++;
				}
				CHECK_STATUS((p - ptr) & 1, FLP_ODD_DIGITS, cleanup);
				length = (size_t)(p - ptr) / 2;
				data = malloc(length);
				dataPtr = data;
				for ( i = 0; i < length; i++ ) {
					getHexByte(dataPtr++);
					ptr += 2;
				}
				#ifdef WIN32
					QueryPerformanceCounter(&tvStart);
					fStatus = flWriteChannel(handle, (uint8)chan, length, data, error);
					QueryPerformanceCounter(&tvEnd);
					totalTime = (double)(tvEnd.QuadPart - tvStart.QuadPart);
					totalTime /= freq.QuadPart;
					speed = (double)length / (1024*1024*totalTime);
				#else
					gettimeofday(&tvStart, NULL);
					fStatus = flWriteChannel(handle, (uint8)chan, length, data, error);
					gettimeofday(&tvEnd, NULL);
					startTime = tvStart.tv_sec;
					startTime *= 1000000;
					startTime += tvStart.tv_usec;
					endTime = tvEnd.tv_sec;
					endTime *= 1000000;
					endTime += tvEnd.tv_usec;
					totalTime = (double)(endTime - startTime);
					totalTime /= 1000000;  // convert from uS to S.
					speed = (double)length / (1024*1024*totalTime);
				#endif
				if ( enableBenchmarking ) {
					printf(
						"Wrote "PFSZD" bytes (checksum 0x%04X) to channel %lu at %f MiB/s\n",
						length, calcChecksum(data, length), chan, speed);
				}
				CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
				free(data);
				data = NULL;
			} else {
				FAIL(FLP_ILL_CHAR, cleanup);
			}
			break;
		}
		case '+':{
			uint32 conduit;
			char *end;
			ptr++;

			// Get the conduit
			errno = 0;
			conduit = (uint32)strtoul(ptr, &end, 16);
			CHECK_STATUS(errno, FLP_BAD_HEX, cleanup);

			// Ensure that it's 0-127
			CHECK_STATUS(conduit > 255, FLP_CONDUIT_RANGE, cleanup);
			ptr = end;

			// Only two valid chars at this point:
			CHECK_STATUS(*ptr != '\0' && *ptr != ';', FLP_ILL_CHAR, cleanup);

			fStatus = flSelectConduit(handle, (uint8)conduit, error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			break;
		}
		default:
			FAIL(FLP_ILL_CHAR, cleanup);
		}
	} while ( *ptr == ';' );
	CHECK_STATUS(*ptr != '\0', FLP_ILL_CHAR, cleanup);

	dump(0x00000000, dataFromFPGA.data, dataFromFPGA.length);

cleanup:
	bufDestroy(&dataFromFPGA);
	if ( file ) {
		fclose(file);
	}
	free(fileName);
	free(data);
	if ( retVal > FLP_LIBERR ) {
		const int column = (int)(ptr - line);
		int i;
		fprintf(stderr, "%s at column %d\n  %s\n  ", errMessages[retVal], column, line);
		for ( i = 0; i < column; i++ ) {
			fprintf(stderr, " ");
		}
		fprintf(stderr, "^\n");
	}
	return retVal;
}

static const char *nibbles[] = {
	"0000",  // '0'
	"0001",  // '1'
	"0010",  // '2'
	"0011",  // '3'
	"0100",  // '4'
	"0101",  // '5'
	"0110",  // '6'
	"0111",  // '7'
	"1000",  // '8'
	"1001",  // '9'

	"XXXX",  // ':'
	"XXXX",  // ';'
	"XXXX",  // '<'
	"XXXX",  // '='
	"XXXX",  // '>'
	"XXXX",  // '?'
	"XXXX",  // '@'

	"1010",  // 'A'
	"1011",  // 'B'
	"1100",  // 'C'
	"1101",  // 'D'
	"1110",  // 'E'
	"1111"   // 'F'
};

int main(int argc, char *argv[]) {
	ReturnCode retVal = FLP_SUCCESS, pStatus;
	struct arg_str *ivpOpt = arg_str0("i", "ivp", "<VID:PID>", "            vendor ID and product ID (e.g 04B4:8613)");
	struct arg_str *vpOpt = arg_str1("v", "vp", "<VID:PID[:DID]>", "       VID, PID and opt. dev ID (e.g 1D50:602B:0001)");
	struct arg_str *fwOpt = arg_str0("f", "fw", "<firmware.hex>", "        firmware to RAM-load (or use std fw)");
	struct arg_str *portOpt = arg_str0("d", "ports", "<bitCfg[,bitCfg]*>", " read/write digital ports (e.g B13+,C1-,B2?)");
	struct arg_str *queryOpt = arg_str0("q", "query", "<jtagBits>", "         query the JTAG chain");
	struct arg_str *progOpt = arg_str0("p", "program", "<config>", "         program a device");
	struct arg_uint *conOpt = arg_uint0("c", "conduit", "<conduit>", "        which comm conduit to choose (default 0x01)");
	struct arg_str *actOpt = arg_str0("a", "action", "<actionString>", "    a series of CommFPGA actions");
	struct arg_lit *shellOpt  = arg_lit0("s", "shell", "                    start up an interactive CommFPGA session");
	struct arg_lit *benOpt  = arg_lit0("b", "benchmark", "                enable benchmarking & checksumming");
	struct arg_lit *rstOpt  = arg_lit0("r", "reset", "                    reset the bulk endpoints");
	struct arg_str *dumpOpt = arg_str0("l", "dumploop", "<ch:file.bin>", "   write data from channel ch to file");
	struct arg_lit *helpOpt  = arg_lit0("h", "help", "                     print this help and exit");
	struct arg_str *eepromOpt  = arg_str0(NULL, "eeprom", "<std|fw.hex|fw.iic>", "   write firmware to FX2's EEPROM (!!)");
	struct arg_str *backupOpt  = arg_str0(NULL, "backup", "<kbitSize:fw.iic>", "     backup FX2's EEPROM (e.g 128:fw.iic)\n");
	struct arg_end *endOpt   = arg_end(20);
	void *argTable[] = {
		ivpOpt, vpOpt, fwOpt, portOpt, queryOpt, progOpt, conOpt, actOpt,
		shellOpt, benOpt, rstOpt, dumpOpt, helpOpt, eepromOpt, backupOpt, endOpt
	};
	const char *progName = "flcli";
	int numErrors;
	struct FLContext *handle = NULL;
	FLStatus fStatus;
	const char *error = NULL;
	const char *ivp = NULL;
	const char *vp = NULL;
	bool isNeroCapable, isCommCapable;
	uint32 numDevices, scanChain[16], i;
	const char *line = NULL;
	uint8 conduit = 0x01;

	if ( arg_nullcheck(argTable) != 0 ) {
		fprintf(stderr, "%s: insufficient memory\n", progName);
		FAIL(1, cleanup);
	}

	numErrors = arg_parse(argc, argv, argTable);

	if ( helpOpt->count > 0 ) {
		printf("FPGALink Command-Line Interface Copyright (C) 2012-2014 Chris McClelland\n\nUsage: %s", progName);
		arg_print_syntax(stdout, argTable, "\n");
		printf("\nInteract with an FPGALink device.\n\n");
		arg_print_glossary(stdout, argTable,"  %-10s %s\n");
		FAIL(FLP_SUCCESS, cleanup);
	}

	if ( numErrors > 0 ) {
		arg_print_errors(stdout, endOpt, progName);
		fprintf(stderr, "Try '%s --help' for more information.\n", progName);
		FAIL(FLP_ARGS, cleanup);
	}

	fStatus = flInitialise(0, &error);
	CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);

	vp = vpOpt->sval[0];

	printf("Attempting to open connection to FPGALink device %s...\n", vp);
	fStatus = flOpen(vp, &handle, NULL);
	if ( fStatus ) {
		if ( ivpOpt->count ) {
			int count = 60;
			uint8 flag;
			ivp = ivpOpt->sval[0];
			printf("Loading firmware into %s...\n", ivp);
			if ( fwOpt->count ) {
				fStatus = flLoadCustomFirmware(ivp, fwOpt->sval[0], &error);
			} else {
				fStatus = flLoadStandardFirmware(ivp, vp, &error);
			}
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			
			printf("Awaiting renumeration");
			flSleep(1000);
			do {
				printf(".");
				fflush(stdout);
				fStatus = flIsDeviceAvailable(vp, &flag, &error);
				CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
				flSleep(250);
				count--;
			} while ( !flag && count );
			printf("\n");
			if ( !flag ) {
				fprintf(stderr, "FPGALink device did not renumerate properly as %s\n", vp);
				FAIL(FLP_LIBERR, cleanup);
			}

			printf("Attempting to open connection to FPGLink device %s again...\n", vp);
			fStatus = flOpen(vp, &handle, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		} else {
			fprintf(stderr, "Could not open FPGALink device at %s and no initial VID:PID was supplied\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}

	printf(
		"Connected to FPGALink device %s (firmwareID: 0x%04X, firmwareVersion: 0x%08X)\n",
		vp, flGetFirmwareID(handle), flGetFirmwareVersion(handle)
	);

	if ( eepromOpt->count ) {
		if ( !strcmp("std", eepromOpt->sval[0]) ) {
			printf("Writing the standard FPGALink firmware to the FX2's EEPROM...\n");
			fStatus = flFlashStandardFirmware(handle, vp, &error);
		} else {
			printf("Writing custom FPGALink firmware from %s to the FX2's EEPROM...\n", eepromOpt->sval[0]);
			fStatus = flFlashCustomFirmware(handle, eepromOpt->sval[0], &error);
		}
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
	}

	if ( backupOpt->count ) {
		const char *fileName;
		const uint32 kbitSize = strtoul(backupOpt->sval[0], (char**)&fileName, 0);
		if ( *fileName != ':' ) {
			fprintf(stderr, "%s: invalid argument to option --backup=<kbitSize:fw.iic>\n", progName);
			FAIL(FLP_ARGS, cleanup);
		}
		fileName++;
		printf("Saving a backup of %d kbit from the FX2's EEPROM to %s...\n", kbitSize, fileName);
		fStatus = flSaveFirmware(handle, kbitSize, fileName, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
	}

	if ( rstOpt->count ) {
		// Reset the bulk endpoints (only needed in some virtualised environments)
		fStatus = flResetToggle(handle, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
	}

	if ( conOpt->count ) {
		conduit = (uint8)conOpt->ival[0];
	}

	isNeroCapable = flIsNeroCapable(handle);
	isCommCapable = flIsCommCapable(handle, conduit);

	if ( portOpt->count ) {
		uint32 readState;
		char hex[9];
		const uint8 *p = (const uint8 *)hex;
		printf("Configuring ports...\n");
		fStatus = flMultiBitPortAccess(handle, portOpt->sval[0], &readState, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		sprintf(hex, "%08X", readState);
		printf("Readback:   28   24   20   16    12    8    4    0\n          %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf("  %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf(" %s\n", nibbles[*p++ - '0']);
		flSleep(100);
	}

	if ( queryOpt->count ) {
		if ( isNeroCapable ) {
			fStatus = flSelectConduit(handle, 0x00, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = jtagScanChain(handle, queryOpt->sval[0], &numDevices, scanChain, 16, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			if ( numDevices ) {
				printf("The FPGALink device at %s scanned its JTAG chain, yielding:\n", vp);
				for ( i = 0; i < numDevices; i++ ) {
					printf("  0x%08X\n", scanChain[i]);
				}
			} else {
				printf("The FPGALink device at %s scanned its JTAG chain but did not find any attached devices\n", vp);
			}
		} else {
			fprintf(stderr, "JTAG chain scan requested but FPGALink device at %s does not support NeroProg\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}

	if ( progOpt->count ) {
		printf("Programming device...\n");
		if ( isNeroCapable ) {
			fStatus = flSelectConduit(handle, 0x00, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = flProgram(handle, progOpt->sval[0], NULL, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		} else {
			fprintf(stderr, "Program operation requested but device at %s does not support NeroProg\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}

	if ( benOpt->count ) {
		enableBenchmarking = true;
	}
	
	if ( actOpt->count ) {
		printf("Executing CommFPGA actions on FPGALink device %s...\n", vp);
		if ( isCommCapable ) {
			uint8 isRunning;
			fStatus = flSelectConduit(handle, conduit, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = flIsFPGARunning(handle, &isRunning, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			if ( isRunning ) {
				pStatus = parseLine(handle, actOpt->sval[0], &error);
				CHECK_STATUS(pStatus, pStatus, cleanup);
			} else {
				fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --program?\n", vp);
				FAIL(FLP_ARGS, cleanup);
			}
		} else {
			fprintf(stderr, "Action requested but device at %s does not support CommFPGA\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}

	if ( dumpOpt->count ) {
		const char *fileName;
		unsigned long chan = strtoul(dumpOpt->sval[0], (char**)&fileName, 10);
		FILE *file = NULL;
		const uint8 *recvData;
		uint32 actualLength;
		if ( *fileName != ':' ) {
			fprintf(stderr, "%s: invalid argument to option -l|--dumploop=<ch:file.bin>\n", progName);
			FAIL(FLP_ARGS, cleanup);
		}
		fileName++;
		printf("Copying from channel %lu to %s", chan, fileName);
		file = fopen(fileName, "wb");
		CHECK_STATUS(!file, FLP_CANNOT_SAVE, cleanup);
		sigRegisterHandler();
		fStatus = flSelectConduit(handle, conduit, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		fStatus = flReadChannelAsyncSubmit(handle, (uint8)chan, 22528, NULL, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		do {
			fStatus = flReadChannelAsyncSubmit(handle, (uint8)chan, 22528, NULL, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fwrite(recvData, 1, actualLength, file);
			printf(".");
		} while ( !sigIsRaised() );
		printf("\nCaught SIGINT, quitting...\n");
		fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		fwrite(recvData, 1, actualLength, file);
		fclose(file);
	}

	if ( shellOpt->count ) {
		printf("\nEntering CommFPGA command-line mode:\n");
		if ( isCommCapable ) {
		   uint8 isRunning;
			fStatus = flSelectConduit(handle, conduit, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = flIsFPGARunning(handle, &isRunning, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			if ( isRunning ) {
				do {
					do {
						line = readline("> ");
					} while ( line && !line[0] );
					if ( line && line[0] && line[0] != 'q' ) {
						add_history(line);
						pStatus = parseLine(handle, line, &error);
						CHECK_STATUS(pStatus, pStatus, cleanup);
						free((void*)line);
					}
				} while ( line && line[0] != 'q' );
			} else {
				fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
				FAIL(FLP_ARGS, cleanup);
			}
		} else {
			fprintf(stderr, "Shell requested but device at %s does not support CommFPGA\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}
				

				//to do -- read network.txt

					uint8 chan_read;
					uint8 chan_write;
					uint32 count = 1;
					FLStatus mystatus;
					int change_info = 0;
		
					_Bool P[32];
					_Bool C[32];
					_Bool rec_Ack0[32];
					int pos_X;
					int pos_Y;
					_Bool out[8];
					_Bool fpgainfo[8];
			
				for (int chan_i = 0; chan_i < 64; ++chan_i){ //big loop polling
					printf("in loop %d\n", chan_i);
					int proceed = 1;
					chan_read = 2*chan_i;
					chan_write = 2*chan_i+1;
					for (int i = 0; i < 4; ++i) //read 4 byte for pos
					{	count = 1;
						struct Buffer Readbuffer = {0,};
						BufferStatus bStatus;

						bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						size_t oldLength = Readbuffer.length;
						bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						if ( isCommCapable ){
				 			 uint8 isRunning;
							fStatus = flSelectConduit(handle, conduit, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							fStatus = flIsFPGARunning(handle, &isRunning, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							
							if ( isRunning ){

								mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
							} 
						else {
						fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
						FAIL(FLP_ARGS, cleanup);
						}
						}
				
						 uint8_tobit(*Readbuffer.data,out);
						 int k = 0;
						 for (int j = i*8; j < i*8 + 8; ++j)
						 {
						 	P[j] = out[j-i*8];
						 	k++;
							
						 }

					} //end of read 4 byte for pos
				
				
				 	decrypter(P,K,C);
				
					pos_X = bit_touint8(C+24)/16;
					pos_Y = bit_touint8(C+24)%16;

					
					encrypter(C,K,P);		
					
					for (int i = 0; i < 4; ++i) //write back received 4 byte
					{
						count = 1;
						uint8 Writebuffer;
						Writebuffer = bit_touint8(P+i*8);
						
						
						if ( isCommCapable ){
				 			 uint8 isRunning;
							fStatus = flSelectConduit(handle, conduit, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							fStatus = flIsFPGARunning(handle, &isRunning, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							
							if ( isRunning ){
								mystatus = flWriteChannel(handle,chan_write,count,&Writebuffer,&error);
							} 
						else {
						fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
						FAIL(FLP_ARGS, cleanup);
						}
						}
						
					} //end of write back received 4 byte

					for (int i = 0; i < 4; ++i) // read the ack from the board
					{	count = 1;
						struct Buffer Readbuffer = {0,};
						BufferStatus bStatus;

						bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						size_t oldLength = Readbuffer.length;
						bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						if ( isCommCapable ){
				 			 uint8 isRunning;
							fStatus = flSelectConduit(handle, conduit, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							fStatus = flIsFPGARunning(handle, &isRunning, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							
							if ( isRunning ){
								mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
							} 
						else {
						fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
						FAIL(FLP_ARGS, cleanup);
						}
						}
				
						 uint8_tobit(*Readbuffer.data,out);
						 int k = 0;
						 for (int j = i*8; j < i*8 + 8; ++j)
						 {
						 	rec_Ack0[j] = out[j-i*8];
						 	k++;
							
						 }

					} // end of read of ack

					decrypter(rec_Ack0,K,C);

					// check equal Ack0,rec_Ack0
					for(int i=0; i<32; i++){
						if(C[i]==Ack0[i]){
							proceed=1;
						}
						else{proceed=0;
							 break;}
					}

					if(proceed==0){
						flSleep(5000);
							for (int i = 0; i < 4; ++i){ // wait and again read the ack
								count = 1;
								struct Buffer Readbuffer = {0,};
								BufferStatus bStatus;

								bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
								CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
								size_t oldLength = Readbuffer.length;
								bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
								CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
								if ( isCommCapable ){
						 			 uint8 isRunning;
									fStatus = flSelectConduit(handle, conduit, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									fStatus = flIsFPGARunning(handle, &isRunning, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									
									if ( isRunning ){
										mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
									} 
								else {
								fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
								FAIL(FLP_ARGS, cleanup);
								}
								}
						
								 uint8_tobit(*Readbuffer.data,out);
								 int k = 0;
								 for (int j = i*8; j < i*8 + 8; ++j)
								 {
								 	rec_Ack0[j] = out[j-i*8];
								 	k++;
									
								 }

							}

							decrypter(rec_Ack0,K,C);

							// check equal Ack0,rec_Ack0
							for(int i=0; i<32; i++){
								if(C[i]==Ack0[i]){
									proceed=1;
								}
								else{proceed=0;
									 break;}
							}
							if(proceed==0){
								change_info = 0;
								if(chan_i = 63){
									chan_i = -1;
								}
								
								continue;}
					}


					else if(proceed==1){

					// save map co-ordinates with chan_i 
					
							encrypter(Ack1,K,C);
									
							for (int i = 0; i < 4; ++i)	{ // send ack1 for communication
								count = 1;
								uint8 Writebuffer;
								Writebuffer = bit_touint8(C+i*8);
							
							
								if ( isCommCapable ){
					 				 uint8 isRunning;
									fStatus = flSelectConduit(handle, conduit, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									fStatus = flIsFPGARunning(handle, &isRunning, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
								
									if ( isRunning ){
										mystatus = flWriteChannel(handle,chan_write,count,&Writebuffer,&error);
									} 
								else {
								fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
								FAIL(FLP_ARGS, cleanup);
								}
								}
							
							}
							if (change_info == 0)
							{
								readCsv(pos_X,pos_Y);
							}
							if (change_info == 1)
							{
								for (int i = 0; i < 8; ++i)
								{
									if(info[i][2] == fpgainfo[2] && info[i][3] == fpgainfo[3] && info[i][4] == fpgainfo[4]){
										
										info[i][1] = fpgainfo[1];
										info[i][5] = fpgainfo[5];
										info[i][6] = fpgainfo[6];
										info[i][7] = fpgainfo[7];
									}
								}
							}


							for (int i = 0; i < 32; ++i){
								P[i] = info[i/8][i-8*(i/8)];
							}

					//encode P
							encrypter(P,K,C);

							for (int i = 0; i < 4; ++i){ // write first 4 info byte
								count = 1;
								uint8 Writebuffer;
								Writebuffer = bit_touint8(C+i*8);
							
							
								if ( isCommCapable ){
					 				 uint8 isRunning;
									fStatus = flSelectConduit(handle, conduit, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									fStatus = flIsFPGARunning(handle, &isRunning, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
								
									if ( isRunning ){
										mystatus = flWriteChannel(handle,chan_write,count,&Writebuffer,&error);
									} 
								else {
								fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
								FAIL(FLP_ARGS, cleanup);
								}
								}
							
							}

							for (int i = 0; i < 4; ++i){ //read 4 byte for ack0
								count = 1;
								struct Buffer Readbuffer = {0,};
								BufferStatus bStatus;

								bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
								CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
								size_t oldLength = Readbuffer.length;
								bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
								CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
								if ( isCommCapable ){
					 				 uint8 isRunning;
									fStatus = flSelectConduit(handle, conduit, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									fStatus = flIsFPGARunning(handle, &isRunning, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
								
									if ( isRunning ){
										mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
									} 
								else {
									fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
								FAIL(FLP_ARGS, cleanup);
								}
							}
				
							 uint8_tobit(*Readbuffer.data,out);
							 int k = 0;
							 for (int j = i*8; j < i*8 + 8; ++j)
							 {
							 	rec_Ack0[j] = out[j-i*8];
							 	k++;
								
							 	}

							}

					//decrypt rec_Ack0
							decrypter(rec_Ack0,K,C);

					// check equal Ack0,rec_Ack0
							for(int i=0; i<32; i++){
								if(C[i]==Ack0[i]){
									proceed=1;
								}
								else{proceed=0;
										break;}
							}

	
					if(proceed==0){
							for(int x=0; x<256; x++){
								flSleep(1000);
								for (int i = 0; i < 4; ++i){
								count = 1;
								struct Buffer Readbuffer = {0,};
								BufferStatus bStatus;

								bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
								CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
								size_t oldLength = Readbuffer.length;
								bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
								CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
								if ( isCommCapable ){
						 			 uint8 isRunning;
									fStatus = flSelectConduit(handle, conduit, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									fStatus = flIsFPGARunning(handle, &isRunning, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									
									if ( isRunning ){
										mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
									} 
								else {
								fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
								FAIL(FLP_ARGS, cleanup);
								}
								}
						
								 uint8_tobit(*Readbuffer.data,out);
								 int k = 0;
								 for (int j = i*8; j < i*8 + 8; ++j)
								 {
								 	rec_Ack0[j] = out[j-i*8];
								 	k++;
									
								 }

								}

								decrypter(rec_Ack0,K,C);


					// check equal Ack0,rec_Ack0
								for(int i=0; i<32; i++){
									if(C[i]==Ack0[i]){
										proceed=1;
									}
									else{proceed=0;
										 break;}
								}

								if(proceed==0){continue;}
								else if(proceed==1){break;}

							}

						if(proceed==0){
								change_info = 0;
								if(chan_i = 63){
									chan_i = -1;
								}
								
								continue;}
						}
				
						else if(proceed==1){


							for (int i = 32; i < 64; ++i)
							{
								P[i-32] = info[i/8][i-8*(i/8)];
							}

						//encode P
							encrypter(P,K,C);

								for (int i = 0; i < 4; ++i)
							{
								count = 1;
								uint8 Writebuffer;
								Writebuffer = bit_touint8(C+i*8);
								
								
								if ( isCommCapable ){
						 			 uint8 isRunning;
									fStatus = flSelectConduit(handle, conduit, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									fStatus = flIsFPGARunning(handle, &isRunning, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									
									if ( isRunning ){
										mystatus = flWriteChannel(handle,chan_write,count,&Writebuffer,&error);
									} 
								else {
								fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
								FAIL(FLP_ARGS, cleanup);
								}
								}
								
							}

							for (int i = 0; i < 4; ++i)
							{	count = 1;
								struct Buffer Readbuffer = {0,};
								BufferStatus bStatus;

								bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
								CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
								size_t oldLength = Readbuffer.length;
								bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
								CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
								if ( isCommCapable ){
						 			 uint8 isRunning;
									fStatus = flSelectConduit(handle, conduit, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									fStatus = flIsFPGARunning(handle, &isRunning, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									
									if ( isRunning ){
										mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
									} 
								else {
								fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
								FAIL(FLP_ARGS, cleanup);
								}
								}
						
								 uint8_tobit(*Readbuffer.data,out);
								 int k = 0;
								 for (int j = i*8; j < i*8 + 8; ++j)
								 {
								 	rec_Ack0[j] = out[j-i*8];
								 	k++;
									
								 }

							}

					//decrypt rec_Ack0
							decrypter(rec_Ack0,K,C);

					// check equal Ack0,rec_Ack0
							for(int i=0; i<32; i++){
								if(C[i]==Ack0[i]){
									proceed=1;
								}
								else{proceed=0;
										break;}
							}


							if(proceed==0){
								for(int x=0; x<256; x++){

									flSleep(1000);
									for (int i = 0; i < 4; ++i){
									count = 1;
									struct Buffer Readbuffer = {0,};
									BufferStatus bStatus;

									bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
									CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
									size_t oldLength = Readbuffer.length;
									bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
									CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
									if ( isCommCapable ){
							 			 uint8 isRunning;
										fStatus = flSelectConduit(handle, conduit, &error);
										CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
										fStatus = flIsFPGARunning(handle, &isRunning, &error);
										CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
										
										if ( isRunning ){
											mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
										} 
									else {
									fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
									FAIL(FLP_ARGS, cleanup);
									}
									}
							
									 uint8_tobit(*Readbuffer.data,out);
									 int k = 0;
									 for (int j = i*8; j < i*8 + 8; ++j)
									 {
									 	rec_Ack0[j] = out[j-i*8];
									 	k++;
										
									 }

									}

										decrypter(rec_Ack0,K,C);

							// check equal Ack0,rec_Ack0
										for(int i=0; i<32; i++){
											if(C[i]==Ack0[i]){
												proceed=1;
											}
											else{proceed=0;
												 break;}
										}
										if(proceed==0){continue;}
										else if(proceed==1){break;}
								}


								if(proceed==0){
								change_info = 0;
								if(chan_i = 63){
									chan_i = -1;
								}
								
								continue;}
							}

					else if(proceed==1){

							encrypter(Ack1,K,C);
										
							for (int i = 0; i < 4; ++i)
							{
								count = 1;
								uint8 Writebuffer;
								Writebuffer = bit_touint8(C+i*8);
								
								
								if ( isCommCapable ){
						 			 uint8 isRunning;
									fStatus = flSelectConduit(handle, conduit, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									fStatus = flIsFPGARunning(handle, &isRunning, &error);
									CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
									
									if ( isRunning ){
										mystatus = flWriteChannel(handle,chan_write,count,&Writebuffer,&error);
									} 
								else {
								fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
								FAIL(FLP_ARGS, cleanup);
								}
								}
								
							}
						}
						}
						}
				flSleep(24000);
				if(proceed == 1){
					chan_i--;
				}
				int dataread;
				for (int i = 0; i < 1; ++i)
				{
					
					count = 1;
						struct Buffer Readbuffer = {0,};
						BufferStatus bStatus;

						bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						size_t oldLength = Readbuffer.length;
						bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						if ( isCommCapable ){
				 			 uint8 isRunning;
							fStatus = flSelectConduit(handle, conduit, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							fStatus = flIsFPGARunning(handle, &isRunning, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							
							if ( isRunning ){
								mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
							} 
						else {
						fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
						FAIL(FLP_ARGS, cleanup);
						}
						}
						dataread = *Readbuffer.data;

					}

					if(dataread != 1){
						flSleep(20000);
						continue;}

						int mycount = 20;

						while(dataread == 1 && mycount > 0){
							flSleep(1000);
							mycount--;
							count = 1;
						struct Buffer Readbuffer = {0,};
						BufferStatus bStatus;

						bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						size_t oldLength = Readbuffer.length;
						bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						if ( isCommCapable ){
				 			 uint8 isRunning;
							fStatus = flSelectConduit(handle, conduit, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							fStatus = flIsFPGARunning(handle, &isRunning, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							
							if ( isRunning ){
								mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
							} 
						else {
						fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
						FAIL(FLP_ARGS, cleanup);
						}
						}
						dataread = *Readbuffer.data;
						}

						if(mycount == 0){
							flSleep(20000);
							continue;
						}

					if(dataread == 3){
						printf("got 3");
						count = 1;
						struct Buffer Readbuffer = {0,};
						BufferStatus bStatus;

						bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						size_t oldLength = Readbuffer.length;
						bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						if ( isCommCapable ){
				 			 uint8 isRunning;
							fStatus = flSelectConduit(handle, conduit, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							fStatus = flIsFPGARunning(handle, &isRunning, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							
							if ( isRunning ){
								mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
							} 
						else {
						fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
						FAIL(FLP_ARGS, cleanup);
						}
						}
						dataread = *Readbuffer.data;
					}

					for (int i = 0; i < 4; ++i){
						count = 1;
						struct Buffer Readbuffer = {0,};
						BufferStatus bStatus;

						bStatus = bufInitialise(&Readbuffer, 1024, 0x00, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						size_t oldLength = Readbuffer.length;
						bStatus = bufAppendConst(&Readbuffer, 0x00, count, error);
						CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
						if ( isCommCapable ){
				 			 uint8 isRunning;
							fStatus = flSelectConduit(handle, conduit, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							fStatus = flIsFPGARunning(handle, &isRunning, &error);
							CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
							
							if ( isRunning ){
								mystatus = flReadChannel(handle,chan_read,count,Readbuffer.data + oldLength,&error); 
							} 
						else {
						fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
						FAIL(FLP_ARGS, cleanup);
						}
						}
				
						 uint8_tobit(*Readbuffer.data,out);
						 int k = 0;
						 for (int j = i*8; j < i*8 + 8; ++j)
						 {
						 	C[j] = out[j-i*8];
						 	k++;
							
						 }

					}

					decrypter(C,K,P);

					for (int i = 0; i < 8; ++i)
					{
						fpgainfo[i] = P[24+i];
						change_info = 1;
						printf("%d\n", fpgainfo[i] );
					}

					flSleep(32000);




					


			
		}





					

			
			

cleanup:
	free((void*)line);
	flClose(handle);
	if ( error ) {
		fprintf(stderr, "%s\n", error);
		flFreeError(error);
	}
	return retVal;
}
