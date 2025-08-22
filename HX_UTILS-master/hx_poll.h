#ifndef __HXL_POLL__
#define __HXL_POLL__

/*
request opeartor,that might be call many times with 'timeout4retry',
until 'ack' operator successed or escape of 'timeout'. 
follow stream:
                                           'timeout4retry'  'timeout'
										              |        |
    [First Stage] -...-> [Lask Stage] -> 'req' send --+--------+----> 'ack' get -> [Next Stage]
           /\                              /\         |        |
			\		                        \---------/        |
			 \-------------------------------------------------/
							  
*/

typedef struct __REQ_OPERATOR {
	const void *p;
	void (*f)(void *this, const void *p);
} REQ_OP_T;

typedef struct __ACK_OPERATOR {
	const void *p;
	int (*f)(void *this, const void *p);
} ACK_OP_T;

typedef struct __POLL_STAGE_T {
	REQ_OP_T req;		
	ACK_OP_T ack;
	int timeout4retry;
	int timeout;
} POLL_STAGE_T;

typedef struct __POLL_T {
	const POLL_STAGE_T * pool_tbl;	//poll table
	int tbl_size;
	void *prv;		//private data like params etc.
	int step;					//current stage step
	int req_count;				//request times
	int tmof_stage_change;		//time of last stage changed.
	int tmof_req;				//time of last req sent.
} POLL_T;


#endif



