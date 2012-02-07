/**
 * Hamlog
 *
 * Copyright (C) 2011, Jan Kaluza <hanzz.k@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include "hamlib.h"
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "reply.h"
#include "session.h"
#include "storagebackend.h"
#include "../md5.h"
#include "logbook_table.h"
#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"
#include <boost/algorithm/string.hpp>
#include "server.h"
#include "log.h"
#include "config.h"

#include <hamlib/rig.h>


namespace HamLog {
namespace Responder {

DEFINE_LOGGER(logger, "Hamlib");

static std::string roundedCast(double toCast, unsigned precision = 2u) {
	std::ostringstream result;
	result.precision(precision);
	result << toCast;
	return result.str();
}

	
Hamlib::Hamlib(Server *server) : RequestResponder("Hamlib module", "/hamlib", Module::UNKNOWN, true) {
	m_server = server;
}


void Hamlib::sendFrequency(Session::ref session, Request::ref request, Reply::ref reply) {
	Config *config = m_server->getConfig();
	if (config->getUnregistered().find("hamlib.device") == config->getUnregistered().end()) {
		LOG_ERROR(logger, "Set '[hamlib] device' in config file.");
	}

	std::string device = config->getUnregistered().find("hamlib.device")->second;
	
	RIG *my_rig;
	freq_t freq;
	int retcode;

	rig_set_debug(RIG_DEBUG_NONE);
	//  rig_set_debug(RIG_DEBUG_TRACE);

	rig_model_t myrig_model = RIG_MODEL_IC706MKIIG;
	my_rig = rig_init(myrig_model);
	my_rig->state.rigport.type.rig = RIG_PORT_SERIAL;
	my_rig->state.rigport.parm.serial.rate = 19200;
	my_rig->state.rigport.parm.serial.data_bits = 8;
	my_rig->state.rigport.parm.serial.stop_bits = 1;
	my_rig->state.rigport.parm.serial.parity = RIG_PARITY_NONE;
	my_rig->state.rigport.parm.serial.handshake = RIG_HANDSHAKE_NONE;
	strncpy(my_rig->state.rigport.pathname, device.c_str(), FILPATHLEN - 1);
	if ((retcode = rig_open(my_rig)) != RIG_OK)
	{
		fprintf(stderr, "rig_open: error = %s\n", rigerror(retcode));
		LOG_ERROR(logger, "rig_open: error " << rigerror(retcode));
	}
	if ((retcode = rig_get_freq(my_rig, RIG_VFO_CURR, &freq) != RIG_OK))
	{
		LOG_ERROR(logger, "rig_set_freq: error " << rigerror(retcode));
	}
	rig_get_freq(my_rig, RIG_VFO_CURR, &freq);
	rig_close(my_rig);
	rig_cleanup(my_rig);

	freq = freq / 1000;

	std::string f = roundedCast(freq, 6);
	reply->setContent(f);
}

void Hamlib::setFrequency(Session::ref session, Request::ref request, Reply::ref reply) {
	Config *config = m_server->getConfig();
	if (config->getUnregistered().find("hamlib.device") == config->getUnregistered().end()) {
		LOG_ERROR(logger, "Set '[hamlib] device' in config file.");
	}

	std::string device = config->getUnregistered().find("hamlib.device")->second;

	RIG *my_rig;
	freq_t freq;
	int retcode;

	std::string f = request->getContent();

	if (sscanf(f.c_str(), "%lf", &freq) != 1) {
		LOG_WARN(logger, "Unable to read freq " << f)
		fprintf(stderr, "ERROR: Unable to read freq.\n");
	}
	std::cout << boost::lexical_cast<std::string>((double) freq) << "\n";
	rig_set_debug(RIG_DEBUG_NONE);
	//  rig_set_debug(RIG_DEBUG_TRACE);

	rig_model_t myrig_model = RIG_MODEL_IC706MKIIG;
	my_rig = rig_init(myrig_model);
	my_rig->state.rigport.type.rig = RIG_PORT_SERIAL;
	my_rig->state.rigport.parm.serial.rate = 19200;
	my_rig->state.rigport.parm.serial.data_bits = 8;
	my_rig->state.rigport.parm.serial.stop_bits = 1;
	my_rig->state.rigport.parm.serial.parity = RIG_PARITY_NONE;
	my_rig->state.rigport.parm.serial.handshake = RIG_HANDSHAKE_NONE;
	strncpy(my_rig->state.rigport.pathname, device.c_str(), FILPATHLEN - 1);
	if ((retcode = rig_open(my_rig)) != RIG_OK)
	{
		fprintf(stderr, "rig_open: error = %s\n", rigerror(retcode));
		LOG_ERROR(logger, "rig_open: error " << rigerror(retcode));
	}
	if ((retcode = rig_set_freq(my_rig, RIG_VFO_CURR, freq) != RIG_OK))
	{
		LOG_ERROR(logger, "rig_set_freq: error " << rigerror(retcode));
	}
	rig_get_freq(my_rig, RIG_VFO_CURR, &freq);
	rig_close(my_rig);
	rig_cleanup(my_rig);
}

bool Hamlib::handleRequest(Session::ref session, Request::ref request, Reply::ref reply) {
	std::string uri = request->getURI();

	if (uri == "/hamlib") {
		if (request->getMethod() == "POST") {
			setFrequency(session, request, reply);
		}
		else {
			sendFrequency(session, request, reply);
		}
	}
	else {
		return false;
	}

	return true;
}

extern "C" {
	Module *module_init(Server *);
    Module *module_init(Server *server) {
		return new Hamlib(server);
    }
}

}
}
