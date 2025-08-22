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

enum PoEvent{
	PE_REQUEST,		//����
	PE_CHECK_ACK,		//Ӧ��
	
	PE_ON_INIT,			//�״γ�ʼ��
	PE_ON_COMPLETE,		//��ѵ���
	PE_ON_STAGE_BEG,	//һ����ʼ
	PE_ON_STAGE_END,	//һ������
	PE_ON_TIMEOUT_RESET,	//��ʱ��λ
	
};

struct Poll {
	const void *poll_tbl;	//const struct PoStage **
	int tbl_size;				//stage table size
	void *prv;		//private data like params etc.
	int step;					//current stage step
	int req_count;				//request times
	int tmof_stage_change;		//time of last stage changed.
	int tmof_req;				//time of last req sent.
};

struct PoStage {
	int timeout;		//timeout for all stage lives, 0 is invalable
	int timeout4retry;	//timeout for retry ,0 is invalable
	int (*f)(const struct PoStage *this, struct Poll *pt, enum PoEvent pe);		//dell poll event
};




extern int hxl_poll_init(
	struct Poll *pt,		/* poll instance*/
	const void *tbl,	/* poll table that items extands from class PoStage */
	int tbl_size,			/* table size */
	void *prv				/* user defined private data like params etc. */
);
/*
	0 is end
	others is not
*/
extern int hxl_poll(struct Poll *pt);	




#endif



