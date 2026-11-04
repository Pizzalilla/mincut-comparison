// sw_probe.cpp: counting probe for the report's analysis. Not timed.
//
// Written by Claude (AI) at Kartikay's request on 2026-11-04.
// Rebuilds the exact benchmark instances (same seeds as bench.cpp) and runs a
// copy of Stoer-Wagner's loops that counts, instead of timing, how often inA
// changes along the active list. Also prints Karger-Stein's recursive call count.
// Build: c++ -std=c++20 -O2 -Iinclude -Icommon analysis/sw_probe.cpp common/generators.cpp -o build/sw_probe
// Run:   ./build/sw_probe > analysis/sw_flips.csv

#include <cstdio>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include "generators.hpp"
#include <string>
std::uint64_t splitmix64(std::uint64_t x){x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);}
std::uint64_t instanceSeed(const std::string& family,int n,int instance){std::uint64_t h=1469598103934665603ULL;for(unsigned char c:family)h=(h^c)*1099511628211ULL;return splitmix64(splitmix64(h^(std::uint64_t)n)^(std::uint64_t)instance);}
long long ksCalls(int n){ if(n<=2) return 1; int t=(int)std::ceil(1.0+n/std::sqrt(2.0)); if(t>=n)t=n-1; if(t<2)t=2; return 1+2*ksCalls(t);}
struct SWStats{ long long scans=0, changes=0, flips=0, evals=0; };
SWStats swProbe(const Graph& g){
  int n=g.n; std::vector<std::vector<long long>> mat(n,std::vector<long long>(n,0));
  for(auto&e:g.edges){mat[e.u][e.v]+=e.w;mat[e.v][e.u]+=e.w;}
  std::vector<int> active(n); std::iota(active.begin(),active.end(),0);
  std::vector<long long> w(n); std::vector<bool> inA(n); SWStats s;
  while(active.size()>1){
    for(int v:active){w[v]=0;inA[v]=false;}
    int prev=-1,last=-1;
    for(size_t step=0;step<active.size();++step){
      int next=-1;
      {bool pr=false; for(int v:active){ ++s.evals; if(inA[v]!=pr) ++s.flips; pr=inA[v];}} for(int v:active){ if(inA[v]) continue; ++s.scans; if(next==-1||w[v]>w[next]){ if(next!=-1) ++s.changes; next=v;} }
      inA[next]=true; prev=last; last=next;
      for(int u:active) if(!inA[u]) w[u]+=mat[next][u];
    }
    for(int u=0;u<n;++u) mat[prev][u]+=mat[last][u];
    for(int u=0;u<n;++u) mat[u][prev]+=mat[u][last];
    active.erase(std::find(active.begin(),active.end(),last));
  }
  return s;
}
int main(){
  for(int n:{25,50,100,200,400}) std::fprintf(stderr,"KS calls n=%d: %lld  (calls/n^2=%.2f)\n",n,ksCalls(n),ksCalls(n)/(double)(n*n));
  std::printf("family,n,instance,m,flips,evals,changes\n");
  for(int n:{25,50,100}) for(int inst=0;inst<3;++inst){
    int k=std::min(n,std::max(3,n/7));
    auto sd=[&](const char*f){return instanceSeed(f,n,inst);};
    struct {const char*name; Graph g;} fs[]={{"random_dense",generateRandomGraph(n,0.5,100,sd("random_dense"))},{"random_sparse",generateRandomGraph(n,0.1,100,sd("random_sparse"))},{"barbell",generateBarbellGraph(n,100,sd("barbell"))},{"cycle",generateCycleGraph(n,1)},{"many_cuts",generateManyCutsGraph(n,k,1,3,2,sd("many_cuts"))}};
    for(auto&f:fs){auto s=swProbe(f.g); std::printf("%s,%d,%d,%zu,%lld,%lld,%lld\n",f.name,n,inst,f.g.edges.size(),s.flips,s.evals,s.changes);}
  }
}
