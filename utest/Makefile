CXX=g++
CC=gcc
CXXFLAGS= -c -Wall -Werror -std=c++1y -pedantic -I. -I../libs -I../thirdparty -I../include -I../src -O0 -DLOG_LEVEL_WARNING -DBOOST_UUID_NO_TYPE_TRAITS -funsigned-char -g
CCFLAGS= -c -I. -I../libs  -I../thirdparty -I../include -I../src -O0 -g
LDFLAGS= -L/opt/local/lib -L/lib/x86_64-linux-gnu/ -L/lib -L/usr/lib -L/usr/local/lib
LIBS= -lcrypto -lssl -lpthread -ldl -lgtest

SOURCES=\
  test.o                         \
	../thirdparty/sqlite3.o        \
  util_bytes.test.o              \
  util_base64.test.o             \
  util_bitmask.test.o            \
  util_buffer.test.o             \
  util_swiss.test.o              \
  util_hash.test.o               \
  util_crypto.test.o             \
  util_print.test.o              \
  util_random.test.o             \
  util_sst.test.o                \
  util_js.test.o                 \
  util_exception.test.o          \
  util_retry.test.o              \
  util_utf8.test.o               \
  util_websocket.test.o          \
  util_sqlite.test.o             \
#  db_sequence.test.o             \
#  db_acl.test.o                  \
#  db_auth.test.o                 \
#  db_session.test.o              \
#  db_users.test.o                \

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test.tsk

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@  $(LIBS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

%.o : %.c
	$(CC) $(CCFLAGS) $< -o $@

clean:
	rm -rf *o test.tsk
