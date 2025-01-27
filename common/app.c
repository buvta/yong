#include "llib.h"
#include "common.h"
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

struct app_item{
	void *next;
	char *exe;
	LKeyFile *config;
};
static void app_item_free(struct app_item *p)
{
	l_free(p->exe);
	l_key_file_free(p->config);
	l_free(p);
}

static LHashTable *app;

int y_im_load_app_config(void)
{
	char *file=y_im_get_config_string("IM","app");
	if(!file)
	{
		return 0;
	}
	LKeyFile *key_file=l_key_file_open(
					file,0,
					y_im_get_path("HOME"),
					y_im_get_path("DATA"),
					NULL);
	l_free(file);
	if(!key_file)
	{
		return 0;
	}
	char **groups=l_key_file_get_groups(key_file);
	int count=l_strv_length(groups);
	if(count==0)
	{
		l_strfreev(groups);
		l_key_file_free(key_file);
		return 0;
	}
	app=L_HASH_TABLE_STRING(struct app_item,exe,0);
	int i;
	for(i=0;i<count;i++)
	{
		const char *group,*exe,*val;
		LKeyFile *config=NULL;
		struct app_item *item;
		group=groups[i];
		exe=l_key_file_get_data(key_file,group,"exe");
		if(!exe)
			continue;
		val=l_key_file_get_string(key_file,group,"config");
		if(val && val[0])
		{
			config=l_key_file_load(val,-1);
			if(!config)
				continue;
		}
		val=l_key_file_get_data(key_file,group,"disable");
		if(val && val[0])
		{
			if(!config)
				config=l_key_file_load("",0);
			l_key_file_set_int(config,"IM","enable",!atoi(val));
		}
		val=l_key_file_get_data(key_file,group,"onspot");
		if(val && val[0])
		{
			if(!config)
				config=l_key_file_load("",0);
			l_key_file_set_int(config,"IM","onspot",atoi(val));
		}
		val=l_key_file_get_data(key_file,group,"lang");
		if(val && val[0])
		{
			if(!config)
				config=l_key_file_load("",0);
			l_key_file_set_int(config,"IM","lang",atoi(val));
		}
		if(!config)
			continue;
		item=l_new(struct app_item);
		item->exe=l_strdup(exe);
		l_strdown(item->exe);
		item->config=config;
		if(!l_hash_table_insert(app,item))
			app_item_free(item);
	}
	l_strfreev(groups);
	l_key_file_free(key_file);
	return 0;
}

void y_im_free_app_config(void)
{
	l_hash_table_free(app,(LFreeFunc)app_item_free);
}

LKeyFile *y_im_get_app_config(const char *exe)
{
	struct app_item *item;
	if(!app)
		return NULL;
	item=l_hash_table_lookup(app,exe);
	if(!item)
		return NULL;
	return item->config;
}

#ifdef _WIN32
extern DWORD OSMajorVersion;
static BOOL (WINAPI * p_QueryFullProcessImageNameW)(HANDLE hProcess,DWORD  dwFlags,LPWSTR  lpExeName,PDWORD lpdwSize);
static int xp=-1;
char *y_im_get_app_exe_by_hwnd(HWND w,char out[256])
{
	HANDLE h;
	DWORD pid=0;
	GetWindowThreadProcessId(w,&pid);
	if(!pid)
		return NULL;
	if(OSMajorVersion>=6 && !p_QueryFullProcessImageNameW)
	{
		HINSTANCE hInst;
		hInst=GetModuleHandle(_T("kernel32.dll"));
		if(hInst)
		{
			p_QueryFullProcessImageNameW=(void*)GetProcAddress(hInst,"QueryFullProcessImageNameW");
		}
	}

	if(xp==0)
	{
		h=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
	}
	else if(xp==1)
	{
		h=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
	}
	else
	{
		h=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
		if(h==NULL)
		{
			h=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
			if(h!=NULL)
				xp=1;
		}
		else
		{
			xp=0;
		}
	}
	if(h==NULL)
		return NULL;
	WCHAR wpath[256];
	if(p_QueryFullProcessImageNameW)
	{
		DWORD size=255;
		BOOL ret=p_QueryFullProcessImageNameW(h,0,wpath,&size);
		if(ret!=TRUE)
		{
			printf("get name1 fail %ld\n",GetLastError());
			return NULL;
		}
	}
	else
	{
		DWORD ret=GetModuleFileNameEx(h,NULL,wpath,255);
		if(ret==0)
		{
			printf("get name fail %ld\n",GetLastError());
			return NULL;
		}
		wpath[ret]=0;
	}
	CloseHandle(h);
	char path[256],*p;
	l_utf16_to_gb(wpath,path,256);
	p=strrchr(path,'\\');
	if(!p)
		return NULL;
	p++;
	l_strup(p);
	strcpy(out,p);
	return out;
}

LKeyFile *y_im_get_app_config_by_pid(int pid)
{
	HANDLE h;
	
	if(!app)
		return NULL;
	if(OSMajorVersion>=6 && !p_QueryFullProcessImageNameW)
	{
		HINSTANCE hInst;
		hInst=GetModuleHandle(_T("kernel32.dll"));
		if(hInst)
		{
			p_QueryFullProcessImageNameW=(void*)GetProcAddress(hInst,"QueryFullProcessImageNameW");
		}
	}

	if(xp==0)
	{
		h=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
	}
	else if(xp==1)
	{
		h=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
	}
	else
	{
		h=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
		if(h==NULL)
		{
			h=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
			if(h!=NULL)
				xp=1;
		}
		else
		{
			xp=0;
		}
	}
	if(h==NULL)
		return NULL;
	WCHAR wpath[256];
	if(p_QueryFullProcessImageNameW)
	{
		DWORD size=255;
		BOOL ret=p_QueryFullProcessImageNameW(h,0,wpath,&size);
		if(ret!=TRUE)
		{
			printf("get name1 fail %ld\n",GetLastError());
			return NULL;
		}
	}
	else
	{
		DWORD ret=GetModuleFileNameEx(h,NULL,wpath,255);
		if(ret==0)
		{
			printf("get name fail %ld\n",GetLastError());
			return NULL;
		}
		wpath[ret]=0;
	}
	CloseHandle(h);
	char path[256],*p;
	l_utf16_to_utf8(wpath,path,256);
	p=strrchr(path,'\\');
	if(!p)
		return NULL;
	p++;
	l_strdown(p);
	return y_im_get_app_config(p);
}
#else
LKeyFile *y_im_get_app_config_by_pid(int pid)
{
	int ret;
	char data[256];
	char *tmp;
	char path[128];
	
	if(!app)
		return NULL;
		
	sprintf(path,"/proc/%d/exe",pid);
	ret=readlink(path,data,256);
	if(ret<=0 || ret>=256)
		return NULL;
	data[ret]=0;
	tmp=strrchr(data,'/');
	if(!tmp)
		return NULL;
	tmp++;
	l_strdown(tmp);
	return y_im_get_app_config(tmp);
}
#endif

#ifdef _WIN32
LKeyFile *y_im_get_app_config_by_hwnd(HWND w)
{
	if(!app)
		return NULL;
	DWORD pid=0;
	GetWindowThreadProcessId(w,&pid);
	if(!pid)
		return NULL;
	return y_im_get_app_config_by_pid((int)pid);
}
#endif
