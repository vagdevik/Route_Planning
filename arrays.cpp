#include<stdio.h>
#include<iostream>
#include<queue>
#include<omp.h>
#include<time.h>

using namespace std;

struct list
{
	int start_time;
	int arr_time;
	int dist;
};
typedef struct list List;


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
	List **L;
	int *curr_count;

	bool CheckDominance(List,List);
	public:
	ShortFast(int,int,int);
	void AddTuple(Tuple);
	void process(int);
	void Updation(List,int);
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
	curr_count=new int[vertices];
	for(i=0;i<vertices;i++)
	{
		curr_count[i]=0;
	}
	
	L=new List*[vertices];
	for(i=0;i<vertices;i++)
	{
		L[i]=new List[vertices];
	}
	journey_time=new int[vertices];
	travel_time=new int[vertices];
	Tu=new Tuple[edges];
	
	
	for(i=0;i<vertices;i++)
	{
		journey_time[i]=travel_time[i]=99999;
	}
}




void ShortFast::AddTuple(Tuple a)
{
	Tu[count]=a;
	q.push(a);
	count=count+1;
}

bool ShortFast::CheckDominance(List a,List c)
{
	bool flag=false;

	//s2==s1 and a1<=a2 if  d1<=d2 then keep first one 
	if(c.start_time==a.start_time)
	{

		if(a.arr_time<=c.arr_time && a.dist<=c.dist)
		{
			flag=true;
		}
		
	}
	//same ending time 
	else if(c.arr_time==a.arr_time)
	{
		if(c.start_time<a.start_time)
		{
			flag=true;
		}
	}
	//a triple in the  list lies inside the incoming triple			
	else if(c.start_time<a.start_time && c.arr_time>a.start_time)
	{
		flag=true;
	}
		
	
	return flag;					
}

void ShortFast::Updation(List selected,int v)
{
	//cout<<"selected.."<<selected.start_time<<" "<<selected.arr_time<<" "<<selected.dist<<endl;
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
					//cout<<"new.."<<selected.dist<<"previous.."<<travel_time[v]<<endl;
					if(selected.dist<travel_time[v])
					{
						travel_time[v]=selected.dist;
					}
				}

}

void ShortFast::process(int src)
{
	Tuple top;
	int src_time,diff,i;
	List selected,now,parent;
	bool status1,status2,flag;
	
	
	journey_time[src]=0;
	travel_time[src]=0;
	while(!q.empty())
	{
		top=q.top();
		q.pop();
		flag=false;
		//inserting triples for source	
		if(top.u==src)
		{
			now.start_time=top.t;
			now.arr_time=top.t;
			now.dist=0;
			L[top.u][curr_count[top.u]]=now;
			curr_count[top.u]++;
			
			//inserting triples for the another vertex v such that (src,v,t,lambda) is the choosen tuple
			//choosing start_time of source like to choose 2 or 3 when we have both (2,2,0) and (3,3,0) in source's list
			
			for(i=0;i<curr_count[top.u];i++)
			{
				if(L[top.u][i].arr_time==top.t)
					parent=L[top.u][i];

			}
			if(parent.arr_time<=top.t)
			{
				now.start_time=parent.start_time;
				now.arr_time=top.t+top.lambda;
				now.dist=parent.dist+top.lambda;
				L[top.v][curr_count[top.v]]=now;
				curr_count[top.v]++;
				journey_time[top.v]=now.arr_time-now.start_time;
				travel_time[top.v]=now.dist;				
			}			
		}
		else
		{
			//selecting triple with maximum arrival time in L(u)
			if(curr_count[top.u]>=1)
			{
				for(i=curr_count[top.u]-1;i>=0;i--)
				{
					if(L[top.u][i].arr_time<=top.t)
					{
						flag=true;
						selected=L[top.u][i];
						break;
					}
				}
			}
			
			//If there is atleast one valid triple .....do the remaining operations on it 
			if(flag==true)
			{
				selected.arr_time=top.t+top.lambda;
				selected.dist=selected.dist+top.lambda;
				//If the new triple is not dominated by any of the existing triples of L(v)
				if(curr_count[top.v]!=0)
				{
					status1=CheckDominance(L[top.v][curr_count[top.v]-1],selected);
					status2=CheckDominance(selected,L[top.v][curr_count[top.v]-1]);
					//If the new triple is not dominated by any of the existing triples of L(v)
					//cout<<"status1..!! "<<status1<<"status2..!!"<<status2<<endl;
					if(status2==true)
					{
						L[top.v][curr_count[top.v]-1].start_time=selected.start_time;
						L[top.v][curr_count[top.v]-1].arr_time=selected.arr_time;
						L[top.v][curr_count[top.v]-1].dist=selected.dist;
						Updation(selected,top.v);
					}
					if(status2==false && status1==false)
					{
						L[top.v][curr_count[top.v]].start_time=selected.start_time;
						L[top.v][curr_count[top.v]].arr_time=selected.arr_time;
						L[top.v][curr_count[top.v]].dist=selected.dist;
						curr_count[top.v]++;
						Updation(selected,top.v);			
					}	
			}
			else
			{
					
					L[top.v][curr_count[top.v]].start_time=selected.start_time;
					L[top.v][curr_count[top.v]].arr_time=selected.arr_time;
					L[top.v][curr_count[top.v]].dist=selected.dist;
					curr_count[top.v]++;
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
	cout<<"---------------------------------------****************-----------------------------------"<<endl;
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

	S.process(src);

	t2=clock();

	float diff ((float)t2-(float)t1);
	cout<<"--"<<float(t1)<<"--"<<float(t2)<<"--"<<float(diff)/CLOCKS_PER_SEC<<endl;
	//S.print();
}
	
			
			
		
