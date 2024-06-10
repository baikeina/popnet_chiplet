#ifndef SIM_FOUNDATION_
#define SIM_FOUNDATION_

#include "sim_router.h"
#include "flit.h"
#include "mess_event.h"
#include <vector>
#include "configuration.h"
#include "SRGen.h"
#include <exception>
#include <utility>
#include <map>
#include <functional>
#include <iostream>
#include <fstream>

//changed at 2020-4-20
#include"changes/router.h"
#include"sim_protoengine.h"

class sim_foundation {

	friend ostream& operator<<(ostream&, const sim_foundation&);

	private:
		//changed at 2022-4-3
		//改为继承关系
		//vector<sim_router_template> inter_network_;
		std::vector<CRouter>inter_network_;
		long ary_size_;//每个维度下的尺寸
		long cube_size_;//维度
		long router_counter_;//路由器总数
		long packet_counter_;
		//changed at 2024-6-3
		ProtoEngine proto_engine_;

		static string file_name_;
		static sim_foundation * s_f_;

	public:
		//changed at 2021-10-26
		//将文件流改为队列，以提高速度
		//ifstream inFile_;
		std::vector<SPacket>inputTraces;

	public:
		class file_error: public exception {
			public:
				file_error(const string & err) : what_(err) {}
				virtual const char * what() const throw() {return what_.c_str();}
				virtual ~file_error() throw();

			private:
				string what_;
		};

		static const sim_foundation & sf() {return *s_f_;}
		static sim_foundation & wsf() {return *s_f_;}
		sim_foundation();
		~sim_foundation(){s_f_ = nullptr;}
		long ary_size() const {return ary_size_;}
		long cube_size() const {return cube_size_;}
		long packet_counter() {return packet_counter_;}
		long packet_counter() const {return packet_counter_;}

		//changed at 2022-4-3
		vector</*sim_router_template*/CRouter> & inter_network() {return inter_network_;}
		const vector</*sim_router_template*/CRouter> & inter_network() const
	   												{return inter_network_;}
		/*sim_router_template*/CRouter & router(const add_type& a);
		const /*sim_router_template*/CRouter & router(const add_type& a) const;
		ProtoEngine& protoEngine() { return proto_engine_; }
		const ProtoEngine& protoEngine() const { return proto_engine_; }
		bool valid_address(const add_type & a) const;

		void receive_EVG_message(mess_event mesg);
		void receive_ROUTER_message(mess_event mesg);
		void receive_WIRE_message(mess_event mesg);
		void receive_CREDIT_message(mess_event mesg);
		//changed at 2022-4-20
		void receive_RECONFIGURATION_message(const mess_event&msg);

		void router_power_exchange();
		
		void simulation_results();
		void simulation_check();

		void init_file();

		//changed at 2021-10-26
		void readTraceFile();

		void inputTrace(const SPacket&packet);
};

void readPacket(SPacket&packet,std::ifstream&ifs,size_t dimension);
#endif
