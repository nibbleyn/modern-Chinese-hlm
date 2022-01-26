#include "testCode.hpp"

static constexpr const char* const nibble_lut_lc = "0123456789abcdef";
static constexpr const char* const nibble_lut_uc = "0123456789ABCDEF";

uint8_t decode(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    throw std::invalid_argument("invalid hex char");
}

std::size_t decode(const void* hex, std::size_t h_len, void* bin, std::size_t b_len)
{
    if (h_len % 2 != 0)
    {
        throw std::invalid_argument("invalid hex len");
    }
    if ((h_len >> 1) > b_len)
    {
        throw std::invalid_argument("insufficient bin len");
    }

    auto hexc = static_cast<const char*>(hex);
    auto binc = static_cast<uint8_t*>(bin);
    for (std::size_t i = 0; i < h_len; i += 2)
    {
        binc[(i >> 1)] = (decode(hexc[i]) << 4) | decode(hexc[i + 1]);
    }
    return h_len >> 1;
}

std::size_t encode(const void* bin, std::size_t b_len, void* hex, std::size_t h_len, bool uppercase)
{
    if ((h_len >> 1) < b_len)
    {
        throw std::invalid_argument("insufficient hex len");
    }

    auto binc = static_cast<const uint8_t*>(bin);
    auto hexc = static_cast<char*>(hex);
    const char* lutc = (uppercase) ? nibble_lut_uc : nibble_lut_lc;
    for (std::size_t hi = 0, bi = 0; bi < b_len;)
    {
        uint8_t c = binc[bi++];
        hexc[hi++] = lutc[c >> 4];
        hexc[hi++] = lutc[c & 0x0f];
    }
    return b_len << 1;
}

std::size_t decode(const void* hex, std::size_t h_len, void* bin, std::size_t b_len);
std::size_t encode(
    const void* bin, std::size_t b_len, void* hex, std::size_t h_len, bool uppercase = false);

template <typename RB = uint8_t, class RA = std::vector<RB>>
inline RA decode(const void* hex, std::size_t len)
{
    RA res(((len >> 1) + sizeof(RB) - 1) / (sizeof(RB)));
    (void)decode(hex, len, res.data(), res.size());
    return res;
}

template <typename RB = char, class RA = std::basic_string<RB>>
inline RA encode(const void* bin, std::size_t len, bool uppercase = false)
{
    RB buf[((len << 1) + sizeof(RB) - 1) / sizeof(RB)];
    (void)encode(bin, len, buf, sizeof(buf), uppercase);
    RA res;
    res.assign(buf, (len << 1));
    return res;
}

template <typename RB = uint8_t,
    class RA = std::vector<RB>,
    typename SB = char,
    class SA = std::basic_string<SB>>
inline RA decode(const SA& hex)
{
    return decode<RB, RA>(hex.data(), hex.size() * sizeof(SB));
}

template <typename RB = char,
    class RA = std::basic_string<RB>,
    typename SB = uint8_t,
    class SA = std::vector<SB>>
inline RA encode_upper(const SA& bin)
{
    return encode<RB, RA>(bin.data(), bin.size() * sizeof(SB), true);
}

template <typename RB = char,
    class RA = std::basic_string<RB>,
    typename SB = uint8_t,
    class SA = std::vector<SB>>
inline RA encode_lower(const SA& bin)
{
    return encode<RB, RA>(bin.data(), bin.size() * sizeof(SB), false);
}

template <typename RB = char,
    class RA = std::basic_string<RB>,
    typename SB = uint8_t,
    class SA = std::vector<SB>>
inline RA encode(const SA& bin)
{
    return encode<RB, RA>(bin.data(), bin.size() * sizeof(SB), false);
}

std::set<std::string> skipNames = {"Global Change","Signal Too Bright","Signal Too Dark","Signal Too Blurry",
		"Signal Loss","Reference Image Check Failed","Invalid Configuration","Flame Detected","Smoke Detected","Default Task"
};

bool isNameSkipped(const std::string&& name){

	for(const auto& s: skipNames){
		if (name.length()>= s.length() && name.substr(0,s.length()) == s)
			return true;
	}
	return false;

}

std::string getStr(){
	return "Signal Loss 1";
}

struct B {
  virtual void bar() { std::cout << "B::bar\n"; }
  virtual ~B() = default;
};
struct D : B
{
    D(const std::string& _name): name(_name) { std::cout <<  name << " D::D\n";  }
    D(const D& d1):name(d1.name + "cp"){std::cout << name << " D/D\n";}
    D(const D&& d1):name(std::move(d1.name)){std::cout << name << " D<-D\n";}
    ~D() { std::cout << name << " D::~D\n";  }
    void bar() override { std::cout << " D::bar\n";  }
    std::string name;
};

// a function consuming a unique_ptr can take it by value or by rvalue reference
std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
    p->bar();
    return p;
}

using D_PTR = std::unique_ptr<D>;
using D_VEC = std::vector<D_PTR>;
using CACHE_VEC = std::vector<D>;
using CACHE_MAP= std::map<std::string, D>;
using D_VEC_RPTR = D_VEC* ;
using D_VEC_PTR = std::unique_ptr<D_VEC>;

//using ObjectPtr = std::unique_ptr<Object>;
//using ObjectList = std::vector<ObjectPtr>;
//using ObjectListRawPtr = ObjectList* ;
//using ObjectListPtr = std::unique_ptr<ObjectList>;

void parseRtpPacket(D_VEC_RPTR v,CACHE_VEC& c, CACHE_MAP m){
	  auto d1 = std::make_unique<D>("1");
	  auto d2 = std::make_unique<D>("2");
	  auto d3 = std::make_unique<D>("3");
	  // cache
	  m.insert(std::make_pair("1",*d1));
	  m.insert(std::make_pair("2",*d2));
	  m.insert(std::make_pair("3",*d3));
	  c.push_back(*d1);
	  c.push_back(*d2);
	  c.push_back(*d3);
//	  m["1"] = (*d1);
//	  m["2"] = (*d2);
//	  m["3"]= (*d3);
	  v->push_back(std::move(d1));
	  v->push_back(std::move(d2));
	  v->push_back(std::move(d3));
	for(auto& p: *v) p->bar();
	std::cout << "nothing should be released. \n";
}

void parseRtpPacket2(D_VEC_RPTR v,CACHE_MAP& m){
	  auto d4 = std::make_unique<D>("4");
	  auto d5 = std::make_unique<D>("5");
//	  c["4"]= (*d4);
//	  c["5"]= (*d5);
	  m.insert(std::make_pair("4",*d4));
	  m.insert(std::make_pair("5",*d5));
	  v->push_back(std::move(d4));
	  v->push_back(std::move(d5));
	for(auto& p: *v) p->bar();
	std::cout << "nothing should be released. \n";
}

void update(D_VEC_PTR v1){
	for(auto& p: *v1) p->bar();
	D_VEC_PTR v2 = std::move(v1);
	std::cout << "original v should hold null\n";
	std::cout << "original v1 should hold null\n";
	assert(!v1); // now v1 owns nothing and holds a null pointer
	for(auto& p: *v2) p->bar();
	std::cout << "temp variable v2 is going to finish and thus held Ds\n";
}

void close_file(std::FILE* fp) { std::fclose(fp); }

struct Obj : public std::enable_shared_from_this<Obj>
{
	std::shared_ptr<Obj> Get() {return shared_from_this();}
        ~Obj() {std::cout << "Destructor called on " << this << std::endl;}
};

char buffer[1];

//int getSingleData(const char* oid, const char** data)
//{
//	const char* val = "1";
//	sprintf(buffer, "%x", val);
//    *data = buffer;
//    return 0;
//}

std::string utf8name(std::u16string name)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(name);
}

#define oidMaxPhaseGroups "1.3.6.1.4.1.1206.4.2.1.1.3.0"

#include <unordered_map>
struct param
{
    const char* name;
    const char* path;
    const char* intf;
    const char* member;
    const char* signature;
};

enum class TARGET_DEVICE
{
    ATC,
    IO_MODULE
};

const std::unordered_map<TARGET_DEVICE, param> TARGET_PARAM_MAP = {
    {TARGET_DEVICE::IO_MODULE, {"com.siemens.c2x.io_module", "/com/siemens/c2x/io_module",
                                   "com.siemens.c2x.io_module.Service", "Command", "ss"}},
    {TARGET_DEVICE::ATC, {"com.siemens.c2x.atc_proxy", "/com/siemens/c2x/atc_proxy",
                             "com.siemens.c2x.atc_proxy.Service", "Command", "ss"}}};

#include "Poco/Process.h"
#include "Poco/PipeStream.h"
#include "Poco/StreamCopier.h"
#include <fstream>
using Poco::Process;
using Poco::ProcessHandle;

#include <stdio.h>      /* puts */
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */
#include <chrono>
char* getFormatedTimestamp(bool onlyTime)
{
    std::time_t rawtime;
    std::tm* timeinfo;
    static char buffer[80];

    memset(buffer, 0, 80);
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    if (onlyTime)
    {
        std::strftime(buffer, 80, "%T", timeinfo);
    }
    else
    {
        std::strftime(buffer, 80, "%F %T", timeinfo);
    }
    return buffer;
}

char* getFormatedTimestamp(std::time_t time, bool onlyTime)
{
    std::tm* timeinfo;
    static char buffer[80];

    memset(buffer, 0, 80);
    timeinfo = std::gmtime(&time);

    if (onlyTime)
    {
        std::strftime(buffer, 80, "%T", timeinfo);
    }
    else
    {
        std::strftime(buffer, 80, "%F %T", timeinfo);
    }
    return buffer;
}
const std::string SYS_DIR = "/tmp/ntcip1218/";
const std::string LOG_DIR = "logs/";

bool timeFromSnmp(std::string& snmpTimeHex, time_t& t)
{
    if (snmpTimeHex.length() == 16)
    {
        auto bytes = decode(snmpTimeHex);
        if (bytes.size())
        {
            std::tm timepoint;
            memset(&timepoint, 0, sizeof(timepoint));

            timepoint.tm_year =
                ((((uint16_t)bytes[0] << 8) & 0xFF00) | ((uint16_t)bytes[1] & 0xFF)) - 1900;
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
bool inet_net_pton_ipv4(const char* src, sockaddr_in* dst)
{
    int a, b, c, d, p;
    if (strchr(src, ':'))
    {
        if (5 != sscanf(src, "%d.%d.%d.%d:%d", &a, &b, &c, &d, &p))
        {
            return false;
        }
    }
    else
    {
        if (4 != sscanf(src, "%d.%d.%d.%d", &a, &b, &c, &d))
        {
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

    dst->sin_addr.s_addr =
        htonl(((a & 0xff) << 24) | ((b & 0xff) << 16) | ((c & 0xff) << 8) | (d & 0xff));
    dst->sin_port = htons(p);
    return 1;
}

#include <arpa/inet.h>
int inet_net_pton_ipv6(const char* src, struct in6_addr* dst)
{
    size_t strLen = strlen(src);
    if (strLen > (INET6_ADDRSTRLEN + 15))
        return -1;

    char parseBuffer[INET6_ADDRSTRLEN + 16];
    strncpy(parseBuffer, src, INET6_ADDRSTRLEN + 15);
    parseBuffer[strLen] = '\0';

    int bits = 128;
    char* prefix = strchr(parseBuffer, '/');
    if (prefix)
    {
        *prefix = '\0';
        bits = strtoul(prefix + 1, nullptr, 10);
    }
    if (1 != inet_pton(AF_INET6, parseBuffer, dst->s6_addr))
    {
        bzero(dst, sizeof(struct in6_addr));
        return 0;
    }
    return bits;
}

char ipv6FormatBuffer[INET6_ADDRSTRLEN + 16];
char* inet_net_ntop_ipv6(const in6_addr* i6addr, int prefixLen, char* dst)
{
    if (dst == nullptr)
    {
        dst = ipv6FormatBuffer;
    }
    ::bzero(dst, INET6_ADDRSTRLEN + 1);
    ::inet_ntop(AF_INET6, i6addr, dst, INET6_ADDRSTRLEN + 1);
    if (prefixLen > 0)
    {
        sprintf(dst + strlen(dst), "/%u", prefixLen); // Format CIDR /width.
    }
    return dst;
}

std::string formatIpV4(sockaddr_in m_sockaddr, bool withPort)
{
	uint32_t myaddr = ntohl(m_sockaddr.sin_addr.s_addr);
	uint32_t myport = ntohs(m_sockaddr.sin_port);

    if (myaddr == 0)
    {
        ipv4FormatBuffer[0] = 0;
    }
    else
    {
        int i0 = (int)((myaddr & 0xff000000) >> 24);
        int i1 = (int)((myaddr & 0x00ff0000) >> 16);
        int i2 = (int)((myaddr & 0x0000ff00) >> 8);
        int i3 = (int)((myaddr & 0x000000ff));

        if (withPort)
            snprintf(
                ipv4FormatBuffer, INET_ADDRSTRLEN + 8, "%u.%u.%u.%u:%u", i0, i1, i2, i3, myport);
        else
            snprintf(ipv4FormatBuffer, INET_ADDRSTRLEN, "%u.%u.%u.%u", i0, i1, i2, i3);
    }
    return std::string(ipv4FormatBuffer);
}

#include "Poco/Net/SocketAddress.h"
int getNibble(uint32_t myaddr, int off){
	 switch (off)
	                {
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

void onMsgRx(const uint8_t* data, const size_t len)
{
    if ((data == nullptr) || (len == 0))
    {
        return;
    }
    std::string trap(reinterpret_cast<const char*>(data), len);
    std::cout<< trap << std::endl;
}
#include "utf8StringUtil.hpp"
using channel_number_mapping = std::unordered_map<uint16_t, uint8_t>;

class Event
{
  public:
    Event(){};
    virtual ~Event(){};
    virtual Poco::JSON::Object json() = 0;
    virtual std::string getName() = 0;
    virtual bool isInterestedEvent(const Poco::JSON::Array& arr);
};

class AlarmEvent : public Event
{
  public:
    AlarmEvent() = default;
    ~AlarmEvent(){}
    AlarmEvent(std::string _time,
        unsigned int _id,
        bool _state_flag,
        bool _delete_flag,
        bool _state_set_flag,
        bool _additional_info_flag,
        unsigned int _change_counter,
        std::u16string _name)
        : time(_time)
        , id(_id)
        , state_flag(_state_flag)
        , delete_flag(_delete_flag)
        , state_set_flag(_state_set_flag)
        , additional_info_flag(_additional_info_flag)
        , change_counter(_change_counter)
        , name(_name)
    {
    }
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

class TaskEvent : public Event
{
  public:
    TaskEvent() = default;
    TaskEvent(std::string _name);
    ~TaskEvent(){}
    TaskEvent(unsigned int _id)
        : id(_id)
    {
    }
    Poco::JSON::Object json() override;
    std::string getName() override;

  private:
    unsigned int id{0};
};

static constexpr const char* TASK_PREFIX = "task ";

bool Event::isInterestedEvent(const Poco::JSON::Array& arr)
{
    auto str = getName();
    for (const auto& event : arr)
    {
        auto check_with = event.convert<std::string>();
        if (str.length() >= check_with.length() && str.substr(0, check_with.length()) == check_with)
        {
            return true;
        }
    }
    return false;
}

TaskEvent::TaskEvent(std::string _name){
	std::string prefix{TASK_PREFIX};
	if (_name.length() > prefix.length() && _name.substr(0, prefix.length()) == prefix)
	        {
	            id = std::stoi(_name.substr(prefix.length()), nullptr, 10);
	        }
}

std::string TaskEvent::getName()
{
    return TASK_PREFIX + std::to_string(id);
}

Poco::JSON::Object TaskEvent::json()
{
    Poco::JSON::Object obj;
    obj.set("name", getName());
    return obj;
}

std::string AlarmEvent::getName()
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(name);
}

Poco::JSON::Object AlarmEvent::json()
{
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

class AAA{
public:
	AAA() {m_wsaArr.fill(0x55); printArr();
    };
	void printArr(){
		std::cout << "printArr"<< std::endl ;
		for (auto& wsa : m_wsaArr)
		    {
		    	std::cout<< wsa << std::endl ;
		    }
	}
private:
	std::array<int, 16> m_wsaArr;
};

inline unsigned int toUnsignedInteger(std::string str)
{

    return (str.length() > 0) ? strtoul(str.c_str(), 0, 0) : 0;
}

enum ItsChannel
{
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

enum AccessSlots
{
    BOTH = 0,
    TIMESLOT0,
    TIMESLOT1,
    NOTUSED
};

static std::string getAccessSlot(AccessSlots access)
{
    auto SLOTS = {"continuous", "slot0", "slot1", "notused"};
    return *(SLOTS.begin() + access);
}

static constexpr std::size_t CCH_NUM{1};
static constexpr std::size_t SCH_NUM{6};

static constexpr std::size_t CHAN_COUNT{CCH_NUM + SCH_NUM};
static constexpr std::size_t CHAN_COUNT_20MHZ{2};

static const char* _channel_name[CHAN_COUNT + CHAN_COUNT_20MHZ] =
{
   "CCH",
   "SCH1",
   "SCH2",
   "SCH3",
   "SCH4",
   "SCH5",
   "SCH6",
   "CH20_0",
   "CH20_1"
};

static inline constexpr bool CHANNEL_VALID_20MHZ(std::size_t chan)
{
    return chan < (CHAN_COUNT + CHAN_COUNT_20MHZ);
}

ItsChannel MAKE_CHANNEL_20MHZ(std::size_t chan)
{
    return CHANNEL_VALID_20MHZ(chan) ? (ItsChannel)chan : ItsChannel::INVALID;
}

ItsChannel parseChannelText(const char* pszId)
{
    if (pszId == nullptr)
        return ItsChannel::INVALID;

    size_t len = strlen(pszId);
    if (len < 3)
        return ItsChannel::INVALID;
    if (len > 6)
        return ItsChannel::INVALID;

    for (std::size_t i = 0; i < (CHAN_COUNT + CHAN_COUNT_20MHZ); ++i)
    {
        if (0 == strcasecmp(_channel_name[i], pszId))
        {
            return MAKE_CHANNEL_20MHZ(i);
        }
    }
    return ItsChannel::INVALID;
}
#include <set>

enum LogByDir : uint32_t
{
    INBOUNDONLY = 1,
    OUTBOUNDONLY,
    BISEPARATE,
    BICOMBINED
};

void tryCode() {

    const std::string SEPERATOR = "_";
    const std::string DIR_TX = "tx";
    const std::string DIR_RX = "rx";
    const std::string DIR_BOTH = "Both";

    /*  file name convention <identifier>_<interface>_<direction>_<time>
       -RSU ID (see rsuID_oid)
       -Interface ID
       -transmit, receive, or both
       -date and time (UTC date and time when the file was created
       If the rsuID = rsu1, rsuIfaceName = dsrc,
       both directions are collected in the same file,
       and the date and time the file was created is January 3, 2019, at 01:35:03,
       the filename would be rsu1_dsrc_Both_20190103_013503
   */

    time_t now = std::time(nullptr);
    auto timeinfo = std::gmtime(&now);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    auto rsuId = "rsu1";
    auto desc = "dsrc";
    std::string sep = "_";
    std::string filenameStr;
    auto dir = LogByDir::BISEPARATE;
        	filenameStr =  rsuId + SEPERATOR + desc + SEPERATOR + DIR_TX + SEPERATOR + buffer + ",";
        	filenameStr +=  rsuId + SEPERATOR + desc + SEPERATOR + DIR_RX + SEPERATOR + buffer;
        	std::cout << filenameStr << std::endl;

    auto half = filenameStr.length()/2; //filenameStr has two substrings of same lengths
    std::cout << filenameStr.substr(0,half)<< std::endl;
    std::cout <<filenameStr.substr(half+1)<< std::endl;

    std::set<int> odd = { 5, 3 };
    std::set<int> even = { 6, 4};

    std::set<int> s(odd);
    s.insert(even.begin(), even.end());

    for (auto const &e: s) {
        std::cout << e << ' ';
    }




	Poco::JSON::Object obj;
	auto tt = "20:03:34";
	    obj.set("time", tt);

	    std::ostringstream os;
	    obj.stringify(os, 0);
	    std::cout<< os.str() << std::endl ;

	std::cout<< getAccessSlot(AccessSlots::NOTUSED) << std::endl ;
	std::cout<< getAccessSlot(AccessSlots::TIMESLOT1) << std::endl ;

	std::cout<< parseChannelText("CCH") << std::endl ;
	  std::map<std::pair<int, int>,std::string> m_wsas;

	  // populating container:
	  m_wsas[std::make_pair(1,1)] = "Washington";
	  m_wsas[std::make_pair(2,2)] = "London";
	  m_wsas[std::make_pair(3,3)] = "L333";

	    auto it = m_wsas.begin();
	    while (it != m_wsas.end())
	    {
	        auto& wsa = it->second;

	        if (wsa == "London")
	        {
	            it = m_wsas.erase(it);
	        }
	        else
	        {
	            it++;
	        }
	    }
//	  // show content:
	    for ( auto& x: m_wsas )
	      std::cout << x.first.first << ": " << x.second << std::endl;

	unsigned char val = 0x34;
	                    auto len = encode(&val, sizeof(val));
//	                    std::cout<< len << std::endl ;

	                    std::string len_str{"40"};
	                        	    	            auto bytes = decode(len_str);
	                                if (bytes.size())
	                                {
	                                	int val = bytes[0];
//	                                    std::cout<< val << std::endl;
	                                }

	                                std::string psid = "bfff";
	                                std::cout<< toUnsignedInteger(psid)<< std::endl;
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

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	std::u16string utf16 = convert.from_bytes("xx");
    auto event1 = std::make_unique<AlarmEvent>("timestamp", 1, true, true, true, true, 1, utf16);
    auto event2 = std::make_unique<TaskEvent>(11);
    auto event3 = std::make_unique<TaskEvent>("task 123");

//    std::cout<< event1->getName()  << std::endl << event2->getName() << std::endl;
//    std::cout<< event3->getName()  << std::endl;
	channel_number_mapping CV2X_IEEE{{54590,172},{54690,174}};

//	std::string replyText = R"({"radio":{"fwVersion":"AG15UAR01A12M4G","fwVersionRequired":"AG15UAR01A12M4G","moduleState":4,"moduleStateStr":"Modem ready","moduleVersion":"AG15","non-ip":{"carrier":true,"cv2x":"V2X_ACTIVE","frequencies":[{"id":0,"max_freq":55230,"min_freq":55050}],"l2addr":10722072,"sps":0,"state":0},
//"preconfig":"2090035D932FD000000000BFFFFE5802540697FFFFCB004A9238810006000000000000282300E04A2AC094102162C58B162C102162C58B162C102162C58B162C102162C58B162C102162C58B162C102162C58B162C102162C58B162C102162C58B162C0A280003072019C420E012DE8321070096F4"}})";

//    std::time_t now = time(NULL);
    std::tm tm_now;
    gmtime_r(&now, &tm_now);
    int year_minute = (((tm_now.tm_yday * 24) + tm_now.tm_hour) * 60) + tm_now.tm_min;
    std::cout<< year_minute << std::endl;
    Poco::DynamicStruct cmd;
    cmd["command"] = "set_relay";
    cmd["status"] =  1;
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
//	const char* replyText = R"({"cert":{"storeInfo":{"enrol-active":"b3b6607b6d9fe237"}}})";
//			R"({"result":{"detail":{"args":{"name":"https://ra.pilot.v2x.isscms.com:8892"},"key":"SCMS_enroled_to"},"state":"OK"}})";
	std::string replyText = R"([{"Events":["task 11"],"index": 1},{"Events":[],"index": 21},{"Events":["xx"],"index": 41}])";
	static constexpr const char* TRIGGER_RULE_EVENTS = "Events";

    try
    {
    	Poco::JSON::Array::Ptr m_trigger_rules =
    	            Poco::JSON::Parser().parse(replyText).extract<Poco::JSON::Array::Ptr>();
        // verify event list is non-empty
        for (const auto& it : *m_trigger_rules)
        {
            auto rule = it.extract<Poco::JSON::Object::Ptr>();

            if (!rule->has(TRIGGER_RULE_EVENTS))
            {
            	std::cout<< "invalid : no mandatory element" << std::endl;
            }

            auto events = rule->getArray(TRIGGER_RULE_EVENTS);

            if (!events)
            {
            	std::cout<< "invalid : not an array or empty" << std::endl;
            	continue;
            }
            if(rule->getValue<int>("index") == 21){
            	events->add("ppp");
            	rule->set("found", true);
            }
//            std::cout<< event1->isInterestedEvent(*events)  << std::endl << event2->isInterestedEvent(*events) << std::endl;
            std::ostringstream os;
            events->stringify(os, 0);
//            std::cout<< os.str() << std::endl;
        }

        std::ostringstream os;
        m_trigger_rules->stringify(os, 0);
//        std::cout<< os.str() << m_trigger_rules->size() <<        		std::endl;

        Poco::JSON::Object obj;
        Poco::JSON::Array arr;
        arr.add("qqq");

        obj.set("index",71);
        obj.set(TRIGGER_RULE_EVENTS,arr);
//        m_trigger_rules->add(obj);


    	auto idx = 0;
        // modify existing channel
        for (const auto& it : *m_trigger_rules)
        {
            auto channel = it.extract<Poco::JSON::Object::Ptr>();
            if (channel->getValue<int>("index") == 81)
            {
            	break;
            }
            idx ++;
        }
        if(idx >= m_trigger_rules->size()){
        	std::cout<< "Entry with index %ld not found" << std::endl;
        }
        else{
        	m_trigger_rules->set(idx,obj);
        }

        std::ostringstream os1;
        m_trigger_rules->stringify(os1, 0);
//        std::cout<< os1.str() << m_trigger_rules->size() <<        		std::endl;

    }
    catch (const std::exception& ex)
    {
    	std::cout<< ex.what()<< std::endl;
    }

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
//        			std::cout<< non_ip->getValue<int>("l2addr")<< std::endl;
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
//	m_last_psid_matched_msg += (kind == 1)                                  ? "rx"                                  : "tx";
//    m_last_psid_matched_msg += R"(","psid":)" + std::to_string(psid) + '}';
//    std::cout<< m_last_psid_matched_msg << std::endl;
//    std::ostringstream os;
//    os << R"(GPS deviation exceeded reference, deviation: )" << 100 << R"( [cm].)";
//    std::string msg = os.str();
//    std::cout<< msg << std::endl;
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
//		std::cout<< ipv6FormatBuffer << "  "<< sizeof(ipv6FormatBuffer)<< "  "<< std::string(ipv6FormatBuffer).length()<< std::endl;
//		char buff[INET6_ADDRSTRLEN]; //give to core settings
//		snprintf(buff, sizeof(buff), "%08x%08x%08x%08x", htonl(myAddr.s6_addr32[0]),
//				htonl(myAddr.s6_addr32[1]), htonl(myAddr.s6_addr32[2]),
//				htonl(myAddr.s6_addr32[3]));
//		//	        	std::cout<< std::string(a) << std::endl;
//		std::cout<< std::string(buff) << std::endl;
//	        	    //	        	auto part0 = htonl(myAddr.s6_addr32[0]);
//	        	    //	        	auto part1 =
//	        	    //	        	        htonl(myAddr.s6_addr32[1]);
//	        	    //	        	auto part2 = htonl(myAddr.s6_addr32[2]);
//	        	    //	        	auto part3 =
//	        	    //	        	        htonl(myAddr.s6_addr32[3]);
//	        	    //
//	        	    //	        	snprintf(buff, sizeof(buff), "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
//	        	    //	        			getNibble(part0,1),getNibble(part0,2),getNibble(part0,3),getNibble(part0,4),
//	        	    //	        			getNibble(part1,1),getNibble(part1,2),getNibble(part1,3),getNibble(part1,4),
//	        	    //						getNibble(part2,1),getNibble(part2,2),getNibble(part2,3),getNibble(part2,4),
//	        	    //						getNibble(part3,1),getNibble(part3,2),getNibble(part3,3),getNibble(part3,4));
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
//	        auto res = inet_net_pton_ipv4(b, &m_sockaddr); //parse and verify
//	        if (res)
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
////            	snprintf(buff, sizeof(buff), "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
////            	inet_net_pton_ipv4(buff, &m_sockaddr);
////                auto myaddr = ntohl(m_sockaddr.sin_addr.s_addr);
////                int i0 = (int)((myaddr & 0xff000000) >> 24);
////                int i1 = (int)((myaddr & 0x00ff0000) >> 16);
////                int i2 = (int)((myaddr & 0x0000ff00) >> 8);
////                int i3 = (int)((myaddr & 0x000000ff));
////                snprintf(buff, sizeof(buff), "%02x%02x%02x%02x", i0, i1, i2, i3);
////                std::cout<< buff<< std::endl;
////            }

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
////	std::time_t start_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
////    std::ostringstream ss;
////    ss << R"("time":")"
////       << getFormatedTimestamp(start_t,false) << R"(")";
////    std::cout<< ss.str()<< std::endl;
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

//	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
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
//	    std::unique_ptr<B> p = std::make_unique<D>(); // p is a unique_ptr that owns a D
//	                                                  // as a pointer to base
//	    p->bar(); // virtual dispatch
//
//	    std::vector<std::unique_ptr<B>> v;  // unique_ptr can be stored in a container
//	    v.push_back(std::make_unique<D>());
//	    v.push_back(std::move(p));
//	    v.emplace_back(new D);
//	    for(auto& p: v) p->bar(); // virtual dispatch
//	  } // ~D called 3 times
//
//	  std::cout << "Custom deleter demo\n";
//	  std::ofstream("demo.txt") << 'x'; // prepare the file to read
//	  {
//	      std::unique_ptr<std::FILE, decltype(&close_file)> fp(std::fopen("demo.txt", "r"),
//	                                                           &close_file);
//	      if(fp) // fopen could have failed; in which case fp holds a null pointer
//	        std::cout << (char)std::fgetc(fp.get()) << '\n';
//	  } // fclose() called here, but only if FILE* is not a null pointer
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
//		Poco::MemoryInputStream *inStream = new Poco::MemoryInputStream(payload, 8);
//
//		uint32_t a;
//
//		Poco::BinaryReader*    _myBinaryReader = new Poco::BinaryReader(*inStream,Poco::BinaryReader::NETWORK_BYTE_ORDER);
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
//    auto body = decode("0047006c006f00620061006c0020004300680061006e006700650000", len*4);
//	char* payload = (char*) body.data();
//	if(payload[len*2 - 1] == 0 && payload[len*2 - 2]==0){
//		len --;
//	}
//	Poco::MemoryInputStream *inStream = new Poco::MemoryInputStream(payload, len*2);
//
//	int16_t a[len];
//
//	Poco::BinaryReader*    _myBinaryReader = new Poco::BinaryReader(*inStream,Poco::BinaryReader::NETWORK_BYTE_ORDER);
//	for(int i = 0; i<len ; i++){
//	(*_myBinaryReader) >> a[i];
//	}
//	std::u16string utf16 = std::u16string((char16_t*)a,len);
//
////	u16string utf16 = u"\u0047\u006c\u006f\u0062\u0061\u006c\u0020\u0043\u0068\u0061\u006e\u0067\u0065\u0000";
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

