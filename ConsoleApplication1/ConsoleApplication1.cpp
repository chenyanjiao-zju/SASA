// ConsoleApplication4.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <windows.h>
#include <time.h>
using namespace std;
//定义一些数字符号
#define SNum 15//卖家数量
#define BNum (SNum*10)//买家数量
typedef __int64 int64_t;
typedef long long int64t;
int OrderedSpec[SNum];//排好序的频谱
int InitGraph[SNum][BNum][BNum];//为1则表示干涉，为0则表示不干涉,第一维表示对频谱1的干涉图
struct Seller
{
	int ID;
	int spectrum;
	int value;
};
struct Buyer
{
	int ID;
	int bids[SNum];
	bool deleted;//是否已经从干涉图中删除
};
void Register()
{

	//对频谱进行排序

}
void Post()
{
	//
}

int64_t GetSysTimeMicros()
{
#ifdef _WIN32
	// 从1601年1月1日0:0:0:000到1970年1月1日0:0:0:000的时间(单位100ns)
#define EPOCHFILETIME   (116444736000000000UL)
	FILETIME ft;
	LARGE_INTEGER li;
	int64_t tt = 0;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	// 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
	tt = (li.QuadPart - EPOCHFILETIME) / 10;
	return tt;
#else
	timeval tv;
	gettimeofday(&tv, 0);
	return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif // _WIN32
	return 0;
}
void InitInterfernce()//生成原始干涉图
{
	for (int i = 0; i < SNum; i++)
	{

		if (i == 0)//序号为1的干涉图随机生成
		{
			for (int j = 0; j < BNum; j++)
			{
				for (int k = j; k < BNum; k++)
				{
					if (j == k)
					{
						InitGraph[i][k][j] = 1;
						InitGraph[i][j][k] = 1;
					}
					else
					{

						InitGraph[i][j][k] = rand() % 2;//0-1的随机数
						InitGraph[i][k][j] = InitGraph[i][j][k];
					}

				}
			}
		}
		if (i != 0)//后面的干涉图依次为前一个的子集
		{
			for (int j = 0; j < BNum; j++)
			{
				for (int k = j; k < BNum; k++)
				{
					if (j == k)
					{
						InitGraph[i][k][j] = 1;
						InitGraph[i][j][k] = 1;
					}
					else
					{
						if (InitGraph[i - 1][j][k] == 1)//如果为1，则随机选择为1或者为0，
						{

							InitGraph[i][j][k] = rand() % 2;
							InitGraph[i][k][j] = InitGraph[i][j][k];
						}
						else//如果为0，则继续为0
						{

						}
					}

				}
			}
		}

	}
}
int main()
{

	//以下代码用于记录时间
	clock_t start, finish;

	start = clock();




	//用户记录500次的数据
	float ratioofwinningbuyers[500];
	float soldspecratio[500];
	int socialwelfare[500] = { 0 };
	int sellerrevenue[500] = { 0 };
	float totalwinnernum=0.0;
	float totalsoldspec=0.0;
	srand((int)GetSysTimeMicros());

	for (int i = 0; i < 500; i++)//每种情况执行500次
	{
		InitInterfernce();
		//打印干涉图
		/*cout << "以下为干涉图" << endl;
		for (int i = 0; i < SNum; i++)
		{

		for (int j = 0; j < BNum; j++)
		{
		for (int k = 0; k < BNum; k++)
		{
		cout << InitGraph[i][j][k];
		cout << " ";
		}
		cout << endl;
		}
		cout << "    " << endl;
		}*/
		struct Seller Mysellers[SNum];
		struct Buyer Mybuyers[BNum];

		//生成sellers数据
		for (int m = 0; m< SNum; m++)
		{
			Mysellers[m].ID = m;
			Mysellers[m].spectrum = m;

			Mysellers[m].value = rand() % 100;//估值有待改正
		}
		//打印sellers数据
		/*cout << "以下为seller的数据：" << endl;
		for (int m = 0; m< SNum; m++)
		{
		cout << m;
		cout << " ";
		cout << Mysellers[m].spectrum;
		cout << " ";
		cout << Mysellers[m].value << endl;
		}*/
		//生成Buyers数据
		for (int m = 0; m < BNum; m++)
		{
			Mybuyers[m].ID = m;
			for (int j = 0; j < SNum; j++)
			{

				Mybuyers[m].bids[j] = rand() % 150;//竞标价格，有待改正
			}
			Mybuyers[m].deleted = 0;
		}

		//打印Buyer数据
		/*cout << "以下为buyer的数据：" << endl;
		for (int m = 0; m < BNum; m++)
		{
		cout << "buyer";
		cout << m << endl;
		for (int j = 0; j < SNum; j++)
		{
		cout << Mybuyers[m].bids[j];
		cout << " ";
		}
		cout << endl;
		}
		cout << "----" << endl;*/
		//分组
		vector <Buyer> Gk[SNum];//用于存放当前频谱的gk
		for (int m = 0; m < SNum; m++)//对于每一个频谱，都进行一次分组
		{
			for (int j = 0; j < BNum; j++)//每一个买家
			{
				if ((Mybuyers[j].deleted != 1) && (Mybuyers[j].bids[m] != 0))//如果没有被从图中删除而且竞价不等于0
				{
					bool intervene = 0;//设置一个标志，如果干涉则为1，不干涉则为0

									   //遍历gk内成员，如果和gk内的所有成员都不干涉，则标志设为0

					for (int k = 0; k<Gk[m].size(); k++)
					{

						if (InitGraph[m][Gk[m][k].ID][j] == 1)
						{
							intervene = 1;
						}
					}

					//如果标志为1，则不加入g'k，如果为0，则加入gk,deleted变成1
					if (intervene == 0)
					{
						Gk[m].push_back(Mybuyers[j]);
						Mybuyers[j].deleted = 1;
					}
					else
					{
						//do nothing
					}

				}

			}
		}

		//打印分组
		/*cout << "以下为分组：" << endl;
		for (int m = 0; m < SNum; m++)
		{

		if (Gk[m].empty() == 1)
		{
		cout << m;
		cout << "组为空" << endl;
		}
		else
		{
		cout << m;
		cout << "组：" << endl;
		for (int j = 0; j < Gk[m].size(); j++)
		{
		cout << Gk[m][j].ID;
		cout << " ";
		}
		cout << endl;
		}

		}
		cout << "----" << endl;*/
		//竞标决策
		//对于每一个组，判断是组竞标是否大于seller的valuation
		bool win[SNum];//用于标记组竞标是否成功
		vector <Buyer> Winners[SNum];//用于存储竞标成功者

		int tempbid[SNum];//用于记录组内竞标的最小值
		int tempbidder;//用于记录竞标最小值的序号
		for (int m = 0; m < SNum; m++)//对于每一组
		{
			if (Gk[m].empty() != 1)
			{
				tempbid[m] = Gk[m][0].bids[m];//用于记录组内竞标的最小值
				tempbidder = 0;//用于记录竞标最小值的序号
				for (int j = 0; j < Gk[m].size(); j++)//判断是否成功+找到竞标最低的买家
				{
					if (Gk[m][j].bids[m] < tempbid[m])
					{
						tempbid[m] = Gk[m][j].bids[m];
						tempbidder = j;
					}

				}
				/*cout << "tempbid" ;
				cout << m  ;
				cout << tempbid[m];*/

				if (tempbid[m] * (Gk[m].size() - 1) >= Mysellers[m].value)//生成竞标成功者
				{
					win[m] = 1;
					for (int j = 0; j < Gk[m].size(); j++)
					{
						if (j != tempbidder)
						{
							Winners[m].push_back(Gk[m][j]);
						}
					}

				}

			}

		}


		//打印成功竞标的买家
		/*cout << "以下为成功的买家：" << endl;
		for (int m = 0; m < SNum; m++)
		{

		if (Winners[m].empty() == 1)
		{
		cout << m;
		cout << "组不成功" << endl;
		}
		else
		{
		cout << m;
		cout << "组：" << endl;
		for (int j = 0; j < Winners[m].size(); j++)
		{
		cout << Winners[m][j].ID;
		cout << " ";
		}
		cout << endl;
		}

		}
		cout << "----" << endl;*/
		//ratio of winning buyers
		float winnernum = 0.0;
		for (int m = 0; m < SNum; m++)
		{
			//cout << Winners[i].size() << endl;
			winnernum = winnernum + Winners[m].size();
		}
		ratioofwinningbuyers[i] = winnernum / BNum;
		/*cout << BNum<<endl;
		cout << winnernum << endl;
		cout << ratioofwinningbuyers[i] << endl;
		cout << "ratio of winning buyers";
		cout << winnernum / BNum<<endl;*/

		//售出频谱占全部频谱的比例
		float soldspec = 0.0;
		for (int m = 0; m < SNum;m++)
		{
			if (Winners[m].empty() == 1)//如果该组为空
			{
				//do nothing
			}
			else
			{
				soldspec += 1.0;
			}
		}
		soldspecratio[i] = soldspec / SNum;
		//cout << soldspec;
		/*cout << "soldspecratio";
		cout << soldspecratio[i];*/
		
		
		//cout << soldspec / SNum << endl;


		//social welfare (sum of all winning bidders’ utility)

		for (int m = 0; m < SNum; m++)
		{
			if (Winners[m].empty() != 1)
			{
				for (int n = 0; n < Winners[m].size(); n++)
				{
					//cout << Winners[m][n].bids[m]<<endl;
					//cout << tempbid[m] << endl;

					socialwelfare[i] += (Winners[m][n].bids[m] - tempbid[m]);
					//cout << socialwelfare[i] << endl;
					//cout << "--------------"<<endl;
				}
			}
		}
		/*cout << "socialwelfare";
		cout << socialwelfare[i] << endl;*/
		//sellers revenue所有卖家的收入之和
		for (int m = 0; m < SNum; m++)
		{
			if (Winners[m].empty() != 1)
			{
				sellerrevenue[i] += Winners[m].size()*tempbid[m];
				//for (int n = 0; n < Winners[m].size(); n++)
				//{
				//	//cout << Winners[m][n].bids[m]<<endl;
				//	//cout << tempbid[m] << endl;

				//	sellerrevenue[i] += (tempbid[m]);
				
				//	//cout << "--------------"<<endl;
				//}
			}
		}
		/*cout << "sellerrevenue";
		cout << sellerrevenue[i] << endl;*/
		//Spectrum Reusability degree，the ratio of winning proﬁles to the number of sold spectrums
		/*cout << "winnernum";
		cout << winnernum;
		cout << "soldsepctrum";
		cout << soldspec<<endl;*/
		totalwinnernum+= winnernum;
		totalsoldspec+= soldspec;
		/*specreuseratio[i] = winnernum / soldspec;
		cout << "specreuseratio";
		cout << specreuseratio[i] << endl;*/
	
		//清除内存
		for (int m = 0; m < SNum; m++)
		{
			Gk[m].clear();
			Winners[m].clear();
		}

		//cout << "-------------------分割线-----------------" << endl;
	}
	finish = clock();
	double time = finish - start;
	cout << "过了" << time << "ms" << endl;
	cout << "除5等于" << time / 500 << "ms" << endl;
	float sum1 = 0.0;//用于记录ratioofwinningbuyers的总和
	float sum2 = 0.0;//用于记录solspecratio的总和
	float sum3 = 0.0;//socialwelfare的总和
	float sum4 = 0.0;//sellersrevenue的总和
	//float sum5 = 0.0;//specreuseratio的总和
	for (int i = 0; i < 500; i++)
	{
		sum1 += ratioofwinningbuyers[i];
		sum2 += soldspecratio[i];
		sum3 += socialwelfare[i];
		sum4 += sellerrevenue[i];
		//sum5 += specreuseratio[i];
	}

	cout << "平均ratio of winning buyers:";
	cout << sum1 / 500 << endl;
	cout << "平均ratio of sold spectrums:";
	cout << sum2 / 500 << endl;
	cout << "平均socialwelfare:";
	cout << sum3 / 500 << endl;
	cout << "平均sellerrevenue:";
	cout << sum4 / 500 << endl;
	float specreuseratio = totalwinnernum / totalsoldspec;
	cout << "平均specreuseratio:";
	cout << totalwinnernum/totalsoldspec<< endl;
	system("PAUSE");
	return 0;
}

