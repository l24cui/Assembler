#include "kind.h"
#include "lexer.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::map;
using std::endl;
using std::cerr;
using std::cin;
using std::cout;
using std::getline;
using ASM::Token;
using ASM::Lexer;

map<string, int> symtab;

void savelabel(string label, int addr) {
  int len = label.length();
  string copy = label.substr(0, len-1);
  if (symtab.count(copy) != 0) {
    throw std::string("ERROR: LABEL EXISTED!");
  } else {
    symtab.insert(std::pair<string, int> (copy, addr));
  }
}

void printlong(int longInt) {
  signed char c;
  c = longInt >> 24;
  cout << c;
  c = longInt >> 16;
  cout << c;
  c = longInt >> 8;
  cout << c;
  c = longInt;
  cout << c; 
}

int main(int argc, char* argv[]){

  unsigned int addr = 0;

  // Nested vector representing lines of Tokens
  // Needs to be used here to cleanup in the case
  // of an exception
  vector< vector<Token*> > tokLines;
  try{
    // Create a MIPS recognizer to tokenize
    // the input lines
    Lexer lexer;
    // Tokenize each line of the input
    string line;
    while(getline(cin,line)){
      tokLines.push_back(lexer.scan(line));
    }

    vector<vector<Token*> >::iterator itlabel;
    for (itlabel = tokLines.begin(); itlabel != tokLines.end(); ++itlabel){
      vector<Token*>::iterator itlabel2;
      for(itlabel2 = itlabel->begin(); itlabel2 != itlabel->end(); ++itlabel2){
        string instr = (*(*itlabel2)).toString();
        if (instr == "LABEL") {
          string label = (*(*itlabel2)).getLexeme();
          savelabel(label, addr);
        } else {
          addr += 4;
          break;
        }
      }
    }
    addr = 0;

    // Iterate over the lines of tokens and print them
    // to standard error
    vector<vector<Token*> >::iterator it;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){

      int count = 0;
      vector<Token*>::iterator it1;
      for (it1 = it->begin(); it1 != it->end(); ++it1) {
        count++;
      }

      vector<Token*>::iterator it2;
      for(it2 = it->begin(); it2 != it->end(); ++it2){
        string instr = (*(*it2)).toString();
        if ((instr != "DOTWORD") && (instr != "LABEL") && (instr != "ID")) {
          throw std::string("ERROR: INSTRUCTION INVALID!");
        } else if (instr =="DOTWORD") {
          if (count != 2) {
            throw std::string("ERROR: INCORRECT NUMBER OF INPUT!");
          } else {
            it2++;
            string kind = (*(*it2)).toString();
            if ((kind == "HEXINT") || (kind == "INT")) {
              int word = (*(*it2)).toInt();
              printlong(word);
            } else if (kind == "ID") {
              string label = (*(*it2)).getLexeme();
              int exist = symtab.count(label);
              if (exist) {
                int labeladdr = symtab[label];
                printlong(labeladdr);
                addr += 4;
              } else {
                throw std::string("ERROR: LABEL NOT EXIST!");
              }
            } else {
              throw std::string("ERROR: INVALID INPUT!");
            }
          }
        } else if (instr == "LABEL") {
          count --;        
        } else if (instr == "ID") {
          string opcode = (*(*it2)).getLexeme();
          if ((opcode == "jr")|| (opcode == "jalr")) {
            int regstr,tmp,asmcode;
            if (count != 2) {
              throw std::string("ERROR: INCORRECT NUMBER OF INPUT!");
            } else {
              it2++;
              if ((*(*it2)).toString() == "REGISTER") {
                regstr = (*(*it2)).toInt();
                if (opcode == "jr") {
                  tmp = 8;
                } else if (opcode == "jalr") {
                  tmp = 9;
                }
                asmcode = ((0 << 26)|(regstr << 21)|(tmp));
                printlong(asmcode);
              } else {
                throw std::string("ERROR: INVALID INPUT!");
              }
            }
          } else if ((opcode == "add") || (opcode == "sub") || (opcode == "slt") || (opcode == "sltu")) {
            int d,s,t,tmp,asmcode;
            if (count != 6) {
              throw std::string("ERROR: INCORRECT NUMBER OF INPUT!");
            } else {
              if (opcode == "add") {
                tmp = 32;
              } else if (opcode == "sub") {
                tmp = 34;
              } else if (opcode == "slt") {
                tmp = 42;
              } else if (opcode == "sltu") {
                tmp = 43;
              }
              it2++;
              if ((*(*it2)).toString() == "REGISTER") {
                d = (*(*it2)).toInt();
                it2++;
                if ((*(*it2)).toString() == "COMMA") {
                  it2++;
                  if ((*(*it2)).toString() == "REGISTER") {
                    s = (*(*it2)).toInt();
                    it2++;
                    if ((*(*it2)).toString() == "COMMA") {
                      it2++;
                      if ((*(*it2)).toString() == "REGISTER") {
                        t = (*(*it2)).toInt();
                        asmcode = ((0 << 26)|(s << 21)|(t << 16)|(d << 11)|(tmp));
                        printlong(asmcode);
                      } else {
                        throw std::string("ERROR: INVALID INPUT!");
                      }
                    } else {
                      throw std::string("ERROR: INVALID INPUT!");
                    }
                  } else {
                    throw std::string("ERROR: INVALID INPUT!");
                  }
                } else {
                  throw std::string("ERROR: INVALID INPUT!");
                }
              } else {
                throw std::string("ERROR: INVALID INPUT!");
              }
            }
          } else if ((opcode == "beq") || (opcode == "bne")) {
            int s,t,i,tmp,asmcode;
            if (count != 6) {
              throw std::string("ERROR: INCORRECT NUMBER OF INPUT!");
            } else {
              if (opcode == "beq") {
                tmp = 4;
              } else if (opcode == "bne") {
                tmp = 5;
              }
              it2++;
              if ((*(*it2)).toString() == "REGISTER") {
                s = (*(*it2)).toInt();
                it2++;
                if ((*(*it2)).toString() == "COMMA") {
                  it2++;
                  if ((*(*it2)).toString() == "REGISTER") {
                    t = (*(*it2)).toInt();
                    it2++;
                    if ((*(*it2)).toString() == "COMMA") {
                      it2++;
                      string kind = (*(*it2)).toString();
                      if ((kind == "INT") || (kind == "HEXINT")) {
                        i = (*(*it2)).toInt();
                        if ((kind == "INT") && (i <= 32767) && (i >= -32768)) {
                          asmcode = ((tmp << 26)|(s << 21)|(t << 16)|(i & 0xffff));
                          printlong(asmcode);
                        } else if ((kind == "HEXINT") && (i >= 0x0000) && (i <= 0xffff)) {
                          asmcode = ((tmp << 26)|(s << 21)|(t << 16)|(i & 0xffff));
                          printlong(asmcode);
                        } else {
                          throw std::string("ERROR: OFFSET INPUT OUT OF RANGE!");
                        }
                      } else if (kind == "ID") {
                        string label = (*(*it2)).getLexeme();
                        int exist = symtab.count(label);
                        if (exist) {
                          int labeladdr = symtab.at(label);
                          i = ((labeladdr - addr - 4)/4);
                          addr = labeladdr - 4;//??????????????????????????????????????????????????
                          if ((i <= 32767) && (i >= -32768)) {
                            asmcode = ((tmp << 26)|(s << 21)|(t << 16)|(i & 0xffff));
                            printlong(asmcode);
                          } else {
                            throw std::string("ERROR: OFFSET INPUT OUT OF RANGE!");
                          }
                        } else {
                          throw std::string("ERROR: LABEL NOT EXIST!");
                        }
                      } else {
                        throw std::string("ERROR: INVALID INPUT!");
                      }
                    } else {
                      throw std::string("ERROR: INVALID INPUT!");
                    }
                  } else {
                    throw std::string("ERROR: INVALID INPUT!");
                  }
                } else {
                  throw std::string("ERROR: INVALID INPUT!");
                }
              } else {
                throw std::string("ERROR: INVALID INPUT!");
              }
            }
          } else if ((opcode == "lis") || (opcode == "mflo") || (opcode == "mfhi")) {
            int d,tmp,asmcode;
            if (count != 2) {
              throw std::string("ERROR: INCORRECT NUMBER OF INPUT!");
            } else {
              if (opcode == "lis") {
                tmp = 20;
              } else if (opcode == "mflo") {
                tmp = 18;
              } else if (opcode == "mfhi") {
                tmp = 16;
              }
              it2++;
              if ((*(*it2)).toString() == "REGISTER") {
                d = (*(*it2)).toInt();
                asmcode = ((0 << 16)|(d << 11)|(tmp));
                printlong(asmcode);
              } else {
                throw std::string("ERROR: INVALID INPUT!");
              }
            }
          } else if ((opcode == "mult") || (opcode == "multu") || (opcode == "div") || (opcode == "divu")) {
            int s,t,tmp,asmcode;
            if (count != 4) {
              throw std::string("ERROR: INCORRECT NUMBER OF INPUT!");
            } else {
              if (opcode == "mult") {
                tmp = 24;
              } else if (opcode == "multu") {
                tmp = 25;
              } else if (opcode == "div") {
                tmp = 26;
              } else if (opcode == "divu") {
                tmp = 27;
              }
              it2++;
              if ((*(*it2)).toString() == "REGISTER") {
                s = (*(*it2)).toInt();
                it2++;
                if ((*(*it2)).toString() == "COMMA") {
                  it2++;
                  if ((*(*it2)).toString() == "REGISTER") {
                    t = (*(*it2)).toInt();
                    asmcode = ((0 << 26)|(s << 21)|(t << 16)|(tmp));
                    printlong(asmcode);
                  } else {
                    throw std::string("ERROR: INVALID INPUT!");
                  }
                } else {
                  throw std::string("ERROR: INVALID INPUT!");
                }
              } else {
                throw std::string("ERROR: INVALID INPUT!");
              }
            }
          } else if ((opcode == "sw") || (opcode == "lw")) {
            int t,i,s,tmp,asmcode;
            if (count != 7) {
              throw std::string("ERROR: INCORRECT NUMBER OF INPUT!");
            } else {
              if (opcode == "sw") {
                tmp = 43;
              } else if (opcode == "lw") {
                tmp = 35;
              }
              it2++;
              if ((*(*it2)).toString() == "REGISTER") {
                t = (*(*it2)).toInt();
                it2++;
                if ((*(*it2)).toString() == "COMMA") {
                  it2++;
                  string kind = (*(*it2)).toString();
                  if ((kind == "INT") || (kind == "HEXINT")) {
                    i = (*(*it2)).toInt();
                    it2++;
                    if ((*(*it2)).toString() == "LPAREN") {
                      it2++;
                      if ((*(*it2)).toString() == "REGISTER") {
                        s = (*(*it2)).toInt();
                        it2++;
                        if ((*(*it2)).toString() == "RPAREN") {
                          if ((kind == "INT") && (i <= 32767) && (i >= -32768)) {
                            asmcode = ((tmp << 26)|(s) << 21|(t << 16)|(i & 0xffff));
                            printlong(asmcode);
                          } else if ((kind == "HEXINT") && (i >= 0x0000) && (i <= 0xffff)) {
                            asmcode = ((tmp << 26)|(s) << 21|(t << 16)|(i & 0xffff));
                            printlong(asmcode);
                          } else {
                            throw std::string("ERROR: OFFSET INPUT OUT OF RANGE!");
                          }
                        } else {
                          throw std::string("ERROR: INVALID INPUT!");
                        }
                      } else {
                        throw std::string("ERROR: INVALID INPUT!");
                      }
                    } else {
                      throw std::string("ERROR: INVALID INPUT!");
                    }
                  } else {
                    throw std::string("ERROR: INVALID INPUT!");
                  }
                } else {
                  throw std::string("ERROR: INVALID INPUT!");
                }
              } else {
                throw std::string("ERROR: INVALID INPUT!");
              }
            }
          }
          addr += 4;
        }
        //cerr << "  Token: "  << *(*it2) << endl;
      }
    }

  } catch(const string& msg){
    // If an exception occurs print the message and end the program
    cerr << msg << endl;
  }
  // Delete the Tokens that have been made
  vector<vector<Token*> >::iterator it;
  for(it = tokLines.begin(); it != tokLines.end(); ++it){
    vector<Token*>::iterator it2;
    for(it2 = it->begin(); it2 != it->end(); ++it2){
      delete *it2;
    }
  }
}
