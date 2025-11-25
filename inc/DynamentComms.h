#ifndef _DYNAMENTCOMMS
#define _DYNAMENTCOMMS

#include <stdint.h>


/* Global type definitions ----------------------------------------------------*/
typedef void (*GetDualFloat_CallBack_t)(int valueStatus, float fval1, float fval2);

typedef void (*GetFloat_CallBack_t)(int valueStatus, float fval);
/* Global constants -----------------------------------------------------------*/


/* Global variables -----------------------------------------------------------*/


#define FRAME_TIMEOUT 3 // the number of 10ms steps of receiving no data after ahving been receiving data to determine that a packet has finished being received
#define MESSAGE_TIMEOUT 50 // the number of 10ms steps of receiving no response after a data requiest before timing out
// // status responses to be used tin teh read measurand callback
#define READ_RESPONSE_TIMED_OUT          0
#define READ_RESPONSE_INVALID_REGISTER   1
#define READ_RESPONSE_VALUE_INVALID      2
#define READ_RESPONSE_VALUE_VALID        3

/* Function Prototypes --------------------------------------------------------*/
void InitialiseDynamentComms();
void DynamentCommsHandler();

// void DecodeMessage(void);
// void ReadMeasurand( uint16_t address, GetFloat_CallBack_t cb );
// void MessageTimedOut(void);
void RequestLiveDataSimple (GetFloat_CallBack_t cb );
void RequestLiveData2 (GetDualFloat_CallBack_t cb );

#endif

/*** end of file ***/
