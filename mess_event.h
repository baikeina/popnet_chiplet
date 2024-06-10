#ifndef NETWORK_MESS_EVENT_H_
#define NETWORK_MESS_EVENT_H_

#include "index.h"
#include "flit.h"
#include <vector>
#include <iostream>

//事件类，用于相邻路由器之间
class mess_event {
	friend ostream & operator<<(ostream& os, const mess_event & sg);

	struct SFlitInfo{
		long pc,vc;
	};
	union UEventInfo
	{
		SFlitInfo flitInfo;
		time_type routingPeriod;

		UEventInfo();
	};
	

	private:
		time_type time_;
		mess_type mess_;
		add_type src_;
		add_type des_;
		//changed at 2022-4-14
		//增加不同的路由周期
		/* long pc_;
		long vc_; */
		UEventInfo eventInfo;
		flit_template flit_;

	public:
		//changed at 2024-6-10
		void set_event_start(time_type a) {time_ = a;}

		time_type event_start() {return time_;}
		time_type event_start() const {return time_;}
		mess_type event_type() {return mess_;}
		mess_type event_type() const {return mess_;}
		add_type & src() {return src_;}
		const add_type & src() const {return src_;}
		add_type & des() {return des_;}
		const add_type & des() const {return des_;}
		long pc()
		{
			//return pc_;
			return eventInfo.flitInfo.pc;
		}
		long pc() const 
		{
			//return pc_;
			return eventInfo.flitInfo.pc;
		}
		long vc() 
		{
			//return vc_;
			return eventInfo.flitInfo.vc;
		}
		long vc() const 
		{
			//return vc_;
			return eventInfo.flitInfo.vc;
		}
		flit_template & get_flit() {return flit_;}
		const flit_template & get_flit() const {return flit_;}

		//EVG_ and ROUTER_ message
		mess_event(time_type t, mess_type mt,time_type routing_period=PIPE_DELAY_);
		//CREDIT_ message
		mess_event(time_type t, mess_type mt, 
		 const add_type & a, const add_type & b, long c, long d);
		//WIRE_ message c is the phy and d is the vc
		mess_event(time_type t, mess_type mt, 
		 const add_type & a, const add_type & b, long c, long d,
		 const flit_template & f);
		mess_event(time_type t, mess_type mt, const flit_template & f);

		mess_event(mess_event& me);
		mess_event(const mess_event& me);

		time_type getRoutingPeriod()const;
};
#endif
