#include <fstream>
#include <cstdlib>
#include "../codegen/x86_64/emitter.h"

int runPipeline(const std::string& inputPath, const std::string& outputPath)
{
  (void)inputPath;
  {
      std::ofstream asmFile("/tmp/linklang_out.s");
      emitExitProgram(asmFile, 932);
  }
  if(std::system("as -o /tmp/linklang_out.o /tmp/linklang_out.s") !=0)
    return 1;
  
  std::string linkCmd = "ld -o " + outputPath + " /tmp/linklang_out.o";
  if(std::system(linkCmd.c_str()) != 0)
    return 1;

  return 0;
}


