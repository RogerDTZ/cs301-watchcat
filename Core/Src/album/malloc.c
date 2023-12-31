#include "malloc.h"	    

u8 membase[MEM_MAX_SIZE]__attribute__ ((aligned (4)));;
u16 memmapbase[MEM_ALLOC_TABLE_SIZE];

const u32 memtblsize=MEM_ALLOC_TABLE_SIZE;
const u32 memblksize=MEM_BLOCK_SIZE;
const u32 memsize=MEM_MAX_SIZE;

struct _m_mallco_dev mallco_dev=
{
	mem_init,
	mem_perused,
	membase,
	memmapbase,
	0,
};

void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  

void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	   

void mem_init(void)  
{  
    mymemset(mallco_dev.memmap, 0,memtblsize*2);
	mymemset(mallco_dev.membase, 0,memsize);
	mallco_dev.memrdy=1;
}  

u8 mem_perused(void)  
{  
    u32 used=0;  
    u32 i;  
    for(i=0;i<memtblsize;i++)  
    {  
        if(mallco_dev.memmap[i])used++; 
    } 
    return (used*100)/(memtblsize);  
}  

u32 mem_malloc(u32 size)  
{  
    signed long offset=0;  
    u16 nmemb;	//��Ҫ���ڴ����  
	u16 cmemb=0;//�������ڴ����
    u32 i;  
    if(!mallco_dev.memrdy)mallco_dev.init();	//δ��ʼ��,��ִ�г�ʼ�� 
    if(size==0)return 0XFFFFFFFF;				//����Ҫ����
    nmemb=size/memblksize;  					//��ȡ��Ҫ����������ڴ����
    if(size%memblksize)nmemb++;  
    for(offset=memtblsize-1;offset>=0;offset--)	//���������ڴ������  
    {     
		if(!mallco_dev.memmap[offset])cmemb++;	//�������ڴ��������
		else cmemb=0;							//�����ڴ������
		if(cmemb==nmemb)						//�ҵ�������nmemb�����ڴ��
		{
            for(i=0;i<nmemb;i++)  				//��ע�ڴ��ǿ� 
            {  
                mallco_dev.memmap[offset+i]=nmemb;  
            }  
            return (offset*memblksize);			//����ƫ�Ƶ�ַ  
		}
    }  
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��  
}  

u8 mem_free(u32 offset)  
{  
    int i;  
    if(!mallco_dev.memrdy)//δ��ʼ��,��ִ�г�ʼ��
	{
		mallco_dev.init();    
        return 1;//δ��ʼ��  
    }  
    if(offset<memsize)//ƫ�����ڴ����. 
    {  
        int index=offset/memblksize;		//ƫ�������ڴ�����  
        int nmemb=mallco_dev.memmap[index];	//�ڴ������
        for(i=0;i<nmemb;i++)  				//�ڴ������
        {  
            mallco_dev.memmap[index+i]=0;  
        }
        return 0;  
    }else return 2;//ƫ�Ƴ�����.  
}  

void myfree(void *ptr)  
{  
	u32 offset;  
    if(ptr==NULL)return;//��ַΪ0.  
 	offset=(u32)ptr-(u32)mallco_dev.membase;  
    mem_free(offset);	//�ͷ��ڴ�     
}  

void *mymalloc(u32 size)  
{  
    u32 offset;  									      
	offset=mem_malloc(size);  	   				   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)mallco_dev.membase+offset);  
}  

void *myrealloc(void *ptr,u32 size)  
{  
    u32 offset;  
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((u32)mallco_dev.membase+offset),ptr,size);
        myfree(ptr);
        return (void*)((u32)mallco_dev.membase+offset);
    }  
}
