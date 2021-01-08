#include "convert_svgs.h"

#include <dirent.h>
#include <atomic>
#include <omp.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

inline string replace_string(std::string result, const std::string &original, const std::string &newstr){
  auto pos = result.find(original);
  if(pos != std::string::npos){
    result.replace(std::begin(result) + pos, std::begin(result) + pos + original.size(), newstr);
  }
  return result;
}

inline void svg2png(const string& fn_svg, const string& svg_dir, const string& jaggy_folder, const string& smooth_folder, const string& dim, const bool is_gray, const bool is_background) {
  string fn_png = replace_string(fn_svg, ".svg", ".png");
  string in_svg = svg_dir + fn_svg;
  string out_png_jaggy = jaggy_folder + fn_png;
  string out_png_smooth = smooth_folder + fn_png;
  string fn_pdf = replace_string(in_svg, ".svg", ".pdf");
  if(!system(("rsvg-convert -w " + dim + " -h " + dim + " -f pdf -o \"" + fn_pdf + "\"" + " \"" + in_svg + "\"").data())) {
    (void)!system((string("convert ") + (is_background ? "" : "-background none") +
                   " -flatten +antialias " +
                   (is_gray ? "-colorspace gray -quantize gray -depth 8 -colors 256 " : "-colorspace RGB -depth 8 " + string(is_background ? "-format PNG24" : "-format PNG32")) + " \"" +
                   fn_pdf + "\"" + " \"" + out_png_jaggy + "\"")
                      .data());
    (void)!system((string("convert ") + (is_background ? "" : "-background none") +
                   " -flatten -antialias " +
                   (is_gray ? "-colorspace gray -quantize gray -depth 8 -colors 256 " : "-colorspace RGB -depth 8 " + string(is_background ? "-format PNG24" : "-format PNG32")) + " \"" +
                   fn_pdf + "\"" + " \"" + out_png_smooth + "\"")
                      .data());
    (void)!system((string("rm ") + fn_pdf).data());
  } else {
//    (void)!system((string("rm ") + in_svg).data());
//#pragma omp critical
//    {
//      cout << "error: " << fn_svg << endl;
//      cout << "deleted: " << fn_svg << endl;
//    }
  }
}

void convert_svgs(const char* _svg_dir, const char* _out_dir, const char* _dim, int _is_gray, int _is_background){
  string svg_dir = _svg_dir;
  string out_dir = _out_dir;
  string dim = _dim;
  bool is_gray = _is_gray;
  bool is_background = _is_background;

// make folders
  string main_folder = out_dir + dim + (is_gray ? "_gray" + string(is_background ? "" : "_alpha"): "_" + string(is_background ? "rgb" : "rgba"));
  string jaggy_folder = main_folder + "/jaggy/";
  string smooth_folder = main_folder + "/smooth/";
  (void)!system(("mkdir -p " + out_dir).data());
  (void)!system(("mkdir -p " + main_folder).data());
  (void)!system(("mkdir -p " + jaggy_folder).data());
  (void)!system(("mkdir -p " + smooth_folder).data());

  atomic<uint64_t> counter{0};

  auto tick = omp_get_wtime();

  vector<string> files;

  struct dirent* d;
  DIR* dr;
  dr = opendir(svg_dir.data());
  if (dr != nullptr) {
    while ((d = readdir(dr)) != nullptr) {
      string fn_svg = d->d_name;
      auto pos = fn_svg.find(".svg");
      if (pos != std::string::npos) {
        files.push_back(fn_svg);
      }
    }
  }
  closedir(dr);

#pragma omp parallel for schedule(dynamic)
  for (size_t i = 0; i < files.size(); ++i) {
    svg2png(files[i], svg_dir, jaggy_folder, smooth_folder, dim, is_gray, is_background);
    auto c = counter.fetch_add(1);
    if(c % 100 == 0 && c != 0){
#pragma omp critical
      cout << "amount processed: " << c << endl;
    }
  }

  cout << "\ntotal processed: " << counter << endl;
  cout << "time: " << omp_get_wtime() - tick << " s" << endl;
}

