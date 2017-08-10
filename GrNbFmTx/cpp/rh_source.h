
#ifndef INCLUDED_REDHAWK_RH_SOURCE_H
#define INCLUDED_REDHAWK_RH_SOURCE_H

#include <gnuradio/sync_block.h>

#include "GrNbFmTx.h"

class rh_source: virtual public gr::sync_block {
public:
	typedef boost::shared_ptr<rh_source> sptr;

	static sptr make(bulkio::InShortPort *audio);
};

#endif /* INCLUDED_REDHAWK_RH_SOURCE_H */

