#include "hx_utils.h"
#include "stddef.h"
#include "string.h"

int hxl_poll_init(
	struct Poll *pt,
	const void *tbl,
	int tbl_size,
	void *prv
)
{
	memset(pt,0,sizeof(struct Poll));
	pt->poll_tbl = tbl;
	pt->tbl_size = tbl_size;
	pt->prv = prv;
	return 0;
}


/*
	0 is end
	others is not
*/
int hxl_poll(struct Poll *pt)
{
	int res;
	int step = pt->step;
	int count = pt->tbl_size;
	if(step>=count)
		return 0;
	const struct PoStage *ps = ((const struct PoStage **)(pt->poll_tbl))[step];
	if(pt->req_count==0){
		if(step==0){
			pt->tmof_stage_change = hx_get_tick_count();
			pt->tmof_req = pt->tmof_stage_change = hx_get_tick_count();
			ps->f(ps,pt,PE_ON_INIT);
		}
		ps->f(ps,pt,PE_ON_STAGE_BEG);
		ps->f(ps,pt,PE_REQUEST);
		pt->req_count++;
		pt->tmof_req = hx_get_tick_count();		//req first times
		return -1;
	}else{
		res = ps->f(ps,pt,PE_CHECK_ACK);
		if(res==0){
			step ++;
			if(step>=count){
				pt->step = count;
				ps->f(ps,pt,PE_ON_COMPLETE);
				return 0;	//all stage complete.
			}else{
				ps->f(ps,pt,PE_ON_STAGE_END);
				pt->step = step;
				pt->req_count = 0;
				pt->tmof_stage_change = hx_get_tick_count();
				pt->tmof_req = pt->tmof_stage_change = hx_get_tick_count();
				ps->f(ps,pt,PE_ON_STAGE_BEG);
				return -1;	//stage change
			}
		}else{
			if(ps->timeout && hx_check_timeout(pt->tmof_stage_change,ps->timeout)){
				pt->step = 0;		//global timeout escape
				pt->req_count = 0;
				pt->tmof_stage_change = hx_get_tick_count();
				pt->tmof_req = pt->tmof_stage_change = hx_get_tick_count();
				ps->f(ps,pt,PE_ON_TIMEOUT_RESET);
				return -2;		//timeout reset
			}
			if(ps->timeout4retry && hx_check_timeout(pt->tmof_req,ps->timeout4retry)){
				ps->f(ps,pt,PE_REQUEST);
				pt->req_count++;
				pt->tmof_req = hx_get_tick_count();	//req timeout escape
				return -3;	//timeout request
			}
		}
	}
	return -4;	//idle
}



//-------------------------------------------------------------------------
int hxl_poll_info_dbg(struct Poll *pt)
{
	static int step = -1;
	if(step != pt->step){
		step = pt->step;
		hx_dbg(0,"step: %d\n",pt->step);
		hx_dbg(0,"step count: %d\n",pt->tbl_size);
	}
	return 0;
}

typedef const struct  {
	struct PoStage ps;
	char *r;
	char *q;
} ATCStage;

int atc_cmd_res_find(const struct PoStage *this, struct Poll *pt, enum PoEvent pe)
{
	ATCStage *atcs = (ATCStage*)this;
	switch(pe){
		case PE_REQUEST:
			atc_puts(atcs->r);
			return 0;
		case PE_CHECK_ACK:
			return 0;//atc_gets_noblock_strstr(atcs->q);
		default:
			return 0;
	}
}
ATCStage *atc_tbl[] = {
	&(ATCStage){{20000,2000,atc_cmd_res_find},.r="AT",.q="OK"},
	&(ATCStage){{20000,2000,atc_cmd_res_find},.r="AT+CPIN?",.q="+CPIN: OK"},
};


int poll_test(void)
{
	struct Poll pt;
	hxl_poll_init(
		&pt,
		atc_tbl,
		sizeof(atc_tbl)/sizeof(atc_tbl[0]),
		0);
	while(1){
		hxl_poll(&pt);
		hxl_poll_info_dbg(&pt);
	}
}

