#include "llib.h"
#include "xim.h"
#include "ybus.h"
#include "common.h"
#include "lcall.h"

static int xim_getpid(CONN_ID conn_id);
static int xim_config(CONN_ID conn_id,CLIENT_ID client_id,const char *config,...);
static void xim_open_im(CONN_ID conn_id,CLIENT_ID client_id);
static void xim_close_im(CONN_ID conn_id,CLIENT_ID client_id);
static void xim_preedit_clear(CONN_ID conn_id,CLIENT_ID client_id);
static int xim_preedit_draw(CONN_ID conn_id,CLIENT_ID client_id,const char *s);
static void xim_send_string(CONN_ID conn_id,CLIENT_ID client_id,const char *s,int flags);
static void xim_send_key(CONN_ID conn_id,CLIENT_ID client_id,int key,int repeat);
static int xim_init(void);

static void wm_state(CONN_ID conn_id,int state);
static void wm_make_above(CONN_ID conn_id,const char *title);
static void wm_move(CONN_ID conn_id,const char *title,int x,int y,int rel);
static void wm_icon(CONN_ID conn_id,const char *icon1,const char *icon2);

static YBUS_PLUGIN plugin={
	.name="lcall",
	.init=xim_init,
	.getpid=xim_getpid,
	.config=xim_config,
	.open_im=xim_open_im,
	.close_im=xim_close_im,
	.preedit_clear=xim_preedit_clear,
	.preedit_draw=xim_preedit_draw,
	.send_string=xim_send_string,
	.send_key=xim_send_key,

	.wm_state=wm_state,
	.wm_make_above=wm_make_above,
	.wm_move=wm_move,
	.wm_icon=wm_icon,
};

typedef struct{
	int pid;
}YBUS_CLIENT_PRIV;

typedef struct{
	LCallConn *conn;
	guint client;
	int key;
	int status;
	int sync;
	guint32 time;
	uint16_t seq;
	uint16_t ack;
}SERV_KEY;

static LCallServ *serv;
static int trigger=CTRL_SPACE;

static void serv_init(LCallConn *conn)
{
	// printf("add %p\n",conn);
	ybus_add_connect(&plugin,(uintptr_t)conn);
	if(trigger!=CTRL_SPACE && trigger)
	{
		l_call_conn_call(conn,"trigger",0,"i",trigger);
	}		
}
static void serv_free(LCallConn *conn)
{
	YBUS_CONNECT *yconn;
	yconn=ybus_find_connect(&plugin,(uintptr_t)conn);
	if(!yconn) return;
	// printf("del %p %d\n",conn,yconn->pid);
	ybus_free_connect(yconn);
	
}

static void forward_key(SERV_KEY *kev,int handled)
{
	if(kev->ack)
	{
		// fprintf(stderr,"forward key double acked %x\n",kev->key);
		return;
	}
	kev->ack=kev->seq;
	if(handled)
	{
		if(kev->sync!=0)
			l_call_conn_return(kev->conn,kev->seq,1);
	}
	else
	{
		if(kev->sync!=0)
			l_call_conn_return(kev->conn,kev->seq,0);
		else
			l_call_conn_call(kev->conn,"forward",NULL,"ii",kev->client,kev->key);
	}
}

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
static void set_cursor_location_default(YBUS_CONNECT *yconn,YBUS_CLIENT *client)
{
	GdkDisplay *dpy;
	Display *xdpy;
	Window root;
	Window window;
	Window child;
	XWindowAttributes xwa;
	
	if(client->track)
		return;
	
	dpy=gdk_display_get_default();
#if GTK_CHECK_VERSION(3,0,0)
	if(!GDK_IS_X11_DISPLAY(dpy))
		return;
#endif
	xdpy=GDK_DISPLAY_XDISPLAY(dpy);
	root=DefaultRootWindow(xdpy);
	Atom a= XInternAtom (xdpy, "_NET_ACTIVE_WINDOW", True);
	Atom type;
	int format;
	unsigned long items,bytes;
	unsigned char *prop=NULL;
	if(Success!=XGetWindowProperty(xdpy,root,a,0,4,False,AnyPropertyType,&type,&format,&items,&bytes,&prop))
	{
		return;
	}
	if(format!=32)
	{
		XFree(prop);
		return;
	}
	memcpy(&window,prop,4);
	XFree(prop);
	if(window==0 || window==(Window)-1)
		return;
	
	XGetWindowAttributes (xdpy, window, &xwa);
	XTranslateCoordinates (xdpy, window,
			xwa.root,
			0,
			xwa.height,
			&client->x,
			&client->y,
			&child);

	YongMoveInput(client->x,client->y);
}

static int serv_input(YBUS_CONNECT *yconn,YBUS_CLIENT *client,SERV_KEY *kev)
{
	int Key=kev->key;
	
	static int last_press;
	static guint32 last_press_time;
	static int bing;
	
	Key&=~KEYM_UP;
	
	if(kev->status==0)
	{
		set_cursor_location_default(yconn,client);

		if(im.Bing && ((Key>='a' && Key<='z') || Key==' '))
		{
			int diff=y_im_diff_hand(bing,Key);
			if(!bing)
			{
				if(Key!=' ')
					bing=Key;
			}
			else if(kev->time-last_press_time>=im.BingSkip[diff])
			{
				if(Key==' ') Key=bing;
				Key|=KEYM_BING;
				//printf("%d\n",kev->time-last_press_time);
			}
		}
		last_press=Key;
		last_press_time=kev->time;
	}
	if(YK_CODE(Key)>=YK_LSHIFT && YK_CODE(Key)<=YK_RWIN)
	{
		bing=0;
		forward_key(kev,0);
		if(kev->status==0)
		{
			return 0;
		}
		if(Key!=last_press || kev->time-last_press_time>300)
		{
			return 0;
		}
	}
	if(kev->status==1)
	{
		if(Key==last_press)
			last_press=0;
		bing=0;
	}
	
	switch(Key){
	case CTRL_SPACE:
	case SHIFT_SPACE:
	case KEYM_SUPER|YK_SPACE:
	{
		if(kev->status==0 && ybus_on_key(&plugin,yconn->id,client->id,Key))
		{
			forward_key(kev,1);
			return 0;
		}
		break;
	}
	case CTRL_LSHIFT:
	case CTRL_RSHIFT:
	case CTRL_LALT:
	case CTRL_RALT:
	{
		ybus_on_key(&plugin,yconn->id,client->id,Key);
		return 0;
	}
	case YK_LCTRL:
	case YK_RCTRL:
	case YK_LSHIFT:
	case YK_RSHIFT:
	case KEYM_CTRL|YK_LWIN:
	case KEYM_CTRL|YK_RWIN:
	{
		ybus_on_key(&plugin,yconn->id,client->id,Key);
		return 0;
	}
	default:
	{
		break;
	}}
	
	if(im.layout && !im.Bing && yconn->state)
	{
		int tmp;
		tmp=Key&~KEYM_KEYPAD;
		if(!(tmp&KEYM_MASK))
		{
			tmp=YK_CODE(tmp);
			if(kev->status==1)
			{
				tmp=y_layout_keyup(im.layout,tmp,kev->time);
			}
			else
			{
				tmp=y_layout_keydown(im.layout,tmp,kev->time);
			}
			if(tmp>0)
			{
				char *p=(char*)&tmp;
				int i;
				if(yconn->lang==LANG_CN)
				{
					forward_key(kev,1);
					for(i=0;i<=3 && p[i];i++)
					{
						int ret=ybus_on_key(&plugin,yconn->id,client->id,p[i]);
						if(ret)
						{
							y_im_speed_update(p[i],0);
						}
						else
						{
							xim_send_key(yconn->id,client->id,p[i],1);
						}
					}
					return 0;
				}
				else
				{
					l_call_conn_return((LCallConn*)yconn->id,kev->seq,1);
					for(i=0;i<=3 && p[i];i++)
					{
						xim_send_key(yconn->id,client->id,p[i],1);
					}
					return 0;
				}
			}
			else if(tmp==0)
			{
				//l_call_conn_return((LCallConn*)yconn->id,kev->seq,1);
				forward_key(kev,1);
				return 0;
			}
		}
	}
	if(yconn->state && kev->status==0)
	{
		int ret;
		//clock_t start=clock();
		ret=ybus_on_key(&plugin,yconn->id,client->id,Key);
		//printf("key %.3f\n",(clock()-start)*1.0/CLOCKS_PER_SEC);
		if(ret)
		{
			forward_key(kev,1);
			return 0;
		}
	}
	
	forward_key(kev,0);
	
	return 0;
}

static void init_client_priv(LCallConn *conn,YBUS_CLIENT *client)
{
#if !defined(_GNU_SOURCE)
	return;
#else
	if(!client)
		return;
	YBUS_CLIENT_PRIV *priv=(YBUS_CLIENT_PRIV*)client->priv;
	if(priv->pid)
		return;
	priv->pid=l_call_conn_peer_pid(conn);
	if(priv->pid<=0)
		return;
	LKeyFile *app=y_im_get_app_config_by_pid(priv->pid);
	if(!app)
	{
		return;
	}
	const char *t=l_key_file_get_data(app,"IM","enable");
	if(t && atoi(t)==0)
	{
		client->state=0;
	}
	t=l_key_file_get_data(app,"IM","lang");
	if(t)
	{
		// TODO:
	}
#endif
}

static int serv_dispatch(LCallConn *conn,const char *name,LCallBuf *buf)
{
	YBUS_CONNECT *yconn;
	int ret;

	if(!strcmp(name,"tool"))
	{
		int type,param;
		
		ret=l_call_buf_get_val(buf,type);
		ret|=l_call_buf_get_val(buf,param);
		if(ret!=0) return -1;
		ret=ybus_on_tool(&plugin,(uintptr_t)conn,type,param);
		if((buf->flag&L_CALL_FLAG_SYNC)!=0)
		{
			l_call_conn_return(conn,buf->seq,ret);
		}
		return 0;
	}
	
	yconn=ybus_find_connect(&plugin,(uintptr_t)conn);
	if(!yconn) return 0;

	if(!strcmp(name,"cursor"))
	{
		YBUS_CLIENT *client;
		guint client_id;
		int x,y,w,h,rel=0;
		ret=l_call_buf_get_val(buf,client_id);
		if(ret!=0) return -1;
		client=ybus_add_client(yconn,client_id,sizeof(YBUS_CLIENT_PRIV));
		if(!client) return -1;
		ret=l_call_buf_get_val(buf,x);
		ret|=l_call_buf_get_val(buf,y);
		ret|=l_call_buf_get_val(buf,w);
		ret|=l_call_buf_get_val(buf,h);
		if(ret!=0) return -1;
		l_call_buf_get_val(buf,rel);
		// fprintf(stderr,"cursor %d %d %d %d %d\n",x,y,w,h,rel);
#if 0
		client->track=1;
		client->x=x+w;
		client->y=y+h;
		YBUS_CLIENT *active=NULL;
		ybus_get_active(NULL,&active);
		//printf("\t%d %d\n",client->x,client->y);
		if(active==client)
			YongMoveInput(client->x,client->y);
#else
		ybus_on_cursor(&plugin,(CONN_ID)conn,client_id,x,y+h,rel);
#endif
	}
	else if(!strcmp(name,"input"))
	{
		guint client_id;
		SERV_KEY key;
		YBUS_CLIENT *client,*active=0;
		ret=l_call_buf_get_val(buf,client_id);
		if(ret!=0) return -1;
		client=ybus_add_client(yconn,client_id,sizeof(YBUS_CLIENT_PRIV));
		if(!client) return -1;
		ybus_get_active(0,&active);
		if(active!=client)
		{
			l_call_conn_return(conn,buf->seq,0);
			return 0;
		}
		ret|=l_call_buf_get_val(buf,key.key);
		ret|=l_call_buf_get_val(buf,key.time);
		if(ret!=0) return -1;
		key.status=(key.key&KEYM_UP)?1:0;
		key.conn=conn;
		key.client=client_id;
		key.seq=buf->seq;
		key.sync=buf->flag&L_CALL_FLAG_SYNC;
		key.ack=0;
		// printf("\tstatus:%d key:%x seq:%d\n",key.status,key.key,buf->seq);
		ret=serv_input(yconn,client,&key);
#if 1
		if(key.ack!=key.seq)
		{
			fprintf(stderr,"\t%d %x not dealed\n",key.status,key.key);
		}
#endif
		return ret;
	}
	else if(!strcmp(name,"focus_in"))
	{
		guint client_id;
		ret=l_call_buf_get_val(buf,client_id);
		if(ret!=0) return -1;
		//printf("\t%u\n",client_id);
		YBUS_CLIENT *client=ybus_add_client(yconn,client_id,sizeof(YBUS_CLIENT_PRIV));
		init_client_priv(conn,client);
		ybus_on_focus_in(&plugin,yconn->id,client_id);
	}
	else if(!strcmp(name,"focus_out"))
	{
		guint client_id;
		ret=l_call_buf_get_val(buf,client_id);
		if(ret!=0) return -1;
		//printf("\t%u\n",client_id);
		ybus_on_focus_out(&plugin,yconn->id,client_id);
	}
	else if(!strcmp(name,"enable"))
	{
		guint client_id;
		ret=l_call_buf_get_val(buf,client_id);
		if(ret!=0) return -1;
		//printf("\t%u\n",client_id);
		YBUS_CLIENT *client=ybus_add_client(yconn,client_id,sizeof(YBUS_CLIENT_PRIV));
		init_client_priv(conn,client);
		ybus_on_open(&plugin,yconn->id,client_id);
	}
	else if(!strcmp(name,"add_ic"))
	{
		guint client_id;
		ret=l_call_buf_get_val(buf,client_id);
		if(ret!=0) return -1;
		YBUS_CLIENT *client=ybus_add_client(yconn,client_id,sizeof(YBUS_CLIENT_PRIV));
		init_client_priv(conn,client);
	}
	else if(!strcmp(name,"del_ic"))
	{
		YBUS_CLIENT *client;
		guint client_id;
		ret=l_call_buf_get_val(buf,client_id);
		if(ret!=0) return -1;
		client=ybus_find_client(yconn,client_id);
		if(!client) return -1;
		ybus_free_client(yconn,client);
	}
	return 0;
}
static LCallUser serv_user={
	serv_init,serv_free,serv_dispatch
};

static int xim_init(void)
{
	serv=l_call_serv_new(NULL,&serv_user);
	return 0;
}

int ybus_lcall_init(void)
{
	ybus_add_plugin(&plugin);
	return 0;
}

static int xim_getpid(CONN_ID conn_id)
{
#ifdef _GNU_SOURCE
	return l_call_conn_peer_pid((LCallConn*)conn_id);
#else
	return 0;
#endif
}

static int xim_config(CONN_ID conn_id,CLIENT_ID client_id,const char *config,...)
{
	va_list ap;
	int ret=0;
	va_start(ap,config);
	if(!strcmp(config,"trigger"))
	{
		int key=va_arg(ap,int);		
		trigger=key;
	}
	else
	{
		ret=-1;
	}
	va_end(ap);
	return ret;
}

static void xim_open_im(CONN_ID conn_id,CLIENT_ID client_id)
{
	l_call_conn_call((LCallConn*)conn_id,"enable",0,"i",(int)client_id);
	ybus_on_open(&plugin,conn_id,client_id);
}

static void xim_close_im(CONN_ID conn_id,CLIENT_ID client_id)
{
	l_call_conn_call((LCallConn*)conn_id,"disable",0,"i",(int)client_id);
	ybus_on_close(&plugin,conn_id,client_id);
}

static void xim_preedit_clear(CONN_ID conn_id,CLIENT_ID client_id)
{
	l_call_conn_call((LCallConn*)conn_id,"preedit_clear",0,"i",(int)client_id);
}

static int xim_preedit_draw(CONN_ID conn_id,CLIENT_ID client_id,const char *s)
{
	l_call_conn_call((LCallConn*)conn_id,"preedit",0,"is",(int)client_id,s);
	return 0;
}

static void xim_send_string(CONN_ID conn_id,CLIENT_ID client_id,const char *s,int flags)
{
	char out[512];
	y_im_str_encode(s,out,flags);
	l_call_conn_call((LCallConn*)conn_id,"commit",0,"is",(int)client_id,out);
}

static void xim_send_key(CONN_ID conn_id,CLIENT_ID client_id,int key,int repeat)
{
	l_call_conn_call((LCallConn*)conn_id,"forward",0,"iii",(int)client_id,key,repeat);
}

static void wm_state(CONN_ID conn_id,int state)
{
	l_call_conn_call((LCallConn*)conn_id,"wm_state",NULL,"i",state);
}

static void wm_make_above(CONN_ID conn_id,const char *title)
{
	l_call_conn_call((LCallConn*)conn_id,"wm_make_above",NULL,"s",title);
}

static void wm_move(CONN_ID conn_id,const char *title,int x,int y,int rel)
{
	l_call_conn_call((LCallConn*)conn_id,"wm_make_above",NULL,"siii",title,x,y,rel);
}

static void wm_icon(CONN_ID conn_id,const char *icon1,const char *icon2)
{
	if(!icon2 || !icon2[0])
		l_call_conn_call((LCallConn*)conn_id,"wm_icon",NULL,"s",icon1);
	else
		l_call_conn_call((LCallConn*)conn_id,"wm_icon",NULL,"ss",icon1,icon2);
}

