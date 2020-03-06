g++ -std=c++11 test_minihls.cpp
if [ $? -eq 0 ]; then
  echo "Cmd: " $1
  ./a.out $1
else
  echo "Build FAILED"
fi

