#include<stdio.h>
#include<iostream>
#include<queue>

using namespace std;

struct list
{
	int start_time;
	int arr_time;
	int dist;
};
typedef struct list List;

struct triple
{
	List l;
	struct triple *next;
};
typedef struct triple Triple;

struct tuple
{
	int u;
	int v;
	int t;
	int lambda;
};
typedef struct tuple Tuple;


struct Comp
{
	bool operator()(const Tuple& a,const Tuple& b){
		if(a.t!=b.t)
			return a.t>b.t;
		else
			return a.u>b.u;
	}
};


		
class ShortFast
{
	int edges,count,vertices,src;
	int *counti;
	Tuple *Tu;
	Triple **head;
	priority_queue<Tuple,vector<Tuple>,Comp> q;
	int *journey_time;
	int *travel_time;	
	

	
	Triple* createNode(List);
	void InsertEnd(List s,int a);
	bool CheckDominance(List,int);
	void RemoveDominated(List,int);
	void RemoveNode(List,int);
	public:
	ShortFast(int,int,int);
	void AddTuple(Tuple);
	void process();
	void print();
	
	
};
ShortFast::ShortFast(int a,int b,int c)
{
	edges=a;
	int i;
	counti=new int[b];
	vertices=b;
	src=c;
	count=0;
	journey_time=new int[vertices];
	travel_time=new int[vertices];
	Tu=new Tuple[edges];
	head=new Triple*[vertices];	
	
	for(i=0;i<vertices;i++)
	{
		journey_time[i]=travel_time[i]=99999;
	}
}

Triple* ShortFast::createNode(List x)
{
Triple *temp;

	temp = new Triple;
	temp->l = x;
	temp->next = NULL;

return temp;
}

void ShortFast::InsertEnd(List s,int a)
{
	Triple *p=head[a];
	while(p->next != NULL)
	{
		p = p->next;
	}
	p->next = createNode(s);
}

void ShortFast::AddTuple(Tuple a)
{
	Tu[count]=a;
	q.push(a);
	count=count+1;
}

bool ShortFast::CheckDominance(List c,int b)
{
	int type=0;
	bool flag=true;
	Triple *p;
	List a;
	p=head[b];
	p=p->next;
	while(p!=NULL)
	{
		a=p->l;
		//s2==s1 and a1<=a2 if  d1<=d2 then keep first one 
		if(c.start_time==a.start_time)
		{

			if(a.arr_time<=c.arr_time && a.dist<=c.dist)
			{
				flag=false;
				break;
			}
			
		}
		//same ending time 
		else if(c.arr_time==a.arr_time)
		{
			if(c.start_time<a.start_time)
			{
				flag=false;
				break;
			}
		}
		//a triple in the  list lies inside the incoming triple			
		else if(c.start_time<a.start_time && c.arr_time>a.start_time)
		{
			flag=false;
			break;
		}
		p=p->next;		
	}
	return flag;					
}
void ShortFast::RemoveDominated(List c,int index)
{
	int type=0;
	Triple *p;
	List a;
	p=head[index];
	p=p->next;
	while(p!=NULL)
	{
		a=p->l;
		if(a.start_time==c.start_time)
		{
			if(c.arr_time<=a.arr_time && c.dist<=a.dist)
				RemoveNode(a,index);
			
		}
		if(c.arr_time==a.arr_time)
		{
			if(a.start_time<c.start_time)
				RemoveNode(a,index);		
		}			
		if(a.start_time<c.start_time && a.arr_time>c.arr_time)
		{
			RemoveNode(a,index);
		}
		p=p->next;		
	}					
}
void ShortFast::RemoveNode(List z,int index)
{
	Triple *u;Triple *x;
	
	u=head[index];
	while(u->next!=NULL)
	{
		x=u->next;
		if(x->l.start_time==z.start_time && x->l.arr_time==z.arr_time && x->l.dist==z.dist)
		{
			if(x->next!=NULL)
				u->next=x->next;
			else
				u->next=NULL;
			delete(x);
			break;
		}
		u=u->next;
	}
}

void ShortFast::process()
{
	Tuple top;
	Triple *access;
	int src_time,diff,max=-1;
	List selected,now,parent;
	bool status,flag=false;
	
	for(int i=0;i<vertices;i++)
	{
		now.start_time=now.arr_time=now.dist=-1;
		head[i]=new Triple;
		head[i]->l=now;
		head[i]->next=NULL;					
	}
	journey_time[src]=0;
	travel_time[src]=0;
	while(!q.empty())
	{
		top=q.top();
		q.pop();
		max=-1;
		//inserting triples for source	
		if(top.u==src)
		{
			now.start_time=top.t;
			now.arr_time=top.t;
			now.dist=0;
			InsertEnd(now,top.u);
			access=head[top.u];
			access=access->next;
			//inserting triples for the another vertex v such that (src,v,t,lambda) is the choosen tuple
			//choosing start_time of source like to choose 2 or 3 when we have both (2,2,0) and (3,3,0) in source's list
			while(access!=NULL)
			{
				if(access->l.arr_time==top.t)
				{
					parent=access->l;
					break;
				}
				access=access->next;
			}
			if(parent.arr_time<=top.t)
			{
				now.start_time=parent.start_time;
				now.arr_time=top.t+top.lambda;
				now.dist=parent.dist+top.lambda;
				InsertEnd(now,top.v);
				journey_time[top.v]=now.arr_time-now.start_time;
				travel_time[top.v]=now.dist;				
			}			
		}
		else
		{
			//selecting triple with maximum arrival time in L(u)
			access=head[top.u];
			access=access->next;
			while(access!=NULL)
			{
				if(access->l.arr_time<=top.t)
				{
					if(access->l.arr_time>max)
					{
						flag=true;
						max=access->l.arr_time;
						selected=access->l;	
					}
				}
				access=access->next;
			}
			//If there is atleast one valid triple .....do the remaining operations on it 
			if(flag==true)
			{
				selected.arr_time=top.t+top.lambda;
				selected.dist=selected.dist+top.lambda;
				status=CheckDominance(selected,top.v);
				//If the new triple is not dominated by any of the existing triples of L(v)
				if(status==true)
				{
					RemoveDominated(selected,top.v);
					InsertEnd(selected,top.v);
					diff=selected.arr_time-selected.start_time;
					if(diff<journey_time[top.v])
					{
						journey_time[top.v]=diff;
						travel_time[top.v]=selected.dist;
					}
					//If two paths are equally fast,then select the shorter one
					else if(diff==journey_time[top.v])
					{
						if(selected.dist<travel_time[top.v])
						{
							travel_time[top.v]=selected.dist;
						}
					}
				}
			}
		}
			
	}
}
void ShortFast::print()
{
	for(int i=0;i<vertices;i++)
	{
		cout<<"Node "<<i<<" journey_time ....!!!  "<<"travel_time .....!!!   "<<journey_time[i]<<"  "<<travel_time[i]<<endl;
	}
	cout<<endl;
	cout<<"--------------------------------------****************----------------------------------"<<endl;
}
	
int main()
{
	clock_t t1,t2;
	int i=0,edg,dstn,u,v,t,y,src,n;
	Tuple T;
	//	cout<<"Enter the number vertices and  edges:"<<endl;
	FILE *fp;
	fp=fopen("germanData.txt","r");
	fscanf(fp,"%d",&n);
	fscanf(fp,"%d",&edg);
	//	cout<<"Enter number of vertices:"<<endl;

	//	cout<<"Enter source vertex:"<<endl;
	src=0;
	ShortFast S(edg,n,src);
	for(i=0;i<edg;i++)
	{
		fscanf(fp,"%d %d %d %d",&T.u,&T.v,&T.t,&T.lambda);		
		//	cin>>T.u>>T.v>>T.t>>T.lambda;
		S.AddTuple(T);		

	}
	printf("%d\n",i);
	fclose(fp);
	t1=clock();

	S.process();

	t2=clock();

	float diff ((float)t2-(float)t1);
	cout<<".."<<float(t1)<<".."<<float(t2)<<".."<<float(diff)/CLOCKS_PER_SEC<<endl;
	S.print();
}
	
			
			
		
