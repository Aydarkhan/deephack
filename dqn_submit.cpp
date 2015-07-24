#include <cmath>
#include <iostream>
#include <ale_interface.hpp>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "prettyprint.hpp"
#include "dqn.hpp"
#include <fstream>

DEFINE_bool(gpu, false, "Use GPU to brew Caffe");
DEFINE_bool(gui, false, "Open a GUI window");
DEFINE_string(rom, "/home/ubuntu/caffe-ruslan/games/gopher.bin", "Atari 2600 ROM to play");
DEFINE_string(game_name, "gopher", "Game name");
DEFINE_string(solver, "/home/ubuntu/caffe-ruslan/Net/dqn_gopher_solver.prototxt", "Solver parameter file (*.prototxt)");
DEFINE_int32(memory, 500000, "Capacity of replay memory");
DEFINE_int32(explore, 1000000, "Number of iterations needed for epsilon to reach 0.1");
DEFINE_double(gamma, 0.95, "Discount factor of future rewards (0,1]");
DEFINE_int32(memory_threshold, 100, "Enough amount of transitions to start learning");
DEFINE_int32(skip_frame, 3, "Number of frames skipped");
DEFINE_bool(show_frame, false, "Show the current frame in CUI");
DEFINE_string(model, "/home/ubuntu/caffe-ruslan/Net/Snapshot-gopher/gopher_iter_247500.caffemodel", "Model file to load");
DEFINE_bool(evaluate, true, "Evaluation mode: only playing a game, no updates");
DEFINE_double(evaluate_with_epsilon, 0.05, "Epsilon value to be used in evaluation mode");
DEFINE_double(repeat_games, 30, "Number of games played in evaluation mode");

double CalculateEpsilon(const int iter) {
  if (iter < FLAGS_explore) {
    return 1.0 - 0.9 * (static_cast<double>(iter) / FLAGS_explore);
  } else {
    return 0.1;
  }
}

unsigned char hex(char x) {
  if (x >= '0' and x <= '9') {
    return x - '0';
  } else {
    return 10 + x - 'A';
  }
}

int zero_count = 0;
int total_score = 0;

bool read_screen(std::vector<std::vector<unsigned char>> &raw_screen){

  bool term = false;
  char a, b;
  char terminate;
  char reward[10];

  // read screen
  for (int i = 0; i < 210; ++i) {
      for (int j = 0; j < 160; ++j) {

          int scan_res = fscanf(stdin, "%c%c", &a, &b);
          if (b == 'I') { // DIE
              term = true;
              break;
          }
          raw_screen[i][j] = hex(a) * (unsigned char)16 + hex(b);
      }
  }



  char temp;
  int scan_res = fscanf(stdin, "%c", &temp); // :
  scan_res = fscanf(stdin, "%c", &terminate);
  scan_res = fscanf(stdin, "%c", &temp); // :
  std::cin.get(reward, 9, ':');
  if (reward[0] != '0') {
    std::cerr << zero_count << " zeros\n" << reward << std::endl;
    zero_count = 0;
    total_score += atoi(reward);
  } else {
    zero_count++;
  }
  if (terminate == '1') {
      term = true;
  }
  //char temp[70000];
  //fgets(temp, 69999, stdin);
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  return term;
}

void make_action(ALEInterface ale, Action action){
      
      fprintf(stdout, "%d,18\n", action);
      fflush(stdout);

      if(FLAGS_gui)
          ale.act(action);
      return;
}

int main(int argc, char** argv) {
  /*int test;
  std::ifstream in("/home/shrubb/test.txt");
  for (int i = 0; i < 10; ++i) {
    test = in.get();
    std::cout << test << std::endl;
  }
  return 0;*/

    //google::
        //minloglevel=google::ERROR;

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  google::LogToStderr();

  if (FLAGS_gpu) {
    caffe::Caffe::set_mode(caffe::Caffe::GPU);
  } else {
    caffe::Caffe::set_mode(caffe::Caffe::CPU);
  }

  //freopen("simple_in", "r", stdin);

  fprintf(stdout, "team_4,CZELol,%s\n", FLAGS_game_name.c_str());
  fflush(stdout);

  //std::ofstream out("test.txt");

  char temp[70000];
  fgets(temp, 69999, stdin);
  //out << temp << std::endl;
  //out.close();

  fprintf(stdout, "0,0,0,1\n");
  fflush(stdout);

  ALEInterface ale(FLAGS_gui);
  // Load the ROM file
  ale.loadROM(FLAGS_rom);

  // Get the vector of legal actions
  const auto legal_actions = ale.getMinimalActionSet();

  dqn::DQN dqn(legal_actions, FLAGS_solver, FLAGS_memory, FLAGS_gamma);
  dqn.Initialize();
  dqn.LoadTrainedModel(FLAGS_model);

//  char test[70000];
//  fscanf(stdin, "%s", test);
//  std::cout << strlen(test);
//  return 0;

  std::deque<dqn::FrameDataSp> past_frames;
  dqn::FrameDataSp current_frame;
  std::vector<std::vector<unsigned char>> raw_screen(210, std::vector<unsigned char>(160));

  bool term;

  // If there are not past frames enough for DQN input, just select NOOP
  float max_qvalue;
  int frame = 0;
  for (; frame < 4; ++frame){
      term = read_screen(raw_screen);
      if (term)
          goto konec;

      //std::cout << "Term " << term << std::endl;

      current_frame = dqn::PreprocessArrayScreen(raw_screen);
      past_frames.push_back(current_frame);

      make_action(ale, PLAYER_A_NOOP);

  }

  for (;; ++frame) {

      term = read_screen(raw_screen);
      if (term)
          goto konec;

      //std::cout << "Term " << term << std::endl;

      if (frame % 500 == 0) std::cerr << "frame: " << frame << std::endl;
      current_frame = dqn::PreprocessArrayScreen(raw_screen);
      past_frames.push_back(current_frame);
      past_frames.pop_front();

      dqn::InputFrames input_frames;
      std::copy(past_frames.begin(), past_frames.end(), input_frames.begin());
      const auto action = dqn.SelectAction(input_frames, FLAGS_evaluate_with_epsilon, max_qvalue);
      auto immediate_score = 0.0;
      //for (auto i = 0; i < FLAGS_skip_frame + 1  && !ale.game_over(); ++i) {
      for (auto i = 0; i < FLAGS_skip_frame; ++i) {
          // Last action is repeated on skipped frames
          make_action(ale, action);
          term = read_screen(raw_screen);
          //std::cout << "Term " << term << std::endl;
          if (term)
              goto konec;
      }
      make_action(ale, action);
      
  }
  //out.close();

  konec:
  std::cerr << "total score: " << total_score << std::endl;
  return 0;
}
