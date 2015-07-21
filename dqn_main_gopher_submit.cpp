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
DEFINE_string(rom, "/home/shrubb/Programs/ALE/games/gopher.bin", "Atari 2600 ROM to play");
DEFINE_string(solver, "/home/shrubb/Projects/dqn-in-the-caffe/Net/dqn_seaquest_solver.prototxt", "Solver parameter file (*.prototxt)");
DEFINE_int32(memory, 500000, "Capacity of replay memory");
DEFINE_int32(explore, 1000000, "Number of iterations needed for epsilon to reach 0.1");
DEFINE_double(gamma, 0.95, "Discount factor of future rewards (0,1]");
DEFINE_int32(memory_threshold, 100, "Enough amount of transitions to start learning");
DEFINE_int32(skip_frame, 3, "Number of frames skipped");
DEFINE_bool(show_frame, false, "Show the current frame in CUI");
DEFINE_string(model, "/home/shrubb/Projects/dqn-in-the-caffe/Net/Snapshot-gopher/gopher_iter_162500.caffemodel", "Model file to load");
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

int main(int argc, char** argv) {
  /*int test;
  std::ifstream in("/home/shrubb/test.txt");
  for (int i = 0; i < 10; ++i) {
    test = in.get();
    std::cout << test << std::endl;
  }
  return 0;*/

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  google::LogToStderr();

  if (FLAGS_gpu) {
    caffe::Caffe::set_mode(caffe::Caffe::GPU);
  } else {
    caffe::Caffe::set_mode(caffe::Caffe::CPU);
  }

  freopen("/home/shrubb/Projects/dqn-in-the-caffe/build/simple_in", "r", stdin);

  fprintf(stdout, "team_4,CZELol,gopher\n");
  fflush(stdout);
  std::ofstream out("/home/shrubb/test.txt");

  char temp[70000];
  fgets(temp, 69999, stdin);
  out << temp << std::endl;
  //out.close();

  fprintf(stdout, "0,0,0,1\n");

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
  auto total_score = 0.0;
  for (auto frame = 0; ; ++frame) {

    std::vector<std::vector<unsigned char>> raw_screen(210, std::vector<unsigned char>(160));

    // read screen
    for (int i = 0; i < 210; ++i) {
      std::cout << i << std::endl;
      for (int j = 0; j < 160; ++j) {

        char a, b;
        fscanf(stdin, "%c%c", &a, &b);
        // out << a << b;
        if (b == 'I') { // DIE
          return 0;
        }
        raw_screen[i][j] = hex(a) * (unsigned char)16 + hex(b);
        //out << " " << (int)raw_screen[i][j] << std::endl;
      }
    };

    fscanf(stdin, "%*c"); // :
    char terminate;
    fscanf(stdin, "%c", &terminate);
    if (terminate == '1') {
      return 0;
    }
    fgets(temp, 69999, stdin);
      // TODO std::cout << "frame: " << frame << std::endl;
      const auto current_frame = dqn::PreprocessArrayScreen(raw_screen, out);
    std::cout << "AAA";
      past_frames.push_back(current_frame);
      if (past_frames.size() < dqn::kInputFrameCount) {
        // If there are not past frames enough for DQN input, just select NOOP
        for (auto i = 0; i < FLAGS_skip_frame + 1; ++i) {
          fprintf(stdout, "%d,18", PLAYER_A_NOOP);
          fflush(stdout);
          fscanf(stdin, "%*c");

          for (int i = 0; i < 210; ++i) {
            for (int j = 0; j < 160; ++j) {
              fscanf(stdin, "%*c%*c");
            }
          }
          std::cin.get(); // :

          char term;
          fscanf(stdin, "%c", &term); // 0
          if (term == '1')
            return 0;

          fgets(temp, 69999, stdin);
        }
      } else {
        if (past_frames.size() > dqn::kInputFrameCount) {
          past_frames.pop_front();
        }
        dqn::InputFrames input_frames;
        std::copy(past_frames.begin(), past_frames.end(), input_frames.begin());
        const auto action = dqn.SelectAction(input_frames, FLAGS_evaluate_with_epsilon);
        auto immediate_score = 0.0;
        for (auto i = 0; i < FLAGS_skip_frame + 1 && !ale.game_over(); ++i) {
          // Last action is repeated on skipped frames
          fprintf(stdout, "%d,18", action);
          fflush(stdout);
          fscanf(stdin, "%*c");
          for (int i = 0; i < 210; ++i) {
            for (int j = 0; j < 160; ++j) {
              fscanf(stdin, "%*c%*c");
            }
          }
          fscanf(stdin, "%*c"); // :

          char term;
          fscanf(stdin, "%c", &term); // 0
          if (term == '1')
            return 0;

          fgets(temp, 69999, stdin);
        }
      }
  }

  return 0;
}