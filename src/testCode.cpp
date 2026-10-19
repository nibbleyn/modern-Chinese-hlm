#include "testCode.hpp"

static constexpr const char *const nibble_lut_lc = "0123456789abcdef";
static constexpr const char *const nibble_lut_uc = "0123456789ABCDEF";

uint8_t decode(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
  if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }
  throw std::invalid_argument("invalid hex char");
}

std::size_t decode(const void *hex, std::size_t h_len, void *bin,
                   std::size_t b_len) {
  if (h_len % 2 != 0) {
    throw std::invalid_argument("invalid hex len");
  }
  if ((h_len >> 1) > b_len) {
    throw std::invalid_argument("insufficient bin len");
  }

  auto hexc = static_cast<const char *>(hex);
  auto binc = static_cast<uint8_t *>(bin);
  for (std::size_t i = 0; i < h_len; i += 2) {
    binc[(i >> 1)] = (decode(hexc[i]) << 4) | decode(hexc[i + 1]);
  }
  return h_len >> 1;
}

std::size_t encode(const void *bin, std::size_t b_len, void *hex,
                   std::size_t h_len, bool uppercase) {
  if ((h_len >> 1) < b_len) {
    throw std::invalid_argument("insufficient hex len");
  }

  auto binc = static_cast<const uint8_t *>(bin);
  auto hexc = static_cast<char *>(hex);
  const char *lutc = (uppercase) ? nibble_lut_uc : nibble_lut_lc;
  for (std::size_t hi = 0, bi = 0; bi < b_len;) {
    uint8_t c = binc[bi++];
    hexc[hi++] = lutc[c >> 4];
    hexc[hi++] = lutc[c & 0x0f];
  }
  return b_len << 1;
}

std::size_t decode(const void *hex, std::size_t h_len, void *bin,
                   std::size_t b_len);
std::size_t encode(const void *bin, std::size_t b_len, void *hex,
                   std::size_t h_len, bool uppercase = false);

template <typename RB = uint8_t, class RA = std::vector<RB>>
inline RA decode(const void *hex, std::size_t len) {
  RA res(((len >> 1) + sizeof(RB) - 1) / (sizeof(RB)));
  (void)decode(hex, len, res.data(), res.size());
  return res;
}

template <typename RB = char, class RA = std::basic_string<RB>>
inline RA encode(const void *bin, std::size_t len, bool uppercase = false) {
  RB buf[((len << 1) + sizeof(RB) - 1) / sizeof(RB)];
  (void)encode(bin, len, buf, sizeof(buf), uppercase);
  RA res;
  res.assign(buf, (len << 1));
  return res;
}

template <typename RB = uint8_t, class RA = std::vector<RB>, typename SB = char,
          class SA = std::basic_string<SB>>
inline RA decode(const SA &hex) {
  return decode<RB, RA>(hex.data(), hex.size() * sizeof(SB));
}

template <typename RB = char, class RA = std::basic_string<RB>,
          typename SB = uint8_t, class SA = std::vector<SB>>
inline RA encode_upper(const SA &bin) {
  return encode<RB, RA>(bin.data(), bin.size() * sizeof(SB), true);
}

template <typename RB = char, class RA = std::basic_string<RB>,
          typename SB = uint8_t, class SA = std::vector<SB>>
inline RA encode_lower(const SA &bin) {
  return encode<RB, RA>(bin.data(), bin.size() * sizeof(SB), false);
}

template <typename RB = char, class RA = std::basic_string<RB>,
          typename SB = uint8_t, class SA = std::vector<SB>>
inline RA encode(const SA &bin) {
  return encode<RB, RA>(bin.data(), bin.size() * sizeof(SB), false);
}

std::set<std::string> skipNames = {
    "Global Change",         "Signal Too Bright",
    "Signal Too Dark",       "Signal Too Blurry",
    "Signal Loss",           "Reference Image Check Failed",
    "Invalid Configuration", "Flame Detected",
    "Smoke Detected",        "Default Task"};

bool isNameSkipped(const std::string &&name) {

  for (const auto &s : skipNames) {
    if (name.length() >= s.length() && name.substr(0, s.length()) == s)
      return true;
  }
  return false;
}

std::string getStr() { return "Signal Loss 1"; }

struct B {
  virtual void bar() { std::cout << "B::bar\n"; }
  virtual ~B() = default;
};
struct D : B {
  D(const std::string &_name) : name(_name) { std::cout << name << " D::D\n"; }
  D(const D &d1) : name(d1.name + "cp") { std::cout << name << " D/D\n"; }
  D(const D &&d1) : name(std::move(d1.name)) { std::cout << name << " D<-D\n"; }
  ~D() { std::cout << name << " D::~D\n"; }
  void bar() override { std::cout << " D::bar\n"; }
  std::string name;
};

// a function consuming a unique_ptr can take it by value or by rvalue reference
std::unique_ptr<D> pass_through(std::unique_ptr<D> p) {
  p->bar();
  return p;
}

using D_PTR = std::unique_ptr<D>;
using D_VEC = std::vector<D_PTR>;
using CACHE_VEC = std::vector<D>;
using CACHE_MAP = std::map<std::string, D>;
using D_VEC_RPTR = D_VEC *;
using D_VEC_PTR = std::unique_ptr<D_VEC>;

// using ObjectPtr = std::unique_ptr<Object>;
// using ObjectList = std::vector<ObjectPtr>;
// using ObjectListRawPtr = ObjectList* ;
// using ObjectListPtr = std::unique_ptr<ObjectList>;

void parseRtpPacket(D_VEC_RPTR v, CACHE_VEC &c, CACHE_MAP m) {
  auto d1 = std::make_unique<D>("1");
  auto d2 = std::make_unique<D>("2");
  auto d3 = std::make_unique<D>("3");
  // cache
  m.insert(std::make_pair("1", *d1));
  m.insert(std::make_pair("2", *d2));
  m.insert(std::make_pair("3", *d3));
  c.push_back(*d1);
  c.push_back(*d2);
  c.push_back(*d3);
  //	  m["1"] = (*d1);
  //	  m["2"] = (*d2);
  //	  m["3"]= (*d3);
  v->push_back(std::move(d1));
  v->push_back(std::move(d2));
  v->push_back(std::move(d3));
  for (auto &p : *v)
    p->bar();
  std::cout << "nothing should be released. \n";
}

void parseRtpPacket2(D_VEC_RPTR v, CACHE_MAP &m) {
  auto d4 = std::make_unique<D>("4");
  auto d5 = std::make_unique<D>("5");
  //	  c["4"]= (*d4);
  //	  c["5"]= (*d5);
  m.insert(std::make_pair("4", *d4));
  m.insert(std::make_pair("5", *d5));
  v->push_back(std::move(d4));
  v->push_back(std::move(d5));
  for (auto &p : *v)
    p->bar();
  std::cout << "nothing should be released. \n";
}

void update(D_VEC_PTR v1) {
  for (auto &p : *v1)
    p->bar();
  D_VEC_PTR v2 = std::move(v1);
  std::cout << "original v should hold null\n";
  std::cout << "original v1 should hold null\n";
  assert(!v1); // now v1 owns nothing and holds a null pointer
  for (auto &p : *v2)
    p->bar();
  std::cout << "temp variable v2 is going to finish and thus held Ds\n";
}

void close_file(std::FILE *fp) { std::fclose(fp); }

struct Obj : public std::enable_shared_from_this<Obj> {
  std::shared_ptr<Obj> Get() { return shared_from_this(); }
  ~Obj() { std::cout << "Destructor called on " << this << std::endl; }
};

char buffer[1];

// int getSingleData(const char* oid, const char** data)
//{
//	const char* val = "1";
//	sprintf(buffer, "%x", val);
//    *data = buffer;
//    return 0;
//}

std::string utf8name(std::u16string name) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(name);
}

#define oidMaxPhaseGroups "1.3.6.1.4.1.1206.4.2.1.1.3.0"

#include <unordered_map>
struct param {
  const char *name;
  const char *path;
  const char *intf;
  const char *member;
  const char *signature;
};

enum class TARGET_DEVICE { ATC, IO_MODULE };

const std::unordered_map<TARGET_DEVICE, param> TARGET_PARAM_MAP = {
    {TARGET_DEVICE::IO_MODULE,
     {"com.siemens.c2x.io_module", "/com/siemens/c2x/io_module",
      "com.siemens.c2x.io_module.Service", "Command", "ss"}},
    {TARGET_DEVICE::ATC,
     {"com.siemens.c2x.atc_proxy", "/com/siemens/c2x/atc_proxy",
      "com.siemens.c2x.atc_proxy.Service", "Command", "ss"}}};

#include "Poco/PipeStream.h"
#include "Poco/Process.h"
#include "Poco/StreamCopier.h"
#include <fstream>
using Poco::Process;
using Poco::ProcessHandle;

#include <chrono>
#include <stdio.h> /* puts */
#include <time.h>  /* time_t, struct tm, time, localtime, strftime */
char *getFormatedTimestamp(bool onlyTime) {
  std::time_t rawtime;
  std::tm *timeinfo;
  static char buffer[80];

  memset(buffer, 0, 80);
  std::time(&rawtime);
  timeinfo = std::localtime(&rawtime);

  if (onlyTime) {
    std::strftime(buffer, 80, "%T", timeinfo);
  } else {
    std::strftime(buffer, 80, "%F %T", timeinfo);
  }
  return buffer;
}

char *getFormatedTimestamp(std::time_t time, bool onlyTime) {
  std::tm *timeinfo;
  static char buffer[80];

  memset(buffer, 0, 80);
  timeinfo = std::gmtime(&time);

  if (onlyTime) {
    std::strftime(buffer, 80, "%T", timeinfo);
  } else {
    std::strftime(buffer, 80, "%F %T", timeinfo);
  }
  return buffer;
}
const std::string SYS_DIR = "/tmp/ntcip1218/";
const std::string LOG_DIR = "logs/";

bool timeFromSnmp(std::string &snmpTimeHex, time_t &t) {
  if (snmpTimeHex.length() == 16) {
    auto bytes = decode(snmpTimeHex);
    if (bytes.size()) {
      std::tm timepoint;
      memset(&timepoint, 0, sizeof(timepoint));

      timepoint.tm_year =
          ((((uint16_t)bytes[0] << 8) & 0xFF00) | ((uint16_t)bytes[1] & 0xFF)) -
          1900;
      timepoint.tm_mon = bytes[2] - 1;
      timepoint.tm_mday = bytes[3];
      timepoint.tm_hour = bytes[4];
      timepoint.tm_min = bytes[5];
      timepoint.tm_sec = bytes[6];
      // timepoint.tm_sec = bytes[7]; //deci-sec???

      t = std::mktime(&timepoint);
      return true;
    }
  }

  return false;
}
static std::unique_ptr<Poco::Pipe> outPipePtr = std::make_unique<Poco::Pipe>();

char ipv4FormatBuffer[INET_ADDRSTRLEN + 8];
bool inet_net_pton_ipv4(const char *src, sockaddr_in *dst) {
  int a, b, c, d, p;
  if (strchr(src, ':')) {
    if (5 != sscanf(src, "%d.%d.%d.%d:%d", &a, &b, &c, &d, &p)) {
      return false;
    }
  } else {
    if (4 != sscanf(src, "%d.%d.%d.%d", &a, &b, &c, &d)) {
      return false;
    }
    p = 0;
  }
  if (a < 0 || a > 255)
    return false;
  if (b < 0 || b > 255)
    return false;
  if (c < 0 || c > 255)
    return false;
  if (d < 0 || d > 255)
    return false;
  if (p < 0 || d > 65535)
    return false;

  dst->sin_addr.s_addr = htonl(((a & 0xff) << 24) | ((b & 0xff) << 16) |
                               ((c & 0xff) << 8) | (d & 0xff));
  dst->sin_port = htons(p);
  return 1;
}

#include <arpa/inet.h>
int inet_net_pton_ipv6(const char *src, struct in6_addr *dst) {
  size_t strLen = strlen(src);
  if (strLen > (INET6_ADDRSTRLEN + 15))
    return -1;

  char parseBuffer[INET6_ADDRSTRLEN + 16];
  strncpy(parseBuffer, src, INET6_ADDRSTRLEN + 15);
  parseBuffer[strLen] = '\0';

  int bits = 128;
  char *prefix = strchr(parseBuffer, '/');
  if (prefix) {
    *prefix = '\0';
    bits = strtoul(prefix + 1, nullptr, 10);
  }
  if (1 != inet_pton(AF_INET6, parseBuffer, dst->s6_addr)) {
    bzero(dst, sizeof(struct in6_addr));
    return 0;
  }
  return bits;
}

char ipv6FormatBuffer[INET6_ADDRSTRLEN + 16];
char *inet_net_ntop_ipv6(const in6_addr *i6addr, int prefixLen, char *dst) {
  if (dst == nullptr) {
    dst = ipv6FormatBuffer;
  }
  ::bzero(dst, INET6_ADDRSTRLEN + 1);
  ::inet_ntop(AF_INET6, i6addr, dst, INET6_ADDRSTRLEN + 1);
  if (prefixLen > 0) {
    sprintf(dst + strlen(dst), "/%u", prefixLen); // Format CIDR /width.
  }
  return dst;
}

std::string formatIpV4(sockaddr_in m_sockaddr, bool withPort) {
  uint32_t myaddr = ntohl(m_sockaddr.sin_addr.s_addr);
  uint32_t myport = ntohs(m_sockaddr.sin_port);

  if (myaddr == 0) {
    ipv4FormatBuffer[0] = 0;
  } else {
    int i0 = (int)((myaddr & 0xff000000) >> 24);
    int i1 = (int)((myaddr & 0x00ff0000) >> 16);
    int i2 = (int)((myaddr & 0x0000ff00) >> 8);
    int i3 = (int)((myaddr & 0x000000ff));

    if (withPort)
      snprintf(ipv4FormatBuffer, INET_ADDRSTRLEN + 8, "%u.%u.%u.%u:%u", i0, i1,
               i2, i3, myport);
    else
      snprintf(ipv4FormatBuffer, INET_ADDRSTRLEN, "%u.%u.%u.%u", i0, i1, i2,
               i3);
  }
  return std::string(ipv4FormatBuffer);
}

#include "Poco/Net/SocketAddress.h"
int getNibble(uint32_t myaddr, int off) {
  switch (off) {
  case 1:
    return ((myaddr & 0xff000000) >> 24);
  case 2:
    return ((myaddr & 0x00ff0000) >> 16);
  case 3:
    return ((myaddr & 0x0000ff00) >> 8);
  case 4:
    return (myaddr & 0x000000ff);
  default:
    return 0;
  }
}

void onMsgRx(const uint8_t *data, const size_t len) {
  if ((data == nullptr) || (len == 0)) {
    return;
  }
  std::string trap(reinterpret_cast<const char *>(data), len);
  std::cout << trap << std::endl;
}
#include "utf8StringUtil.hpp"
using channel_number_mapping = std::unordered_map<uint16_t, uint8_t>;

class Event {
public:
  Event(){};
  virtual ~Event(){};
  virtual Poco::JSON::Object json() = 0;
  virtual std::string getName() = 0;
  virtual bool isInterestedEvent(const Poco::JSON::Array &arr);
};

class AlarmEvent : public Event {
public:
  AlarmEvent() = default;
  ~AlarmEvent() {}
  AlarmEvent(std::string _time, unsigned int _id, bool _state_flag,
             bool _delete_flag, bool _state_set_flag,
             bool _additional_info_flag, unsigned int _change_counter,
             std::u16string _name)
      : time(_time), id(_id), state_flag(_state_flag),
        delete_flag(_delete_flag), state_set_flag(_state_set_flag),
        additional_info_flag(_additional_info_flag),
        change_counter(_change_counter), name(_name) {}
  Poco::JSON::Object json() override;
  std::string getName() override;

private:
  std::string time{""};
  unsigned int id{0};
  bool state_flag{false};
  bool delete_flag{false};
  bool state_set_flag{false};
  bool additional_info_flag{false};
  unsigned int change_counter{0};
  std::u16string name;
};

class TaskEvent : public Event {
public:
  TaskEvent() = default;
  TaskEvent(std::string _name);
  ~TaskEvent() {}
  TaskEvent(unsigned int _id) : id(_id) {}
  Poco::JSON::Object json() override;
  std::string getName() override;

private:
  unsigned int id{0};
};

static constexpr const char *TASK_PREFIX = "task ";

bool Event::isInterestedEvent(const Poco::JSON::Array &arr) {
  auto str = getName();
  for (const auto &event : arr) {
    auto check_with = event.convert<std::string>();
    if (str.length() >= check_with.length() &&
        str.substr(0, check_with.length()) == check_with) {
      return true;
    }
  }
  return false;
}

TaskEvent::TaskEvent(std::string _name) {
  std::string prefix{TASK_PREFIX};
  if (_name.length() > prefix.length() &&
      _name.substr(0, prefix.length()) == prefix) {
    id = std::stoi(_name.substr(prefix.length()), nullptr, 10);
  }
}

std::string TaskEvent::getName() { return TASK_PREFIX + std::to_string(id); }

Poco::JSON::Object TaskEvent::json() {
  Poco::JSON::Object obj;
  obj.set("name", getName());
  return obj;
}

std::string AlarmEvent::getName() {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(name);
}

Poco::JSON::Object AlarmEvent::json() {
  Poco::JSON::Object obj;
  obj.set("time", time);
  obj.set("id", id);
  obj.set("state_flag", state_flag);
  obj.set("delete_flag", delete_flag);
  obj.set("state_set_flag", state_set_flag);
  obj.set("additional_info_flag", additional_info_flag);
  obj.set("change_counter", change_counter);
  obj.set("name", getName());
  return obj;
}

class AAA {
public:
  AAA() {
    m_wsaArr.fill(0x55);
    printArr();
  };
  void printArr() {
    std::cout << "printArr" << std::endl;
    for (auto &wsa : m_wsaArr) {
      std::cout << wsa << std::endl;
    }
  }

private:
  std::array<int, 16> m_wsaArr;
};

inline unsigned int toUnsignedInteger(std::string str) {

  return (str.length() > 0) ? strtoul(str.c_str(), 0, 0) : 0;
}

enum ItsChannel {
  CCH = 1,

  SCH1,
  SCH2,
  SCH3,
  SCH4,
  SCH5,
  SCH6,

  CH20_0,
  CH20_1,

  PC5 = 254,
  INVALID = 255,
};

enum AccessSlots { BOTH = 0, TIMESLOT0, TIMESLOT1, NOTUSED };

// static std::string getAccessSlot(AccessSlots access) {
//   auto SLOTS = {"continuous", "slot0", "slot1", "notused"};
//   return *(SLOTS.begin() + access);
// }

static constexpr std::size_t CCH_NUM{1};
static constexpr std::size_t SCH_NUM{6};

static constexpr std::size_t CHAN_COUNT{CCH_NUM + SCH_NUM};
static constexpr std::size_t CHAN_COUNT_20MHZ{2};

static const char *_channel_name[CHAN_COUNT + CHAN_COUNT_20MHZ] = {
    "CCH", "SCH1", "SCH2", "SCH3", "SCH4", "SCH5", "SCH6", "CH20_0", "CH20_1"};

static inline constexpr bool CHANNEL_VALID_20MHZ(std::size_t chan) {
  return chan < (CHAN_COUNT + CHAN_COUNT_20MHZ);
}

ItsChannel MAKE_CHANNEL_20MHZ(std::size_t chan) {
  return CHANNEL_VALID_20MHZ(chan) ? (ItsChannel)chan : ItsChannel::INVALID;
}

ItsChannel parseChannelText(const char *pszId) {
  if (pszId == nullptr)
    return ItsChannel::INVALID;

  size_t len = strlen(pszId);
  if (len < 3)
    return ItsChannel::INVALID;
  if (len > 6)
    return ItsChannel::INVALID;

  for (std::size_t i = 0; i < (CHAN_COUNT + CHAN_COUNT_20MHZ); ++i) {
    if (0 == strcasecmp(_channel_name[i], pszId)) {
      return MAKE_CHANNEL_20MHZ(i);
    }
  }
  return ItsChannel::INVALID;
}
#include <set>

enum LogByDir : uint32_t {
  INBOUNDONLY = 1,
  OUTBOUNDONLY,
  BISEPARATE,
  BICOMBINED
};

inline std::string &trim_begin(std::string &a, const char *d) {
  return a.erase(0, a.find_first_not_of(d, 0));
}

inline std::string &trim_end(std::string &a, const char *d) {
  return a.erase(a.find_last_not_of(d, std::string::npos) + 1,
                 std::string::npos);
}

inline std::string &trim(std::string &a, const char *d) {
  return trim_begin(trim_end(a, d), d);
}

enum TestV2IIntersectionStatusBit {
  MANUAL_CONTROL_ENABLE_ACTIVE = 0,
  STOP_TIME__ACTIVE = 1,
  FAULT_FLASH_ACTIVE = 2,
  PREEMPT_ACTIVE = 3,
  TSP_ACTIVE = 4,
  // COORDINATION_ACTIVE = 5,
  // COORDINATION_IN_TRANSITION = 6,
  // PROGRAMMED_FLASH_ACTIVE = 7,

  // for Omniair test only
  FIXED_TIME_OPERATION = 5,
  TRAFFIC_DEPENDENT_OPERATION = 6,
  STANDBY_OPERATION = 7,
  FAILURE_MODE = 8,
  INTERSECTION_STATUS_OBJECT__OFF = 9,
  RECENT_MAP_MESSAGE_UPDATE = 10,
  RECENT_CHANGE_IN_MAP_ASSIGNED_LANES_IDS_USED = 11,
  NO_VALID_MAP_IS_AVAILABLE_AT_THIS_TIME = 12,
  NO_VALID_SPAT_IS_AVAILABLE_AT_THIS_TIME = 13
};
/**
 * The rank of the named bit saemanualControlIsEnabled in the named bit table.
 */
enum { saemanualControlIsEnabled = 0 };
/**
 * The rank of the named bit saestopTimeIsActivated in the named bit table.
 */
enum { saestopTimeIsActivated = 1 };
/**
 * The rank of the named bit saefailureFlash in the named bit table.
 */
enum { saefailureFlash = 2 };
/**
 * The rank of the named bit saepreemptIsActive in the named bit table.
 */
enum { saepreemptIsActive = 3 };
/**
 * The rank of the named bit saesignalPriorityIsActive in the named bit table.
 */
enum { saesignalPriorityIsActive = 4 };
/**
 * The rank of the named bit saefixedTimeOperation in the named bit table.
 */
enum { saefixedTimeOperation = 5 };
/**
 * The rank of the named bit saetrafficDependentOperation in the named bit
 * table.
 */
enum { saetrafficDependentOperation = 6 };
/**
 * The rank of the named bit saestandbyOperation in the named bit table.
 */
enum { saestandbyOperation = 7 };
/**
 * The rank of the named bit saefailureMode in the named bit table.
 */
enum { saefailureMode = 8 };
/**
 * The rank of the named bit saeIntersectionStatusObject__off in the named bit
 * table.
 */
enum { saeIntersectionStatusObject__off = 9 };
/**
 * The rank of the named bit saerecentMAPmessageUpdate in the named bit table.
 */
enum { saerecentMAPmessageUpdate = 10 };
/**
 * The rank of the named bit saerecentChangeInMAPassignedLanesIDsUsed in the
 * named bit table.
 */
enum { saerecentChangeInMAPassignedLanesIDsUsed = 11 };
/**
 * The rank of the named bit saenoValidMAPisAvailableAtThisTime in the named bit
 * table.
 */
enum { saenoValidMAPisAvailableAtThisTime = 12 };
/**
 * The rank of the named bit saenoValidSPATisAvailableAtThisTime in the named
 * bit table.
 */
enum { saenoValidSPATisAvailableAtThisTime = 13 };

static constexpr const char *BasicSafetyMessageRequestLatitude = "latitude";
static constexpr const char *BasicSafetyMessageRequestLongitude = "longitude";
static constexpr const char *BasicSafetyMessageRequestElevation = "elevation";
static constexpr const char *BasicSafetyMessageRequestSpeed = "speed";
static constexpr const char *BasicSafetyMessageRequestHeading = "heading";

static constexpr const char *PedDetectionMessageRequest =
    "pedDetectionMessageRequest";
static constexpr const char *PedDetectionRequestType = "pedDetectionType";
static constexpr const char *PedDetectionRequestLatitude =
    BasicSafetyMessageRequestLatitude;
static constexpr const char *PedDetectionRequestLongitude =
    BasicSafetyMessageRequestLongitude;
static constexpr const char *PedDetectionRequestElevation =
    BasicSafetyMessageRequestElevation;
static constexpr const char *PedDetectionRequestHeading =
    BasicSafetyMessageRequestHeading;

enum PEDDECTIONYTYPE : int { DISABLED = 0, ENABLED, DETECTED };

using ObjectTaskIdMap =
    std::map<std::string, unsigned int>; // Object ID -> camera task ordinal
ObjectTaskIdMap m_obj_to_task_id;

std::string logObjectTaskIdMap() {
  for (const auto &it : m_obj_to_task_id) {
    std::cout << it.first << " ->  " << it.second << std::endl;
  }
  return "";
}
#include <Poco/Condition.h>
#include <Poco/Mutex.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>
#include <iostream>
#include <queue>

// class SharedQueue {
// public:
//     void push(int value) {
//         Poco::Mutex::ScopedLock lock(mutex_);
//         queue_.push(value);
//         condition_.signal();  // Notify the consumer that data is available
//     }
//
//     int pop() {
//         Poco::Mutex::ScopedLock lock(mutex_);
//         while (queue_.empty()) {
//             condition_.wait(mutex_);  // Wait until there is data
//         }
//         int value = queue_.front();
//         queue_.pop();
//         return value;
//     }
//
// private:
//     std::queue<int> queue_;
//     Poco::Mutex mutex_;
//     Poco::Condition condition_;
// };

class MyClass {
public:
  MyClass(int v) : value(v) {
    //    std::cout << "MyClass constructor called " << value << std::endl;
  }
  ~MyClass() {
    //    std::cout << "MyClass destructor called " << value << std::endl;
  }
  void show() { std::cout << "MyClass::show() " << value << std::endl; }
  int value;
};

class MyClass2 {
public:
  MyClass2(int v) : value(v) {
    //    std::cout << "MyClass2 constructor called " << value << std::endl;
  }
  ~MyClass2() {
    //    std::cout << "MyClass2 destructor called " << value << std::endl;
  }
  void show() {
    //	  std::cout << "MyClass2::show() " << value << std::endl;
  }
  int value;
};

struct CAMObject {
  static constexpr const int MAX_ID_LEN = 8;

  static constexpr const char *OBJECT_CLASS_PERSON_TYPE = "Pedestrian";
  static constexpr const char *OBJECT_CLASS_CAR_BIKE = "Bike";
  static constexpr const char *OBJECT_CLASS_BICYCLE =
      "Bicycle"; // xfer classification
  static constexpr const char *OBJECT_CLASS_CAR_TYPE = "Car";
  static constexpr const char *OBJECT_CLASS_TRUCK_TYPE = "Truck";
  CAMObject() {
    std::cout << "Constructor without iD " << iD << std::endl;
    //    	C2XAPP_WRN("Constructor without iD.");
  }
  CAMObject(std::string _iD) : iD(_iD) {
    std::cout << "Constructor " << iD << std::endl;
    //        C2XAPP_WRN("Constructor iD: %s ", iD.c_str());
  }
  CAMObject(std::string _time, std::string _type, std::string _iD)
      : time(_time), type(_type), iD(_iD) {
    std::cout << "Constructor " << iD << " and others" << std::endl;
  }
  CAMObject(const CAMObject &) = default;
  CAMObject(CAMObject &&) = default;
  ~CAMObject() {
    std::cout << "Destructor " << iD << " " << type << std::endl;
    //    	C2XAPP_WRN("Destructor: %s ", iD.c_str());
  }

  std::string time{""};
  std::string type{"1"};
  std::string iD{""}; // up to 8 chars, BSM id like Hex bytes
};

// template <typename T> class SharedQueue {
// public:
//   void push(T *ptr) {
//     Poco::Mutex::ScopedLock lock(mutex_);
//     queue_.push(ptr);
//     condition_.signal(); // Notify the consumer that data is available
//   }
//
//   // cannot return unique ptr
//   T *pop() {
//     Poco::Mutex::ScopedLock lock(mutex_);
//     while (queue_.empty()) {
//       condition_.wait(mutex_); // Wait until there is data
//     }
//
//     auto ptr = queue_.front();
//     //        ptr->show();
//     queue_.pop();
//     return ptr;
//   }
//
// private:
//   std::queue<T *> queue_;
//   Poco::Mutex mutex_;
//   Poco::Condition condition_;
// };

using ObjectPtr = std::unique_ptr<MyClass>;
using Object2Ptr = std::unique_ptr<MyClass2>;

class SharedTwoQueue {
public:
  bool push(ObjectPtr ptr) {
    Poco::Mutex::ScopedLock lock(mutex_);
    if (stop) {
      return false;
    }
    queue_.push(std::move(ptr));
    condition_.signal(); // Notify the consumer that data is available
    return true;
  }
  bool push(Object2Ptr ptr) {
    Poco::Mutex::ScopedLock lock(mutex_);
    if (stop) {
      return false;
    }
    queue_2.push(std::move(ptr));
    condition_.signal(); // Notify the consumer that data is available
    return true;
  }

  std::pair<ObjectPtr, Object2Ptr> pop() {
    Poco::Mutex::ScopedLock lock(mutex_);
    while (!stop && queue_.empty() && queue_2.empty()) {
      condition_.wait(mutex_); // Wait until there is data
    }
    ObjectPtr p1 = nullptr;
    Object2Ptr p2 = nullptr;

    if (!stop) {
      if (!queue_.empty()) {
        p1 = std::move(queue_.front());
        queue_.pop();
      }
      if (!queue_2.empty()) {
        p2 = std::move(queue_2.front());
        queue_2.pop();
      }
    }

    return std::make_pair(std::move(p1), std::move(p2));
  }

  void Stop() {
    Poco::Mutex::ScopedLock lock(mutex_);
    std::cout << "Stop" << std::endl;
    stop = true;
    condition_.signal();
  }

private:
  std::queue<ObjectPtr> queue_;
  std::queue<Object2Ptr> queue_2;
  Poco::Mutex mutex_;
  Poco::Condition condition_;
  bool stop{false};
};

// using ObjectPtrQueue = SharedQueue<MyClass>;

class Producer : public Poco::Runnable {
public:
  Producer(SharedTwoQueue &queue) : queue_(queue) {}

  void run() override {
    int i = 0;
    while (true) {
      std::cout << "Producing: " << i << std::endl;
      if (i % 2 == 0) {
        if (!queue_.push(std::make_unique<MyClass>(i))) {
          std::cout << "Producer to exit" << std::endl;
          break;
        };
      } else {
        if (!queue_.push(std::make_unique<MyClass2>(i))) {
          std::cout << "Producer to exit" << std::endl;
          break;
        };
      }
      Poco::Thread::sleep(40); // Simulate work
      ++i;
    }
  }

private:
  SharedTwoQueue &queue_;
};

ObjectPtr m_bsm = std::make_unique<MyClass>(0);
;

void callShow() {
  //	  std::cout << "callShow: " << std::endl;
  if (m_bsm)
    m_bsm->show();
  //	  std::cout << "callShow. " << std::endl;
}

void fill(ObjectPtr event) {
  //	  std::cout << "fill: " << std::endl;
  callShow();
  if (m_bsm)
    m_bsm->value = event->value;
  callShow();
  //	  std::cout << "fill. " << std::endl;
}

void recvEventFrom(ObjectPtr event) {
  std::cout << "recvEventFrom: " << std::endl;
  //  m_bsm = std::make_unique<MyClass>(0);
  fill(std::move(event));
  callShow();
}

void recvEventFrom(Object2Ptr event) {
  std::cout << "recvEventFrom: " << std::endl;
  event->show();
}

class Consumer : public Poco::Runnable {
public:
  Consumer(SharedTwoQueue &queue) : queue_(queue) {}

  void run() override {
    while (true) {
      auto res = queue_.pop();
      auto p1 = std::move(res.first);
      auto p2 = std::move(res.second);
      if (!p1 && !p2) {
        std::cout << "Consumer to exit" << std::endl;
        break;
      }
      if (p1) {
        //        ObjectPtr ptr = std::move();
        //        std::cout << "Consuming: " << ptr->value << std::endl;
        recvEventFrom(std::move(p1));
      }
      if (p2) {
        //        Object2Ptr ptr = std::move(res.second);
        //        std::cout << "Consuming: " << ptr->value << std::endl;
        recvEventFrom(std::move(p2));
      }
      Poco::Thread::sleep(40); // Simulate work
    }
  }

private:
  SharedTwoQueue &queue_;
};

void example() {
  std::unique_ptr<MyClass> ptr =
      std::make_unique<MyClass>(1); // Allocates an int
  // When 'ptr' goes out of scope at the end of this function, the int is
  // deleted
}
static constexpr const char *ENDPOINT_TYPE_MAP = "map";
static constexpr const char *ENDPOINT_TYPE_SPAT = "spat";
static constexpr const char *ENDPOINT_TYPE_TIM = "tim";
static constexpr const char *ENDPOINT_TYPE_PED_SIG = "pedsig";

static constexpr const char *ENDPOINT_TYPE_DEV_MAP = "dev_map";
static constexpr const char *ENDPOINT_TYPE_DEV_SPAT = "dev_spat";
static constexpr const char *ENDPOINT_TYPE_DEV_TIM = "dev_tim";
static constexpr const char *ENDPOINT_TYPE_DEV_PED_SIG = "dev_pedsig";

static constexpr const char *NULL_ENDPOINT = "no_sending";
static constexpr const char *INVALID_ENDPOINT = "invalid";

std::set<const char *> ENDPOINT_TYPES = {
    ENDPOINT_TYPE_MAP,     ENDPOINT_TYPE_SPAT,       ENDPOINT_TYPE_TIM,
    ENDPOINT_TYPE_PED_SIG, ENDPOINT_TYPE_DEV_MAP,    ENDPOINT_TYPE_DEV_SPAT,
    ENDPOINT_TYPE_DEV_TIM, ENDPOINT_TYPE_DEV_PED_SIG};

std::map<const char *, const char *> TO_DEV_ENDPOINTS = {
    {ENDPOINT_TYPE_MAP, ENDPOINT_TYPE_DEV_MAP},
    {ENDPOINT_TYPE_SPAT, ENDPOINT_TYPE_DEV_SPAT},
    {ENDPOINT_TYPE_TIM, ENDPOINT_TYPE_DEV_TIM},
    {ENDPOINT_TYPE_PED_SIG, ENDPOINT_TYPE_DEV_PED_SIG}};

bool isDev(const char *type) {
  if (!type) {
    return false;
  }
  for (const auto &mapping : TO_DEV_ENDPOINTS) {
    if (type == mapping.second) {
      return true;
    }
  }
  return false;
}

const char *getDev(const char *type) {
  if (!type) {
    return INVALID_ENDPOINT;
  }
  auto iter = TO_DEV_ENDPOINTS.find(type);
  if (iter != TO_DEV_ENDPOINTS.end()) {
    return iter->second;
  }
  return INVALID_ENDPOINT;
}

std::function<void(std::vector<std::string>)> m_parse_func{nullptr};
void SetParseDataFunction(std::function<void(std::vector<std::string>)> func) {
  m_parse_func = func;
};

class MyParse {
public:
  void parse_app_messages(std::vector<std::string> messages,
                          const std::string &message_type) {
    std::cout << message_type << std::endl;
  }
};

std::pair<bool, int> isDivideable(int i) {

  std::set<int, std::greater<int>> base{4, 5, 6, 7, 9};
  bool yes{false};
  int which{0};

  for (const auto &j : base) {
    if (i % j == 0) {
      yes = true;
      which = j;
      break;
    }
  }
  return std::make_pair(yes, which);
}

std::pair<int, int> tryAround(int i) {

  int inc = 0;
  while (true) {
    auto res = isDivideable(i + inc);
    if (res.first) {
      return std::make_pair(i + inc, res.second);
    }
    if (!inc) {
      res = isDivideable(i - inc);
      if (res.first) {
        return std::make_pair(i - inc, res.second);
      }
    }
    inc++;
    if (inc > 3) {
      return std::make_pair(0, 0);
    }
  }
}
//#include <iostream>
//#include <memory>
//
// class MyClass_S_ptr {
// private:
//    // Private constructor
//    MyClass_S_ptr() {
//        std::cout << "Private Constructor Called!" << std::endl;
//    }
//
// public:
//    // Static factory method to create a shared_ptr
//    static std::shared_ptr<MyClass_S_ptr> create() {
//        // Call the private constructor through std::shared_ptr
//        return std::make_shared<MyClass_S_ptr>();
//    }
//};
#define CHRONO_MS(x)                                                           \
  std::chrono::duration_cast<std::chrono::milliseconds>(x).count()

using task_queue = std::vector<int>;
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

// Producer: produces a vector of values
void producer(std::promise<task_queue> &&prom) {
  //  int i = 0;
  //  while (i++ < 3)
  {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate work

    task_queue data = {1, 2, 3, 4, 5}; // The produced data
    std::cout << "Producer: produced data" << std::endl;

    prom.set_value(data); // Set the produced data into the promise
  }
}

// Consumer: consumes a vector of values
void consumer(std::future<task_queue> &&fut, std::future<int> stop) {

  while (true) {
    if (stop.wait_for(std::chrono::milliseconds(100)) ==
        std::future_status::ready) {
      stop.get();
      std::cout << "Task completed" << std::endl;
      break;
    }
    std::cout << "Consumer: waiting for data..." << std::endl;

    task_queue data = fut.get(); // Get the produced data
    std::cout << "Consumer: consumed data: ";
    for (int value : data) {
      std::cout << value << " ";
    }
    std::cout << std::endl;
  }
}

#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/StringTokenizer.h>

void updateMember(const std::string &token, double &member) {
  member = 0.0;
  try {
    member = std::stod(token);
    // std::cout << std::setprecision(9) << member << std::endl;
  } catch (const std::invalid_argument &e) {
    std::cerr << "Invalid number: " << token << std::endl;
  } catch (const std::out_of_range &e) {
    std::cerr << "Number out of range: " << token << std::endl;
  }
}

double convertMphToMps(double mph) { return mph * 0.44704; }

const std::string EAR_ZONE = "EAR";
const std::string EPER_ZONE = "EPER";
const std::string EXR_ZONE = "EXR";
const std::string EISLR_ZONE = "EISLR";
const std::string P_EISLR_ZONE = "EPISLR";

const std::string WAR_ZONE = "WAR";
const std::string WPER_ZONE = "WPER";
const std::string WXR_ZONE = "WXR";
const std::string WISLR_ZONE = "WISLR";
const std::string P_WISLR_ZONE = "WPISLR";

class BusState {
  enum _State { IDLE = 1, EAR, EPER, EXR, WAR, WPER, WXR } m_state{IDLE};
  std::string stateName();
  bool isIdle() { return m_state == IDLE; }
  void onEnter(_State to);

  std::string m_enter_into{""};
  std::string m_current_zone{""};

  bool isInsideZone(std::string zone_name) { return m_enter_into == zone_name; }
  bool notFoundAny() { return m_enter_into.empty(); }
  bool isEnoughSpeedSameZone() { return m_current_zone == m_enter_into; }
  bool isSpeedLost() { return !isIdle() && notFoundAny(); }
  bool isPossibleWestOverlap() { return m_enter_into == "WXR"; }

  void turnOnChannels();
  void turnOffChannels();

  static const std::multimap<std::string, int>& getZoneChannelMap()
  {
      // not include EPBR and WPBR
      static const std::multimap<std::string, int> map = {{EAR_ZONE, 1}, {EPER_ZONE, 1},
          {EPER_ZONE, 2}, {EXR_ZONE, 1}, {EXR_ZONE, 2}, {EXR_ZONE, 3}, {EISLR_ZONE, 4},
          {WAR_ZONE, 6}, {WPER_ZONE, 6}, {WPER_ZONE, 7}, {WXR_ZONE, 6}, {WXR_ZONE, 7},
          {WXR_ZONE, 8}, {WISLR_ZONE, 9}};
      return map;
  }

  static constexpr const int MAX_COUND_DOWN_SEC = 3;
  int m_leaving_countdown;

public:
  std::set<int> getChannelsToSet(std::string zone);
  void enter(std::string zone) { m_enter_into = zone; }
  void changeState();
  bool isPossibleEastOverlap() { return m_enter_into == "EXR"; }
};

void BusState::changeState() {
  if (isSpeedLost()) {
    turnOffChannels();
  } else if (isEnoughSpeedSameZone()) {
    turnOnChannels();
    if (m_current_zone == "EXR" || m_current_zone == "WXR") {
      m_leaving_countdown = MAX_COUND_DOWN_SEC;
    }
  }
  if (m_current_zone.empty() && isIdle()) {
    if (isInsideZone("EAR")) {
      onEnter(EAR);
      m_current_zone = "EAR";
    }
    if (isInsideZone("WAR")) {
      onEnter(WAR);
      m_current_zone = "WAR";
    }
  }
  if (m_current_zone == "EAR" && isInsideZone("EPER")) {
    onEnter(EPER);
    m_current_zone = "EPER";
  }
  if (m_current_zone == "WAR" && isInsideZone("WPER")) {
    onEnter(WPER);
    m_current_zone = "WPER";
  }
  if (m_current_zone == "EPER" && isInsideZone("EXR")) {
    onEnter(EXR);
    m_current_zone = "EXR";
    m_leaving_countdown = MAX_COUND_DOWN_SEC;
  }
  if (m_current_zone == "WPER" && isInsideZone("WXR")) {
    onEnter(WXR);
    m_current_zone = "WXR";
    m_leaving_countdown = MAX_COUND_DOWN_SEC;
  }
  if (m_current_zone == "EXR" && notFoundAny()) {
    if (m_leaving_countdown-- == 0) {
      onEnter(IDLE);
      m_current_zone = "IDLE";
      std::cout << "east direction bus disappears. " << std::endl;
    } else {
      std::cout << "leaving: " << m_leaving_countdown << std::endl;
    }
  }
  if (m_current_zone == "WXR" && notFoundAny()) {
    if (m_leaving_countdown-- == 0) {
      onEnter(IDLE);
      m_current_zone = "IDLE";
      std::cout << "west direction bus disappears. " << std::endl;
    } else {
      std::cout << "leaving: " << m_leaving_countdown << std::endl;
    }
  }
}

void BusState::turnOnChannels() {
  for (const auto &chan : getChannelsToSet(stateName())) {
    std::cout << "turn on channel: " << chan << std::endl;
  }
}

void BusState::turnOffChannels() {
  for (const auto &chan : getChannelsToSet(stateName())) {
    std::cout << "turn off channel: " << chan << std::endl;
  }
}

void BusState::onEnter(_State to) {
  //  std::cout << "before: " << stateName() << std::endl;
  m_state = to;
  //  std::cout << "after: " << stateName() << std::endl;
  turnOnChannels();
}

std::string BusState::stateName() {
  if (isIdle()) {
    return "IDLE";
  }
  if (m_state == EAR) {
    return "EAR";
  }
  if (m_state == EPER) {
    return "EPER";
  }
  if (m_state == EXR) {
    return "EXR";
  }
  if (m_state == WAR) {
    return "WAR";
  }
  if (m_state == WPER) {
    return "WPER";
  }
  if (m_state == WXR) {
    return "WXR";
  }
  return "BAD";
}

bool includes(Poco::JSON::Array &arr, int value) {
  auto found = false;
  for (const auto &item : arr) {
    if (item.convert<int>() == value) {
      found = true;
      break;
    }
  }
  return found;
}

// pre-defined devices
constexpr const char *IO_DEVICE_1_NAME = "device_1";
constexpr const char *IO_DEVICE_2_NAME = "device_2";
constexpr const char *IO_DEVICE_3_NAME = "device_3";
constexpr const char *IO_DEVICE_4_NAME = "device_4";

static const std::set<std::string> defined_devices{
    IO_DEVICE_1_NAME, IO_DEVICE_2_NAME, IO_DEVICE_3_NAME, IO_DEVICE_4_NAME};

void removeAllTaskPrefix(Poco::JSON::Array &jsonArray) {
  std::string check_with{"task"};
  for (int i = jsonArray.size() - 1; i >= 0; --i) {
    auto to_check = jsonArray.getElement<std::string>(i);
    if (to_check.length() >= check_with.length() &&
        to_check.substr(0, check_with.length()) == check_with) {
      jsonArray.remove(i);
    }
  }
}
#include <iostream>
#include <list>
#include <memory>

class Number {
public:
  explicit Number(int val) : value(val) {}
  void show() const { std::cout << "Number: " << value << std::endl; }
  ~Number() { std::cout << "destory: " << value << std::endl; }

private:
  int value;
};

std::vector<std::string> split(const std::string &a, const char *d) {
  std::vector<std::string> tokens;

  std::string::size_type pos = a.find_first_not_of(d, 0);
  std::string::size_type end{0};
  while ((end = a.find_first_of(d, pos)) != std::string::npos) {
    tokens.push_back(a.substr(pos, end - pos));
    pos = a.find_first_not_of(d, end);
  }
  if (pos != std::string::npos) {
    tokens.push_back(a.substr(pos, std::string::npos));
  }
  return tokens;
}

template <typename T, typename = std::enable_if_t<
                          std::is_same_v<T, Poco::JSON::Object::Ptr> ||
                          std::is_same_v<T, Poco::JSON::Array::Ptr>>>
std::string getStringOf(T ptr) {
  {
    std::ostringstream os;
    ptr->stringify(os, 0);
    return os.str();
  }
  return "";
}

void getArray(const Poco::JSON::Object &conf, const char *json_key,
              Poco::JSON::Array::Ptr &arr_ptr, std::string &arr_str,
              bool clear_arr_str = false) {
  if (clear_arr_str) {
    arr_str = "[]";
  }
  if (!conf.has(json_key)) {
    return;
  }
  try {
    arr_ptr = conf.getArray(json_key);
    arr_str = getStringOf(arr_ptr);
  } catch (const std::exception &ex) {
    std::cout << "exception : %s" << ex.what();
  }
}

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

std::time_t getFromFormatedTimestamp(const std::string &timeStr,
                                     bool onlyTime = true) {
  std::istringstream ss(timeStr);
  std::tm tm = {};

  if (!onlyTime) {
    // Parse time (Y-m-d HH:MM:SS)
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
      std::cerr << "Failed to parse date time\n";
    }
  } else {
    // Get current UTC date
    std::time_t now = std::time(nullptr);
    tm = *std::gmtime(&now);

    std::tm timeOnly = {};
    // Parse time (HH:MM:SS)
    ss >> std::get_time(&timeOnly, "%H:%M:%S");
    if (!ss.fail()) {
      // Combine today's date (UTC) with parsed time
      tm.tm_hour = timeOnly.tm_hour;
      tm.tm_min = timeOnly.tm_min;
      tm.tm_sec = timeOnly.tm_sec;
    } else {
      std::cerr << "Failed to parse time\n";
    }
  }

  // Convert to time_t assuming UTC
  std::time_t result = timegm(&tm);
  if (result == -1) {
    std::cerr << "timegm failed\n";
  }

  return result;
}

std::set<int> BusState::getChannelsToSet(std::string zone)
{
    std::set<int> res;
    const auto& map = getZoneChannelMap();
//    std::cout << "to_turn_off channel:" << map.size()<< "\n";
    auto msg_range = map.equal_range(zone);

//    std::cout << std::distance(msg_range.first, msg_range.second) << "\n";
    for (auto msg_it = msg_range.first; msg_it != msg_range.second; ++msg_it)
    {
    	std::cout << msg_it->second  << "\n";
        res.insert(msg_it->second);
    }
    return res;
}

std::vector<std::string> getAllRealZones()
{
    std::vector<std::string> res;
    Poco::JSON::Object::Ptr map_json{nullptr};
    try
    {
        static constexpr const char* ZoneChannelMap = R"({
            "EAR": [1],
            "EPER": [1, 2],
            "EXR": [1, 2, 3],
            "EISLR": [4]
        })";
        Poco::JSON::Parser parser;
        map_json = parser.parse(ZoneChannelMap).extract<Poco::JSON::Object::Ptr>();

        res.reserve(map_json->size());

        for (auto it = map_json->begin(); it != map_json->end(); ++it)
        {
        	std::cout << "zone:" << it->first << "\n";
            res.emplace_back(it->first); // key = zone name
        }
    }
    catch (const std::exception& ex)
    {
        return res;
    }
    return res;
}

void tryCode() {

	getAllRealZones();
//	    BusState east_state, west_state;
//
//    std::set<int> to_turn_off;
//
//    to_turn_off = west_state.getChannelsToSet(WXR_ZONE);
//    for (const auto channel : to_turn_off)
//    {
//    	std::cout << "to_turn_off channel:" << channel << "\n";
//    }
//    auto islr_off = west_state.getChannelsToSet(WISLR_ZONE);
//    for (const auto channel : islr_off)
//    {
//    	std::cout << "islr_off channel:" << channel << "\n";
//    }
//    for (const auto channel : islr_off)
//    {
//        to_turn_off.emplace(channel);
//    }
//    for (const auto channel : to_turn_off)
//    {
//    	std::cout << "channel:" << channel << "\n";
//    }
//    return;

//  std::ifstream log_file("history_requests.log");
//  std::string content;
//
//  while (std::getline(log_file, content)) {
//    std::cout << "aaa:" << content << "\n";
//    Poco::JSON::Parser parser;
//    try {
//      auto json = parser.parse(content).extract<Poco::JSON::Object::Ptr>();
//      if (json->has("request_id")) {
//        std::cout << "request_id " << json->getValue<int>("request_id") << "\n";
//      }
//    } catch (const std::exception &ex) {
//    	std::cout << ex.what()<< "\n";
//    }
//  }

//  std::string content =
//      R"({"request_id":17,"type":"Priority","action":"closedStrategyError","grant":"PENDING","in_lane":10,"out_lane":7,"start_time":"2025-06-06 21:56:38","duration":5,"original_eta":24})";
//  Poco::JSON::Parser parser;
//  try {
//    auto json = parser.parse(content).extract<Poco::JSON::Object::Ptr>();
//    if (json->has("request_id")) {
//      std::cout << "request_id " << json->getValue<int>("request_id") << "\n";
//    }
//  } catch (const std::exception &ex) {
//  }
  //  bool onlyTime = false;
  //  std::string timeStr = "2024-06-04 14:23:45";
  //    bool onlyTime = true;
  //    std::string timeStr = "14:23:45";
  //
  //  std::cout << "UTC time_t: " << getFromFormatedTimestamp(timeStr, onlyTime)
  //            << "\n";

  //	constexpr const char* VIRTUAL_CAMERA_NUMBER = "virtual_camera_number";
  //	constexpr const char* ZONE_TRIGGER_NUMBER = "number";
  //	constexpr const char* ZONENAMES = "zones";
  //
  //	std::string virtual_camera_zones =
  // R"([{"virtual_camera_number":7,"zones":["beta"]}])"; Poco::JSON::Array
  // message_xmit_zones_conf;
  //    Poco::JSON::Parser parser;
  //    auto conf_arr =
  //    parser.parse(virtual_camera_zones).extract<Poco::JSON::Array::Ptr>();
  //    for (std::size_t i = 0; i < conf_arr->size(); i++)
  //            {
  //                auto json_ptr = conf_arr->getObject(i);
  //                if (!json_ptr->has(VIRTUAL_CAMERA_NUMBER) ||
  //                !json_ptr->has(ZONENAMES))
  //                {
  // break;
  //                }
  //                Poco::JSON::Object statusObj;
  //                auto number =
  //                json_ptr->getValue<int>(VIRTUAL_CAMERA_NUMBER); auto zones =
  //                json_ptr->getArray(ZONENAMES); std::cout << number <<
  //                std::endl; std::cout << getStringOf(zones) << std::endl;
  //                    statusObj.set(ZONE_TRIGGER_NUMBER, number);
  //                    statusObj.set(ZONENAMES, zones);
  //                message_xmit_zones_conf.add(statusObj);
  //            }
  //
  //    std::ostringstream os;
  //    message_xmit_zones_conf.stringify(os, 0);
  //    std::cout << os.str() << std::endl;
//  return;

  //    Poco::JSON::Parser parser;
  //    auto on_set = parser.parse("[]").extract<Poco::JSON::Array::Ptr>();
  //    parser.reset();
  //    auto off_set = parser.parse("[]").extract<Poco::JSON::Array::Ptr>();
  //
  //    std::cout << getStringOf(on_set) << std::endl;
  //    std::cout << getStringOf(off_set) << std::endl;
  //
  //    std::string dummy = R"({"on":[1,7]})";
  //    parser.reset();
  //    auto conf = parser.parse(dummy).extract<Poco::JSON::Object::Ptr>();
  //
  //    getArray(*conf,"on",on_set,dummy);
  //    std::cout << getStringOf(on_set) << std::endl;
  //    std::cout << dummy << std::endl;
  //
  //	return;
  //
  //	constexpr const double DEFAULT_LATITUDE = -90.000000;
  //	std::cout << std::to_string(DEFAULT_LATITUDE) << std::endl;
  //	auto b = std::to_string(DEFAULT_LATITUDE) == "-90.000000";
  //	std::cout << b << std::endl;

  //	auto str1 =
  // R"([{"channel":182,"enable":1,"index":2,"options":"80","payload":"FE","priority":6,"psid":"e0000017"}])";
  //	Poco::JSON::Parser parser;
  //    auto m_special_entry_arr =
  //    parser.parse(str1).extract<Poco::JSON::Array::Ptr>();
  //
  //    std::cout << getStringOf(m_special_entry_arr) << std::endl;
  //	        // normal entry
  //	        for (const auto& special_entry : *m_special_entry_arr)
  //	        {
  //	            auto ptr = special_entry.extract<Poco::JSON::Object::Ptr>();
  //	            if(ptr->has("index")){
  //	            std::cout << ptr->getValue<int>("index") << std::endl;
  //	            }
  //
  //	        }
  //	std::string device  = "device_2";
  //	std::string do_or_di = "DI";
  //	std::string scope =  device + "$" + do_or_di;
  //	std::cout <<scope<< std::endl;
  //
  //	const char* delims = "$";
  //
  //	auto result = split(scope, delims);
  //	std::cout << result.size() << std::endl;
  //    if(result.size() < 2){
  //    	return;
  //    }
  //	  for (const auto &it : result) {
  //	    std::cout << it << std::endl;
  //	  }

  //	int num = 12345;
  //	printf("Padded Hex: 0X%04x\n", num);
  //
  //	constexpr const char* IO_INPUT_DEVICE_1_TYPE_NAME = "IOInputDevice1";
  //	constexpr const char* IO_INPUT_DEVICE_2_TYPE_NAME = "IOInputDevice2";
  //	constexpr const char* IO_INPUT_DEVICE_3_TYPE_NAME = "IOInputDevice3";
  //	constexpr const char* IO_INPUT_DEVICE_4_TYPE_NAME = "IOInputDevice4";
  //	constexpr const char* IO_DEVICE_1_NAME = "device_1";
  //	constexpr const char* IO_DEVICE_2_NAME = "device_2";
  //	constexpr const char* IO_DEVICE_3_NAME = "device_3";
  //	constexpr const char* IO_DEVICE_4_NAME = "device_4";
  //
  //    const std::map<std::string, std::string> DEVICE_IO_TYPE_NAME_MAP = {
  //        {IO_INPUT_DEVICE_1_TYPE_NAME, IO_DEVICE_1_NAME},
  //        {IO_INPUT_DEVICE_2_TYPE_NAME, IO_DEVICE_2_NAME},
  //        {IO_INPUT_DEVICE_3_TYPE_NAME, IO_DEVICE_3_NAME},
  //        {IO_INPUT_DEVICE_4_TYPE_NAME, IO_DEVICE_4_NAME}};
  //    for (const auto type : {IO_INPUT_DEVICE_1_TYPE_NAME,
  //    IO_INPUT_DEVICE_2_TYPE_NAME,
  //             IO_INPUT_DEVICE_3_TYPE_NAME, IO_INPUT_DEVICE_4_TYPE_NAME})
  //    {
  //    	std::cout << DEVICE_IO_TYPE_NAME_MAP.at(type)<< std::endl;
  //    }

  // Create a list of unique_ptr<Number>

  //	using TaskEventPtr = std::unique_ptr<Number>;
  //	using TaskEventList = std::vector<TaskEventPtr>;
  //	using TaskEventListPtr = std::unique_ptr<TaskEventList>;
  //
  //	TaskEventListPtr task_event_list = std::make_unique<TaskEventList>();
  //
  //    task_event_list->emplace_back(std::make_unique<Number>(10));
  //    task_event_list->emplace_back(std::make_unique<Number>(20));
  //    task_event_list->emplace_back(std::make_unique<Number>(30));
  //
  //    std::cout << "Before moving an element:\n";
  //    for (const auto& numPtr : *task_event_list) {
  //        if (numPtr) numPtr->show();
  //    }
  //
  //    while(task_event_list->size() != 0){
  //    // Move the first element out of the list
  //    TaskEventPtr extracted = std::move(task_event_list->back());
  //    task_event_list->pop_back(); // Remove the moved-out (nullptr) element
  //    from the list extracted->show(); // Successfully transferred ownership
  //    }
  //
  //    std::cout << "\nAfter moving an element out of the list:\n";
  //    for (const auto& numPtr : *task_event_list) {
  //        if (numPtr) numPtr->show();
  //        else std::cout << "Moved-out element (nullptr)\n";
  //    }
  //    if (task_event_list.get() != nullptr){
  //    	std::cout << "task_event_list Pointer is valid." << std::endl;
  //    }
  //    task_event_list.reset();  // Cleanup
  //
  //       if (!task_event_list) {
  //           std::cout << "Pointer has been cleaned up." << std::endl;
  //       }
  ////    std::cout << "\nExtracted element after move:\n";
  ////    extracted->show(); // Successfully transferred ownership
  //
  //    return ;

  //	Poco::JSON::Array jsonArray;
  //	jsonArray.add("task1");
  //	jsonArray.add("wwe");
  //	jsonArray.add("task3");
  //	jsonArray.add("ppp");
  //
  //	  std::stringstream ss;
  //	  jsonArray.stringify(ss, 2); // Pretty-print with indentation
  //
  //	  std::cout << ss.str() << std::endl;
  //
  //	  removeAllTaskPrefix(jsonArray);
  //	  std::stringstream ss2;
  //	  jsonArray.stringify(ss2, 2); // Pretty-print with indentation
  //
  //	  std::cout << ss2.str() << std::endl;

  //	std::array<std::string, 4> arr = {IO_DEVICE_1_NAME, IO_DEVICE_2_NAME,
  // IO_DEVICE_3_NAME, IO_DEVICE_4_NAME}; 	std::cout << arr[0] <<
  // std::endl; 	std::cout << arr[1] << std::endl; 	std::cout << arr[2]
  // <<
  // std::endl; 	std::cout << arr[3] << std::endl;
  //    Poco::JSON::Array on;
  //    on.add(1);
  //    on.add(2);
  //    on.add(3);
  //
  //    auto value = 2;
  //    if(includes(on, value)){
  //    	std::cout << "found" << std::endl;
  //    }
  //    else{
  //    	std::cout << "not found" << std::endl;
  //    }
  //  Poco::JSON::Array jsonArray;

  //  Poco::JSON::Object::Ptr obj1 = new Poco::JSON::Object;
  //  obj1->set("1", "EAR");
  //
  //  Poco::JSON::Object::Ptr obj2 = new Poco::JSON::Object;
  //  obj2->set("2", "EPER");
  //
  //  jsonArray.add(obj1);
  //  jsonArray.add(obj2);
  //
  //  std::stringstream ss;
  //  jsonArray.stringify(ss, 2); // Pretty-print with indentation
  //
  //  std::cout << ss.str() << std::endl;
  //
  //  for (const auto &it : jsonArray) {
  //    auto rule = it.extract<Poco::JSON::Object::Ptr>();
  //
  //    if (rule->has("1")) {
  //      std::cout << rule->getValue<std::string>("1") << std::endl;
  //    }
  //    if (rule->has("2")) {
  //      std::cout << rule->getValue<std::string>("2") << std::endl;
  //    }
  //  }

  //  std::vector<std::string> east = {
  //      "EAR",  "EAR", "EAR", "",     "EAR",  "EPER", "EPER", "EPER",
  //      "EPER", "",    "",    "EPER", "EPER", "",     "",     "EPER",
  //      "EXR",  "",    "EXR", "EXR",  "EXR",  "EXR",  "EXR",  "EXR",
  //      "EXR",  "",    "",    "",     "",     ""};
  //  std::vector<std::string> west = {
  //      "WAR", "WAR", "",    "",    "WPER", "WPER", "WPER", "WPER", "WXR",
  //      "",    "",    "WXR", "WXR", "",     "",     "WXR",  "WXR",  "WXR",
  //      "WXR", "WXR", "",    "",    "",     "",     ""};
  //
  //
  //  int east_start = 0;
  //  int east_cnt = 0;
  //  int west_start = 0;
  //  int west_cnt = 0;

  //  auto it1 = east.begin();
  //  auto it2 = west.begin();
  //  while (true) {
  //    if (it1 == east.end() && it2 == west.end()) {
  //      break;
  //    }
  //    if (it1 != east.end()) {
  //      if (east_cnt++ < east_start) {
  //        continue;
  //      } else {
  //        std::cout << "east direction, passing zone: " << *it1 << std::endl;
  //        east_state.enter(*it1);
  //        east_state.changeState();
  //      }
  //      ++it1;
  //    }
  //    if (it2 != west.end()) {
  //      if (west_cnt++ < west_start) {
  //        continue;
  //      } else {
  //        std::cout << "west direction, passing zone: " << *it2 << std::endl;
  //        west_state.enter(*it2);
  //        west_state.changeState();
  //      }
  //      ++it2;
  //    }
  //  }
  //
  //  std::cout << "finish 1." << std::endl;

  //  auto it1 = east.begin();
  //  auto it2 = west.begin();
  //  while (true) {
  //    if (it1 == east.end() && it2 == west.end()) {
  //      break;
  //    }
  //    if (it1 != east.end()) {
  //      if (east_cnt++ < east_start) {
  //        continue;
  //      } else {
  //        std::cout << "east direction, passing zone: " << *it1 << std::endl;
  //        east_state.enter(*it1);
  //        east_state.changeState();
  //        if (east_state.isPossibleEastOverlap()) {
  //          east_state.enter("WXR");  // simulate another bsm in the zone is
  //                                    // received from west direction
  //          east_state.changeState(); // no effect shall be seen, and
  //          m_enter_into
  //                                    // is not changed in this function
  //          east_state.enter("EXR");  // just reset m_enter_into
  //        }
  //      }
  //      ++it1;
  //    }
  //    if (it2 != west.end()) {
  //      if (west_cnt++ < west_start) {
  //        continue;
  //      } else {
  //        std::cout << "west direction, passing zone: " << *it2 << std::endl;
  //        west_state.enter(*it2);
  //        west_state.changeState();
  //      }
  //      ++it2;
  //    }
  //  }
  //
  //  std::cout << "finish 2." << std::endl;

  //  Poco::File ff("bsm_input_1.csv");
  //  if (!ff.exists()) {
  //    std::cout << "not exist bsm_input_1.csv" << std::endl;
  //  } else {
  //    std::cout << "size : " << ff.getSize() << std::endl;
  //  }
  //
  //  Poco::FileInputStream fis("bsm_input_1.csv");
  //  if (!fis) {
  //    std::cerr << "Failed to open file: "
  //              << "bsm_input_1.csv" << std::endl;
  //    return;
  //  }
  //
  //  std::ifstream file("bsm_input_1.csv");
  //  std::string content;
  //
  //  Poco::FileOutputStream fos("output_bsm.txt", std::ios::app);
  //
  //  if (!fos.good()) {
  //    std::cerr << "Failed to open file for appending!" << std::endl;
  //    return;
  //  }
  //
  //  while (file >> content) {
  //    // std::cout << content << std::endl;
  //    Poco::StringTokenizer tokenizer(content, ",",
  //                                    Poco::StringTokenizer::TOK_TRIM);
  //    double lat{0.0};
  //    double lon{0.0};
  //    double elevation{0.0};
  //    double heading{0.0};
  //
  //    updateMember(tokenizer[0], lat);
  //    updateMember(tokenizer[1], lon);
  //    updateMember(tokenizer[2], heading);
  //    updateMember(tokenizer[3], elevation); // speed
  //
  //    std::cout << std::setprecision(11) << lat << " " << lon << " " <<
  //    heading
  //              << " " << convertMphToMps(elevation) << std::endl;
  //
  //    fos << lat << std::setprecision(11) << " " << lon << " " << heading << "
  //    "
  //        << convertMphToMps(elevation) << std::endl;
  //  }
  //  // Flush and close the stream
  //  fos.flush();
  //  fos.close();

  //  Poco::JSON::Array::Ptr jsonArray = new Poco::JSON::Array;
  //  jsonArray->add(10);
  //  jsonArray->add(20);
  //  jsonArray->add(30);
  //  jsonArray->add(40);
  //  for (const auto &item : *jsonArray) {
  //    std::cout << item.convert<int>() << std::endl;
  //  }
  //  // auto var = ;
  //  std::cout << jsonArray->get(0).convert<int>() << std::endl;
  //
  //  Poco::JSON::Object obj1;
  //  obj1.set("bsm_xmit", 11);
  //  Poco::JSON::Object obj2;
  //  obj2.set("psm_xmit", 22);
  //  Poco::JSON::Object res;
  //  for (const auto &key : obj1) {
  //    res.set(key.first, key.second);
  //  }
  //  for (const auto &key : obj2) {
  //    res.set(key.first, key.second);
  //  }
  //  std::ostringstream os;
  //  res.stringify(os, 0);
  //  std::cout << os.str() << std::endl;

  //	constexpr const uint32_t SNMP_GROUP_SIZE = 8;
  //
  //	auto phase = 8;
  //
  //	std::cout << (phase + SNMP_GROUP_SIZE - 1) / SNMP_GROUP_SIZE <<
  // std::endl;

  //	std::string ss{""};
  //	std::cout << encode(ss) << std::endl;

  //  // Create a promise and future for a vector of integers
  //  std::promise<task_queue> prom;
  //  std::future<task_queue> fut = prom.get_future();
  //  std::promise<int> prom2;
  //  std::future<int> fut2 = prom2.get_future();
  //
  //  // Launch producer and consumer threads
  //  std::thread producerThread(producer,
  //                             std::move(prom)); // Producer produces data
  //  std::thread consumerThread(consumer, std::move(fut),
  //                             std::move(fut2)); // Consumer waits for data
  //
  //  Poco::Thread::sleep(300); // Simulate work
  //  std::cout << "Main Stop" << std::endl;
  //  prom2.set_value(1);
  //
  //  // Join threads
  //  producerThread.join();
  //  consumerThread.join();

  //  return;

  //	 auto now1 = std::chrono::system_clock::now();
  //	    auto now1 =
  //	        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
  //
  //	 auto now_cnt = CHRONO_MS(now1.time_since_epoch());
  //	 std::cout << now_cnt<< std::endl;
  //	 printf("%llu",now_cnt);
  //    std::shared_ptr<MyClass_S_ptr> obj = MyClass_S_ptr::create();  // Works!

  //  for (int i = 4; i <= 600; i++) {
  //
  //    auto around = tryAround(i);
  //    if (around.second == 0) {
  //      std::cout << i << " not found" << std::endl;
  //    } else {
  //      std::cout << i << " " << around.first << " " << around.second
  //                << " inc: " << around.first - i << std::endl;
  //      if(around.first - i ==3 || around.first - i == -3){
  //    	  std::cout << i << " reach biggest offset"<< std::endl;
  //      }
  //    }
  //  }
  //  return;

  //	  using CAMObjectPtr = std::unique_ptr<CAMObject>;
  //
  //	{
  //		 std::string objectId = "zzz";
  //		 auto utd = std::make_unique<CAMObject>(objectId);
  //
  //		 int a = 4;
  //
  //		 if(a == 4){
  //			 return;
  //		 }
  //		 auto ptr = utd.release();
  //		 CAMObjectPtr td4(ptr);
  //		     std::cout << td4.get() << td4->iD << " " << td4->type <<
  // std::endl; 		     return;
  //
  //	}

  //	double lat = 35.7807584;
  //	printf("%f", lat);

  //  using ObjectListCache = std::map<std::string, std::pair<CAMObject,
  //  uint64_t>>; using ObjectListCache = std::map<std::string, uint64_t>;
  //  ObjectListCache cachedObjectList;
  //  {
  //
  //    std::string objectId = "xxx";
  //    auto td = new CAMObject(objectId);
  //    std::cout << td << td->iD << " " << td->type << std::endl;
  //    cachedObjectList.insert(std::make_pair(objectId,  1111));
  ////        std::make_pair(objectId, std::make_pair((*td), 1111)));
  //
  //    CAMObjectPtr td3(td);
  //    std::cout << td3.get() << td3->iD << " " << td3->type << std::endl;
  //    std::cout << "11111111111111" << std::endl;
  //
  //    td = new CAMObject(objectId);
  //    td->type = "2";
  //    auto it = std::find_if(
  //        cachedObjectList.cbegin(), cachedObjectList.cend(),
  //        [&objectId](const auto &g) { return g.first == objectId; });
  //
  //    if (it != cachedObjectList.end()) {
  ////      td = new CAMObject(it->second.first); // cp ctor
  ////      td->type = "2";
  //      std::cout << "erase" << std::endl;
  //      cachedObjectList.erase(it);
  //    }
  //
  //    cachedObjectList.insert(std::make_pair(objectId, 222));
  ////        std::make_pair(objectId, std::make_pair((*td), 1111)));
  //
  //    std::cout << "2222222222222" << std::endl;
  //    std::cout << td << td->iD << " " << td->type << std::endl;
  //
  //    CAMObjectPtr td4(td);
  //    std::cout << td4.get() << td4->iD << " " << td4->type << std::endl;
  //  }
  ////  for (const auto &it : cachedObjectList) {
  ////    std::cout << it.second.first.iD << " " << it.second.first.type <<
  /// std::endl; /  }
  //  std::cout << "333333333333" << std::endl;
  //  return;
  //
  //	int kkk = 1;
  //  for (int i = 0; i < 3; i++) {
  //	  switch (i){
  //	  case 1:
  //		  std::cout << i << std::endl;
  //		  if(kkk == 1){
  //			  break;
  //		  }
  //		  break;
  //      default:
  //          break;
  //	  }
  //
  //    std::cout << i << std::endl;
  //  }

  //	std::cout << isDev("") << std::endl;
  //	std::cout << isDev(nullptr) << std::endl;
  //	std::cout << isDev(ENDPOINT_TYPE_MAP) << std::endl;
  //	std::cout << getDev(ENDPOINT_TYPE_TIM) << std::endl;
  //	std::cout << isDev(ENDPOINT_TYPE_DEV_SPAT) << std::endl;
  //	std::cout << getDev(ENDPOINT_TYPE_DEV_PED_SIG) << std::endl;
  //	for (const auto& type : ENDPOINT_TYPES)
  //	    {
  //		std::cout << isDev(type)  << std::endl;
  //	    }
  //
  //	MyParse parse;
  //	SetParseDataFunction(
  //	        bind(&MyParse::parse_app_messages, &parse,
  // std::placeholders::_1, ENDPOINT_TYPE_PED_SIG)); 	std::vector<std::string>
  // messages; 	m_parse_func(messages); 	SetParseDataFunction(
  //	        bind(&MyParse::parse_app_messages, &parse,
  // std::placeholders::_1, ENDPOINT_TYPE_DEV_PED_SIG));
  // m_parse_func(messages);

  //  SharedTwoQueue queue;
  //
  //  Producer producer(queue);
  //  Consumer consumer(queue);
  //
  //  Poco::Thread producerThread;
  //  Poco::Thread consumerThread;
  //
  //  producerThread.start(producer);
  //  consumerThread.start(consumer);
  //
  //  Poco::Thread::sleep(300); // Simulate work
  //  std::cout << "Main Stop" << std::endl;
  //  queue.Stop();
  //
  //  producerThread.join();
  //  consumerThread.join();

  //	example();
  //	auto ptr1 = new MyClass(1);
  //	ObjectPtr ptr(ptr1);
  //	ptr.reset();
  //	ObjectPtr ptr2 = make_unique<MyClass>(2);
  //	ptr2.reset();

  //  std::string iD = "67g";
  //  uint32_t obj_id, obj_id2;
  //  try {
  //    obj_id =
  //        stol("ffffffaf", nullptr, 16); // 4278192658 4278192658
  //        4,294,967,295
  //    std::cout << obj_id << std::endl;
  //    constexpr const uint16_t TOLERATE_OFFSET = 32;
  //
  //    uint32_t delete_start = (obj_id < (0xffffffff - TOLERATE_OFFSET))
  //                                ? obj_id + TOLERATE_OFFSET
  //                                : obj_id;
  //    //        obj_id2 = stoui("ff000a12", nullptr, 16);
  //    //        obj_id2= 0xff000a12;
  //    std::cout << delete_start << std::endl;
  //  } catch (std::invalid_argument const &ex) {
  //    std::cout << "std::invalid_argument::what(): " << ex.what() << '\n';
  //  } catch (std::out_of_range const &ex) {
  //    std::cout << "std::out_of_range::what(): " << ex.what() << '\n';
  //  } catch (exception const &ex) {
  //    std::cout << "bad" << std::endl;
  //    return;
  //  }
  //
  //  return;
  //#include <iostream>
  //#include <map>
  //  std::map<int, std::string> myMap;
  //  // Populate the map with some values
  //  myMap[12] = "one";
  //  myMap[24] = "two";
  //  myMap[33] = "three";
  //  myMap[41] = "four";
  //  myMap[52] = "five";
  //
  //  // Define the threshold value
  //  int threshold = 38;
  //  int max_to_delete = 4;
  //
  //  std::cout << myMap.rbegin()->first << std::endl;
  //
  //  // Iterate through the map in reverse and remove elements with keys
  //  greater
  //  // than the threshold
  //  for (auto it = myMap.rbegin(); it != myMap.rend();) {
  //    //    	std::cout << it->first << ": " << it->second << std::endl;
  //    if (it->first > threshold) {
  //      it = std::map<int, std::string>::reverse_iterator(
  //          myMap.erase(std::next(it).base()));
  //      if (--max_to_delete <= 0) {
  //        break;
  //      }
  //    } else {
  //      break;
  //    }
  //  }

  // Print the remaining elements in the map
  //  for (const auto &pair : myMap) {
  //    std::cout << pair.first << ": " << pair.second << std::endl;
  //  }
  //
  //  const auto data = {
  //      "45",
  //      "words and 2",
  //      "12345678901",
  //  };
  //
  //  for (const std::string s : data) {
  //    std::size_t pos{};
  //    try {
  //      std::cout << "std::stoi(" << std::quoted(s) << "): ";
  //      const int i{std::stoi(s, &pos)};
  //      std::cout << i << "; pos: " << pos << '\n';
  //    } catch (exception const &ex) {
  //      std::cout << "exception what(): " << ex.what() << '\n';
  //    }
  //    //	        catch (std::invalid_argument const& ex)
  //    //	        {
  //    //	            std::cout << "std::invalid_argument::what(): " <<
  //    ex.what()
  //    //<< '\n';
  //    //	        }
  //    //	        catch (std::out_of_range const& ex)
  //    //	        {
  //    //	            std::cout << "std::out_of_range::what(): " <<
  //    ex.what() <<
  //    //'\n'; 	            const long long ll{std::stoll(s, &pos)};
  //    std::cout
  //    //<< "std::stoll("
  //    //<< std::quoted(s) << "): " << ll
  //    //	                      << "; pos: " << pos << '\n';
  //    //	        }
  //  }
  //    m_obj_to_task_id["22"] = 1;
  //    m_obj_to_task_id["33"] = 7;
  //    m_obj_to_task_id["44"] = 3;
  //
  //    logObjectTaskIdMap();
  //    constexpr const uint16_t TOTAL_SIZE_THRESHOLD = 2;
  //    if (m_obj_to_task_id.size() <= TOTAL_SIZE_THRESHOLD)
  //    {
  //        return;
  //    }
  //    int32_t counts = m_obj_to_task_id.size() - TOTAL_SIZE_THRESHOLD;
  //    auto iter = m_obj_to_task_id.begin();
  //    while (iter != m_obj_to_task_id.end())
  //    {
  //        iter = m_obj_to_task_id.erase(iter);
  //        if (--counts <= 0)
  //        {
  //            break;
  //        }
  //    }
  //    logObjectTaskIdMap();

  //  uint32_t min = 4095;
  //
  //  std::cout << min << std::endl;
  //
  //  auto min_str = std::to_string(min);
  //
  //  std::cout << min_str << std::endl;
  //
  //  uint32_t min_tmp = stol(min_str, nullptr, 16);
  //
  //  std::cout << min_tmp << std::endl;
  //
  //  std::cout << stoi(min_str, nullptr, 10) << std::endl;
  //
  //  std::stringstream sstream;
  //  sstream << std::hex << stoi(min_str, nullptr, 10);
  //  std::string result = sstream.str();
  //  std::cout << result << std::endl;
  //
  //  uint16_t permissiveFlashing = 13;
  //  auto phaseColor = Poco::format(
  //      "Green or flashing %?, displayed as flashing red",
  //      permissiveFlashing);
  //  std::cout << phaseColor << std::endl;
  //    Poco::JSON::Object res1;
  //    Poco::JSON::Object response;
  //    auto type = PEDDECTIONYTYPE::DETECTED;
  ////    response.set(PedDetectionRequestType, type);
  //    int code = type;
  //    response.set(PedDetectionRequestType, code);
  //
  //    double lat = 42.2;
  //    double lon = 44.2;
  //    double elevation = 2.2;
  //    double heading = 4.2;
  //    if (type == PEDDECTIONYTYPE::DETECTED)
  //    {
  //        response.set(PedDetectionRequestLatitude, lat);
  //        response.set(PedDetectionRequestLongitude, lon);
  //        response.set(PedDetectionRequestElevation, elevation);
  //        response.set(PedDetectionRequestHeading, heading);
  //    }
  //    res1.set(PedDetectionMessageRequest, response);
  //    std::ostringstream os;
  //    res1.stringify(os, 0);
  //    std::cout << os.str() << std::endl;
  //
  //
  //
  //	auto cc ="20";
  //	uint16_t data = std::stoul(cc, nullptr, 16);
  //	std::cout << data << std::endl;
  //	uint16_t input_status = static_cast<uint16_t>(ntohs(data));
  //	std::cout << input_status << std::endl;
  //
  //	uint16_t res = 0;
  //	            if ((input_status &
  //	                    (1 <<
  // TestV2IIntersectionStatusBit::MANUAL_CONTROL_ENABLE_ACTIVE)) != 0)
  //	            {
  //	                res = saemanualControlIsEnabled;
  //	            }
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::STOP_TIME__ACTIVE)) != 0)
  //	            {
  //	                res = saestopTimeIsActivated;
  //	            }
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::FAULT_FLASH_ACTIVE)) != 0)
  //	            {
  //	                res = saefailureFlash;
  //	            }
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::PREEMPT_ACTIVE)) != 0)
  //	            {
  //	                res = saepreemptIsActive;
  //	            }
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::TSP_ACTIVE)) != 0)
  //	            {
  //	                res = saesignalPriorityIsActive;
  //	            }
  //
  //	            // for Omniair test only
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::FIXED_TIME_OPERATION)) != 0)
  //	            {
  //	                res = saefixedTimeOperation;
  //	            }
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::TRAFFIC_DEPENDENT_OPERATION)) != 0)
  //	            {
  //	                res = saetrafficDependentOperation;
  //	            }
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::STANDBY_OPERATION)) != 0)
  //	            {
  //	                res = saestandbyOperation;
  //	            }
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::FAILURE_MODE)) != 0)
  //	            {
  //	                res = saefailureMode;
  //	            }
  //	            if ((input_status &
  //	                    (1 <<
  // TestV2IIntersectionStatusBit::INTERSECTION_STATUS_OBJECT__OFF)) != 0)
  //	            {
  //	                res = saeIntersectionStatusObject__off;
  //	            }
  //	            if ((input_status & (1 <<
  // TestV2IIntersectionStatusBit::RECENT_MAP_MESSAGE_UPDATE)) != 0)
  //	            {
  //	                res = saerecentMAPmessageUpdate;
  //	            }
  //	            if ((input_status & (1 << TestV2IIntersectionStatusBit::
  //	                                        RECENT_CHANGE_IN_MAP_ASSIGNED_LANES_IDS_USED))
  //!= 0)
  //	            {
  //	                res = saerecentChangeInMAPassignedLanesIDsUsed;
  //	            }
  //	            if ((input_status &
  //	                    (1 <<
  // TestV2IIntersectionStatusBit::NO_VALID_MAP_IS_AVAILABLE_AT_THIS_TIME)) !=
  // 0)
  //	            {
  //	                res = saenoValidMAPisAvailableAtThisTime;
  //	            }
  //	            if ((input_status &
  //	                    (1 <<
  // TestV2IIntersectionStatusBit::NO_VALID_SPAT_IS_AVAILABLE_AT_THIS_TIME)) !=
  //	                0)
  //	            {
  //	                res = saenoValidSPATisAvailableAtThisTime;
  //	            }
  //	            Poco::JSON::Object status_data;
  //	            status_data.set("input_status", input_status);
  //	            status_data.set("set_bit", res);
  //	            std::stringstream ss;
  //	            status_data.stringify(ss);
  //	            std::cout << ss.str() << std::endl;

  // Set up SSL context
  //  Poco::Net::initializeSSL();
  //  Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> certHandler(
  //      new Poco::Net::ConsoleCertificateHandler(false));
  //  Poco::Net::Context::Ptr context(
  //      new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "",
  //                             Poco::Net::Context::VERIFY_RELAXED));
  //  Poco::Net::SSLManager::instance().initializeClient(0, certHandler,
  //  context);
  //
  //  //  Poco::URI uri("http://192.168.1.48:4000/");
  //  Poco::URI uri("https://jsonplaceholder.typicode.com/posts");
  //  std::string path(uri.getPathAndQuery());
  //  if (path.empty())
  //    path = "/";
  //
  //  //  const Poco::Net::Context::Ptr context( new Poco::Net::Context(
  //  //  Poco::Net::Context::CLIENT_USE, "", "", "rootcert.pem" ) );
  //  Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
  //
  //  //  Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
  //  Poco::Net::HTTPRequest m_request(Poco::Net::HTTPRequest::HTTP_GET, path,
  //                                   Poco::Net::HTTPMessage::HTTP_1_1);
  //  //  Poco::Net::HTTPRequest m_request(Poco::Net::HTTPRequest::HTTP_POST,
  //  path,
  //  //                                 Poco::Net::HTTPMessage::HTTP_1_1);
  //
  //  m_request.setContentType("application/json");
  //
  //  Poco::JSON::Object obj;
  //  //  obj.set("name", "blah");
  //  //  obj.set("language", "english");
  //  //
  //  //  std::stringstream ss;
  //  //  obj.stringify(ss);
  //  //  m_request.setContentLength(ss.str().size());
  //
  //  std::ostream &o = session.sendRequest(m_request);
  //
  //  obj.stringify(o);
  //
  //  Poco::Net::HTTPResponse m_response;
  //  // this line is where you get your response
  //  std::istream &rs = session.receiveResponse(m_response);
  //  int retStatus = m_response.getStatus();
  //  std::cout << "Answer from web: " << retStatus << " " <<
  //  m_response.getReason()
  //            << "\n";
  //  if (retStatus == Poco::Net::HTTPResponse::HTTP_OK) {
  //    //  	      Poco::StreamCopier::copyStream(rs,  std::cout);
  //    //  	      processResponseHeaders(m_response);
  //  }
  //
  //  std::ostringstream responseStream;
  //  Poco::StreamCopier::copyStream(rs, responseStream);
  //  Poco::JSON::Parser parser;
  //  Poco::JSON::Array::Ptr ret =
  //      parser.parse(responseStream.str()).extract<Poco::JSON::Array::Ptr>();
  //
  //  std::stringstream ss1;
  //  (*ret).stringify(ss1);
  //  std::cout << ss1.str() << std::endl;
  //
  //  // Clean up SSL
  //  Poco::Net::uninitializeSSL();
  //  return;

  // (*ret) will contain all the members in a json structure returned
  //  if ((*ret).get("success") != true) {
  //    std::cout << "Failed to upload: " << (*ret).get("message").toString();
  //    return;
  //  }

  {
    //	  this->createRequest(uri);
    //	    session.sendRequest(*m_request) << m_body;
    //
    //	    std::istream &rs = session.receiveResponse(*m_response);
    //	    int retStatus = m_response->getStatus();
    //	    g_log.debug() << "Answer from web: " << retStatus << " "
    //	                  << m_response->getReason() << "\n";

    //	    if (retStatus == HTTP_OK ||
    //	        (retStatus == HTTP_CREATED && m_method ==
    // HTTPRequest::HTTP_POST)) { 	      Poco::StreamCopier::copyStream(rs,
    // responseStream); 	      processResponseHeaders(*m_response);
    // return retStatus; 	    } else if ((retStatus == HTTP_FORBIDDEN &&
    // isAuthenticated()) || 	               (retStatus
    //== HTTP_UNAUTHORIZED) || 	               (retStatus == HTTP_NOT_FOUND)) {
    //	      // If authentication fails you can get HTTP_UNAUTHORIZED or
    // HTTP_NOT_FOUND
    //	      // If the limit runs out you can get HTTP_FORBIDDEN
    //	      return this->processAnonymousRequest(*m_response, uri,
    // responseStream); 	    } else if (isRelocated(retStatus)) {
    // return
    // this->processRelocation(*m_response, responseStream); 	    } else {
    //	      Poco::StreamCopier::copyStream(rs, responseStream);
    //	      return processErrorStates(*m_response, rs, uri.toString());
    //	    }
  }
  //
  //  std::string x = "0000200600";
  //  const char *delims = "0";
  //  std::cout << trim(x, delims) << std::endl;
  //
  //  const std::string SEPERATOR = "_";
  //  const std::string DIR_TX = "tx";
  //  const std::string DIR_RX = "rx";
  //  const std::string DIR_BOTH = "Both";
  //
  //  /*  file name convention <identifier>_<interface>_<direction>_<time>
  //     -RSU ID (see rsuID_oid)
  //     -Interface ID
  //     -transmit, receive, or both
  //     -date and time (UTC date and time when the file was created
  //     If the rsuID = rsu1, rsuIfaceName = dsrc,
  //     both directions are collected in the same file,
  //     and the date and time the file was created is January 3, 2019, at
  //     01:35:03, the filename would be rsu1_dsrc_Both_20190103_013503
  // */
  //
  //  time_t now = std::time(nullptr);
  //  auto timeinfo = std::gmtime(&now);
  //  char buffer[32];
  //  std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
  //  auto rsuId = "rsu1";
  //  auto desc = "dsrc";
  //  std::string sep = "_";
  //  std::string filenameStr;
  //  auto dir = LogByDir::BISEPARATE;
  //  filenameStr =
  //      rsuId + SEPERATOR + desc + SEPERATOR + DIR_TX + SEPERATOR + buffer +
  //      ",";
  //  filenameStr +=
  //      rsuId + SEPERATOR + desc + SEPERATOR + DIR_RX + SEPERATOR + buffer;
  //  std::cout << filenameStr << std::endl;
  //
  //  auto half = filenameStr.length() /
  //              2; // filenameStr has two substrings of same lengths
  //  std::cout << filenameStr.substr(0, half) << std::endl;
  //  std::cout << filenameStr.substr(half + 1) << std::endl;

  //    std::set<int> odd = { 5, 3 };
  //    std::set<int> even = { 6, 4};
  //
  //    std::set<int> s(odd);
  //    s.insert(even.begin(), even.end());
  //
  //    for (auto const &e: s) {
  //        std::cout << e << ' ';
  //    }

  //	Poco::JSON::Object obj;
  //	auto tt = "20:03:34";
  //	    obj.set("time", tt);
  //
  //	    std::ostringstream os;
  //	    obj.stringify(os, 0);
  //	    std::cout<< os.str() << std::endl ;

  //  std::cout << getAccessSlot(AccessSlots::NOTUSED) << std::endl;
  //  std::cout << getAccessSlot(AccessSlots::TIMESLOT1) << std::endl;
  //
  //  std::cout << parseChannelText("CCH") << std::endl;
  //  std::map<std::pair<int, int>, std::string> m_wsas;
  //
  //  // populating container:
  //  m_wsas[std::make_pair(1, 1)] = "Washington";
  //  m_wsas[std::make_pair(2, 2)] = "London";
  //  m_wsas[std::make_pair(3, 3)] = "L333";
  //
  //  auto it = m_wsas.begin();
  //  while (it != m_wsas.end()) {
  //    auto &wsa = it->second;
  //
  //    if (wsa == "London") {
  //      it = m_wsas.erase(it);
  //    } else {
  //      it++;
  //    }
  //  }
  //  //	  // show content:
  //  for (auto &x : m_wsas)
  //    std::cout << x.first.first << ": " << x.second << std::endl;
  //
  //  unsigned char val = 0x34;
  //  auto len = encode(&val, sizeof(val));
  //	                    std::cout<< len << std::endl ;

//    std::string len_str{"1130"};
//    auto bytes = decode(len_str);
    std::array<int,2> rsuSpat_preempt_standby{17, 48};

    uint8_t bytes[] = {
        0x04,       // unused bits in final content octet (bits 12–15 unused)
		rsuSpat_preempt_standby[0],
		rsuSpat_preempt_standby[1]        // bits 8–15: bit10=1, bit11=1 (defaults)
    };

//    if (bytes.size())
    {
      int val = bytes[0];
//      std::cout << bytes.size() << " " << val << std::endl;
      std::cout << val << std::endl;
      val = bytes[1];
      std::cout << val << std::endl;
      val = bytes[2];
      std::cout << val << std::endl;
//      std::cout << bytes.size() << " " << val << std::endl;
    }

  //
  //  std::string psid = "bfff";
  //  std::cout << toUnsignedInteger(psid) << std::endl;
  //	auto wsa_handler = std::make_unique<AAA>();
  //	auto pp = wsa_handler.get();
  //	pp->printArr();
  //	auto w2 = std::move(wsa_handler);
  //	pp->printArr();

  //
  //    auto wsa_handler = std::make_unique<TaskEvent>(11);
  //    std::cout<<  wsa_handler.get()<< std::endl ;
  //    auto  h2 = std::move(wsa_handler);
  //    std::cout<<  h2.get()<< std::endl ;

  //  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  //  std::u16string utf16 = convert.from_bytes("xx");
  //  auto event1 = std::make_unique<AlarmEvent>("timestamp", 1, true, true,
  //  true,
  //                                             true, 1, utf16);
  //  auto event2 = std::make_unique<TaskEvent>(11);
  //  auto event3 = std::make_unique<TaskEvent>("task 123");

  //    std::cout<< event1->getName()  << std::endl << event2->getName() <<
  //    std::endl; std::cout<< event3->getName()  << std::endl;
  //  channel_number_mapping CV2X_IEEE{{54590, 172}, {54690, 174}};

  //	std::string replyText =
  // R"({"radio":{"fwVersion":"AG15UAR01A12M4G","fwVersionRequired":"AG15UAR01A12M4G","moduleState":4,"moduleStateStr":"Modem
  // ready","moduleVersion":"AG15","non-ip":{"carrier":true,"cv2x":"V2X_ACTIVE","frequencies":[{"id":0,"max_freq":55230,"min_freq":55050}],"l2addr":10722072,"sps":0,"state":0},
  //"preconfig":"2090035D932FD000000000BFFFFE5802540697FFFFCB004A9238810006000000000000282300E04A2AC094102162C58B162C102162C58B162C102162C58B162C102162C58B162C102162C58B162C102162C58B162C102162C58B162C102162C58B162C0A280003072019C420E012DE8321070096F4"}})";

  //    std::time_t now = time(NULL);
  //  std::tm tm_now;
  //  gmtime_r(&now, &tm_now);
  //  int year_minute =
  //      (((tm_now.tm_yday * 24) + tm_now.tm_hour) * 60) + tm_now.tm_min;
  //  std::cout << year_minute << std::endl;
  //  Poco::DynamicStruct cmd;
  //  cmd["command"] = "set_relay";
  //  cmd["status"] = 1;
  //    std::cout<< cmd.toString() << std::endl;

  //	std::string name_to_search = "MSG_2021";
  //	std::string replyText = R"({
  //"list":[
  //{
  //"attachCertInt":1,
  //"center":false,
  //"data":"PD9=",
  //"dot3":{"chan":"CCH","dest":"ffffffffffff","needDot2Hdr":false,"priority":3,"psid":"8003","security":{"cert":true,"crypt":false,"prof":false,"sign":true},"xtension":0},
  //"duration":-1,
  //"erase":true,
  //"interval":1000,
  //"loop":false,
  //"name":"MSG_2021-05-04_12-39-16",
  //"pause":false,
  //"size":142,
  //"sta":true,
  //"tim":true,
  //"type":"MessageFrame",
  //"valid":true
  //},
  //{
  //"attachCertInt":1,
  //"center":false,
  //"data":"PD94==",
  //"dot3":{"chan":"CCH","dest":"ffffffffffff","priority":3,"psid":"8003","security":{"cert":true,"crypt":false,"prof":false,"sign":true},"slot":"CONTINUOUS","xtension":0},
  //"duration":120,
  //"erase":false,
  //"interval":1000,
  //"loop":false,
  //"name":"SNMP_SRM_1_1",
  //"pause":true,
  //"size":80,
  //"sta":true,
  //"tim":true,
  //"type":"OTHER",
  //"valid":true
  //}
  //],
  //"ref":"its-msg"})";
  //	replyText += "(651610972)";
  //	replyText += R"("}]}}})";
  //	std::cout<< replyText << std::endl;
  //	const char* replyText =
  // R"({"cert":{"storeInfo":{"enrol-active":"b3b6607b6d9fe237"}}})";
  //			R"({"result":{"detail":{"args":{"name":"https://ra.pilot.v2x.isscms.com:8892"},"key":"SCMS_enroled_to"},"state":"OK"}})";
  //  std::string replyText =
  //      R"([{"Events":["task 11"],"index": 1},{"Events":[],"index":
  //      21},{"Events":["xx"],"index": 41}])";
  //  static constexpr const char *TRIGGER_RULE_EVENTS = "Events";
  //
  //  try {
  //    Poco::JSON::Array::Ptr m_trigger_rules =
  //        Poco::JSON::Parser().parse(replyText).extract<Poco::JSON::Array::Ptr>();
  //    // verify event list is non-empty
  //    for (const auto &it : *m_trigger_rules) {
  //      auto rule = it.extract<Poco::JSON::Object::Ptr>();
  //
  //      if (!rule->has(TRIGGER_RULE_EVENTS)) {
  //        std::cout << "invalid : no mandatory element" << std::endl;
  //      }
  //
  //      auto events = rule->getArray(TRIGGER_RULE_EVENTS);
  //
  //      if (!events) {
  //        std::cout << "invalid : not an array or empty" << std::endl;
  //        continue;
  //      }
  //      if (rule->getValue<int>("index") == 21) {
  //        events->add("ppp");
  //        rule->set("found", true);
  //      }
  //      //            std::cout<< event1->isInterestedEvent(*events)  <<
  //      std::endl
  //      //            << event2->isInterestedEvent(*events) << std::endl;
  //      std::ostringstream os;
  //      events->stringify(os, 0);
  //      //            std::cout<< os.str() << std::endl;
  //    }
  //
  //    std::ostringstream os;
  //    m_trigger_rules->stringify(os, 0);
  //    //        std::cout<< os.str() << m_trigger_rules->size() <<
  //    //        std::endl;
  //
  //    Poco::JSON::Object obj;
  //    Poco::JSON::Array arr;
  //    arr.add("qqq");
  //
  //    obj.set("index", 71);
  //    obj.set(TRIGGER_RULE_EVENTS, arr);
  //    //        m_trigger_rules->add(obj);
  //
  //    auto idx = 0;
  //    // modify existing channel
  //    for (const auto &it : *m_trigger_rules) {
  //      auto channel = it.extract<Poco::JSON::Object::Ptr>();
  //      if (channel->getValue<int>("index") == 81) {
  //        break;
  //      }
  //      idx++;
  //    }
  //    if (idx >= m_trigger_rules->size()) {
  //      std::cout << "Entry with index %ld not found" << std::endl;
  //    } else {
  //      m_trigger_rules->set(idx, obj);
  //    }
  //
  //    std::ostringstream os1;
  //    m_trigger_rules->stringify(os1, 0);
  //    //        std::cout<< os1.str() << m_trigger_rules->size() <<
  //    //        std::endl;
  //
  //  } catch (const std::exception &ex) {
  //    std::cout << ex.what() << std::endl;
  //  }

  //    try
  //    {
  //        Poco::JSON::Object::Ptr json =
  //            Poco::JSON::Parser().parse(replyText).extract<Poco::JSON::Object::Ptr>();
  //        if (json->has("radio"))
  //        {
  //        	auto rule = json->getObject("radio");
  //        	if (rule->has("non-ip")){
  //        		auto non_ip = rule->getObject("non-ip");
  //        		if(non_ip->has("l2addr")){
  //        			std::cout<< non_ip->getValue<int>("l2addr")<<
  //        std::endl;
  //        		}
  //
  //        	        	}
  //        }
  //    }
  //    catch (const std::exception& e)
  //    {
  //    	std::cout<< "bad json:" << replyText << std::endl;
  //    }
  //	int kind = 2, psid = 0x8888;
  //	std::string m_last_psid_matched_msg = R"({"kind":")";
  //	m_last_psid_matched_msg += (kind == 1) ? "rx" : "tx";
  //    m_last_psid_matched_msg += R"(","psid":)" + std::to_string(psid) + '}';
  //    std::cout<< m_last_psid_matched_msg << std::endl;
  //    std::ostringstream os;
  //    os << R"(GPS deviation exceeded reference, deviation: )" << 100 << R"(
  //    [cm].)"; std::string msg = os.str(); std::cout<< msg << std::endl;
  //    onMsgRx((uint8_t*)msg.data(), msg.length());
  //
  ////	Poco::Net::IPAddress addr("2031:0:130F::9C0:876A:130B");
  ////	std::cout<< addr.toString() << std::endl;
  ////	const char* a = "2031:0:130F::9C0:876A:130B";
  //	const char* a = "FE80:0000:0000:0000:0250:56FF:FE99:BF09";
  ////	const char* a = "2031:0::130F::9C0:876A:130B"; //invalid
  //	struct in6_addr myAddr;
  //	bzero(&myAddr, sizeof(struct in6_addr));
  //	int result = inet_net_pton_ipv6(a, &myAddr); //parse and verify
  //	if (result > 0)
  //	{
  //		inet_net_ntop_ipv6(&myAddr, -1, ipv6FormatBuffer);
  //		std::cout<< ipv6FormatBuffer << "  "<<
  // sizeof(ipv6FormatBuffer)<<
  //"
  //"<< std::string(ipv6FormatBuffer).length()<< std::endl; 		char
  // buff[INET6_ADDRSTRLEN]; //give to core settings 		snprintf(buff,
  // sizeof(buff),
  //"%08x%08x%08x%08x", htonl(myAddr.s6_addr32[0]),
  // htonl(myAddr.s6_addr32[1]), htonl(myAddr.s6_addr32[2]),
  // htonl(myAddr.s6_addr32[3]));
  //		//	        	std::cout<< std::string(a) << std::endl;
  //		std::cout<< std::string(buff) << std::endl;
  //	        	    //	        	auto part0 =
  // htonl(myAddr.s6_addr32[0]);
  //	        	    //	        	auto part1 =
  //	        	    // htonl(myAddr.s6_addr32[1]);
  //	        	    //	        	auto part2 =
  // htonl(myAddr.s6_addr32[2]);
  //	        	    //	        	auto part3 =
  //	        	    // htonl(myAddr.s6_addr32[3]);
  //	        	    //
  //	        	    //	        	snprintf(buff, sizeof(buff),
  //"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
  //	        	    //
  // getNibble(part0,1),getNibble(part0,2),getNibble(part0,3),getNibble(part0,4),
  //	        	    //
  // getNibble(part1,1),getNibble(part1,2),getNibble(part1,3),getNibble(part1,4),
  //	        	    //
  // getNibble(part2,1),getNibble(part2,2),getNibble(part2,3),getNibble(part2,4),
  //	        	    //
  // getNibble(part3,1),getNibble(part3,2),getNibble(part3,3),getNibble(part3,4));
  //
  //	        }
  //	        else
  //	        {
  //	        	std::cout<< "invalid address" << std::endl;
  //	        }
  ////    char buff[INET_ADDRSTRLEN];
  ////    int i0 = 172;
  ////    int i1 = 24;
  ////    int i2 = 5;
  ////    int i3 = 253;
  ////    snprintf(buff, sizeof(buff), "%02x%02x%02x%02x", i0, i1, i2, i3);
  ////    std::cout<< buff<< std::endl;
  //////
  //	        const char* b = "172.24.5.254";
  ////	        const char* b = "172.24.5.264"; //invalid
  //	        sockaddr_in m_sockaddr;
  //	        auto res = inet_net_pton_ipv4(b, &m_sockaddr); //parse and
  // verify 	        if (res)
  //	        {
  //	        	std::cout<< formatIpV4(m_sockaddr, false) << std::endl;
  //	        }
  //	        else
  //	        {
  //	        	std::cout<< "invalid address" << std::endl;
  //	        }
  ////    	        std::string ipv4 = "ac1805fe";
  ////    	    	            auto bytes = decode(ipv4);
  ////            if (bytes.size())
  ////            {
  ////            	snprintf(buff, sizeof(buff), "%d.%d.%d.%d", bytes[0],
  /// bytes[1], bytes[2], bytes[3]); /            	inet_net_pton_ipv4(buff,
  ///&m_sockaddr); /                auto myaddr =
  /// ntohl(m_sockaddr.sin_addr.s_addr); /                int i0 = (int)((myaddr
  ///& 0xff000000) >> 24); /                int i1 = (int)((myaddr & 0x00ff0000)
  ///>> 16); /                int i2 = (int)((myaddr & 0x0000ff00) >> 8); / int
  /// i3 = (int)((myaddr & 0x000000ff)); /                snprintf(buff,
  /// sizeof(buff), "%02x%02x%02x%02x", i0, i1, i2, i3); / std::cout<< buff<<
  /// std::endl; /            }

  //    if (ipv4.size() == 8)
  //    {
  //        ipv4.insert(6, 1, '.');
  //        ipv4.insert(4, 1, '.');
  //        ipv4.insert(2, 1, '.');
  //        int a, b, c, d;
  //        auto res = sscanf(ipv4.c_str(), "%2x.%2x.%2x.%2x", &a, &b, &c, &d);
  //        std::cout<< res << std::endl;
  //        inet_net_pton_ipv4(ipv4.c_str(), &m_sockaddr);
  //    }

  //	std::string start ="07E5031016010000";
  //	std::string stop ="07EE031916010000";
  //	time_t sys_log_query_start_time{0};
  //	time_t sys_log_query_stop_time{0};
  //	timeFromSnmp(start, sys_log_query_start_time);
  //
  ////	std::time_t start_t =
  /// std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); /
  /// std::ostringstream ss; /    ss << R"("time":")" /       <<
  /// getFormatedTimestamp(start_t,false) << R"(")"; /    std::cout<< ss.str()<<
  /// std::endl;
  //		std::string since{R"(2021-03-10 22:01:00)"};
  //		std::string until{R"(2031-03-25 22:01:00)"};
  //
  ////		since = getFormatedTimestamp(sys_log_query_start_time, false);
  ////		timeFromSnmp(stop, sys_log_query_stop_time);
  ////		until= getFormatedTimestamp(sys_log_query_stop_time, false);
  //
  //		std::cout<< since<< std::endl;
  //		std::cout<< until<< std::endl;
  ////
  //	std::string cmd("/bin/journalctl");
  //	std::vector<std::string> args;
  ////	args.push_back("-o short-precise");
  //	args.push_back("--no-pager");
  //	args.push_back("--since");
  //	args.push_back(since);
  //	args.push_back("--until");
  //	args.push_back(until);
  //	ProcessHandle ph = Process::launch(cmd, args, 0, &(*outPipePtr), 0);
  //	::sleep(10);
  //	    {
  //		Poco::PipeInputStream istr(*outPipePtr);
  //		std::ofstream ostr("processes.txt");
  //		Poco::StreamCopier::copyStream(istr, ostr);
  //		outPipePtr.reset();
  //		istr.close();
  //		std::cout<< since<< std::endl;
  //				std::cout<< until<< std::endl;
  //	    }
  //	    outPipePtr = std::make_unique<Poco::Pipe>();
  //	ph = Process::launch(cmd, args, 0, &(*outPipePtr), 0);
  //	::sleep(20);
  //	{
  //		Poco::PipeInputStream istr1(*outPipePtr);
  //		std::ofstream ostr1("processes1.txt");
  //		Poco::StreamCopier::copyStream(istr1, ostr1);
  //		std::cout<< since<< std::endl;
  //				std::cout<< until<< std::endl;
  //	}

  //	auto target = TARGET_DEVICE::ATC;
  ////	auto target = TARGET_DEVICE::IO_MODULE;
  //    auto it = TARGET_PARAM_MAP.find(target);
  //    if (it != TARGET_PARAM_MAP.end())
  //    {
  //    	std::cout<<  it->second.name << std::endl;
  //    	std::cout<<  it->second.path << std::endl;
  //    	std::cout<<  it->second.intf << std::endl;
  //    	std::cout<<  it->second.member << std::endl;
  //    	std::cout<<  it->second.signature << std::endl;
  //    }
  //    const char* res = "0";
  //    int phases = 0;
  //    getSingleData(oidMaxPhaseGroups, &res);
  //        phases = *res;
  //
  //        std::cout<< phases<< std::endl;
  //        std::cout<< res<< std::endl;
  //
  //        std::ostringstream os;
  //        os << R"({"data_num":)" << res;
  //         os << '}';
  //         std::cout<< os.str()<< std::endl;

  //	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
  // convert;
  //    std::string event_name = "ab";
  //    std::u16string utf16 = convert.from_bytes(event_name);
  //
  //    std::cout<<utf8name(utf16) << "\n";
  //
  //	std::cout<< "start\n";
  //    Poco::JSON::Array arr;
  //    Poco::JSON::Object obj;
  //    try
  //    {
  //    for (unsigned int i = 0; i < 3; i++)
  //    {
  //    	Poco::JSON::Array conf;
  //
  ////        obj.set("number", i);
  ////        conf.add(1);
  ////        obj.set("conf", conf);
  ////        arr.add(obj);
  //    	arr.add("str");
  //    }
  //        	std::stringstream ss;
  //        	arr.stringify(ss);
  //        	std::cout<<  ss.str()<< std::endl;
  ////    arr.clear();
  ////    obj.clear();
  ////    obj.set("number", 12);
  ////    arr.add(obj);
  //    for(const auto& it: arr){
  //
  ////    	auto rule = it.extract<Poco::JSON::Object>();
  ////    	std::cout<<  rule.getValue<int>("number")<< std::endl;
  //    			//->add(1);
  ////    	auto conf = rule.getArray("conf");
  ////    	std::stringstream ss;
  ////    	rule.stringify(ss);
  ////    	std::cout<<  ss.str()<< std::endl;
  //    }
  ////        for (unsigned int i = 0; i < arr.size(); i++)
  ////        {
  ////            auto config = arr.getObject(i);
  ////            std::stringstream ss;
  ////            config->stringify(ss);
  ////            std::cout<< "config" <<  ss.str();
  ////        }
  //    }
  //    catch (const std::exception& ex)
  //    {
  //    	std::cout<< "exception" << ex.what();
  //    }
  //	std::shared_ptr<Obj> sptr_1 = std::make_shared<Obj>();
  //	std::cout << sptr_1.use_count()<< std::endl;
  //	std::shared_ptr<Obj> sptr_2 = sptr_1->Get();
  //	std::cout << sptr_2.use_count()<< std::endl;
  //	std::cout << "unique ownership semantics demo\n";
  //	  {
  //	      auto p = std::make_unique<D>(); // p is a unique_ptr that owns a D
  //	      auto q = pass_through(std::move(p));
  //	      assert(!p); // now p owns nothing and holds a null pointer
  //	      p.reset(); // still safe to reset it
  //	      q->bar();   // and q owns the D object
  //	  } // ~D called here

  //	  {
  //		  auto v = std::make_unique<D_VEC>();
  //		  CACHE_VEC c;
  //		  CACHE_MAP m;
  //	  parseRtpPacket(v.get(),c,m);
  //	  parseRtpPacket2(v.get(),m);
  //	  update(std::move(v));
  //	  // print copied/cached objects
  //	  assert(!v); // now v owns nothing and holds a null pointer
  //	  std::cout << "3 Ds should be gone\n";
  //	  }
  //	  std::cout << "Runtime polymorphism demo\n";
  //	  {
  //	    std::unique_ptr<B> p = std::make_unique<D>(); // p is a unique_ptr
  // that owns a D
  //	                                                  // as a pointer to
  // base 	    p->bar(); // virtual dispatch
  //
  //	    std::vector<std::unique_ptr<B>> v;  // unique_ptr can be stored in a
  // container 	    v.push_back(std::make_unique<D>());
  // v.push_back(std::move(p)); 	    v.emplace_back(new D); for(auto& p:
  // v) p->bar();
  //// virtual dispatch 	  } // ~D called 3 times
  //
  //	  std::cout << "Custom deleter demo\n";
  //	  std::ofstream("demo.txt") << 'x'; // prepare the file to read
  //	  {
  //	      std::unique_ptr<std::FILE, decltype(&close_file)>
  // fp(std::fopen("demo.txt", "r"),
  // &close_file); 	      if(fp) // fopen could have failed; in which case
  // fp holds a null pointer 	        std::cout << (char)std::fgetc(fp.get())
  //<< '\n'; 	  } // fclose() called here, but only if FILE* is not a null
  // pointer
  //	    // (that is, if fopen succeeded)
  //
  //	  std::cout << "Custom lambda-expression deleter demo\n";
  //	  {
  //	    std::unique_ptr<D, std::function<void(D*)>> p(new D, [](D* ptr)
  //	        {
  //	            std::cout << "destroying from a custom deleter...\n";
  //	            delete ptr;
  //	        });  // p owns D
  //	    p->bar();
  //	  } // the lambda above is called and D is destroyed
  //
  //	  std::cout << "Array form of unique_ptr demo\n";
  //	  {
  //	      std::unique_ptr<D[]> p{new D[3]};
  //	  } // calls ~D 3 times

  //	auto body = decode("CBCF3FF5", 8);
  //		char* payload = (char*) body.data();
  //		Poco::MemoryInputStream *inStream = new
  // Poco::MemoryInputStream(payload, 8);
  //
  //		uint32_t a;
  //
  //		Poco::BinaryReader*    _myBinaryReader = new
  // Poco::BinaryReader(*inStream,Poco::BinaryReader::NETWORK_BYTE_ORDER);
  //
  //		(*_myBinaryReader) >> a;
  //		cout<< a << endl;
  //		cout<< ntohl(a) << endl;

  //	cout << isNameSkipped(getStr()) << endl;
  //	cout << isNameSkipped("Signal Loss") << endl;
  //	cout << isNameSkipped("Signal Lose") << endl;
  //	std::map<std::string, int> v;
  //	v["cam3"] = 3;
  //	v["cam1"] = 1;
  //	v["cam4"] = 4;
  //	std::size_t dis = std::distance(std::begin(v), v.find("cam4"));
  //    std::cout << std::distance(std::begin(v), v.find("cam3")) << '\n'
  //    		<< std::distance(std::begin(v), v.find("cam1")) << '\n'
  //			<< dis << '\n'
  //			<< std::distance(std::begin(v), v.find("cam5")) << '\n';
  //	Poco::JSON::Object presObj(true);
  //	presObj.set("foo", 0);
  //	presObj.set("bar", 1);
  //	presObj.set("baz", 0);
  //	std::stringstream ss;
  //	presObj.stringify(ss);
  //	cout<< ss.str();

  //	using namespace std;
  //	char payload[4] = {0x61,0,0x62,0};
  //    int len = 14;
  //    auto body =
  //    decode("0047006c006f00620061006c0020004300680061006e006700650000",
  //    len*4);
  //	char* payload = (char*) body.data();
  //	if(payload[len*2 - 1] == 0 && payload[len*2 - 2]==0){
  //		len --;
  //	}
  //	Poco::MemoryInputStream *inStream = new Poco::MemoryInputStream(payload,
  // len*2);
  //
  //	int16_t a[len];
  //
  //	Poco::BinaryReader*    _myBinaryReader = new
  // Poco::BinaryReader(*inStream,Poco::BinaryReader::NETWORK_BYTE_ORDER);
  //	for(int i = 0; i<len ; i++){
  //	(*_myBinaryReader) >> a[i];
  //	}
  //	std::u16string utf16 = std::u16string((char16_t*)a,len);
  //
  ////	u16string utf16 =
  /// u"\u0047\u006c\u006f\u0062\u0061\u006c\u0020\u0043\u0068\u0061\u006e\u0067\u0065\u0000";
  //	for (char16_t c : utf16)
  //	    cout << "[" << c << "] ";
  //	cout<< endl;
  ////	std::u16string str = std::u16string((char16_t*)&payload[0], 2);
  //		for (const auto& c: utf16){
  //			cout<< c << endl;
  //		}
  //		cout << hex << "UTF-16\n\n";
  //		  for (char16_t c : utf16)
  //		    cout << "[" << c << "] ";
  //
  //		  wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>
  //		                                                  convert;
  //		  string utf8 = convert.to_bytes(utf16);
  //
  //		  cout << "\n\nUTF-16 to UTF-8\n\n";
  //		  for (unsigned char c : utf8)
  //		    cout << "[" << c << "] ";
  //
  //		  cout << utf8 << endl;
  //		  cout << "\n\nConverting back to UTF-16\n\n";
  //
  //		  utf16 = convert.from_bytes(utf8);
  //
  //		  for (char16_t c : utf16)
  //		    cout << "[" << c << "] ";
  //		  cout << endl;

  //	std::stringstream ss;
  //	for (const auto& c: str){
  //		ss << static_cast<char>(c);
  //	}
  //	cout<< ss.str() << endl;
  // change this number to run different functions
}
