#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>

#include "probe.h"

#define KEYP_DELAY_US 1000

using namespace std;
typedef long long lld;

static random_device dev;
static mt19937_64 gen(dev());
static uniform_int_distribution<lld> dist(0, llround(std::pow(2,62)));
static uniform_int_distribution<int> U(0, 5);

enum evt {T, Co, Ky, Fr, I, K};
queue<evt> Q;
queue<evt> Q_t;
mutex mtx, mtx_t;

unordered_map<lld, int> memo;
lld sc_spos;

lld get_random_unused()
{
  lld x = dist(gen);
  while (memo.count(x)) x = dist(gen);
  return x;
}

void write(lld address, int value)
{
  if (address == sc_spos)
  {
    if (memo[address] < value)
    {
      int dif = value - memo[address];
      if (dif != 50 && dif != 200 && dif != 500) return;
    }
  }
  memo[address] = value;
  on_write(address, value);
}

void write_set(lld start_pos, set<pair<int, int> > &bst)
{
  write(start_pos, bst.size());
  int ind = 1;
  for (auto kvp : bst)
  {
    write(start_pos + ind, kvp.first);
    write(start_pos + ind + 1, kvp.second);
    ind += 2;
  }
}

set<pair<int, int> > read_set(lld start_pos)
{
  int siz = memo[start_pos];
  int ind = 1;
    
  set<pair<int, int> > ret;
  for (int i=0;i<siz;i++)
  {
    int fst = memo[start_pos + ind];
    int snd = memo[start_pos + ind + 1];
    ret.insert({fst, snd});
    ind += 2;
  }
    
  return ret;
}

int kbhit()
{
  int ch = getch();
  if (ch != ERR)
  {
    ungetch(ch);
    return 1;
  }
  else return 0;
}

void prmpt(bool win, string msg, int sc)
{
  clear();
  
  mvprintw(0, 0, "PetarV-World(TM) 2016");
  
  attron(COLOR_PAIR(win ? 3 : 1));    
  mvprintw(2, 3, "%s", msg.c_str());
  attroff(COLOR_PAIR(win ? 3 : 1));

  if (win)
  {
    mvprintw(4, 3, "Score: ");
    attron(A_BOLD);
    printw("%d", sc);
    attroff(A_BOLD);
  }

  mvprintw((win ? 6 : 4), 0, "Press any key to try again...");

  refresh();

  while (!kbhit());
  getch();
}

void curses()
{
  start_color();

  string s[10] = {
    "##################",
    "#####........#####",
    "#####.######.#####",
    "#................#",
    "#.###.######.###.#",
    "#.###.######.###.#",
    "#.###.######.....#",
    "#.###.############",
    "#.....############",
    "##################"};

  set<pair<int, int> > os_orig = {{1, 10}, {3, 2}, {3, 6}, {3, 10}, {3, 16},
                                  {4, 16}, {6, 1}, {6, 14}, {6, 15}};
  set<pair<int, int> > ss_orig = {{1, 8}, {5, 1}, {6, 16}};
  set<pair<int, int> > ks_orig = {{1, 11}, {3, 9}};
  set<pair<int, int> > fs_orig = {{1, 7}};

  // Now write this data back
  lld s_start = get_random_unused();
  int ind = 0;
  for (int i=0;i<10;i++)
  {
    for (int j=0;j<18;j++)
    {
      write(s_start + ind, s[i][j]);
      ind++;
    }
  }
    
  lld os_start = get_random_unused();
  write_set(os_start, os_orig);
    
  lld ss_start = get_random_unused();
  write_set(ss_start, ss_orig);

  lld ks_start = get_random_unused();
  write_set(ks_start, ks_orig);
    
  lld fs_start = get_random_unused();
  write_set(fs_start, fs_orig);
    
  auto os = read_set(os_start);
  auto ss = read_set(ss_start);
  auto ks = read_set(ks_start);
  auto fs = read_set(fs_start);

  lld xx_spos = get_random_unused();
  lld xy_spos = get_random_unused();
  lld x_old = get_random_unused();
  lld y_old = get_random_unused();
  lld x_new = get_random_unused();
  lld y_new = get_random_unused();
    
  write(xx_spos, 8);
  write(xy_spos, 5);

  write(x_old, 8);
  write(y_old, 2);
    
  sc_spos = get_random_unused();
  lld k_spos = get_random_unused();
  lld t_spos = get_random_unused();
    
  write(sc_spos, 0);
  write(k_spos, 0);
  write(t_spos, 15);
  
  vector<lld> co_temps, ky_temps, fr_temps;
  for (int i=0;i<5;i++)
  {
    co_temps.push_back(get_random_unused());
    ky_temps.push_back(get_random_unused());
    fr_temps.push_back(get_random_unused());
  }

  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(5, COLOR_CYAN, COLOR_BLACK);

  while (true)
  {
    clear();
    mvprintw(0, 0, "PetarV-World(TM) 2016");
    mvprintw(2, 3, "Time left: %d", memo[t_spos]);
    for (int i=0;i<10;i++)
    {
      for (int j=0;j<18;j++)
      {
        mvprintw(i + 4, j + 1, "%c", memo[s_start + i * 18 + j]);
      }
    }
    
    if (memo[k_spos] != 2)
    {
      attron(COLOR_PAIR(4));
      mvprintw(memo[xx_spos] + 4, memo[xy_spos] + 1, "X");
      attroff(COLOR_PAIR(4));
    }

    attron(COLOR_PAIR(3));
    for (auto f : fs)
    {
      mvprintw(f.first + 4, f.second + 1, "F");
    }
    if (memo[k_spos] == 2) mvprintw(memo[xx_spos] + 4, memo[xy_spos] + 1, "X");
    attroff(COLOR_PAIR(3));
       
    attron(COLOR_PAIR(1));
    for (auto s : ss)
    {
      mvprintw(s.first + 4, s.second + 1, "*");
    }
    attroff(COLOR_PAIR(1));
   
    attron(COLOR_PAIR(2));
    for (auto o : os)
    {
      mvprintw(o.first + 4, o.second + 1, "O");
    }
    for (auto k : ks)
    {
      mvprintw(k.first + 4, k.second + 1, "K");
    }
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(5));
    mvprintw(memo[x_old] + 4, memo[y_old] + 1, "P");
    attroff(COLOR_PAIR(5));
    
    mvprintw(15, 3, "Score: %d", memo[sc_spos]);
    mvprintw(17, 3, "Keys: %d/2", memo[k_spos]);
    
    refresh();

    write(x_new, memo[x_old]);
    write(y_new, memo[y_old]);
      
    if (kbhit())
    {
      switch (getch())
      {
        case 87: case 119: write(x_new, memo[x_new] - 1); break;
        case 83: case 115: write(x_new, memo[x_new] + 1); break;
        case 68: case 100: write(y_new, memo[y_new] + 1); break;
        case 65: case 97:  write(y_new, memo[y_new] - 1); break;
        default: break;
      }
    }

    if (memo[s_start + memo[x_new] * 18 + memo[y_new]] != '#')
    {
      if (ss.count({memo[x_new], memo[y_new]}))
      {
        prmpt(false, "Spike!", 0);
          
        write_set(os_start, os_orig);
        write_set(ss_start, ss_orig);
        write_set(ks_start, ks_orig);
        write_set(fs_start, fs_orig);
          
        os = read_set(os_start);
        ss = read_set(ss_start);
        ks = read_set(ks_start);
        fs = read_set(fs_start);
          
        write(xx_spos, 8);
        write(xy_spos, 5);
          
        write(x_old, 8);
        write(y_old, 2);
          
        write(sc_spos, 0);
        write(k_spos, 0);
        write(t_spos, 15);
          
        mtx.lock();
        queue<evt> empty;
        swap(Q, empty);
        mtx.unlock();
        continue;
      }
      else if (ks.count({memo[x_new], memo[y_new]}))
      {
        mtx.lock();
        for (int i=0;i<5;i++) Q.push(Ky);
        mtx.unlock();
        write(k_spos, memo[k_spos] + 1);
        ks.erase({memo[x_new], memo[y_new]});
        write_set(ks_start, ks);
      }
      else if (os.count({memo[x_new], memo[y_new]}))
      {
        mtx.lock();
        for (int i=0;i<5;i++) Q.push(Co);
        mtx.unlock();
        os.erase({memo[x_new], memo[y_new]});
        write_set(os_start, os);
      }
      else if (fs.count({memo[x_new], memo[y_new]}))
      {
        mtx.lock();
        for (int i=0;i<5;i++) Q.push(Fr);
        mtx.unlock();
        fs.erase({memo[x_new], memo[y_new]});
        write_set(fs_start, fs);
      }
      else if (memo[x_new] == memo[xx_spos] && memo[y_new] == memo[xy_spos] && memo[k_spos] == 2)
      {
        prmpt(true, "You won!", memo[sc_spos]);
        
        write_set(os_start, os_orig);
        write_set(ss_start, ss_orig);
        write_set(ks_start, ks_orig);
        write_set(fs_start, fs_orig);
          
        os = read_set(os_start);
        ss = read_set(ss_start);
        ks = read_set(ks_start);
        fs = read_set(fs_start);
          
        write(xx_spos, 8);
        write(xy_spos, 5);
          
        write(x_old, 8);
        write(y_old, 2);
          
        write(sc_spos, 0);
        write(k_spos, 0);
        write(t_spos, 15);
          
        mtx.lock();
        queue<evt> empty;
        swap(Q, empty);
        mtx.unlock();
        continue;
      }
    }
    else
    {
      write(x_new, memo[x_old]);
      write(y_new, memo[y_old]);
    }
      
    swap(x_old, x_new);
    swap(y_old, y_new);

    if (!Q.empty())
    {
      mtx.lock();
      auto e = Q.front();
      Q.pop();
      mtx.unlock();
      if (e == Co)
      {
        write(co_temps[U(gen)], memo[sc_spos] + 50);
        write(sc_spos, memo[sc_spos] + 50);
      }
      else if (e == Ky)
      {
        write(sc_spos, memo[sc_spos] + 200);
        write(ky_temps[U(gen)], memo[sc_spos] + 200);
      }
      else if (e == Fr)
      {
        write(sc_spos, memo[sc_spos] + 500);
        write(fr_temps[U(gen)], memo[sc_spos] + 500);
      }
      else if (e == T)
      {
        write(t_spos, memo[t_spos] - 1);
        if (memo[t_spos] == 0)
        {
          prmpt(false, "Time out!", 0);
            
          write_set(os_start, os_orig);
          write_set(ss_start, ss_orig);
          write_set(ks_start, ks_orig);
          write_set(fs_start, fs_orig);
            
          os = read_set(os_start);
          ss = read_set(ss_start);
          ks = read_set(ks_start);
          fs = read_set(fs_start);
            
          write(xx_spos, 8);
          write(xy_spos, 5);
            
          write(x_old, 8);
          write(y_old, 2);
            
          write(sc_spos, 0);
          write(k_spos, 0);
          write(t_spos, 15);
            
          mtx.lock();
          queue<evt> empty;
          swap(Q, empty);
          mtx.unlock();
          continue;
        }
      }
    }

    usleep(KEYP_DELAY_US);
  }
}

void do_tick()
{
  while (true)
  {
    usleep(1000000);
    mtx.lock();
    Q.push(T);
    mtx.unlock();
  }
}

int main(int argc, char **argv)
{
  initscr();
  
  cbreak();
  noecho();
  curs_set(FALSE);

  nodelay(stdscr, TRUE);
  scrollok(stdscr, TRUE);

  thread gui(curses);
  thread ticks(do_tick);

  gui.join();
  ticks.join();

  endwin(); // restore normal terminal behaviour

  return 0;
}
