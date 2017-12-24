#include<stdio.h>
#include<iostream>
#include<queue>
#include<stack>
#include<time.h>

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
		if(a.t+a.lambda!=b.t+b.lambda)
			return a.t+a.lambda>b.t+b.lambda;
		else
			return a.u>b.u;
	}
};


		
class ShortFast
{
	int edges,count,vertices,src;
	//int *counti;

	Tuple *Tu;
	priority_queue<Tuple,vector<Tuple>,Comp> q;
	int *journey_time;
	int *travel_time;
	stack<List> backup_stack;	

	bool CheckDominance(List,List);
	void Updation(List,int);
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
	//counti=new int[b];
	vertices=b;
	src=c;
	count=0;
	journey_time=new int[vertices];
	travel_time=new int[vertices];
	Tu=new Tuple[edges];
	
	for(i=0;i<vertices;i++)
	{
		journey_time[i]=travel_time[i]=9999999;
	}
}
void ShortFast::AddTuple(Tuple a)
{
	Tu[count]=a;
	q.push(a);
	count=count+1;
}
// To check whether first tuple dominates the second one or not
bool ShortFast::CheckDominance(List s,List c)
{
	bool status=false;
		//s2==s1 and a1<=a2 if  d1<=d2 then keep first one 
		if(c.start_time==s.start_time)
		{

			if(s.arr_time<=c.arr_time && s.dist<=c.dist)
				status=true;
		}
		//same ending time 
		else if(c.arr_time==s.arr_time)
		{
			if(c.start_time<s.start_time)
				status=true;
		}
		//a triple in the  list lies inside the incoming triple			
		else if(c.start_time<s.start_time && c.arr_time>s.start_time)
			status=true;	
	return status;					
}					


void ShortFast::Updation(List selected,int v)
{
	int diff;
	diff=selected.arr_time-selected.start_time;
				if(diff<journey_time[v])
				{
					journey_time[v]=diff;
					travel_time[v]=selected.dist;
				}
				//If two paths are equally fast,then select the shorter one
				else if(diff==journey_time[v])
				{
					if(selected.dist<travel_time[v])
					{
						travel_time[v]=selected.dist;
					}
				}

}

void ShortFast::process()
{
	Tuple top;
	stack<List> S[vertices];
	int src_time,diff,max=-1;
	List selected,now,temp;
	bool status1,status2,flag,flag1;
	int type;

	journey_time[src]=0;
	travel_time[src]=0;
	while(!q.empty())
	{
		//cout<<",,,,,,"<<endl;
		flag1=true;
		flag=false;
		top=q.top();
		q.pop();
		//cout<<top.u<<".."<<top.v<<".."<<top.t<<".."<<top.lambda<<endl;
		//inserting triples for source	
		if(top.u==src)
		{
			//cout<<"src...!!  "<<endl;
			now.start_time=top.t;
			now.arr_time=top.t;
			now.dist=0;
			//S[src].push(now);
			
			if(now.arr_time<=top.t)
			{
				//cout<<"here.."<<endl;
				now.start_time=now.start_time;
				now.arr_time=top.t+top.lambda;
				now.dist=now.dist+top.lambda;
				S[top.v].push(now);
				//cout<<"pushed.."<<top.v<<endl;
				journey_time[top.v]=now.arr_time-now.start_time;
				travel_time[top.v]=now.dist;				
			}			
		}
		else
		{
			//cout<<"else.."<<endl;
			//selecting triple with maximum arrival time in L(u)
			while(flag1)
			{
				//cout<<"in_while.."<<endl;
				if(S[top.u].size()>0)
					temp=S[top.u].top();
				else
					break;
				//cout<<temp.start_time<<".."<<temp.arr_time<<".."<<temp.dist<<endl;
				
				if(temp.arr_time<=top.t)
				{
					selected=temp;
					flag=true;
					flag1=false;					
				}
				else
				{
					backup_stack.push(temp);
					if(S[top.u].size()>0)
					{
						S[top.u].pop();
						//cout<<"back_up"<<S[top.u].size()<<endl;
					}
					else
						break;
				}			
			}
			while(!backup_stack.empty())
			{
				temp=backup_stack.top();
				backup_stack.pop();
				S[top.u].push(temp);
				//cout<<"reverse"<<endl;

			}			

			//If there is atleast one valid triple .....do the remaining operations on it 
			if(flag==true)
			{
				//cout<<"inside if...!! "<<endl;
				selected.arr_time=top.t+top.lambda;
				selected.dist=selected.dist+top.lambda;
				//cout<<S[top.v].top().start_time<<".."<<S[top.v].top().arr_time<<".."<<S[top.v].top().dist<<endl;
				//cout<<selected.start_time<<".."<<selected.arr_time<<".."<<selected.dist<<endl;
				if(!S[top.v].empty())
				{
					status1=CheckDominance(S[top.v].top(),selected);
					status2=CheckDominance(selected,S[top.v].top());
					//If the new triple is not dominated by any of the existing triples of L(v)
					//cout<<"status..!! "<<status1<<endl;
					if(status2==true)
					{
						S[top.v].pop();
						S[top.v].push(selected);
						Updation(selected,top.v);
					}
					if(status2==false && status1==false)
					{
						S[top.v].push(selected);
						Updation(selected,top.v);			
					}	
				}
				else
				{
					
					S[top.v].push(selected);
					Updation(selected,top.v);
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
	//printf("%d\n",i);
	fclose(fp);
	t1=clock();

	S.process();

	t2=clock();

	float diff ((float)t2-(float)t1);
	cout<<".."<<float(t1)<<".."<<float(t2)<<".."<<float(diff)/CLOCKS_PER_SEC<<endl;
	S.print();
}
