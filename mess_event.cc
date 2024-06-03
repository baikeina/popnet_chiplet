#include "mess_event.h"

mess_event::UEventInfo::UEventInfo():flitInfo{0,0}
{
	//nothing
}

mess_event::mess_event(time_type t, mess_type mt,time_type routing_period):
    time_(t),
	mess_(mt),
	src_(),
	des_(),
	/* pc_(),
	vc_(), */
	flit_()
{
	eventInfo.routingPeriod=routing_period;
}

mess_event::mess_event(time_type t, mess_type mt,
		const add_type & a, const add_type & b, long c, long d):
    time_(t),
	mess_(mt),
	src_(a),
	des_(b),
	/* pc_(c),
	vc_(d), */
	flit_()
{
	eventInfo.flitInfo={c,d};
}

mess_event::mess_event(time_type t, mess_type mt,
	const add_type & a, const add_type & b, long c, long d, 
										const flit_template & f):
    time_(t),
	mess_(mt),
	src_(a),
	des_(b),
	/* pc_(c),
	vc_(d), */
	flit_(f)
{
	eventInfo.flitInfo={c,d};
}

mess_event::mess_event(time_type t, mess_type mt, const flit_template & f):
    time_(t),
	mess_(mt),
	src_(),
	des_(),
	/* pc_(),
	vc_(), */
	flit_(f)
{
}

mess_event::mess_event(mess_event& me):
	time_(me.event_start()),
	mess_(me.event_type()),
	src_(me.src()),
	des_(me.des()),
	/* pc_(me.pc()),
	vc_(me.vc()), */
	flit_(me.get_flit())
{
	eventInfo.flitInfo={me.pc(),me.vc()};
}

mess_event::mess_event(const mess_event& me):
	time_(me.event_start()),
	mess_(me.event_type()),
	src_(me.src()),
	des_(me.des()),
	/* pc_(me.pc()),
	vc_(me.vc()), */
	flit_(me.get_flit())
{
	eventInfo.flitInfo={me.pc(),me.vc()};
}

ostream& operator<<(ostream & os, const mess_event & sg) 
{
	os <<"time: "<<sg.time_<<"\tmess type: "<<sg.mess_<<endl;
	return os;
}

time_type mess_event::getRoutingPeriod()const
{
	return eventInfo.routingPeriod;
}
