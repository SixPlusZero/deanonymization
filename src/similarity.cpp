#include "similarity.h"

using namespace std;

static void PrintMatrix(const SimMat &sim_score) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      printf("%lf ", sim_score[i][j]);
    }
    printf("\n");
  }
}

static void InitSimMatrix() {
  sim_score[0].resize(n1 + 1);
  sim_score[1].resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    sim_score[0][i].resize(n2 + 1);
    sim_score[1][i].resize(n2 + 1);
    for (int j = 1; j <= n2; j++) {
      sim_score[0][i][j] = min((double)G1[i].size(), (double)G2[j].size())
                         / max((double)G1[i].size(), (double)G2[j].size());
    }
  }
}

static double MaxMatch(int x, int y, const SimMat &sim_score) {
  size_t nx = G1[x].size();
  size_t ny = G2[y].size();
  SimMat nb_score;
  nb_score.resize(nx);
  for (int i = 0; i < nx; i++) {
    nb_score[i].resize(ny);
    int nbx = G1[x][i];
    for (int j = 0; j < ny; j++) {
      int nby = G2[y][j];
      nb_score[i][j] = sim_score[nbx][nby];
    }
  }

  vector<bool> matched_x(nx, 0);
  vector<bool> matched_y(ny, 0);
  vector<int> top(nx + 1, 0);
  for (int i = 0; i < nx; i++) {
    double tmp_max = 0;
    for (int j = 0; j < ny; j++) {
      if (nb_score[i][j] > tmp_max) {
        tmp_max = nb_score[i][j];
        top[i] = j;
      }
    }
  }

  int cnt = 0;
  double res = 0;
  while (cnt < nx && cnt < ny) {
    int arg = 0;
    double tmp_max = 0;
    for (int i = 0; i < nx; i++) {
      if (nb_score[i][top[i]] > tmp_max
          && matched_x[i] == 0 && matched_y[top[i]] == 0) {
        tmp_max = nb_score[i][top[i]];
        arg = i;
      }
    }
    matched_x[arg] = 1;
    matched_y[top[arg]] = 1;
    res += nb_score[arg][top[arg]];
    cnt++;
    for (int i = 0; i < nx; i++) {
      if (top[i] == top[arg] && i != arg && matched_x[i] == 0) {
        double tmp = 0;
        for (int j = 0; j < ny; j++) {
          if (nb_score[i][j] > tmp && matched_y[j] == 0) {
            tmp = nb_score[i][j];
            top[i] = j;
          }
        }
      }
    }
  }
  return res;
}

static void Iterate(const SimMat &sim_score, SimMat &new_score) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      if (G1[i].size() > 0 && G2[j].size() > 0)
        new_score[i][j] = MaxMatch(i, j, sim_score)
                        / max((double)G1[i].size(), (double)G2[j].size());
      else
        new_score[i][j] = 0;
    }
  }
}

// Calculate similarity matrix
void CalcSimilarity() {
  InitSimMatrix();
  int old = 0;
  for (int i = 0; i < ITER_NUM; i++) {
    old = i & 0x1;
    printf("ineration %d\n", i);
    Iterate(sim_score[old], sim_score[1-old]);
  }
  PrintMatrix(sim_score[ITER_NUM & 0x1]);
}