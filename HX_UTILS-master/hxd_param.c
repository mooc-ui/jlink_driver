/*
 * hxd_param.c
 *
 *  Created on: 2016Äê11ÔÂ17ÈÕ
 *      Author: houxd
 */
#include "stdio.h"
#include "hx_utils.h"
#include "hxd_param.h"
#include "string.h"

int hxd_param_open(HX_DEV *dev,const char *param)
{
	PARAM_DRV_T* drv = (PARAM_DRV_T*)(dev->pdev->driver);
	if(drv->load)
		return drv->load();
	return 0;
}
int hxd_param_read(HX_DEV *dev,void *buf,int _size)
{
	int res;
	char s[128] = {0};
	memcpy(s,buf,_size);
	char *lv = s;
	const PARAM_DEV_T *pdev = (const PARAM_DEV_T *)(dev->pdev);
	const PARAM_ITEM_T *g_param_tbl = pdev->tbl;
	int g_param_count = pdev->count;
	const PARAM_ITEM_T *pm;
	foreach(g_param_tbl,g_param_count,pm){
		if(strcmp(pm->name,lv)==0){
			res = hx_value2str(pm->data,pm->data_type,buf);
			if(res){
				return -2;
			}
			dev->offset = pm-g_param_tbl;
			return 0;
		}
	}
	return -1;
}
int _param_write_item(HX_DEV *dev,const char *s)
{
	if(!s || s[0]==0)
		return -9;
	char item[64] = {0};
	strncpy(item,s,63);
	int res;
	char *ps;
	const char *lv = strtok_r(item,"=",&ps);
	const char *rv = strtok_r(NULL,"=",&ps);
	if(lv==NULL)
		return -10;
	if(rv==NULL)
		rv = "";
	const PARAM_DEV_T *pdev = (const PARAM_DEV_T *)(dev->pdev);
	const PARAM_ITEM_T *g_param_tbl = pdev->tbl;
	int g_param_count = pdev->count;
	const PARAM_ITEM_T *pm;
	foreach(g_param_tbl,g_param_count,pm){
		if(strcmp(pm->name,lv)==0){
			res = hx_str2value(rv,pm->data_type,pm->data);
			if(res){
				hxt_printf("Change Param Error: %s,type:%s\r\n",
					pm->name,pm->data_type);
				return -2;
			}
			dev->offset = pm-g_param_tbl;
			return 0;
		}
	}
	return -1;
}
int hxd_param_write(HX_DEV *dev,const void *buf, int size)
{
	int res;
	if(size>(512-1) || size<=0)
		return -9;
	char s[512] = {0};
	memcpy(s,buf,size);
	char *ps;
	char *p;
	p = strtok_r(s,"\n",&ps);
	while(p){
		res = _param_write_item(dev,p);
		if(res){
			return res;
		}
		p = strtok_r(NULL,"\n",&ps);
	};
	return 0;
}


int hxd_param_close(HX_DEV *dev)
{
	PARAM_DRV_T* drv = (PARAM_DRV_T*)(dev->pdev->driver);
	if(drv->store)
		return drv->store();
	return 0;
}


