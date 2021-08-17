#include <iostream>
#include <string.h>

#define m 5
#define n 5
#define MAX_EDGE 3
#define MAX_WAVEFRONT 25
#define HASH_SIZE 13

using namespace std;

bool M_in[m + 1][n + 1];
bool I_in[m + 1][n + 1];
bool D_in[m + 1][n + 1];

int tran(char a) {
	int c;
	if (a == 'A') {
		c = 0;
	}
	if (a == 'G') {
		c = 1;
	}
	if (a == 'C') {
		c = 2;
	}
	if (a == 'T')
	{
		c = 3;
	}
	return c;
}

typedef struct
{
	int x;
	int o;
	int e;
}penalty;

typedef struct Node
{
	int base;
	int next[MAX_EDGE];
	int edgenum;
}Node;

typedef struct Graph
{
	int num;
	Node node[n + 1];
}Graph;

typedef struct Index
{
	int h;
	int u;
};

typedef struct Wavefront
{
	int score;
	Index index[MAX_WAVEFRONT];
	int index_num;
};

void AddIndex(Wavefront& M, int h, int u, bool in[m + 1][n + 1]);
Wavefront WF_Extend(Wavefront& M, Graph& q, string& t);
Wavefront Extend(int h, int u, Wavefront& M, Graph& q, string& t);
void WF_Next(Wavefront& M, Wavefront& D, Wavefront& I, Wavefront M_temp[], Wavefront D_temp[], Wavefront I_temp[], int s, penalty p, Graph& q, string& t);

Wavefront WF_Extend(Wavefront& M, Graph& q, string& t)
{
	int k = M.index_num;
	for (int i = 0; i < k; i++)
	{
		int u = M.index[i].u;
		int h = M.index[i].h;
		Extend(h, u, M, q, t);
	}
	return M;
}

Wavefront Extend(int h, int u, Wavefront& M, Graph& q, string& t)
{
	for (int j = 0; j < q.node[u].edgenum; j++)
	{
		int v = q.node[u].next[j];
		if (q.node[v].base == tran(t[h]))
		{
			AddIndex(M, h + 1, v, M_in);
			Extend(h + 1, v, M, q, t);
		}
	}
	return M;
}

void AddIndex(Wavefront& M, int h, int u, bool in[m+1][n+1])
{
	if (!in[h][u])
	{
		in[h][u] = 1;
		M.index_num++;
		M.index[M.index_num - 1].h = h;
		M.index[M.index_num - 1].u = u;
	}
}

void WF_Next(Wavefront& M, Wavefront& D, Wavefront& I, Wavefront M_temp[], Wavefront D_temp[], Wavefront I_temp[], int s, penalty p, Graph& q, string& t)
{
	int x_hash = (s + p.x) % HASH_SIZE;
	int o_hash = (s + p.o + p.e) % HASH_SIZE;
	for (int i = 0; i < M.index_num; i++)
	{
		int u = M.index[i].u;
		int h = M.index[i].h;
		for (int j = 0; j < q.node[u].edgenum; j++)
		{
			int v = q.node[u].next[j];
			M_temp[x_hash].index_num++;
			M_temp[x_hash].index[M_temp[x_hash].index_num - 1].u = v;
			M_temp[x_hash].index[M_temp[x_hash].index_num - 1].h = h + 1;
			D_temp[o_hash].index_num++;
			D_temp[o_hash].index[D_temp[o_hash].index_num - 1].u = v;
			D_temp[o_hash].index[D_temp[o_hash].index_num - 1].h = h;
		}
		I_temp[o_hash].index_num++;
		I_temp[o_hash].index[I_temp[o_hash].index_num - 1].u = u;
		I_temp[o_hash].index[I_temp[o_hash].index_num - 1].h = h + 1;
	}
	int e_hash = (s + p.e) % HASH_SIZE;
	for (int i = 0; i < I.index_num; i++)
	{
		int u = I.index[i].u;
		int h = I.index[i].h;
		I_temp[e_hash].index_num++;
		I_temp[e_hash].index[I_temp[e_hash].index_num - 1].u = u;
		I_temp[e_hash].index[I_temp[e_hash].index_num - 1].h = h + 1;
	}
	for (int i = 0; i < D.index_num; i++)
	{
		int u = D.index[i].u;
		int h = D.index[i].h;
		for (int j = 0; j < q.node[u].edgenum; j++)
		{
			int v = q.node[u].next[j];
			D_temp[e_hash].index_num++;
			D_temp[e_hash].index[D_temp[e_hash].index_num - 1].u = v;
			D_temp[e_hash].index[D_temp[e_hash].index_num - 1].h = h;
		}
	}
	if (D_temp[o_hash].index_num > 0)
		D_temp[o_hash].score = s + p.o + p.e;
	if (I_temp[o_hash].index_num > 0)	
		I_temp[o_hash].score = s + p.o + p.e;
	if (M_temp[x_hash].index_num > 0)
		M_temp[x_hash].score = s + p.x;
	if (D_temp[e_hash].index_num > 0)
		D_temp[e_hash].score = s + p.e;
	if (I_temp[e_hash].index_num > 0)
		I_temp[e_hash].score = s + p.e;
}

void InitWavefront(Wavefront& M)
{
	M.score = -1;
	M.index_num = 0;
}

void WF_Graph_Align(Graph& q, string& t, penalty p)
{
	/* Initial setting */
	Wavefront M_temp[HASH_SIZE],I_temp[HASH_SIZE],D_temp[HASH_SIZE],M,I,D;
	for (int i = 0; i < HASH_SIZE; i++)
	{
		InitWavefront(M_temp[i]);
		InitWavefront(I_temp[i]);
		InitWavefront(D_temp[i]);
	}
	InitWavefront(M);
	InitWavefront(I);
	InitWavefront(D);
	for (int i = 0; i <= m; i++)
	{
		for (int j = 0; j <= n; j++)
		{
			M_in[i][j] = 0;
			D_in[i][j] = 0;
			I_in[i][j] = 0;
		}
	}

	/* Add the first Index at score 0 */
	int s = 0;
	M.score = s;
	AddIndex(M, 0, 0, M_in);

	while (1)
	{
		M.score = s;
		D.score = s;
		I.score = s;

		/* merge */
		int k = s % HASH_SIZE;
		for (int i = 0; i < D_temp[k].index_num; i++)
			AddIndex(D, D_temp[k].index[i].h, D_temp[k].index[i].u, D_in);
		for (int i = 0; i < I_temp[k].index_num; i++)
			AddIndex(I, I_temp[k].index[i].h, I_temp[k].index[i].u, I_in);
		for (int i = 0; i < D.index_num; i++)
			AddIndex(M, D.index[i].h, D.index[i].u, M_in);
		for (int i = 0; i < I.index_num; i++)
			AddIndex(M, I.index[i].h, I.index[i].u, M_in);
		for (int i = 0; i < M_temp[k].index_num; i++)
			AddIndex(M, M_temp[k].index[i].h, M_temp[k].index[i].u, M_in);

		InitWavefront(M_temp[k]);
		InitWavefront(D_temp[k]);
		InitWavefront(I_temp[k]);

		/* Extend */
		M = WF_Extend(M, q, t);

		for (int i = 0; i < M.index_num; i++)
		{
			cout << s << '\t' << M.index[i].h << '\t' << M.index[i].u << endl;
		}

		/* Break */
		if (M_in[m][n])
			break;

		/* Next */
		WF_Next(M, D, I, M_temp, D_temp, I_temp, s, p, q, t);

		/* Get the next score */
		int NextScore = INT_MAX;
		for (int i = 0; i < HASH_SIZE; i++)
		{
			if (M_temp[i].score > 0 && M_temp[i].score < NextScore)
				NextScore = M_temp[i].score;
			if (D_temp[i].score > 0 && D_temp[i].score < NextScore)
				NextScore = D_temp[i].score;
			if (I_temp[i].score > 0 && I_temp[i].score < NextScore)
				NextScore = I_temp[i].score;
		}
		s = NextScore;
		//cout << s << endl;

		/* Delete the wavefront*/
		InitWavefront(M);
		InitWavefront(I);
		InitWavefront(D);
	}
	cout << s << endl;
}

int main()
{
	string t = "ACTGC";

	Graph q;
	q.num = m;
	q.node[0].base = -1;
	string s = "TACGA";
	for (int i = 1; i <= m; i++)
	{
		q.node[i].base = tran(s[i - 1]);
	}
	q.node[0].edgenum = 1;
	q.node[0].next[0] = 1;
	q.node[1].edgenum = 1;
	q.node[1].next[0] = 2;
	q.node[2].edgenum = 2;
	q.node[2].next[0] = 3;
	q.node[2].next[1] = 4;
	q.node[3].edgenum = 2;
	q.node[3].next[0] = 1;
	q.node[3].next[1] = 4;
	q.node[4].edgenum = 1;
	q.node[4].next[0] = 5;
	q.node[5].edgenum = 1;
	q.node[5].next[0] = 1;

	penalty p = { 4,6,2 };

	WF_Graph_Align(q, t, p);
	
	return 0;
}